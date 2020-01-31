#ifndef _USERDEF_H_
#define _USERDEF_H_

typedef enum
{
    FALSE,
    TRUE
} BOOL;
#define DYNAMIC
#ifdef DYNAMIC

typedef struct _STRING_
{
    char *str;
    int length;
}STRING, *pSTRING;
#else
#define MAX_FILE_LINE 10000
typedef struct _STRING_
{
    char str[FILENAME_MAX];
    int length;
}STRING, *pSTRING;
#endif

#endif
