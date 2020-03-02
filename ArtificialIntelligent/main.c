#include <stdio.h>
//#include <stdlib.h>

//#include "include/linkedlist.h"
#include "include/mnist_hwn.h"



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

