#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <pthread.h>

#include "include/ann.h"
#include "include/random.h"

static pImageSetFile _image;
static pLabelSetFile _label;

static pthread_mutex_t mutex_lock_input;
static pthread_mutex_t mutex_lock_weight;

static Process *_annSum;

const char *data_file = "weight.dat";

static double normalize(unsigned char val);
static double getSigmoidal(double val);
static double getELU(double val);
static double getReLu(double val);
static double getLeakyReLU(double val);
static void softmax(double *value, int count);
static int getEpochIndex(void);
static int mergeWeight(Process *ann);

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

static double getELU(double val)
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

    if(retVal > 1)
    {
        retVal = 1.;
    }
    

    return retVal;
}

static double getReLu(double val)
{
    double retVal = 0.f;
    // Rectified Linear Unit(LEU)
    /*
    f(x) = x if x > 0
    f(x) = 0 if x <= 0
    */
    if(val > 0)
    {
        retVal = val;
    }
    else
    {
        retVal = 0.;
    }

    if(retVal > 1)
    {
        retVal = 1.;
    }
    

    return retVal;
}

static double getLeakyReLU(double val)
{
    double retVal = 0.f;
    const double alpha = 0.1;
    // Leaky ReLU
    /*
    f(x) = x if x > 0
    f(x) = alpha*x if x <= 0
    */
    if(val > 0)
    {
        retVal = val;
    }
    else
    {
        retVal = alpha * val;
    }

    if(retVal > 1)
    {
        retVal = 1.;
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
    int j;

    pf = fopen(data_file, "rb");

    if(pf != NULL)
    {
        for(i = 0; i < ann->Layer; i++)
        {
            for(j = 0; j < ann->layerWeight[i].endCount; j++)
            {
                fread(ann->layerWeight[i].weight[j], sizeof(double), ann->layerWeight[i].frontCount, pf);
            }
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
    int j;

    pf = fopen(data_file, "wb");

    if(pf != NULL)
    {
        for(i = 0; i < ann->Layer; i++)
        {
            for(j = 0; j < ann->layerWeight[i].endCount; j++)
            {
                fwrite(ann->layerWeight[i].weight[j], sizeof(double), ann->layerWeight[i].frontCount, pf);
            }
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
    const double bias = 0.5;

    for(layerIdx = 0; layerIdx < outputLayerIdx; layerIdx++)
    {
        for(endIdx = 0; endIdx < ann->layerWeight[layerIdx].endCount; endIdx++)
        {
            sum = 0.;
            for(frontIdx = 0; frontIdx < ann->layerWeight[layerIdx].frontCount; frontIdx++)
            {
                sum += ann->data[layerIdx].val[frontIdx] * ann->layerWeight[layerIdx].weight[endIdx][frontIdx];
            }
            ann->hidden[layerIdx].val[endIdx] = ann->actFunc(sum + bias);
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
    ACTIVATION_FUNC func;

    ann->momentum = 0.9;
    ann->learningRate = 0.001;
    ann->epsilon = 0.005;
    ann->error = 1.;

    ann->Layer = layerCount - 1;
    ann->pLayerCount = (int*)malloc(sizeof(int) * layerCount);

    va_start(ap, layerCount);
    for (frontIdx = 0; frontIdx < layerCount; frontIdx++)
    {
        ann->pLayerCount[frontIdx] = va_arg(ap, int);

    }
    func = va_arg(ap, ACTIVATION_FUNC);
    va_end(ap);

    switch (func)
    {
        case ACTIVATION_SIGMOID:
            ann->actFunc = getSigmoidal;
            break;
        case ACTIVATION_RELU:
            ann->actFunc = getReLu;
            break;
        case ACTIVATION_ELU:
            ann->actFunc = getELU;
            break;
        case ACTIVATION_LEAKYRELU:
            ann->actFunc = getLeakyReLU;
            break;
        default:
            break;
    }

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

void *annProcess(void *data)
{
    int imgIdx;
    Process *ann = (Process*)data;
    Node *input;
    int label;
    int epoch;
    int *order;
    int np;
    int op;

    order = (int*)malloc(sizeof(int) * _image->info.numberOfImage);

    for(imgIdx = 0; imgIdx < _image->info.numberOfImage; imgIdx++)
    {
        order[imgIdx] = imgIdx;
    }

    while(1)
    {
        ann->error = 0.;
        epoch = getEpochIndex();

        for(imgIdx = 0; imgIdx < _image->info.numberOfImage; imgIdx++)
        {
            np = imgIdx + ((double)rand()/((double)RAND_MAX+1)) * (_image->info.numberOfImage + 1 - imgIdx);
            op = order[imgIdx];
            order[imgIdx] = order[np];
            order[np] = op;
        }

        for(imgIdx = 0; imgIdx < _image->info.numberOfImage; imgIdx++)
        {
            processUpdateInput(ann, _image->data[order[imgIdx]], (int)_label->label[order[imgIdx]]);
            perception(ann);
            updateWeight(ann);

            squareError(ann);
            if((imgIdx) % 1000 == 0)
            {
                if(imgIdx == 0)
                    printf("[%d]%d\t\t%lf\n", epoch, imgIdx, ann->error);
                else
                    printf("[%d]%d\t\t%lf\n", epoch, imgIdx, ann->error/imgIdx);
            }
        }

        if(updateWeightThread(ann))
            break;
    }
}

int getEpochIndex(void)
{
    static int index = 0;
    pthread_mutex_lock(&mutex_lock_input);
    index++;
    
    pthread_mutex_unlock(&mutex_lock_input);
    return index;
}

int updateWeightThread(Process *ann)
{
    int retVal = 1;
    pthread_mutex_lock(&mutex_lock_weight);
    retVal = mergeWeight(ann);
    pthread_mutex_unlock(&mutex_lock_weight);
    return retVal;
}

void setInitInputDataSet(Process *ann, pImageSetFile image, pLabelSetFile label)
{
    pthread_mutex_init(&mutex_lock_input, NULL);
    pthread_mutex_init(&mutex_lock_weight, NULL);
    _image = image;
    _label = label;
    _annSum = ann;
    _annSum->error *= _image->info.numberOfImage;
}

static int mergeWeight(Process *ann)
{
    int layerIdx;
    int endIdx;
    int frontIdx;
    const double div = 2.;
    int retVal = 0;

    for(layerIdx = 0; layerIdx < ann->Layer; layerIdx++)
    {
        for(endIdx = 0; endIdx < ann->layerWeight[layerIdx].endCount; endIdx++)
        {
            for(frontIdx = 0; frontIdx < ann->layerWeight[layerIdx].frontCount; frontIdx++)
            {
                _annSum->layerWeight[layerIdx].weight[endIdx][frontIdx] += (_annSum->layerWeight[layerIdx].weight[endIdx][frontIdx] - ann->layerWeight[layerIdx].weight[endIdx][frontIdx]);
                ann->layerWeight[layerIdx].weight[endIdx][frontIdx] = _annSum->layerWeight[layerIdx].weight[endIdx][frontIdx];
                _annSum->layerDeltaWeight[layerIdx].weight[endIdx][frontIdx] += (+_annSum->layerDeltaWeight[layerIdx].weight[endIdx][frontIdx] - ann->layerDeltaWeight[layerIdx].weight[endIdx][frontIdx]);
                ann->layerDeltaWeight[layerIdx].weight[endIdx][frontIdx] = _annSum->layerDeltaWeight[layerIdx].weight[endIdx][frontIdx];
            }
        }
    }

    _annSum->error += (_annSum->error - ann->error);

    if((_annSum->error / _image->info.numberOfImage) < ann->epsilon)
    {
        retVal = 1;
    }

    return retVal;
}