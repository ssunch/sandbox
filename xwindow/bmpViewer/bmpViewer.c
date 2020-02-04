#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <string.h>
#include <stdio.h>

#include "include/bmpinfo.h"

static const char *event_names[] = {
   "",
   "",
   "KeyPress",
   "KeyRelease",
   "ButtonPress",
   "ButtonRelease",
   "MotionNotify",
   "EnterNotify",
   "LeaveNotify",
   "FocusIn",
   "FocusOut",
   "KeymapNotify",
   "Expose",
   "GraphicsExpose",
   "NoExpose",
   "VisibilityNotify",
   "CreateNotify",
   "DestroyNotify",
   "UnmapNotify",
   "MapNotify",
   "MapRequest",
   "ReparentNotify",
   "ConfigureNotify",
   "ConfigureRequest",
   "GravityNotify",
   "ResizeRequest",
   "CirculateNotify",
   "CirculateRequest",
   "PropertyNotify",
   "SelectionClear",
   "SelectionRequest",
   "SelectionNotify",
   "ColormapNotify",
   "ClientMessage",
   "MappingNotify"
};

unsigned long GetPixelValue(unsigned char red, unsigned char green, unsigned char blue)
{
    unsigned long val = 0;
    val |= (unsigned long)(red << 16);
    val |= (unsigned long)(green << 8);
    val |= blue;

    return val;
}

int main(int argc, char** argv) {

    pBMPFileHeader pfile;
    pBMPInfoHeader pinfo;

    ErrorState ret;

    if(argc < 2)
	{
		printf("Usege : $%s {BMP file path}\n",argv[0]);
		return 0;
	}

    ret = openBMPFile(argv[1]);

    if(ret != ErrorNone)
    {
        printf("BMP file open error[%d]\n", (int)ret);
        return 0;
    }

    pfile = getBMPFileHeader();
    pinfo = getBMPInfoHeader();

    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        return 1;
    }

    int screen = DefaultScreen(display);

    GC gc = DefaultGC(display, screen);

    Window parent_window = DefaultRootWindow(display);

    int x = 0;
    int y = 0;

    unsigned int width = pinfo->biWidth + 1;
    unsigned int height = pinfo->biHeight + 1;

    unsigned int border_width = 1;

    unsigned int border_color = BlackPixel(display, screen);
    unsigned int background_color = WhitePixel(display, screen);

    // Create window
    Window hello_window = XCreateSimpleWindow(display, parent_window,
                                              x,
                                              y,
                                              width,
                                              height,
                                              border_width,
                                              border_color,
                                              background_color);

    long event_mask = ExposureMask
                    | KeyPressMask
                    | KeyReleaseMask
                    | ButtonPressMask
                    | ButtonReleaseMask
                    | FocusChangeMask
                    ;
    // XSetWindowAttributes xswa;
    // xswa.override_redirect = True;
    // XChangeWindowAttributes ( display, hello_window, CWOverrideRedirect, &xswa );

    // Select window events
    XSelectInput(display, hello_window, event_mask);

    // Make window visible
    XMapWindow(display, hello_window);

    // Set window title
    char title[FILENAME_MAX];
    sprintf(title, "BMP Viewer[%s]", argv[1]);
    XStoreName(display, hello_window, title);

    // Get WM_DELETE_WINDOW atom
    Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", True);

    // Subscribe WM_DELETE_WINDOW message
    XSetWMProtocols(display, hello_window, &wm_delete, 1);

    
    char msg[1024] = "";
    char key[32];

    // Event loop
    for (;;) {
        // Get next event from queue
        XEvent event;
        XNextEvent(display, &event);

        // Print event type
        printf("got event: %s\n", event_names[event.type]);

        // Keyboard
        if (event.type == KeyPress) {
            int len = XLookupString(&event.xkey, key, sizeof(key) - 1, 0, 0);
            key[len] = 0;

            if (strlen(msg) > 50)
                msg[0] = 0;

            strcat(msg, key);
            strcat(msg, " ");
        }

        //Refresh
        if (event.type == KeyPress || event.type == Expose) {
            XClearWindow(display, hello_window);
            //XDrawString(display, hello_window, gc, 10, 20, msg, strlen(msg));
            if(event.type == KeyPress)
            {
                pImg pimage;
                int widthCnt = 0;
                int hightCnt = 0;
                int padding = 0;
                int WIDTH = 0;
                int pixelSize = 0;
                //getchar();
                XImage *image = XGetImage(display, hello_window, 0, 0 , pinfo->biWidth, pinfo->biHeight, AllPlanes, ZPixmap);
                pimage = getBMPBuffer();
                padding = getPadding(pinfo->biWidth , pinfo->biBitCount); // 8: 1byte = 8bit, align with 4byte
                pixelSize = getPixelSize(pinfo->biBitCount);
                WIDTH = pinfo->biWidth * pixelSize + padding;
                for(hightCnt = pinfo->biHeight - 1; hightCnt >= 0 ; hightCnt--)
                {
                    for(widthCnt = 0; widthCnt < pinfo->biWidth; widthCnt++)
                    {
                        pRGBTRI pRGB = (pRGBTRI)&pimage[(hightCnt * WIDTH) + (widthCnt * pixelSize)];
                        unsigned long pixel = GetPixelValue(pRGB->r, pRGB->g, pRGB->b);
                        XPutPixel(image, widthCnt, pinfo->biHeight - hightCnt - 1, pixel); 
                    }
                }
                XPutImage(display, hello_window, gc, image, 0, 0, 0, 0, pinfo->biWidth, pinfo->biHeight);
            }

        }

        // Close button
        if (event.type == ClientMessage) {
            if (event.xclient.data.l[0] == wm_delete) {
                break;
            }
        }
    }

    closeBMP();

    XCloseDisplay(display);
    return 0;
}