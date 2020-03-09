#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#include "include/linkedlist.h"
#include "include/mnist_hwn.h"
#include "include/random.h"

static int operateSimple(pImageSetFile pImgSet, pLabelSetFile pLbSet);
static int operate(pImageSetFile pImgSet, pLabelSetFile pLbSet);
static double normalize(unsigned char val);
static double getSigmoidal(double val);

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

    //operate(&trainningData, &trainningLabel);
    operateSimple(&trainningData, &trainningLabel);

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
    //const int sampleCount = 10;
    const int inputCount = pImgSet->info.numberOfColumns * pImgSet->info.numberOfRows;

    int i, j;
    int idxH1;
    int idxH2;
    int idxOut;
    int idxEpoch;
    int idxSet;

    double error;
    double eta = 1.;
    double alpha = 1.;

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
        weightOutput[i] = (double*)malloc(hiddenSecondNodeCount * sizeof(double));
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
        error = 0.;
        for(idxSet = 0; idxSet < sampleCount; idxSet++)
        {
            for(idxH1 = 0; idxH1 < hiddenFirstNodeCount; idxH1++)
            {
                sumInputW1[idxH1] = weightH1[idxH1][0];
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
                    sumW1W2[idxH2] += hidden1[i] * weightH2[idxH2][i];
                }
                hidden2[idxH2] = getSigmoidal(sumW1W2[idxH2]);
            }

            for(idxOut = 0; idxOut < outputCount; idxOut++)
            {
                sumW2Output[idxOut] = 0.f;
                
                for(i = 0; i < hiddenSecondNodeCount; i++)
                {
                    sumW2Output[idxOut] += hidden2[i] * weightOutput[idxOut][i];
                }
                output[idxOut] = getSigmoidal(sumW2Output[idxOut]);
                
                error += 0.5 * ((double)((pLbSet->label[idxSet] == idxOut)?1.:0.) - output[idxOut]) * ((double)((pLbSet->label[idxSet] == idxOut)?1.:0.) - output[idxOut]);
                deltaOut[idxOut] = ((double)((pLbSet->label[idxSet] == idxOut)?1.:0.) - output[idxOut]) * output[idxOut] * (1.f - output[idxOut]);

            }

            for(idxH2 = 0; idxH2 < hiddenSecondNodeCount; idxH2++)
            {
                sumW1W2[idxH2] = 0.f;
                for(i = 0; i < outputCount; i++)
                {
                    sumW1W2[idxH2] += deltaOut[i] * weightOutput[i][idxH2];
                }
                deltaW2[idxH2] = sumW1W2[idxH2] * hidden2[idxH2] * (1.f - hidden2[idxH2]);
            }

            for(idxH1 = 0; idxH1 < hiddenFirstNodeCount; idxH1++)
            {
                sumInputW1[idxH1] = 0.0f;
                for(i = 0; i < hiddenSecondNodeCount; i++)
                {
                    sumInputW1[idxH1] += weightH1[idxH1][i] * deltaW2[i];
                }
                deltaW1[idxH1] = sumInputW1[idxH1] * hidden1[idxH1] * (1.f - hidden1[idxH1]);
            }

            for(idxH1 = 0; idxH1 < hiddenFirstNodeCount; idxH1++)
            {
                double delta = 0.;
                for(i = 0; i < inputCount; i++)
                {
                    delta = eta * normalize(pImgSet->data[idxSet][i]) * deltaW1[idxH1];
                    weightH1[idxH1][i] += delta; 
                }
            }

            for(idxH2 = 0; idxH2 < hiddenSecondNodeCount; idxH2++)
            {
                double delta = 0.;
                for(i = 0; i < hiddenFirstNodeCount; i++)
                {
                    delta = eta * hidden1[i] * deltaW2[idxH2];
                    weightH2[idxH2][i] += delta;
                }
            }

            for(idxOut = 0; idxOut < outputCount; idxOut++)
            {
                double delta = 0.;
                for(i = 0; i < hiddenSecondNodeCount; i++)
                {
                    delta = eta * hidden2[i] * deltaOut[idxOut];
                    weightOutput[idxOut][i] += delta;
                }
            }

            
            if(idxSet % 1000 == 0)
                printf("s : %5d, e = %lf\n", idxSet, error);
        }
        
        //if(idxEpoch % 100 == 0)
        {
            printf("\nEpoch : %d\n",idxEpoch);
            //printf("\rsample count : %5d, error = %lf", idxSet, error);
        }
        if(error < 0.0004) 
            break;
    }



    // free memory
    for(i = 0;i < hiddenFirstNodeCount; i++)
    {
        free(weightH1[i]);
    }
    for(i = 0;i < hiddenSecondNodeCount; i++)
    {
        free(weightH2[i]);
    }
    for(i = 0;i < outputCount; i++)
    {
        free(weightOutput[i]);
    }

    free(hidden1);
    free(hidden2);
    free(weightH1);
    free(weightH2);
    free(weightOutput);
    free(sumInputW1);
    free(sumW1W2);
    free(sumW2Output);
    free(deltaW1);
    free(deltaW2);
    free(deltaOut);
    free(output);

}

static int operateSimple(pImageSetFile pImgSet, pLabelSetFile pLbSet)
{
    const int hiddenLayerCount = 1;
    //const int hiddenFirstNodeCount = 512;
    const int hiddenFirstNodeCount = 32;
    //const int hiddenSecondNodeCount = 64;
    const int weightCount = hiddenLayerCount + 1;
    const int sumCount = weightCount;
    const int outputCount = 10;

    const int MAX_EPOCH = 100000;

    double *hidden1;
    double **weightH1;
    double **weightOutput;
    double *sumInputW1;
    double *sumW2Output;
    double *deltaW1;
    double *deltaOut;

    double *output;
    
    const int sampleCount = pImgSet->info.numberOfImage;
    //const int sampleCount = 10;
    const int inputCount = pImgSet->info.numberOfColumns * pImgSet->info.numberOfRows;

    int i, j;
    int idxH1;
    int idxH2;
    int idxOut;
    int idxEpoch;
    int idxSet;

    double error;
    double eta = 0.5;
    double alpha = 1.;

    hidden1 = (double*)malloc(hiddenFirstNodeCount * sizeof(double));

    weightH1 = (double**)malloc(hiddenFirstNodeCount * sizeof(double));
    for(i = 0;i < hiddenFirstNodeCount; i++)
    {
        weightH1[i] = (double*)malloc(inputCount * sizeof(double));
    }

    weightOutput = (double**)malloc(outputCount * sizeof(double));
    for(i = 0;i < outputCount; i++)
    {
        weightOutput[i] = (double*)malloc(hiddenFirstNodeCount * sizeof(double));
    }

    sumInputW1 = (double*)malloc(hiddenFirstNodeCount * sizeof(double));
    sumW2Output = (double*)malloc(outputCount * sizeof(double));

    deltaW1 = (double*)malloc(hiddenFirstNodeCount * sizeof(double));
    deltaOut = (double*)malloc(outputCount * sizeof(double));

    output = (double*)malloc(outputCount * sizeof(double));

    for(i = 0; i < hiddenFirstNodeCount; i++)
    {
        for(j = 0; j < inputCount; j++)
        {
            weightH1[i][j] = getRandomValue();
        }
    }

    for(i = 0; i < outputCount; i++)
    {
        for(j = 0; j < hiddenFirstNodeCount; j++)
        {
            weightOutput[i][j] = getRandomValue();
        }
    }

    for(idxEpoch = 0; idxEpoch < MAX_EPOCH; idxEpoch++)
    {
        error = 0.;
        for(idxSet = 0; idxSet < sampleCount; idxSet++)
        {
            for(idxH1 = 0; idxH1 < hiddenFirstNodeCount; idxH1++)
            {
                sumInputW1[idxH1] = weightH1[idxH1][0];
                for(i = 0; i < inputCount; i++)
                {
                    sumInputW1[idxH1] += normalize(pImgSet->data[idxSet][i]) * weightH1[idxH1][i];
                }
                hidden1[idxH1] = getSigmoidal(sumInputW1[idxH1]);
            }

            for(idxOut = 0; idxOut < outputCount; idxOut++)
            {
                sumW2Output[idxOut] = 0.f;
                
                for(i = 0; i < hiddenFirstNodeCount; i++)
                {
                    sumW2Output[idxOut] += hidden1[i] * weightOutput[idxOut][i];
                }
                output[idxOut] = getSigmoidal(sumW2Output[idxOut]);
                
                error += 0.5 * ((double)((pLbSet->label[idxSet] == idxOut)?1.:0.) - output[idxOut]) * ((double)((pLbSet->label[idxSet] == idxOut)?1.:0.) - output[idxOut]);
                deltaOut[idxOut] = ((double)((pLbSet->label[idxSet] == idxOut)?1.:0.) - output[idxOut]) * output[idxOut] * (1.f - output[idxOut]);

            }

            for(idxH1 = 0; idxH1 < hiddenFirstNodeCount; idxH1++)
            {
                sumInputW1[idxH1] = 0.0f;
                for(i = 0; i < outputCount; i++)
                {
                    sumInputW1[idxH1] += weightH1[idxH1][i] * deltaOut[i];
                }
                deltaW1[idxH1] = sumInputW1[idxH1] * hidden1[idxH1] * (1.f - hidden1[idxH1]);
            }

            for(idxH1 = 0; idxH1 < hiddenFirstNodeCount; idxH1++)
            {
                double delta = 0.;
                for(i = 0; i < inputCount; i++)
                {
                    delta = eta * normalize(pImgSet->data[idxSet][i]) * deltaW1[idxH1];
                    weightH1[idxH1][i] += delta; 
                }
            }

            for(idxOut = 0; idxOut < outputCount; idxOut++)
            {
                double delta = 0.;
                for(i = 0; i < hiddenFirstNodeCount; i++)
                {
                    delta = eta * hidden1[i] * deltaOut[idxOut];
                    weightOutput[idxOut][i] += delta;
                }
            }

            
            if(idxSet % 10000 == 0)
                printf("s : %5d, e = %lf\n", idxSet, error);
        }

        //if(idxEpoch % 100 == 0)
        {
            printf("\nEpoch : %d\n",idxEpoch);
            //printf("\rsample count : %5d, error = %lf", idxSet, error);
        }
        if(error < 0.0004) 
            break;
    }



    // free memory
    for(i = 0;i < hiddenFirstNodeCount; i++)
    {
        free(weightH1[i]);
    }
    for(i = 0;i < outputCount; i++)
    {
        free(weightOutput[i]);
    }

    free(hidden1);
    free(weightH1);
    free(weightOutput);
    free(sumInputW1);
    free(sumW2Output);
    free(deltaW1);
    free(deltaOut);
    free(output);

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