#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "include/ann.h"
#include "include/random.h"

const char *data_file = "weight.dat";

static double normalize(unsigned char val);
static double getSigmoidal(double val);
static double getReLu(double val);
static void softmax(double *value, int count);

static double normalize(unsigned char val)
{
    const int MAX_UBYTE = 255;
    double retVal = 0.0f;
    
    if(val != 0)
    {
        retVal = (double)val / MAX_UBYTE;
        //retVal = 1.;
    }

    return retVal;
}

static double getSigmoidal(double val)
{
    double retVal = 0.f;
    
    retVal = 1.0/(1.0 + exp(-val));   // Sigmoidal

    return retVal;
}

static double getReLu(double val)
{
    double retVal = 0.f;
    const double alpha = 0.001;
    // Exponential Linear Unit(LEU)
    /*
    f(x) = x if x > 0
    f(x) = alpha(e^x - 1) if x <= 0
    */
    if(val > 0)
    {
        retVal = val;
    }
    else
    {
        retVal = alpha * (exp(val) -1);
    }
    

    return retVal;
}

static void softmax(double *value, int count)
{
    int i;
    double _max = -__INT_MAX__;
    double _sum = 0;

    for(i = 0; i < count; i++)
    {
        if(_max < value[i])
        {
            _max = value[i];
        }
    }
    for(i = 0; i < count; i++)
    {
        _sum += value[i] = exp(value[i] - _max);
    }
    for(i = 0; i < count; i++)
    {
        value[i] /= _sum;
    }
}

void loadWeight(Process *ann)
{
    FILE *pf;
    int i;

    pf = fopen(data_file, "rb");

    if(pf != NULL)
    {
        for(i = 0; i < ann->Layer; i++)
        {
            fread(ann->layerWeight[i].weight, sizeof(double), ann->layerWeight[i].endCount * ann->layerWeight[i].frontCount, pf);
        }

        fclose(pf);
    }
    else
    {
        printf("fail to open the file\n");
    }
}

void saveWeight(Process *ann)
{
    FILE *pf;
    int i;

    pf = fopen(data_file, "wb");

    if(pf != NULL)
    {
        for(i = 0; i < ann->Layer; i++)
        {
            fwrite(ann->layerWeight[i].weight, sizeof(double), ann->layerWeight[i].endCount * ann->layerWeight[i].frontCount, pf);
        }

        fclose(pf);
    }
    else
    {
        printf("fail to open the file\n");
    }
}

void perception(Process *ann)
{
    int frontIdx;
    int endIdx;
    int layerIdx;
    const int outputLayerIdx = ann->Layer - 1;
    double sum;

    for(layerIdx = 0; layerIdx < outputLayerIdx; layerIdx++)
    {
        for(endIdx = 0; endIdx < ann->layerWeight[layerIdx].endCount; endIdx++)
        {
            sum = 0.;
            for(frontIdx = 0; frontIdx < ann->layerWeight[layerIdx].frontCount; frontIdx++)
            {
                sum += ann->data[layerIdx].val[frontIdx] * ann->layerWeight[layerIdx].weight[endIdx][frontIdx];
            }
            ann->hidden[layerIdx].val[endIdx] = getSigmoidal(sum);
        }
    }

    for(endIdx = 0; endIdx < ann->layerWeight[outputLayerIdx].endCount; endIdx++)
    {
        sum = 0.;
        for(frontIdx = 0; frontIdx < ann->layerWeight[outputLayerIdx].frontCount; frontIdx++)
        {
            sum += ann->data[outputLayerIdx].val[frontIdx] * ann->layerWeight[outputLayerIdx].weight[endIdx][frontIdx];
        }
        ann->hidden[outputLayerIdx].val[endIdx] = sum;
    }

    softmax(ann->hidden[outputLayerIdx].val, ann->pLayerCount[ann->Layer]);
    
}

void updateWeight(Process *ann)
{
    int frontIdx;
    int endIdx;
    int layerIdx;
    const int outputLayerIdx = ann->Layer - 1;
    double expected;
    double sum;

    for(endIdx = 0; endIdx < ann->pLayerCount[ann->Layer]; endIdx++)
    {
        expected = (ann->expect == endIdx) ? 1. : 0.;
        ann->delta[outputLayerIdx].val[endIdx] = (expected - ann->hidden[outputLayerIdx].val[endIdx]) * ann->hidden[outputLayerIdx].val[endIdx] * (1.f - ann->hidden[outputLayerIdx].val[endIdx]); 
    }

    for(layerIdx = (ann->Layer -2); layerIdx >= 0; layerIdx--)
    {
        for(frontIdx = 0; frontIdx < ann->layerWeight[layerIdx+1].frontCount; frontIdx++)
        {
            sum = 0.;
            for(endIdx = 0; endIdx < ann->layerWeight[layerIdx +1].endCount; endIdx++)
            {
                sum += ann->delta[layerIdx + 1].val[endIdx] * ann->layerWeight[layerIdx +1].weight[endIdx][frontIdx];
            }
            ann->delta[layerIdx].val[frontIdx] = sum * ann->hidden[layerIdx].val[frontIdx] * (1.f - ann->hidden[layerIdx].val[frontIdx]);
        }
    }

    for(layerIdx = 0; layerIdx < ann->Layer; layerIdx++)
    {
        for(endIdx = 0; endIdx < ann->layerWeight[layerIdx].endCount; endIdx++)
        {
            for(frontIdx = 0; frontIdx < ann->layerWeight[layerIdx].frontCount; frontIdx++)
            {
                ann->layerDeltaWeight[layerIdx].weight[endIdx][frontIdx] = ann->data[layerIdx].val[frontIdx] * ann->delta[layerIdx].val[endIdx] * ann->learningRate + ann->layerDeltaWeight[layerIdx].weight[endIdx][frontIdx] * ann->momentum;
                ann->layerWeight[layerIdx].weight[endIdx][frontIdx] += ann->layerDeltaWeight[layerIdx].weight[endIdx][frontIdx];
            }
        }
    }

}

void processInit(Process *ann, int layerCount, ...)
{
    va_list ap;
    int frontIdx;
    int endIdx;
    int layerIdx;

    ann->momentum = 0.9;
    ann->learningRate = 0.001;

    ann->Layer = layerCount - 1;
    ann->pLayerCount = (int*)malloc(sizeof(int) * layerCount);

    va_start(ap, layerCount);
    for (frontIdx = 0; frontIdx < layerCount; frontIdx++)
    {
        ann->pLayerCount[frontIdx] = va_arg(ap, int);

    }
    va_end(ap);

    ann->delta = (Node*)malloc(sizeof(Node) * ann->Layer);
    ann->hidden = (Node*)malloc(sizeof(Node) * ann->Layer);
    ann->data = (Node*)malloc(sizeof(Node) * ann->Layer);

    ann->layerWeight = (Weight*)malloc(sizeof(Weight) * ann->Layer);
    ann->layerDeltaWeight = (Weight*)malloc(sizeof(Weight) * ann->Layer);

    ann->data[0].val = (double*)malloc(sizeof(double) * ann->pLayerCount[0]);

    for(layerIdx = 0; layerIdx < ann->Layer; layerIdx++)
    {
        ann->layerWeight[layerIdx].frontCount = ann->pLayerCount[layerIdx];
        ann->layerWeight[layerIdx].endCount = ann->pLayerCount[layerIdx + 1];

        ann->layerDeltaWeight[layerIdx].frontCount = ann->pLayerCount[layerIdx];
        ann->layerDeltaWeight[layerIdx].endCount = ann->pLayerCount[layerIdx + 1];

        ann->layerWeight[layerIdx].weight = (double**)malloc(sizeof(double*) * ann->layerWeight[layerIdx].endCount);
        ann->layerDeltaWeight[layerIdx].weight = (double**)malloc(sizeof(double*) * ann->layerDeltaWeight[layerIdx].endCount);

        ann->delta[layerIdx].val = (double*)malloc(sizeof(double) * ann->pLayerCount[layerIdx + 1]);
        ann->hidden[layerIdx].val = (double*)malloc(sizeof(double) * ann->pLayerCount[layerIdx + 1]);

        if((layerIdx + 1) < ann->Layer)
            ann->data[layerIdx + 1].val = ann->hidden[layerIdx].val;

        for(endIdx = 0; endIdx < ann->layerWeight[layerIdx].endCount; endIdx++)
        {
            ann->layerWeight[layerIdx].weight[endIdx] = (double*)malloc(sizeof(double) * ann->layerWeight[layerIdx].frontCount);
            ann->layerDeltaWeight[layerIdx].weight[endIdx] = (double*)malloc(sizeof(double) * ann->layerDeltaWeight[layerIdx].frontCount);

            for(frontIdx = 0; frontIdx < ann->layerWeight[layerIdx].frontCount; frontIdx++)
            {
                ann->layerDeltaWeight[layerIdx].weight[endIdx][frontIdx] = 0.;
                ann->layerWeight[layerIdx].weight[endIdx][frontIdx] = getHeInit(ann->layerWeight[layerIdx].frontCount);
            }
        }
    }
}

void processDeinit(Process *ann)
{
    int endIdx;
    int layerIdx;

    for(layerIdx = 0; layerIdx < ann->Layer; layerIdx++)
    {
        free(ann->delta[layerIdx].val);
        free(ann->hidden[layerIdx].val);

        for(endIdx = 0; endIdx < ann->layerWeight[layerIdx].endCount; endIdx++)
        {
            free(ann->layerWeight[layerIdx].weight[endIdx]);
            free(ann->layerDeltaWeight[layerIdx].weight[endIdx]);
        }

        free(ann->layerWeight[layerIdx].weight);
        free(ann->layerDeltaWeight[layerIdx].weight);
    }

    free(ann->pLayerCount);
    free(ann->delta);
    free(ann->hidden);
    free(ann->data[0].val);
    free(ann->data);
    free(ann->layerWeight);
    free(ann->layerDeltaWeight);
}

void processUpdateInput(Process *ann, unsigned char *image, int expect)
{
    int i;

    for(i = 0; i < ann->pLayerCount[0]; i++)
    {
        ann->data[0].val[i] = normalize(image[i]);
    }

    ann->expect = expect;
}

void squareError(Process *ann)
{
    int endIdx;
    const int outputLayerIdx = ann->Layer - 1;
    double expected;

    for(endIdx = 0; endIdx < ann->pLayerCount[ann->Layer]; endIdx++)
    {
        expected = (ann->expect == endIdx) ? 1. : 0.;
        ann->error += (expected - ann->hidden[outputLayerIdx].val[endIdx]) * (expected - ann->hidden[outputLayerIdx].val[endIdx]) * 0.5;
    }
}