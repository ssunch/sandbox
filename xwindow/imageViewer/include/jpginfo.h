#ifndef _JPGINFO_H_
#define _JPGINFO_H_
#include <stdio.h>
#include "jpeglib.h"
#include "util.h"
#include "userdef.h"


ErrorState openJPGFile(char *file);
Image *getJPGInfo(void);

#endif