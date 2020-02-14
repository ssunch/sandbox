#ifndef _UTIL_H_
#define _UTIL_H_


typedef enum
{
    TYPEBMP,
    TYPEJPEG,
    TYPEMAX
} IMAGETYPE;

#define CEIL(x) ((int)(x) + ((x <= 0)? 0:1))

typedef struct  _RGB
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
}RGB, *pRGB;

typedef struct _Image
{
    pRGB image;
    int width;
    int height;
    int bitCount;
} Image, *pImage;

#define getTYPENAME(type) TYPENAME[type]

IMAGETYPE getImageType(char *ext);
char *getExtenstion(char *filename);


#endif