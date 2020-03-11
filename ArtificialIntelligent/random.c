#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "include/random.h"


double getRandomValue(void)
{
    const double weight = 0.5;
    double retVal;
    retVal = (double)rand() / ((double)RAND_MAX + 1);
    retVal = 2.0 * (retVal - 0.5) * weight;
    if(retVal > 1)
    {
        retVal = (double)rand() / ((double)RAND_MAX + 1);
        retVal = 2.0 * (retVal - 0.5) * weight;
    }
    return retVal;
}

double getHeInit(int layerCount)
{
    double retVal;
    double temp = sqrt((double)6/layerCount);

    retVal = (temp * 2) * ((double)rand() / ((double)RAND_MAX + 1) - 0.5) - temp;

    return retVal;
}