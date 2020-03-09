#include <stdio.h>
#include <stdlib.h>

#include "include/random.h"


double getRandomValue(void)
{
    const double weight = 0.4;
    double retVal;
    retVal = (double)rand() / ((double)RAND_MAX + 10);
    retVal = 2.0 * (retVal - 0.5) * weight;
    return retVal;
}