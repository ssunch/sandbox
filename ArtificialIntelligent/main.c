#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

//#include "include/linkedlist.h"
#include "include/mnist_hwn.h"
#include "include/random.h"
#include "include/ann.h"
#include "include/core.h"

void processThread(pImageSetFile trainData, pLabelSetFile trainLabel, pImageSetFile testData, pLabelSetFile testLabel);
void processCascade(pImageSetFile trainData, pLabelSetFile trainLabel, pImageSetFile testData, pLabelSetFile testLabel);

int main(char argc, char *argv[])
{
    ImageSetFile trainningData;
    LabelSetFile trainningLabel;

    ImageSetFile testData;
    LabelSetFile testLabel;

    if(argc < 5)
    {
        printf("usage : \n$%s {trainning image Set} {trainning label Set} {test image Set} {test label Set}\n", argv[2]);
    }

    printf("Starting '%s'\n", (argv[0][0] == '.') ? &argv[0][2]: argv[0]);

    readImageSetFile(argv[1], &trainningData);
    readLabelSetFile(argv[2], &trainningLabel);

    readImageSetFile(argv[3], &testData);
    readLabelSetFile(argv[4], &testLabel);

    processCascade(&trainningData, &trainningLabel, &testData, &testLabel);
    //processThread(&trainningData, &trainningLabel, &testData, &testLabel);

    releaseImageSetFile(&trainningData);
    releaseLabelSetFile(&trainningLabel);

    releaseImageSetFile(&testData);
    releaseLabelSetFile(&testLabel);
    return 0;
}


void processThread(pImageSetFile trainData, pLabelSetFile trainLabel, pImageSetFile testData, pLabelSetFile testLabel)
{
    Process trainProc;
    Process testProc;
    Process *procThread;
    Process combProc;
    int imgIdx = 0;
    int epochIdx = 0;

    int threadCnt;
	pthread_t *p_thread;
	int *thr_id;
	int *status;
	int cnt = 0;

    processInit(&trainProc, 4, 
                trainData->info.numberOfColumns * trainData->info.numberOfRows,
                512,
                64,
                10,
                // ACTIVATION_SIGMOID
                ACTIVATION_RELU
    );

    processInit(&combProc, 4, 
                trainData->info.numberOfColumns * trainData->info.numberOfRows,
                512,
                64,
                10,
                // ACTIVATION_SIGMOID
                ACTIVATION_RELU
    );

    threadCnt = getCoreNumber();

	p_thread = (pthread_t*)malloc(sizeof(pthread_t)*threadCnt);
	thr_id = (int*)malloc(sizeof(int)*threadCnt);
	status = (int*)malloc(sizeof(int)*threadCnt);
    procThread = (Process*)malloc(sizeof(Process) * threadCnt);

    setInitInputDataSet(&combProc, trainData, trainLabel);
    //loadWeight(&combProc);

	for (cnt = 0; cnt < threadCnt; cnt++)
	{
        processInit(&procThread[cnt], 4, 
                trainData->info.numberOfColumns * trainData->info.numberOfRows,
                512,
                64,
                10,
                // ACTIVATION_SIGMOID
                ACTIVATION_RELU
        );
        //loadWeight(&procThread[cnt]);
		thr_id[cnt] = pthread_create(&p_thread[cnt], NULL, annProcess, (void*)&procThread[cnt]);
	}

	for (cnt = 0; cnt < threadCnt; cnt++)
	{
		pthread_join(p_thread[cnt], (void **)&status[cnt]);
	}

	free(p_thread);
	free(thr_id);
	free(status);

    trainProc.load(&trainProc, NULL);

    while(1)
    {
        for(epochIdx = 0; epochIdx < 100; epochIdx++)
        {
            trainProc.error = 0.;
            for(imgIdx = 0; imgIdx < trainData->info.numberOfImage; imgIdx++)
            {
                trainProc.trainning(&trainProc, trainData->data[imgIdx], (int)trainLabel->label[imgIdx]);
                if((imgIdx) % 1000 == 0)
                {
                    if(imgIdx == 0)
                        printf("[%d]%d\t\t%lf\n",epochIdx, imgIdx, trainProc.error);
                    else
                        printf("[%d]%d\t\t%lf\n",epochIdx, imgIdx, trainProc.error/imgIdx);
                }
            }
            trainProc.error /= imgIdx;
            if(trainProc.error < 0.01)
                break;
        }
        updateWeightThread(&trainProc);
        if((combProc.error/trainData->info.numberOfImage) < combProc.epsilon)
            break;
    }

    trainProc.save(&trainProc, NULL);
    processDeinit(&trainProc);

    // test with saved weight
    processInit(&testProc, 4,
                trainData->info.numberOfColumns * trainData->info.numberOfRows,
                512,
                64,
                10,
                // ACTIVATION_SIGMOID
                ACTIVATION_RELU

    );

    testProc.load(&testProc, NULL);

    {
        int _correct = 0;
        int idx;
        double accuracy;
        int predict = 0;

        for(imgIdx = 0; imgIdx < testData->info.numberOfImage; imgIdx++)
        {
            testProc.predict(&testProc, testData->data[imgIdx], (int)testLabel->label[imgIdx]);

            predict = 0;
            for(idx = 0; idx < testProc.pLayerCount[testProc.Layer]; idx++)
            {
                if(testProc.hidden[testProc.Layer - 1].val[idx] > testProc.hidden[testProc.Layer - 1].val[predict])
                {
                    predict = idx;
                }
            }
            printf("predict : %d\treal : %d(%lf)\n", predict, testLabel->label[imgIdx], testProc.hidden[testProc.Layer - 1].val[predict]);
            if(predict == testLabel->label[imgIdx])
            {
                _correct++;
            }
        }

        accuracy = (double)_correct / testLabel->info.numberOfItems * 100.f;
        printf("accuracy : %lf\n", accuracy);
    }

    for (cnt = 0; cnt < threadCnt; cnt++)
	{
        processDeinit(&procThread[cnt]);
	}

    processDeinit(&testProc);
    free(p_thread);
    free(thr_id);
    free(status);
    free(procThread);

}


void processCascade(pImageSetFile trainData, pLabelSetFile trainLabel, pImageSetFile testData, pLabelSetFile testLabel)
{
    Process trainProc;
    Process testProc;
    int imgIdx = 0;
    int epochIdx = 0;

    processInit(&trainProc, 4, 
                trainData->info.numberOfColumns * trainData->info.numberOfRows,
                512,
                64,
                10,
                // ACTIVATION_SIGMOID
                ACTIVATION_RELU
    );

    trainProc.load(&trainProc, NULL);

    // trainning
    for(epochIdx = 0; epochIdx < 100; epochIdx++)
    {
        trainProc.error = 0.;
        for(imgIdx = 0; imgIdx < trainData->info.numberOfImage; imgIdx++)
        {
            trainProc.trainning(&trainProc, trainData->data[imgIdx], (int)trainLabel->label[imgIdx]);

            if((imgIdx) % 1000 == 0)
            {
                if(imgIdx == 0)
                    printf("[%d]%d\t\t%lf\n",epochIdx, imgIdx, trainProc.error);
                else
                    printf("[%d]%d\t\t%lf\n",epochIdx, imgIdx, trainProc.error/imgIdx);
            }
        }
        trainProc.error /= imgIdx;
        if(trainProc.error < trainProc.epsilon)
            break;
    }

    trainProc.save(&trainProc, NULL);

    processDeinit(&trainProc);

    // test with saved weight
    processInit(&testProc, 4,
                trainData->info.numberOfColumns * trainData->info.numberOfRows,
                512,
                64,
                10,
                // ACTIVATION_SIGMOID
                ACTIVATION_RELU
    );

    testProc.load(&testProc, NULL);

    {
        int _correct = 0;
        int idx;
        double accuracy;
        int predict = 0;

        for(imgIdx = 0; imgIdx < testData->info.numberOfImage; imgIdx++)
        {
            testProc.predict(&testProc, testData->data[imgIdx], (int)testLabel->label[imgIdx]);

            predict = 0;
            for(idx = 0; idx < testProc.pLayerCount[testProc.Layer]; idx++)
            {
                if(testProc.hidden[testProc.Layer - 1].val[idx] > testProc.hidden[testProc.Layer - 1].val[predict])
                {
                    predict = idx;
                }
            }
            printf("predict : %d\treal : %d(%lf)\n", predict, testLabel->label[imgIdx], testProc.hidden[testProc.Layer - 1].val[predict]);
            if(predict == testLabel->label[imgIdx])
            {
                _correct++;
            }
        }

        accuracy = (double)_correct / testLabel->info.numberOfItems * 100.f;
        printf("accuracy : %lf\n", accuracy);
    }

    processDeinit(&testProc);

}