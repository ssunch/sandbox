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
void visual(Display* display, Window window, Image Buffer);
unsigned long GetPixelValue(unsigned char red, unsigned char green, unsigned char blue);
void imageScaler(pImage dest, pImage src, double ratio);
ErrorState fitToWindowScale(pImage dest, pImage src);
ErrorState fixedfitToWindowScale(pImage dest, pImage src);
void BMPbufferToRGB(pImage dest, char *filename);
void JPGbufferToRGB(pImage dest, char *filename);
void initMemory(void *p, size_t size);
void copyToWindowBuffer(pImage src);

static double fixed_ratio;
static Image windowImageBuffer;
static Image windowImageBuffer_2;
static Image originImage;
static Image scaledImg;
static Image fixedscaledImg;
boolean FIXED;

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

    Display* display = XOpenDisplay("");
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

    windowImageBuffer.image = (pRGB)malloc(sizeof(RGB) * pScreen->width * pScreen->height);
    windowImageBuffer.width = width;
    windowImageBuffer.height = height;
    fixed_ratio = 2.0f;
    printf("%f\n", fixed_ratio);
    memcpy(&windowImageBuffer_2, &windowImageBuffer, sizeof(Image));
    // windowImageBuffer_2.image = (pRGB)malloc(sizeof(RGB) * pScreen->width * pScreen->height* fixed_ratio * fixed_ratio) ;
    windowImageBuffer_2.width = width*fixed_ratio;
    windowImageBuffer_2.height = height*fixed_ratio;
    printf("%d, %d\n", windowImageBuffer.height, windowImageBuffer.width);
    
    printf("%d, %d", windowImageBuffer_2.height, windowImageBuffer_2.width);

    // Create window
    Window window = XCreateSimpleWindow(display, parent_window,
                                              x,
                                              y,
                                              width,
                                              height,
                                              border_width,
                                              border_color,
                                              background_color);

    Window window_1 = XCreateSimpleWindow(display, parent_window,
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
                   // | ResizeRedirectMask
                    ;
    // XSetWindowAttributes xswa;
    // xswa.override_redirect = True;
    // XChangeWindowAttributes ( display, window, CWOverrideRedirect, &xswa );


    // Select window events
    XSelectInput(display, window, event_mask);
    XSelectInput(display, window_1, event_mask);

    // Make window visible
    XMapWindow(display, window);
    XMapWindow(display, window_1);

    // Set window title
    char title[FILENAME_MAX];
    sprintf(title, "Image Viewer[%s]", argv[1]);
    XStoreName(display, window, title);
    XStoreName(display, window_1, title);

    // Get WM_DELETE_WINDOW atom
    Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", True);

    // Subscribe WM_DELETE_WINDOW message
    XSetWMProtocols(display, window, &wm_delete, 1);
    XSetWMProtocols(display, window_1, &wm_delete, 1);

    memset((void*)&scaledImg, 0, sizeof(scaledImg));
    memset((void*)&fixedscaledImg, 0, sizeof(fixedscaledImg));
    memset((void*)&originImage, 0, sizeof(originImage));

    scaledImg.image = NULL;
    fixedscaledImg.image = NULL;
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
        // fixedfitToWindowScale(&fixedscaledImg, &originImage);
        

        // copyToWindowBuffer(&fixedscaledImg);

        // visual(display, window_1, windowImageBuffer_2);

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
            //XClearWindow(display, window);
            if(event.type == Expose)
            {
                Window retWin;
                int ret_x;
                int ret_y;
                int retWidth;
                int retHeight;
                int retBorderWidth;
                int retDepth;
                XGetGeometry(display, window, &retWin, &ret_x, &ret_y, &retWidth, &retHeight, &retBorderWidth, &retDepth);

                if((retWidth != windowImageBuffer.width) || (retHeight != windowImageBuffer.height))
                {
                    windowImageBuffer.width = retWidth;
                    windowImageBuffer.height = retHeight;
                }
                printf("%dx%d\n", windowImageBuffer.width, windowImageBuffer.height);
            }
        }

        if(event.type == ResizeRequest)
        {
            if((event.xresizerequest.width != windowImageBuffer.width) || (event.xresizerequest.height != windowImageBuffer.height))
            {
                windowImageBuffer.width = event.xresizerequest.width;
                windowImageBuffer.height = event.xresizerequest.height;
            }
            printf("%dx%d\n",event.xresizerequest.height, event.xresizerequest.width);

        }

        if(event.type == FocusIn)
        {
            if((preEvent.type == ResizeRequest) || (preEvent.type == Expose))
            {
                //XResizeWindow(display, window, windowImageBuffer.width, windowImageBuffer.height);
                if(fitToWindowScale(&scaledImg, &originImage) == ErrorNone)
                {
                    copyToWindowBuffer(&scaledImg);
                    refresh(display, window);
                }
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
    if(windowImageBuffer_2.image != NULL)
        free(windowImageBuffer_2.image);
    if(originImage.image != NULL)
        free(originImage.image);
    if(scaledImg.image != NULL)
        free(scaledImg.image);
    if(fixedscaledImg.image !=NULL)
        free(fixedscaledImg.image);

    XCloseDisplay(display);
    return 0;
}

void refresh(Display* display, Window window)
{
    XImage *image;
    int screen = DefaultScreen(display);
    GC gc = DefaultGC(display, screen);
    Visual *visual=DefaultVisual(display, 0);

    image = XCreateImage(display, visual, DefaultDepth(display,DefaultScreen(display)), ZPixmap, 0, (char *)windowImageBuffer.image, windowImageBuffer.width, windowImageBuffer.height, 32, 0);

    XPutImage(display, window, gc, image, 0, 0, 0, 0, windowImageBuffer.width, windowImageBuffer.height);
}

void visual(Display* display, Window window, Image Buffer)
{
    XImage *image;
    int screen = DefaultScreen(display);
    GC gc = DefaultGC(display, screen);
    Visual *visual=DefaultVisual(display, 0);

    image = XCreateImage(display, visual, DefaultDepth(display,DefaultScreen(display)), ZPixmap, 0, (char *)Buffer.image, Buffer.width, Buffer.height, 32, 0);

    XPutImage(display, window, gc, image, 0, 0, 0, 0, Buffer.width, Buffer.height);
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
        printf("begen [%s]%s:%d - free(dest->image = 0x%lx)\n",__FILE__, __FUNCTION__, __LINE__, (unsigned long)dest->image );
        free(dest->image);
        dest->image = NULL;
        printf("end [%s]%s:%d - free(dest->image = 0x%lx)\n",__FILE__, __FUNCTION__, __LINE__, (unsigned long)dest->image );
    }

    dest->image = (pRGB)malloc(sizeof(RGB) * (dest->height) * (dest->width));

    if(ratio < 1)
    {
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
    }

    if (ratio > 1)
    {
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
    }
}

ErrorState fitToWindowScale(pImage dest, pImage src)
{
    ErrorState ret = ErrorUnexpected;
    double ratio = 1.0f;

    if((windowImageBuffer.height * windowImageBuffer.width) < (src->width * src->height))
    {
        if(src->width > windowImageBuffer.width)
        {
            ratio = (double)windowImageBuffer.width / src->width;
        }
        
        if((unsigned int)(ratio * src->height) > windowImageBuffer.height)
        {
            ratio = (double)windowImageBuffer.height / src->height;
        }

        while(1)
        {
            if((unsigned int)(ratio * src->width) > windowImageBuffer.width)
            {
                ratio = (double)(windowImageBuffer.width - 1) / src->width;
            }

            if((unsigned int)(ratio * src->height) > windowImageBuffer.height)
            {
                ratio = (double)(windowImageBuffer.height - 1) / src->height;
            }

            if(((unsigned int)(ratio * src->width) < windowImageBuffer.width) &&
                ((unsigned int)(ratio * src->height) < windowImageBuffer.height))
            {
                break;
            }
            else
            {
                ratio = (double)(windowImageBuffer.height - 1) / src->height;
            }
            
        }
        printf("ratio: %f", ratio);

        imageScaler(dest, src, ratio);
        ret = ErrorNone;
    }

    return ret;

}

ErrorState fixedfitToWindowScale(pImage dest, pImage src)
{
    ErrorState ret = ErrorUnexpected;
    double ratio = 2.0f;

    if((windowImageBuffer.height * windowImageBuffer.width) < (src->width * src->height))
    {
        printf("ratio: %f", ratio);

        imageScaler(dest, src, ratio);
        ret = ErrorNone;
        FIXED = TRUE;
    }
    
    fixed_ratio = ratio;

    return ret;

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
        memset((void*)windowImageBuffer.image, 0, sizeof(RGB) * windowImageBuffer.height * windowImageBuffer.width);
        if(FIXED == TRUE){
            memset((void*)windowImageBuffer_2.image, 0, sizeof(RGB) *  windowImageBuffer_2.height *  windowImageBuffer_2.width);
            printf("%ld\n", sizeof(windowImageBuffer_2.image));
        };

        for(i = 0; i < src->height; i++)
        {
            for(j = 0; j < src->width; j++)
            {
                windowImageBuffer.image[((i + heightMargin) * windowImageBuffer.width) + j + widthMargin].r = src->image[(i * src->width) + j].r;
                windowImageBuffer.image[((i + heightMargin) * windowImageBuffer.width) + j + widthMargin].g = src->image[(i * src->width) + j].g;
                windowImageBuffer.image[((i + heightMargin) * windowImageBuffer.width) + j + widthMargin].b = src->image[(i * src->width) + j].b;
                if(FIXED == TRUE){

                    windowImageBuffer_2.image[((i + heightMargin) * windowImageBuffer_2.width) + j + widthMargin].r = src->image[(i * src->width) + j].r;
                    windowImageBuffer_2.image[((i + heightMargin) * windowImageBuffer_2.width) + j + widthMargin].g = src->image[(i * src->width) + j].g;
                    windowImageBuffer_2.image[((i + heightMargin) * windowImageBuffer_2.width) + j + widthMargin].b = src->image[(i * src->width) + j].b;
                    FIXED = FALSE;
                };

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
        return;
    }

    pinfo = getBMPInfoHeader();

    pimage = getBMPBuffer();

    if(dest->image != NULL)
    {
        printf("[%s]%s:%d - free(dest->image = 0x%lx)\n",__FILE__, __FUNCTION__, __LINE__, (unsigned long)dest->image );
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
    
    for(i = pinfo->biHeight-1 ; i >= 0 ; i--)
    {
        for(j = 0; j < pinfo->biWidth; j++)
        {
            pRGBTRI pRGB = (pRGBTRI)&pimage[(i* WIDTH) + (j * pixelSize)];
            dest->image[((pinfo->biHeight - i - 1) * pinfo->biWidth) + j].r = pRGB->r;
            dest->image[((pinfo->biHeight - i - 1) * pinfo->biWidth) + j].g = pRGB->g;
            dest->image[((pinfo->biHeight - i - 1) * pinfo->biWidth) + j].b = pRGB->b;
        }
    }

    closeBMP();
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
        return;
    }

    imgInfo = getJPGInfo();

    if(dest->image != NULL)
    {
        printf("[%s]%s:%d - free(dest->image = 0x%lx)\n",__FILE__, __FUNCTION__, __LINE__, (unsigned long)dest->image );
        free(dest->image);
        dest->image = NULL;
    }

    dest->image = (pRGB)malloc(sizeof(RGB) * imgInfo->width * imgInfo->height);
    dest->height = imgInfo->height;
    dest->width = imgInfo->width;
    dest->bitCount = imgInfo->bitCount;

    memcpy((void*)dest->image, (void*)imgInfo->image, sizeof(RGB) * imgInfo->width * imgInfo->height);

    free(imgInfo->image);
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