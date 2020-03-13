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
static double getReLu(double val);
static void softmax(double *value, int count);

ImageSetFile testData;
LabelSetFile testLabel;

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

    readImageSetFile(argv[3], &testData);
    readLabelSetFile(argv[4], &testLabel);

    operate(&trainningData, &trainningLabel);
    //operateSimple(&trainningData, &trainningLabel);

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
    double **dweightH1;
    double **dweightH2;
    double **dweightOutput;
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

    int assume = 0;
    int correct = 0;

    double error;
    double eta = 0.01;
    double alpha = 0.9;

    int *order;

    order = (int *)malloc(sizeof(int)* sampleCount);

    hidden1 = (double*)malloc(hiddenFirstNodeCount * sizeof(double));
    hidden2 = (double*)malloc(hiddenSecondNodeCount * sizeof(double));

    weightH1 = (double**)malloc(hiddenFirstNodeCount * sizeof(double));
    dweightH1 = (double**)malloc(hiddenFirstNodeCount * sizeof(double));
    for(i = 0;i < hiddenFirstNodeCount; i++)
    {
        weightH1[i] = (double*)malloc(inputCount * sizeof(double));
        dweightH1[i] = (double*)malloc(inputCount * sizeof(double));
    }
    weightH2 = (double**)malloc(hiddenSecondNodeCount * sizeof(double));
    dweightH2 = (double**)malloc(hiddenSecondNodeCount * sizeof(double));
    for(i = 0;i < hiddenSecondNodeCount; i++)
    {
        weightH2[i] = (double*)malloc(hiddenFirstNodeCount * sizeof(double));
        dweightH2[i] = (double*)malloc(hiddenFirstNodeCount * sizeof(double));
    }
    weightOutput = (double**)malloc(outputCount * sizeof(double));
    dweightOutput = (double**)malloc(outputCount * sizeof(double));
    for(i = 0;i < outputCount; i++)
    {
        weightOutput[i] = (double*)malloc(hiddenSecondNodeCount * sizeof(double));
        dweightOutput[i] = (double*)malloc(hiddenSecondNodeCount * sizeof(double));
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
            //weightH1[i][j] = getRandomValue();
            weightH1[i][j] = getHeInit(hiddenFirstNodeCount);
            dweightH1[i][j] = 0.;
        }
    }

    for(i = 0; i < hiddenSecondNodeCount; i++)
    {
        for(j = 0; j < hiddenFirstNodeCount; j++)
        {
            //weightH2[i][j] = getRandomValue();
            weightH2[i][j] = getHeInit(hiddenSecondNodeCount);
            dweightH2[i][j] = 0.;
        }
    }

    for(i = 0; i < outputCount; i++)
    {
        for(j = 0; j < hiddenSecondNodeCount; j++)
        {
            //weightOutput[i][j] = getRandomValue();
            weightOutput[i][j] = getHeInit(outputCount);
            dweightOutput[i][j] = 0.;
        }
    }

    for(idxEpoch = 0; idxEpoch < MAX_EPOCH; idxEpoch++)
    {
        int temp = 0;
        int tempIdx = 0; 
        error = 0.;

        for(idxSet = 0; idxSet < sampleCount; idxSet++)
        {
            order[idxSet] = idxSet;
        }

        for(idxSet = 0; idxSet < sampleCount; idxSet++)
        {
            tempIdx = idxSet + ((double)rand()/((double)RAND_MAX+1)) * ( sampleCount - idxSet);
            temp = order[idxSet];
            order[idxSet] = order[tempIdx];
            order[tempIdx] = temp;
        }

        for(idxSet = 0; idxSet < sampleCount; idxSet++)
        {
            for(idxH1 = 0; idxH1 < hiddenFirstNodeCount; idxH1++)
            {
                sumInputW1[idxH1] = weightH1[idxH1][0];
                for(i = 0; i < inputCount; i++)
                {
                    sumInputW1[idxH1] += normalize(pImgSet->data[order[idxSet]][i]) * weightH1[idxH1][i];
                }
                hidden1[idxH1] = getSigmoidal(sumInputW1[idxH1]);
                //hidden1[idxH1] = getReLu(sumInputW1[idxH1]);
            }

            for(idxH2 = 0; idxH2 < hiddenSecondNodeCount; idxH2++)
            {
                sumW1W2[idxH2] = 0.f;
                for(i = 0; i < hiddenFirstNodeCount; i++)
                {
                    sumW1W2[idxH2] += hidden1[i] * weightH2[idxH2][i];
                }
                hidden2[idxH2] = getSigmoidal(sumW1W2[idxH2]);
                //hidden2[idxH2] = getReLu(sumW1W2[idxH2]);
            }

            for(idxOut = 0; idxOut < outputCount; idxOut++)
            {
                sumW2Output[idxOut] = 0.f;
                
                for(i = 0; i < hiddenSecondNodeCount; i++)
                {
                    sumW2Output[idxOut] += hidden2[i] * weightOutput[idxOut][i];
                }
                //output[idxOut] = getSigmoidal(sumW2Output[idxOut]);
                output[idxOut] = sumW2Output[idxOut];
            }

            softmax(output, outputCount);

            for(idxOut = 0; idxOut < outputCount; idxOut++)
            {
                error += 0.5 * ((double)((pLbSet->label[order[idxSet]] == idxOut)?1.:0.) - output[idxOut]) * ((double)((pLbSet->label[order[idxSet]] == idxOut)?1.:0.) - output[idxOut]);
                deltaOut[idxOut] = ((double)((pLbSet->label[order[idxSet]] == idxOut)?1.:0.) - output[idxOut]) * output[idxOut] * (1.f - output[idxOut]);
            }

            for(idxH2 = 0; idxH2 < hiddenSecondNodeCount; idxH2++)
            {
                sumW1W2[idxH2] = 0.f;
                for(i = 0; i < outputCount; i++)
                {
                    sumW1W2[idxH2] += deltaOut[i] * weightOutput[i][idxH2];
                }
                deltaW2[idxH2] = (sumW1W2[idxH2] * hidden2[idxH2] * (1.f - hidden2[idxH2]));
            }

            for(idxH1 = 0; idxH1 < hiddenFirstNodeCount; idxH1++)
            {
                sumInputW1[idxH1] = 0.0f;
                for(i = 0; i < hiddenSecondNodeCount; i++)
                {
                    sumInputW1[idxH1] += weightH1[idxH1][i] * deltaW2[i];
                }
                deltaW1[idxH1] = (sumInputW1[idxH1] * hidden1[idxH1] * (1.f - hidden1[idxH1]));
            }

            for(idxH1 = 0; idxH1 < hiddenFirstNodeCount; idxH1++)
            {
                for(i = 0; i < inputCount; i++)
                {
                    dweightH1[idxH1][i] = eta * normalize(pImgSet->data[order[idxSet]][i]) * deltaW1[idxH1] + alpha * dweightH1[idxH1][i];
                    weightH1[idxH1][i] += dweightH1[idxH1][i]; 
                }
            }

            for(idxH2 = 0; idxH2 < hiddenSecondNodeCount; idxH2++)
            {
                for(i = 0; i < hiddenFirstNodeCount; i++)
                {
                    dweightH2[idxH2][i] = eta * hidden1[i] * deltaW2[idxH2] + alpha * dweightH2[idxH2][i];
                    weightH2[idxH2][i] += dweightH2[idxH2][i];
                }
            }

            for(idxOut = 0; idxOut < outputCount; idxOut++)
            {
                double delta = 0.;
                for(i = 0; i < hiddenSecondNodeCount; i++)
                {
                    dweightOutput[idxOut][i] = eta * hidden2[i] * deltaOut[idxOut] + alpha * dweightOutput[idxOut][i];
                    weightOutput[idxOut][i] += dweightOutput[idxOut][i];
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
        if(idxEpoch > 5) 
            break;
    }

    // check with test file
    for(idxSet = 0; idxSet < testLabel.info.numberOfItems; idxSet++)
    {
        double predict = 0.;
        assume = 0;
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
            //output[idxOut] = getSigmoidal(sumW2Output[idxOut]);
            output[idxOut] = sumW2Output[idxOut];
        }
        softmax(output, outputCount);
        for(idxOut = 0; idxOut < outputCount; idxOut++)
        {
            if(predict < output[idxOut])
            {
                predict = output[idxOut];
                assume = idxOut;
            }
        }
        printf("predict number : %d(%lf%%)\treal : %d\n", assume, predict * 100, testLabel.label[idxSet]);
        if(assume == testLabel.label[idxSet])
        {
            correct++;
        }

    }

    printf("%d/%d(%f)\n", correct, testLabel.info.numberOfItems, (double)correct / testLabel.info.numberOfItems * 100 );


    // free memory
    for(i = 0;i < hiddenFirstNodeCount; i++)
    {
        free(weightH1[i]);
        free(dweightH1[i]);
    }
    for(i = 0;i < hiddenSecondNodeCount; i++)
    {
        free(weightH2[i]);
        free(dweightH2[i]);
    }
    for(i = 0;i < outputCount; i++)
    {
        free(weightOutput[i]);
        free(dweightOutput[i]);
    }

    free(hidden1);
    free(hidden2);
    free(weightH1);
    free(weightH2);
    free(weightOutput);
    free(dweightH1);
    free(dweightH2);
    free(dweightOutput);
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
    const int hiddenFirstNodeCount = 1024;
    //const int hiddenSecondNodeCount = 64;
    const int weightCount = hiddenLayerCount + 1;
    const int sumCount = weightCount;
    const int outputCount = 10;

    const int MAX_EPOCH = 100000;

    double *hidden1;
    double **weightH1;
    double **weightOutput;
    double **dweightH1;
    double **dweightOutput;
    double *sumInputW1;
    double *sumW2Output;
    double *deltaW1;
    double *deltaOut;

    double *output;
    
    const int sampleCount = pImgSet->info.numberOfImage;
    //const int sampleCount = 10000;
    const int inputCount = pImgSet->info.numberOfColumns * pImgSet->info.numberOfRows;

    int *order;

    int i, j;
    int np;
    int op;

    unsigned char assume;

    int idxH1;
    int idxH2;
    int idxOut;
    int idxEpoch;
    int idxSet;
    int correct = 0;

    double error;
    double eta = 0.5;
    double alpha = 0.5;

    order = (int *)malloc(sizeof(int)* sampleCount);
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

    dweightH1 = (double**)malloc(hiddenFirstNodeCount * sizeof(double));
    for(i = 0;i < hiddenFirstNodeCount; i++)
    {
        dweightH1[i] = (double*)malloc(inputCount * sizeof(double));
    }

    dweightOutput = (double**)malloc(outputCount * sizeof(double));
    for(i = 0;i < outputCount; i++)
    {
        dweightOutput[i] = (double*)malloc(hiddenFirstNodeCount * sizeof(double));
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
            //weightH1[i][j] = getRandomValue();
            weightH1[i][j] = getHeInit(inputCount);
            dweightH1[i][j] = 0.;
        }
    }

    for(i = 0; i < outputCount; i++)
    {
        for(j = 0; j < hiddenFirstNodeCount; j++)
        {
            //weightOutput[i][j] = getRandomValue();
            weightOutput[i][j] = getHeInit(hiddenFirstNodeCount);
            dweightOutput[i][j] = 0.;
        }
    }

    for(idxEpoch = 0; idxEpoch < MAX_EPOCH; idxEpoch++)
    {
        error = 0.;
        for(idxSet = 0; idxSet < sampleCount; idxSet++)
        {
            order[idxSet] = idxSet;
        }

        for(idxSet = 0; idxSet < sampleCount; idxSet++)
        {
            np = idxSet + ((double)rand()/((double)RAND_MAX+1)) * ( sampleCount - idxSet);
            op = order[idxSet];
            order[idxSet] = order[np];
            order[np] = op;
        }

        for(idxSet = 0; idxSet < sampleCount; idxSet++)
        {
            for(idxH1 = 0; idxH1 < hiddenFirstNodeCount; idxH1++)
            {
                sumInputW1[idxH1] = 0.;
                for(i = 0; i < inputCount; i++)
                {
                    sumInputW1[idxH1] += normalize(pImgSet->data[order[idxSet]][i]) * weightH1[idxH1][i];
                }
                hidden1[idxH1] = getSigmoidal(sumInputW1[idxH1]);
                //hidden1[idxH1] = getReLu(sumInputW1[idxH1]);
                //hidden1[idxH1] = sumInputW1[idxH1];
            }
            //softmax(hidden1, hiddenFirstNodeCount);

            for(idxOut = 0; idxOut < outputCount; idxOut++)
            {
                sumW2Output[idxOut] = 0.f;
                
                for(i = 0; i < hiddenFirstNodeCount; i++)
                {
                    sumW2Output[idxOut] += hidden1[i] * weightOutput[idxOut][i];
                }
                output[idxOut] = getSigmoidal(sumW2Output[idxOut]);
                //output[idxOut] = getReLu(sumW2Output[idxOut]);
                //output[idxOut] = sumW2Output[idxOut];
                
                //error += 0.5 * ((double)((pLbSet->label[order[idxSet]] == idxOut)?1.:0.) - output[idxOut]) * ((double)((pLbSet->label[order[idxSet]] == idxOut)?1.:0.) - output[idxOut]);
                //error += 0.5 * ((double)(pLbSet->label[order[idxSet]] - output[idxOut])) * ((double)(pLbSet->label[order[idxSet]] - output[idxOut]));
                //deltaOut[idxOut] = ((double)((pLbSet->label[order[idxSet]] == idxOut)?1.:0.) - output[idxOut]) * output[idxOut] * (1.f - output[idxOut]);
                //deltaOut[idxOut] = ((double)((pLbSet->label[order[idxSet]] == idxOut)?1.:0.) - output[idxOut]);
                //deltaOut[idxOut] = (double)(pLbSet->label[order[idxSet]]) - output[idxOut];

            }
            
            softmax(output, outputCount);
            
            for(idxOut = 0; idxOut < outputCount; idxOut++)
            {
                   
                error += 0.5 * ((double)((pLbSet->label[order[idxSet]] == idxOut)?1.:0.) - output[idxOut]) * ((double)((pLbSet->label[order[idxSet]] == idxOut)?1.:0.) - output[idxOut]);
                //error += 0.5 * ((double)(pLbSet->label[order[idxSet]] - output[idxOut])) * ((double)(pLbSet->label[order[idxSet]] - output[idxOut]));
                deltaOut[idxOut] = ((double)((pLbSet->label[order[idxSet]] == idxOut)?1.:0.) - output[idxOut]) * output[idxOut] * (1.f - output[idxOut]);
                //deltaOut[idxOut] = ((double)((pLbSet->label[order[idxSet]] == idxOut)?1.:0.) - output[idxOut]);
                //deltaOut[idxOut] = (double)(pLbSet->label[order[idxSet]]) - output[idxOut];

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
                    dweightH1[idxH1][i] = eta * normalize(pImgSet->data[order[idxSet]][i]) * deltaW1[idxH1] + alpha * dweightH1[idxH1][i];
                    //delta = eta * normalize(pImgSet->data[idxSet][i]) * deltaW1[idxH1];
                    weightH1[idxH1][i] += dweightH1[idxH1][i]; 
                }
            }

            for(idxOut = 0; idxOut < outputCount; idxOut++)
            {
                double delta = 0.;
                for(i = 0; i < hiddenFirstNodeCount; i++)
                {
                    dweightOutput[idxOut][i] = eta * hidden1[i] * deltaOut[idxOut] + alpha * dweightOutput[idxOut][i];
                    //delta = eta * hidden1[i] * deltaOut[idxOut];
                    weightOutput[idxOut][i] += dweightOutput[idxOut][i];
                    if(weightOutput[idxOut][i] > 1 || weightOutput[idxOut][i] < -1)
                    {
                        //printf("abnormal : %lf\n", weightOutput[idxOut][i]);
                    }
                }
            }

            
            if(idxSet % 10000 == 0)
                printf("s : %5d, e = %lf\n", idxSet, error / sampleCount);
        }
        printf("s : %5d, e = %lf\n", idxSet, error/ sampleCount);
        //if(idxEpoch % 100 == 0)
        {
            printf("\nEpoch : %d\n",idxEpoch);
            //printf("\rsample count : %5d, error = %lf", idxSet, error);
        }
        if((error / sampleCount) < 0.04) 
            break;
        if(idxEpoch > 100)
            break;
    }


    // check with test file
    for(idxSet = 0; idxSet < testLabel.info.numberOfItems; idxSet++)
    {
        double predict = 0.;
        assume = 0;
        for(idxH1 = 0; idxH1 < hiddenFirstNodeCount; idxH1++)
        {
            sumInputW1[idxH1] = 0.;
            for(i = 0; i < inputCount; i++)
            {
                sumInputW1[idxH1] += normalize(testData.data[idxSet][i]) * weightH1[idxH1][i];
            }
            hidden1[idxH1] = getSigmoidal(sumInputW1[idxH1]);
            //hidden1[idxH1] = getReLu(sumInputW1[idxH1]);
            //hidden1[idxH1] = sumInputW1[idxH1];
        }
        //softmax(hidden1, hiddenFirstNodeCount);
        for(idxOut = 0; idxOut < outputCount; idxOut++)
        {
            sumW2Output[idxOut] = 0.f;
            
            for(i = 0; i < hiddenFirstNodeCount; i++)
            {
                sumW2Output[idxOut] += hidden1[i] * weightOutput[idxOut][i];
            }
            //output[idxOut] = getSigmoidal(sumW2Output[idxOut]);
            //output[idxOut] = getReLu(sumW2Output[idxOut]);
            output[idxOut] = sumW2Output[idxOut];

        }
        softmax(output, outputCount);
        for(idxOut = 0; idxOut < outputCount; idxOut++)
        {
            error += 0.5 * ((double)((testLabel.label[idxSet] == idxOut)?1.:0.) - output[idxOut]) * ((double)((testLabel.label[idxSet] == idxOut)?1.:0.) - output[idxOut]);

            if(predict < output[idxOut])
            {
                predict = output[idxOut];
                assume = idxOut;
            }
        }
        printf("predict number : %d(%lf%%)\treal : %d\n", assume, predict * 100, testLabel.label[idxSet]);
        if(assume == testLabel.label[idxSet])
        {
            correct++;
        }

    }

    printf("%d/%d(%f)\n", correct, testLabel.info.numberOfItems, (double)correct / testLabel.info.numberOfItems * 100 );

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
    free(order);

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