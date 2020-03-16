#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#include "include/linkedlist.h"
#include "include/mnist_hwn.h"
#include "include/random.h"
#include "include/ann.h"


int main(char argc, char *argv[])
{
    ImageSetFile trainningData;
    LabelSetFile trainningLabel;

    ImageSetFile testData;
    LabelSetFile testLabel;

    Process trainProc;
    Process testProc;
    int imgIdx = 0;
    int epochIdx = 0;

    if(argc < 5)
    {
        printf("usage : \n$%s {trainning image Set} {trainning label Set} {test image Set} {test label Set}\n", argv[2]);
    }

    printf("Starting '%s'\n", (argv[0][0] == '.') ? &argv[0][2]: argv[0]);

    readImageSetFile(argv[1], &trainningData);
    readLabelSetFile(argv[2], &trainningLabel);

    readImageSetFile(argv[3], &testData);
    readLabelSetFile(argv[4], &testLabel);

    // processInit(&trainProc, 4, 
    //             trainningData.info.numberOfColumns * trainningData.info.numberOfRows,
    //             512,
    //             64,
    //             10
    // );

    processInit(&trainProc, 3,
                trainningData.info.numberOfColumns * trainningData.info.numberOfRows,
                128,
                10
    );

    // trainning
    for(epochIdx = 0; epochIdx < 100; epochIdx++)
    {
        trainProc.error = 0.;
        for(imgIdx = 0; imgIdx < trainningData.info.numberOfImage; imgIdx++)
        {
            processUpdateInput(&trainProc, trainningData.data[imgIdx], (int)trainningLabel.label[imgIdx]);
            perception(&trainProc);
            updateWeight(&trainProc);

            squareError(&trainProc);
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

    saveWeight(&trainProc);
    processDeinit(&trainProc);

    // test with saved weight
    processInit(&testProc, 3,
                trainningData.info.numberOfColumns * trainningData.info.numberOfRows,
                128,
                10
    );

    loadWeight(&testProc);

    {
        int _correct = 0;
        int idx;
        double accuracy;
        int predict = 0;

        for(imgIdx = 0; imgIdx < testData.info.numberOfImage; imgIdx++)
        {
            processUpdateInput(&testProc, testData.data[imgIdx], (int)testLabel.label[imgIdx]);
            perception(&testProc);

            squareError(&testProc);
            predict = 0;
            for(idx = 0; idx < testProc.pLayerCount[testProc.Layer -1]; idx++)
            {
                if(testProc.hidden[testProc.Layer - 1].val[idx] > testProc.hidden[testProc.Layer - 1].val[predict])
                {
                    predict = idx;
                }
            }
            printf("predict : %d\treal : %d(%lf)\n", predict, testLabel.label[imgIdx], testProc.hidden[testProc.Layer - 1].val[predict]);
            if(predict == testLabel.label[imgIdx])
            {
                _correct++;
            }
        }

        accuracy = (double)_correct / testLabel.info.numberOfItems * 100.f;
        printf("accuracy : %lf\n", accuracy);
    }

    releaseImageSetFile(&trainningData);
    releaseLabelSetFile(&trainningLabel);

    releaseImageSetFile(&testData);
    releaseLabelSetFile(&testLabel);
    return 0;
}
