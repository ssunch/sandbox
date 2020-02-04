#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "include/bmpinfo.h"

unsigned long GetPixelValue(unsigned char red, unsigned char green, unsigned char blue);

int main(int argc, char* argv[])
{
    Display *d;
    Window w, root;
	Font f;
	GC gc;
	XSetWindowAttributes xswa;
    FILE *fp;
    char text[256] = {0,};
    int textLine = 0;
    pBMPFileHeader pfile;
    pBMPInfoHeader pinfo;
    XGCValues gv;
    unsigned long Black, White; 

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

	xswa.override_redirect = True;
    d = XOpenDisplay(NULL);

    root = XDefaultRootWindow(d);
    w = XCreateSimpleWindow(d, root, 50, 50, pinfo->biWidth, pinfo->biHeight, 2, BlackPixel(d,0), WhitePixel(d,0));
	XChangeWindowAttributes ( d, w, CWOverrideRedirect, &xswa );

    
    Black = BlackPixel(d, 0); 
    White = WhitePixel(d, 0);


    XMapWindow(d, w);
	
	gc = XCreateGC( d, w, 0L, ( XGCValues * ) NULL );     /* [1] */
    f  = XLoadFont( d, "8x13bold" );                         /* [2] */
    XSetFont ( d, gc, f );                                /* [3] */
    memset(text, 0x20, sizeof(text));
    //XDrawString( d, w, gc, 100, 130, "Hello, Linuxers! Never Seen :)", 16 );
    
    {
        pImg pimage;
        int widthCnt = 0;
        int hightCnt = 0;
        int padding = 0;
		int WIDTH = 0;
		int pixelSize = 0;
		pimage = getBMPBuffer();
		padding = getPadding(pinfo->biWidth , pinfo->biBitCount); // 8: 1byte = 8bit, align with 4byte
		pixelSize = getPixelSize(pinfo->biBitCount);
		WIDTH = pinfo->biWidth * pixelSize + padding;
		for(hightCnt = pinfo->biHeight - 1; hightCnt >= 0 ; hightCnt--)
		{
			for(widthCnt = 0; widthCnt < pinfo->biWidth; widthCnt++)
			{
                pRGBTRI pRGB = (pRGBTRI)&pimage[(hightCnt * WIDTH) + (widthCnt * pixelSize)];
                
                gv.foreground = GetPixelValue(pRGB->r, pRGB->g, pRGB->b);
                gc = XCreateGC(d,w, GCForeground, &gv);
                XDrawPoint(d,w,gc,widthCnt, pinfo->biHeight - hightCnt);
  //              XDrawPoint(d, w, )
			}
		}
    }
    closeBMP();

    XFlush(d);

    getchar();

	XUnloadFont( d, f );
    XFreeGC( d, gc );
    XDestroyWindow( d, w );

    XCloseDisplay(d);

    return 0;
}


unsigned long GetPixelValue(unsigned char red, unsigned char green, unsigned char blue)
{
    unsigned long val = 0;
    val |= (unsigned long)(red << 16);
    val |= (unsigned long)(green << 8);
    val |= blue;

    return val;
}