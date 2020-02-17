#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/bmpinfo.h"
#include "include/jpginfo.h"
#include "include/util.h"

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

void refresh(Display* d, Window window);
unsigned long GetPixelValue(unsigned char red, unsigned char green, unsigned char blue);
void imageScaler(pImage dest, pImage src, double ratio);
void fitToWindowScale(pImage dest, pImage src);
void BMPbufferToRGB(pImage dest, char *filename);
void JPGbufferToRGB(pImage dest, char *filename);
void initMemory(void *p, size_t size);
void copyToWindowBuffer(pImage src);

static Image windowImageBuffer;
static Image originImage;
static Image scaledImg;

int main(int argc, char** argv) {

    pBMPFileHeader pfile;
    pBMPInfoHeader pinfo;
    
    IMAGETYPE type;

    if(argc < 2)
	{
		printf("Usege : $%s {BMP or JPEG file path}\n",argv[0]);
		return 0;
	}

    type = getImageType(getExtenstion(argv[1]));

    if(type == TYPEMAX)
    {
        printf("There is no such as image file\n");
        return 0;
    }

    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        return 1;
    }

    int screen = DefaultScreen(display);

    GC gc = DefaultGC(display, screen);

    Window parent_window = DefaultRootWindow(display);
    Screen* pScreen = ScreenOfDisplay(display, screen);
    printf("Screen Width:%d\tScreen Height:%d\n", pScreen->width, pScreen->height);
    
    

    int x = 0;
    int y = 0;

    unsigned int width = pScreen->width / 3 + 1 + (((pScreen->width / 3 + 1) % 2)?1:0);
    unsigned int height = pScreen->height / 3 + 1 + (((pScreen->height / 3 + 1) % 2)?1:0);

    unsigned int border_width = 1;

    unsigned int border_color = BlackPixel(display, screen);
    unsigned int background_color = WhitePixel(display, screen);

    windowImageBuffer.image = (pRGB)malloc(sizeof(RGB) * width * height);
    windowImageBuffer.width = width;
    windowImageBuffer.height = height;

    // Create window
    Window window = XCreateSimpleWindow(display, parent_window,
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
    // XChangeWindowAttributes ( display, window, CWOverrideRedirect, &xswa );

    // Select window events
    XSelectInput(display, window, event_mask);

    // Make window visible
    XMapWindow(display, window);

    // Set window title
    char title[FILENAME_MAX];
    sprintf(title, "Image Viewer[%s]", argv[1]);
    XStoreName(display, window, title);

    // Get WM_DELETE_WINDOW atom
    Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", True);

    // Subscribe WM_DELETE_WINDOW message
    XSetWMProtocols(display, window, &wm_delete, 1);

    initMemory((void*)&scaledImg, sizeof(scaledImg));
    initMemory((void*)&originImage, sizeof(originImage));

    scaledImg.image = NULL;
    originImage.image = NULL;
    // To do : call BMPbufferToRGB
    switch (type)
    {
    case TYPEBMP:
        BMPbufferToRGB(&originImage, argv[1]);
        
        break;
    case TYPEJPEG:
        JPGbufferToRGB(&originImage, argv[1]);
        break;    
    default:
        break;
    }
    

    fitToWindowScale(&scaledImg, &originImage);
    copyToWindowBuffer(&scaledImg);

    char msg[1024] = "";
    char key[32];

    // Event loop
    for (;;) {
        // Get next event from queue
        XEvent event;
        XEvent preEvent;
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
            XClearWindow(display, window);
            //XDrawString(display, window, gc, 10, 20, msg, strlen(msg));
            if(event.type == Expose)
            {
                Window retWin;
                int ret_x;
                int ret_y;
                int retWidth;
                int retHight;
                int retBorderWidth;
                int retDepth;
                XGetGeometry(display, window, &retWin, &ret_x, &ret_y, &retWidth, &retHight, &retBorderWidth, &retDepth);

                if((retWidth != windowImageBuffer.width) || (retHight != windowImageBuffer.height))
                {
                    windowImageBuffer.width = retWidth;
                    windowImageBuffer.height = retHight;
                }
                //fitToWindowScale(&scaledImg, &originImage);    
            }
            
            //copyToWindowBuffer(&scaledImg);
            //refresh(display, window);
        }

        if(event.type == FocusIn)
        {
            if(preEvent.type == Expose)
            {
                fitToWindowScale(&scaledImg, &originImage);    
                copyToWindowBuffer(&scaledImg);
                refresh(display, window);
            }
        }

        // Close button
        if (event.type == ClientMessage) {
            if (event.xclient.data.l[0] == wm_delete) {
                break;
            }
        }

        preEvent = event;
    }

    
    if(windowImageBuffer.image != NULL)
        free(windowImageBuffer.image);
    if(originImage.image != NULL)
        free(originImage.image);
    if(scaledImg.image != NULL)
        free(scaledImg.image);

    XCloseDisplay(display);
    return 0;
}

void refresh(Display* display, Window window)
{
    int widthCnt = 0;
    int heightCnt = 0;
    XImage *image;
    int screen = DefaultScreen(display);
    GC gc = DefaultGC(display, screen);

    //XGetGeometry(display, window, &retWin, &ret_x, &ret_y, &retWidth, &retHight, &retBorderWidth, &retDepth);
    
    image = XGetImage(display, window, 0, 0 , windowImageBuffer.width, windowImageBuffer.height, AllPlanes, ZPixmap);
    
    for(heightCnt = 0; heightCnt < windowImageBuffer.height; heightCnt++)
    {
        for(widthCnt = 0; widthCnt < windowImageBuffer.width; widthCnt++)
        {
            pRGB p = (pRGB)&windowImageBuffer.image[heightCnt * windowImageBuffer.width + widthCnt];
            unsigned long pixel = GetPixelValue(p->r, p->g, p->b);
            XPutPixel(image, widthCnt, heightCnt, pixel); 
        }
    }
    XPutImage(display, window, gc, image, 0, 0, 0, 0, windowImageBuffer.width, windowImageBuffer.height);
    
}

unsigned long GetPixelValue(unsigned char red, unsigned char green, unsigned char blue)
{
    unsigned long val = 0;
    val |= (unsigned long)(red << 16);
    val |= (unsigned long)(green << 8);
    val |= blue;

    return val;
}

void imageScaler(pImage dest, pImage src, double ratio)
{
    int i;
    int j;
    int newH;
    int newW;

    dest->height = CEIL(ratio * src->height);
    dest->width = CEIL(ratio * src->width);

    if(dest->image != NULL)
    {
        printf("[%s]%s:%d - free(dest->image = 0x%x)\n",__FILE__, __FUNCTION__, __LINE__, dest->image );
        free(dest->image);
        dest->image = NULL;
    }

    dest->image = (pRGB)malloc(sizeof(RGB) * (dest->height + 1) * (dest->width + 1));

    for(i = 0; i < src->height; i++)
    {
        newH = CEIL(ratio * i);
        for(j = 0; j < src->width; j++)
        {
            newW = CEIL(ratio * j);
            dest->image[(newH * dest->width) + newW].r = src->image[(i * src->width) + j].r;
            dest->image[(newH * dest->width) + newW].g = src->image[(i * src->width) + j].g;
            dest->image[(newH * dest->width) + newW].b = src->image[(i * src->width) + j].b;
        }
    }

    if(ratio > 1)
    {
        // To do : add the interpolation
    }
}

void fitToWindowScale(pImage dest, pImage src)
{
    double ratio = 1.0f;

    if((windowImageBuffer.height * windowImageBuffer.width) < (src->width * src->height))
    {
        if(src->width > windowImageBuffer.width)
        {
            ratio = (double)windowImageBuffer.width / src->width;
        }
        
        if((ratio * src->width) > windowImageBuffer.width)
        {
            ratio = (double)(windowImageBuffer.width - 1) / src->width;
        }

        if((int)(ratio * src->height) > windowImageBuffer.height)
        {
            ratio = (double)windowImageBuffer.height / src->height;
        }

        if((ratio * src->height) > windowImageBuffer.height)
        {
            ratio = (double)(windowImageBuffer.height - 1) / src->height;
        }
    }

    imageScaler(dest, src, ratio);

}

void copyToWindowBuffer(pImage src)
{
    int i;
    int j;
    int widthMargin = (windowImageBuffer.width - src->width) / 2;
    int heightMargin = (windowImageBuffer.height - src->height) / 2;
    static int imgHeight = 0;
    static int imgWidth = 0;

    if((src->width != imgWidth) && (src->height != imgHeight))
    {
        if(windowImageBuffer.image != NULL)
        {
            printf("[%s]%s:%d - free(windowImageBuffer.image = 0x%x)\n",__FILE__, __FUNCTION__, __LINE__, windowImageBuffer.image );
            free(windowImageBuffer.image);
            windowImageBuffer.image = NULL;
            windowImageBuffer.image = (pRGB)malloc(sizeof(RGB) * (windowImageBuffer.height + ((windowImageBuffer.height % 2)?1:0))* (windowImageBuffer.width + ((windowImageBuffer.width % 2)?1:0)));
        }
        initMemory((void*)windowImageBuffer.image, sizeof(RGB) * (windowImageBuffer.height + ((windowImageBuffer.height % 2)?1:0))* (windowImageBuffer.width + ((windowImageBuffer.width % 2)?1:0)));

        for(i = 0; i < src->height; i++)
        {
            for(j = 0; j < src->width; j++)
            {
                windowImageBuffer.image[((i + heightMargin) * windowImageBuffer.width) + j + widthMargin].r = src->image[(i * src->width) + j].r;
                windowImageBuffer.image[((i + heightMargin) * windowImageBuffer.width) + j + widthMargin].g = src->image[(i * src->width) + j].g;
                windowImageBuffer.image[((i + heightMargin) * windowImageBuffer.width) + j + widthMargin].b = src->image[(i * src->width) + j].b;
            }
        }

        imgHeight = src->height;
        imgWidth = src->width;
    }
}

void BMPbufferToRGB(pImage dest, char *filename)
{
    pImg pimage;
    pBMPInfoHeader pinfo;
    int i;
    int j;
    int padding = 0;
    int WIDTH = 0;
    int pixelSize = 0;

    ErrorState ret;

    ret = openBMPFile(filename);

    if(ret != ErrorNone)
    {
        printf("BMP file open error[%d]\n", (int)ret);
        return 0;
    }

    closeBMP();

    pinfo = getBMPInfoHeader();

    pimage = getBMPBuffer();

    if(dest->image != NULL)
    {
        printf("[%s]%s:%d - free(dest->image = 0x%x)\n",__FILE__, __FUNCTION__, __LINE__, dest->image );
        free(dest->image);
        dest->image = NULL;
    }

    dest->image = (pRGB)malloc(sizeof(RGB) * pinfo->biHeight * pinfo->biWidth);
    dest->height = pinfo->biHeight;
    dest->width = pinfo->biWidth;
    dest->bitCount = pinfo->biBitCount;

    padding = getPadding(pinfo->biWidth , pinfo->biBitCount); // 8: 1byte = 8bit, align with 4byte
    pixelSize = getPixelSize(pinfo->biBitCount);
    WIDTH = pinfo->biWidth * pixelSize + padding;
    
    for(i = pinfo->biHeight - 1; i >= 0 ; i--)
    {
        for(j = 0; j < pinfo->biWidth; j++)
        {
            pRGBTRI pRGB = (pRGBTRI)&pimage[(i * WIDTH) + (j * pixelSize)];
            dest->image[((pinfo->biHeight - i - 1) * pinfo->biWidth) + j].r = pRGB->r;
            dest->image[((pinfo->biHeight - i - 1) * pinfo->biWidth) + j].g = pRGB->g;
            dest->image[((pinfo->biHeight - i - 1) * pinfo->biWidth) + j].b = pRGB->b;
        }
    }
}

void JPGbufferToRGB(pImage dest, char *filename)
{
    pImg pimage;
    pImage imgInfo;
    int i;
    int j;
    int padding = 0;
    int WIDTH = 0;
    int pixelSize = 0;
    ErrorState ret;

    ret = openJPGFile(filename);

    if(ret != ErrorNone)
    {
        printf("JPG file open error[%d]\n", (int)ret);
        return 0;
    }

    imgInfo = getJPGInfo();

    if(dest->image != NULL)
    {
        printf("[%s]%s:%d - free(dest->image = 0x%x)\n",__FILE__, __FUNCTION__, __LINE__, dest->image );
        free(dest->image);
        dest->image = NULL;
    }

    dest->image = (pRGB)malloc(sizeof(RGB) * imgInfo->width * imgInfo->height);
    dest->height = imgInfo->height;
    dest->width = imgInfo->width;
    dest->bitCount = imgInfo->bitCount;

    memcpy((void*)dest->image, (void*)imgInfo->image, sizeof(RGB) * imgInfo->width * imgInfo->height);
}

void initMemory(void *p, size_t size)
{
    char *_p = (char*)p;
    while(size--)
    {
        _p = 0;
        _p++;
    }
}