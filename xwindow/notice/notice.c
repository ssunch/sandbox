#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


void main(int argc, char* argv[])
{
    Display *d;
    Window w, root;
	Font f;
	GC gc;
	XSetWindowAttributes xswa;
    FILE *fp;
    char text[256] = {0,};
    int textLine = 0;

    if(argc > 1)
    {
        fp = fopen(argv[1], "r");
    }
	xswa.override_redirect = True;
    d = XOpenDisplay(NULL);

    root = XDefaultRootWindow(d);
    w = XCreateSimpleWindow(d, root, 50, 50, 800, 800, 2, BlackPixel(d,0), WhitePixel(d,0));
	XChangeWindowAttributes ( d, w, CWOverrideRedirect, &xswa );

    XMapWindow(d, w);
	
	gc = XCreateGC( d, w, 0L, ( XGCValues * ) NULL );     /* [1] */
    f  = XLoadFont( d, "8x13bold" );                         /* [2] */
    XSetFont ( d, gc, f );                                /* [3] */
    memset(text, 0x20, sizeof(text));
    //XDrawString( d, w, gc, 100, 130, "Hello, Linuxers! Never Seen :)", 16 );
    while(NULL != fgets(text, sizeof(text), fp))
    {
        XDrawString( d, w, gc, 20, 20 + (textLine++ * 20), text, sizeof(text) );
        memset(text, 0x20, sizeof(text));
    }

    XFlush(d);

    getchar();

	XUnloadFont( d, f );
    XFreeGC( d, gc );
    XDestroyWindow( d, w );

    XCloseDisplay(d);
}
