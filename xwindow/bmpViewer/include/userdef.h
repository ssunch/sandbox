#ifndef _USERDEF_H_
#define _USERDEF_H_

typedef enum
{
    ErrorNone,
    ErrorNotFound,
    ErrorUnexpected,
    ErrorRead,
    ErrorWrite,
    ErrorIO,
    ErrorOpen,
    ErrorClose,
    ErrorNotMatched,
    ErrorMAX
} ErrorState;


#endif
