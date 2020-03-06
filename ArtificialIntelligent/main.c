#include <stdio.h>
#include <stdlib.h>

//#include "include/linkedlist.h"
#include "include/mnist_hwn.h"
#include "include/random.h"

static int operate(pImageSetFile pImgSet, pLabelSetFile pLbSet);
static double normalize(unsigned char val);

int main(char argc, char *argv[])
{
    ImageSetFile trainningData;
    LabelSetFile trainningLabel;

    if(argc < 3)
    {
        printf("usage : \n$%s {trainning image Set} {trainning label Set}\n", argv[2]);
    }

    printf("Starting '%s'\n", (argv[0][0] == '.') ? &argv[0][2]: argv[0]);
    readImageSetFile(argv[1], &trainningData);
    readLabelSetFile(argv[2], &trainningLabel);



    releaseImageSetFile(&trainningData);
    releaseLabelSetFile(&trainningLabel);
    return 0;
}

static int operate(pImageSetFile pImgSet, pLabelSetFile pLbSet)
{
    const int FIRST = 0;
    const int SECOND = 1;
    const int THIRD = 2;

    const int hiddenLayerCount = 2;
    const int hiddenFirstNodeCount = 512;
    const int hiddenSecondNodeCount = 64;
    const int weightCount = hiddenLayerCount + 1;
    const int sumCount = weightCount;
    const int outputCount = 10;

    const int MAX_EPOCH = 100000;

    double *hidden1;
    double *hidden2;
    double **weightH1;
    double **weightH2;
    double **weightOutput;
    double *sumInputW1;
    double *sumW1W2;
    double *sumW2Output;
    double *deltaW1;
    double *deltaW2;
    double *deltaOut;

    double *output;
    
    const int sampleCount = pImgSet->info.numberOfImage;
    const int inputCount = pImgSet->info.numberOfColumns * pImgSet->info.numberOfRows;

    int i, j;
    int idxH1;
    int idxH2;
    int idxOut;
    int idxEpoch;
    int idxSet;


    hidden1 = (double*)malloc(hiddenFirstNodeCount * sizeof(double));
    hidden2 = (double*)malloc(hiddenSecondNodeCount * sizeof(double));

    weightH1 = (double**)malloc(hiddenFirstNodeCount * sizeof(double));
    for(i = 0;i < hiddenFirstNodeCount; i++)
    {
        weightH1[i] = (double*)malloc(inputCount * sizeof(double));
    }
    weightH2 = (double**)malloc(hiddenSecondNodeCount * sizeof(double));
    for(i = 0;i < hiddenSecondNodeCount; i++)
    {
        weightH2[i] = (double*)malloc(hiddenFirstNodeCount * sizeof(double));
    }
    weightOutput = (double**)malloc(outputCount * sizeof(double));
    for(i = 0;i < outputCount; i++)
    {
        weightH2[i] = (double*)malloc(hiddenSecondNodeCount * sizeof(double));
    }

    sumInputW1 = (double*)malloc(hiddenFirstNodeCount * sizeof(double));
    sumW1W2 = (double*)malloc(hiddenSecondNodeCount * sizeof(double));
    sumW2Output = (double*)malloc(outputCount * sizeof(double));

    deltaW1 = (double*)malloc(hiddenFirstNodeCount * sizeof(double));
    deltaW2 = (double*)malloc(hiddenSecondNodeCount * sizeof(double));
    deltaOut = (double*)malloc(outputCount * sizeof(double));

    output = (double*)malloc(outputCount * sizeof(double));

    for(i = 0; i < hiddenFirstNodeCount; i++)
    {
        for(j = 0; j < inputCount; j++)
        {
            weightH1[i][j] = getRandomValue();
        }
    }

    for(i = 0; i < hiddenSecondNodeCount; i++)
    {
        for(j = 0; j < hiddenFirstNodeCount; j++)
        {
            weightH2[i][j] = getRandomValue();
        }
    }

    for(i = 0; i < outputCount; i++)
    {
        for(j = 0; j < hiddenSecondNodeCount; j++)
        {
            weightOutput[i][j] = getRandomValue();
        }
    }

    for(idxEpoch = 0; idxEpoch < MAX_EPOCH; idxEpoch++)
    {
        for(idxSet = 0; idxSet < sampleCount; idxSet++)
        {
            for(idxH1 = 0; idxH1 < hiddenFirstNodeCount; idxH1++)
            {
                sumInputW1[idxH1] = 0.0f;
                for(i = 0; i < inputCount; i++)
                {
                    sumInputW1[idxH1] += normalize(pImgSet->data[idxSet][i]) * weightH1[idxH1][i];
                }
                hidden1[idxH1] = getSigmoidal(sumInputW1[idxH1]);
            }

            for(idxH2 = 0; idxH2 < hiddenSecondNodeCount; idxH2++)
            {
                sumW1W2[idxH2] = 0.f;
                for(i = 0; i < hiddenFirstNodeCount; i++)
                {
                    sumW1W2[idxH2] = hidden1[i] * weightH2[idxH2][i];
                }
                hidden2[idxH2] = getSigmoidal(sumW1W2[idxH2]);
            }

            for(idxOut = 0; idxOut < outputCount; idxOut)
            {
                sumW2Output[idxOut] = 0.f;
                for(i = 0; i < hiddenSecondNodeCount; i++)
                {
                    sumW2Output[idxOut] = hidden2[i] * weightOutput[idxOut][i];
                }
                output[idxOut] = getSigmoidal(sumW2Output[idxOut]);
            }
        }
    }
}

static double normalize(unsigned char val)
{
    const int MAX_UBYTE = 255;
    double retVal = 0.0f;
    
    if(val != 0)
    {
        retVal = (double)val / MAX_UBYTE;
    }

    return retVal;
}

static double getSigmoidal(double val)
{
    double retVal = 0.f;
    
    retVal = 1.0/(1.0 + exp(-val));   // Sigmoidal

    return retVal;
}