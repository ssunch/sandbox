#include <stdio.h>
#include <string.h>
#include "include/util.h"

const char TYPENAME[TYPEMAX][4] = {
    "BMP\n",
    "JPG\n"
};

IMAGETYPE getImageType(char *ext)
{
    IMAGETYPE type = TYPEMAX;

    if ((strcmp(ext, "jpg") == 0) ||
		(strcmp(ext, "JPG") == 0))
    {
        type = TYPEJPEG;
    }
    else if ((strcmp(ext, "bmp") == 0) ||
		(strcmp(ext, "BMP") == 0))
    {
        type = TYPEBMP;
    }

    return type;
}

char *getExtenstion(char *filename)
{
    char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) {
		return "";
	}
	return dot + 1;
}