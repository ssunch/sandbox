#ifndef _BMPINFO_H_
#define _BMPINFO_H_
#include "userdef.h"

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned char* pImg;

#pragma pack(push, 1) 

typedef struct _bitMapFileHeader
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
}BMPFileHeader, *pBMPFileHeader;

typedef struct _bitMapInfoHeader
{
    DWORD biSize;
    DWORD biWidth;
    DWORD biHeight;
    WORD biplanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    DWORD biXPelsPerMeter;
    DWORD biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BMPInfoHeader, *pBMPInfoHeader;

typedef struct _RGBTRI
{
    unsigned char b;
    unsigned char g;
    unsigned char r;
}RGBTRI, *pRGBTRI;

//#pragma pack(pop)

#define PIXEL_ALIGN  4

#define getPadding(w, bitcount) ((PIXEL_ALIGN - ((w * bitcount / 8) % PIXEL_ALIGN)) % PIXEL_ALIGN)
#define getPixelSize(bitcount) (bitcount / 8)

#define errorReturn(FILE_POINTER, Err) \
do{ \
printf("*error occured in %s:%d\n", __FILE__ , __LINE__); \
fclose(FILE_POINTER); \
return Err; \
} while(0);

ErrorState openBMPFile(char *path);
void closeBMP(void);
ErrorState writeBMPFile(char *path);
pBMPFileHeader getBMPFileHeader(void);
pBMPInfoHeader getBMPInfoHeader(void);
unsigned char *getBMPBuffer(void);
void _bmpProcess(int argc, char* argv[]);

#endif