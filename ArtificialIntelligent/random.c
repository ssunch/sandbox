#include <stdio.h>
#include <stdlib.h>

#include "include/random.h"


double getRandomValue(void)
{
    double retVal;
    retVal = (double)rand() / (RAND_MAX + 1);
    return retVal;
}