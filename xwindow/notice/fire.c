#if !defined(__def_hwport_source_template_main_c__)
# define __def_hwport_source_template_main_c__ "template_main.c"

#if !defined(_ISOC99_SOURCE)
# define _ISOC99_SOURCE (1L)
#endif

#if !defined(_GNU_SOURCE)
# define _GNU_SOURCE (1L)
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <X11/extensions/Xrender.h>

#include <X11/Xos.h>

#define def_xfire_draw_method (1)

typedef enum {
    xfire_fire0_window = 0,
    xfire_fire1_window,
    xfire_fire2_window,
    xfire_fire3_window,
    xfire_max_window
}__xfire_window_t;
#define xfire_window_t __xfire_window_t
#define def_xfire_window_names {"root", "background", (const char *)0}

typedef struct {
    Window m_root_window;
    int m_x, m_y;
    unsigned int m_width, m_height, m_border_width, m_depth;
}__xfire_geometry_t;
#define xfire_geometry_t __xfire_geometry_t

typedef struct {
    const char *m_display_name;
    Display *m_display;
    Screen *m_screen;

    int m_visualinfo_count, m_select_visualinfo;
    XVisualInfo *m_visualinfo;

    long m_event_mask;

    Colormap m_colormap;
    XSetWindowAttributes m_window_attributes[xfire_max_window];
    Window m_window[xfire_max_window];
    xfire_geometry_t m_geometry[xfire_max_window];
    GC m_gc[xfire_max_window];
    XImage *m_ximage[xfire_max_window];

    XEvent m_event;

#if def_xfire_draw_method == (1)
    /* fire info */
    unsigned char *m_fire_map[xfire_max_window];
    unsigned int m_fire_color_table[xfire_max_window][256];
#endif
}__xfire_t;
#define xfire_t __xfire_t

static int xfire_select_visual(xfire_t *s_xfire);

int main(int s_argc, char **s_argv);

static int xfire_select_visual(xfire_t *s_xfire)
{
    int s_event_base, s_error_base;

    XVisualInfo s_template_visualinfo;
    XRenderPictFormat *s_format;

    if(XRenderQueryExtension(s_xfire->m_display, &s_event_base, &s_error_base) == 0) {
        (void)fprintf(stderr, "XRenderQueryExtension failed !\n");
        return(-1);
    }

    (void)memset((void *)(&s_template_visualinfo), 0, sizeof(s_template_visualinfo));
    s_template_visualinfo.screen = XScreenNumberOfScreen(s_xfire->m_screen);
    s_template_visualinfo.depth = 32;
#if defined(__cplusplus)
    s_template_visualinfo.c_class = TrueColor;
#else
    s_template_visualinfo.class = TrueColor;
#endif

    s_xfire->m_visualinfo = XGetVisualInfo(
        s_xfire->m_display,
        VisualScreenMask | VisualDepthMask | VisualClassMask,
        (XVisualInfo *)(&s_template_visualinfo),
        (int *)(&s_xfire->m_visualinfo_count)
    );
    if(s_xfire->m_visualinfo == ((XVisualInfo *)0)) {
        (void)fprintf(stderr, "XGetVisualInfo failed !\n");
        return(-1);
    }

    for(s_xfire->m_select_visualinfo = 0;s_xfire->m_select_visualinfo < s_xfire->m_visualinfo_count;s_xfire->m_select_visualinfo++) {
        s_format = XRenderFindVisualFormat(s_xfire->m_display, s_xfire->m_visualinfo[s_xfire->m_select_visualinfo].visual);
        if(s_format == ((XRenderPictFormat *)0)) { continue; }
#if 1L /* ARGB (alpha supported visual) */
        if((s_format->type == PictTypeDirect) && (s_format->direct.alphaMask != 0)) { break; }
#else /* RGB */
        if(s_format->type == PictTypeDirect) { break; }
#endif
    }
    if(s_xfire->m_select_visualinfo >= s_xfire->m_visualinfo_count) {
        (void)fprintf(stderr, "not found visual !\n");
        return(-1);
    }

    return(0);
}

int main(int s_argc, char **s_argv)
{
    xfire_t s_xfire_local, *s_xfire;
    int s_window_index;
    int s_is_break, s_tick;

    (void)fprintf(stdout, "Initiailizing...\n");

    s_xfire = (xfire_t *)memset((void *)(&s_xfire_local), 0, sizeof(s_xfire_local));

    if(s_argc >= 2) { s_xfire->m_display_name = (const char *)s_argv[1]; }
    else {
        s_xfire->m_display_name = (const char *)getenv("DISPLAY");
        if(s_xfire->m_display_name == ((const char *)0)) {
            static const char cg_default_display_name[] = {":0"};
            s_xfire->m_display_name = (const char *)(&cg_default_display_name[0]);
        }
    }
    (void)fprintf(stdout, "display name is \"%s\"\n", s_xfire->m_display_name);

    s_xfire->m_display = XOpenDisplay(s_xfire->m_display_name);
    if(s_xfire->m_display == ((Display *)0)) { (void)fprintf(stderr, "XOpenDisplay failed !\n"); goto l_end; }

    s_xfire->m_screen = XDefaultScreenOfDisplay(s_xfire->m_display);
    if(s_xfire->m_screen == ((Screen *)0)) { (void)fprintf(stderr, "XDefaultScreenOfDisplay failed !\n"); goto l_close_display; }

    if(xfire_select_visual(s_xfire) == (-1)) { (void)fprintf(stderr, "xfire_select_visual failed !\n"); goto l_close_display; }

    s_xfire->m_event_mask = NoEventMask | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | SubstructureNotifyMask;

    s_xfire->m_colormap = XCreateColormap(s_xfire->m_display, XDefaultRootWindow(s_xfire->m_display), s_xfire->m_visualinfo[s_xfire->m_select_visualinfo].visual, AllocNone);
    for(s_window_index = 0;s_window_index < xfire_max_window;s_window_index++) {
        s_xfire->m_window_attributes[s_window_index].background_pixmap = None;
        s_xfire->m_window_attributes[s_window_index].background_pixel = 0xe0000000 | 0x00000000;
        s_xfire->m_window_attributes[s_window_index].border_pixmap = CopyFromParent;
        s_xfire->m_window_attributes[s_window_index].border_pixel = 0xff000000 | 0x00ffffff;
        s_xfire->m_window_attributes[s_window_index].bit_gravity = ForgetGravity;
        s_xfire->m_window_attributes[s_window_index].win_gravity = NorthWestGravity;
        s_xfire->m_window_attributes[s_window_index].backing_store = NotUseful;
        s_xfire->m_window_attributes[s_window_index].backing_planes = 1ul;
        s_xfire->m_window_attributes[s_window_index].backing_pixel = 0xff000000 | 0x00000000;
        s_xfire->m_window_attributes[s_window_index].save_under = False;
        s_xfire->m_window_attributes[s_window_index].event_mask = s_xfire->m_event_mask;
        s_xfire->m_window_attributes[s_window_index].do_not_propagate_mask = NoEventMask;
        s_xfire->m_window_attributes[s_window_index].override_redirect = True;
        s_xfire->m_window_attributes[s_window_index].colormap = s_xfire->m_colormap;
        s_xfire->m_window_attributes[s_window_index].cursor = (Cursor)None;
        s_xfire->m_window[s_window_index] = (Window)None;
        s_xfire->m_gc[s_window_index] = (GC)None;
        s_xfire->m_ximage[s_window_index] = (XImage *)0;

        s_xfire->m_window[s_window_index] = XCreateWindow(
            s_xfire->m_display,
            XDefaultRootWindow(s_xfire->m_display),
            0 + (s_window_index * 100), /* x */
            0 + (s_window_index * 100), /* y */
            ((unsigned int)XWidthOfScreen(s_xfire->m_screen)) - ((xfire_max_window - 1) * 100), /* w */
            ((unsigned int)XHeightOfScreen(s_xfire->m_screen)) - ((xfire_max_window - 1) * 100), /* h */
            1, /* border_width */
            s_xfire->m_visualinfo[s_xfire->m_select_visualinfo].depth,
            InputOutput, /* class: InputOutput or InputOnly */
            s_xfire->m_visualinfo[s_xfire->m_select_visualinfo].visual,
            CWBackPixel | CWBorderPixel | CWBitGravity | CWWinGravity | CWBackingStore | CWSaveUnder | CWEventMask | CWDontPropagate | CWOverrideRedirect | CWColormap | CWCursor,
            (XSetWindowAttributes *)(&s_xfire->m_window_attributes[s_window_index])
        );
        (void)XStoreName(s_xfire->m_display, s_xfire->m_window[s_window_index], "fire");
        if(s_xfire->m_window[s_window_index] == ((Window)None)) { continue; }

        (void)XMapWindow(s_xfire->m_display, s_xfire->m_window[s_window_index]);

        (void)XGetGeometry(
            s_xfire->m_display,
            s_xfire->m_window[s_window_index],
            (Window *)(&s_xfire->m_geometry[s_window_index].m_root_window),
            (int *)(&s_xfire->m_geometry[s_window_index].m_x),
            (int *)(&s_xfire->m_geometry[s_window_index].m_y),
            (unsigned int *)(&s_xfire->m_geometry[s_window_index].m_width),
            (unsigned int *)(&s_xfire->m_geometry[s_window_index].m_height),
            (unsigned int *)(&s_xfire->m_geometry[s_window_index].m_border_width),
            (unsigned int *)(&s_xfire->m_geometry[s_window_index].m_depth)
        );

        s_xfire->m_ximage[s_window_index] = XGetImage(
            s_xfire->m_display,
            s_xfire->m_window[s_window_index],
            0, 0,
            s_xfire->m_geometry[s_window_index].m_width,
            s_xfire->m_geometry[s_window_index].m_height,
            XAllPlanes(),
            ZPixmap);
        if(s_xfire->m_ximage[s_window_index] == ((XImage *)0)) { (void)fprintf(stderr, "XGetImage failed ! (window_index is %d)\n", s_window_index); }

        s_xfire->m_gc[s_window_index] = XCreateGC(s_xfire->m_display, s_xfire->m_window[s_window_index], 0ul, (XGCValues *)0);

#if def_xfire_draw_method == (1)
        s_xfire->m_fire_map[s_window_index] = (unsigned char *)malloc(((size_t)s_xfire->m_ximage[s_window_index]->width) * ((size_t)s_xfire->m_ximage[s_window_index]->height) * sizeof(unsigned char));
        if(s_xfire->m_fire_map[s_window_index] != ((unsigned char *)0)) {
            unsigned int s_color_index;
            unsigned int s_level, s_max_level, s_min_level;

            (void)memset((void *)s_xfire->m_fire_map[s_window_index], 0, ((size_t)s_xfire->m_ximage[s_window_index]->width) * ((size_t)s_xfire->m_ximage[s_window_index]->height) * sizeof(unsigned char));

            s_min_level = 0x00u;
            s_max_level = 0xffu;
            for(s_color_index = 0u;s_color_index < 64u;s_color_index++) {
                s_level = ((s_color_index + 1u) << 2) - 1u;
                s_xfire->m_fire_color_table[s_window_index][s_color_index + 0] = (s_level << 16) | (s_min_level << 8) | (s_min_level << 0);
                s_xfire->m_fire_color_table[s_window_index][s_color_index + 64] = (s_max_level << 16) | (s_level << 8) | (s_min_level << 0);
                s_xfire->m_fire_color_table[s_window_index][s_color_index + 128] = (s_max_level << 16) | (s_max_level << 8) | (s_level << 0);
                s_xfire->m_fire_color_table[s_window_index][s_color_index + 192] = (s_max_level << 16) | (s_max_level << 8) | (s_max_level << 0);
            }
        }
#endif
    }
    (void)XSync(s_xfire->m_display, False);

    for(s_is_break = 0, s_tick = 0;s_is_break == 0;s_tick++) {
        while(XPending(s_xfire->m_display) > 0) {
            if(XCheckMaskEvent(s_xfire->m_display, s_xfire->m_event_mask, (XEvent *)(&s_xfire->m_event)) == True) {
                if(s_xfire->m_event.type == Expose) {
                }
                else if(s_xfire->m_event.type == KeyPress) {
                    (void)fprintf(stdout, "KeyPress event. (%04XH)\n", (unsigned int)s_xfire->m_event.xkey.keycode);
                }
                else if(s_xfire->m_event.type == KeyRelease) {
                    (void)fprintf(stdout, "KeyRelease event. (%04XH)\n", (unsigned int)s_xfire->m_event.xkey.keycode);
                }
                else if(s_xfire->m_event.type == ButtonPress) {
                    (void)fprintf(stdout, "ButtonPress event.\n");
                }
                else if(s_xfire->m_event.type == ButtonRelease) {
                    (void)fprintf(stdout, "ButtonRelease event.\n");
                    s_is_break = 1;
                }
                else if(s_xfire->m_event.type == EnterNotify) {
                    (void)fprintf(stdout, "EnterNotify event.\n");
                }
                else if(s_xfire->m_event.type == LeaveNotify) {
                    (void)fprintf(stdout, "LeaveNotify event.\n");
                }
                else {
                    (void)fprintf(stdout, "Ignore event. (%d)\n", (int)s_xfire->m_event.type);
                }
            }
        }

#if def_xfire_draw_method == (1)
        for(s_window_index = 0;s_window_index < xfire_max_window;s_window_index++) {
            size_t s_entry1, s_entry2;
            int s_rand_count;
            unsigned int *s_map;

            s_entry1 = ((size_t)s_xfire->m_ximage[s_window_index]->width) * (((size_t)s_xfire->m_ximage[s_window_index]->height) - ((size_t)1u));
            for(s_rand_count = 0;s_rand_count < ((int)(s_xfire->m_ximage[s_window_index]->width >> 3));s_rand_count++) {
                s_xfire->m_fire_map[s_window_index][s_entry1 + (((size_t)rand()) % ((size_t)s_xfire->m_ximage[s_window_index]->width))] = (unsigned char)((rand() & 0xbf) + 0x40);
            }

            s_entry1 -= (size_t)2u;
            s_entry2 = s_entry1 + (((size_t)s_xfire->m_ximage[s_window_index]->width) - ((size_t)1u));
            s_map = (unsigned int *)s_xfire->m_ximage[s_window_index]->data;
            do {
                s_xfire->m_fire_map[s_window_index][s_entry1] = (unsigned char)((
                    ((unsigned int)s_xfire->m_fire_map[s_window_index][s_entry1]) +
                    ((unsigned int)s_xfire->m_fire_map[s_window_index][s_entry2 + 0]) +
                    ((unsigned int)s_xfire->m_fire_map[s_window_index][s_entry2 + 1]) +
                    ((unsigned int)s_xfire->m_fire_map[s_window_index][s_entry2 + 2])) >> 2);

                if(s_xfire->m_fire_map[s_window_index][s_entry1] > ((unsigned char)0u)) { --s_xfire->m_fire_map[s_window_index][s_entry1]; }

                s_map[s_entry1] = s_xfire->m_fire_color_table[s_window_index][s_xfire->m_fire_map[s_window_index][s_entry1]] | 0x7f000000;

                --s_entry1;
                --s_entry2;
            }while(s_entry1 > ((size_t)0u));

            (void)XPutImage(
                s_xfire->m_display,
                s_xfire->m_window[s_window_index],
                s_xfire->m_gc[s_window_index],
                s_xfire->m_ximage[s_window_index],
                0, 0,
                0, 0,
                (unsigned int)s_xfire->m_ximage[s_window_index]->width,
                (unsigned int)s_xfire->m_ximage[s_window_index]->height
            );
        }
        (void)XSync(s_xfire->m_display, False);
#else
        usleep(10000);
#endif
    }

    for(s_window_index = 0;s_window_index < xfire_max_window;s_window_index++) {
#if def_xfire_draw_method == (1)
        if(s_xfire->m_fire_map[s_window_index] != ((unsigned char *)0)) {
            free((void *)s_xfire->m_fire_map[s_window_index]);
        }
#endif

        if(s_xfire->m_ximage[s_window_index] != ((XImage *)0)) {
            (void)XDestroyImage(s_xfire->m_ximage[s_window_index]);
        }

        if(s_xfire->m_gc[s_window_index] != ((GC)None)) {
            (void)XFreeGC(s_xfire->m_display, s_xfire->m_gc[s_window_index]);
        }

        if(s_xfire->m_window[s_window_index] != ((Window)None)) {
            (void)XDestroySubwindows(s_xfire->m_display, s_xfire->m_window[s_window_index]);
            (void)XDestroyWindow(s_xfire->m_display, s_xfire->m_window[s_window_index]);
        }
    }

    (void)XFreeColormap(s_xfire->m_display, s_xfire->m_colormap);

l_close_display:;
    (void)XFlush(s_xfire->m_display);
    (void)XCloseDisplay(s_xfire->m_display);

l_end:;
    (void)fprintf(stdout, "End.\n");

    return(0);
}

#endif

/* vim: set expandtab: */
/* End of source */
