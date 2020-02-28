#include <stdio.h>
#include <stdlib.h>

#include "include/linkedlist.h"
#include "include/mnist_hwn.h"

void readImageSetFile(char *path, pImageSetFile info);
void readLabelSetFile(char *path, pLabelSetFile info);

int main(char argc, char *argv[])
{
    ImageSetFile trainningData;
    LabelSetFile trainningLabel;



    printf("Starting '%s'\n", (argv[0][0] == '.') ? &argv[0][2]: argv[0]);
    readImageSetFile(argv[1], &trainningData);
    readLabelSetFile(argv[2], &trainningLabel);

    return 0;
}

void readImageSetFile(char *path, pImageSetFile info)
{
    FILE *pf = NULL;
    int i;

    pf = fopen(path, "rb");

    fread(info, sizeof(ImageSetFile) - 4, 1, pf);
    if(info->magicNumber != 0x00000803) // 2051 magic number MSB
    {
        return;
    }

    info->data = (unsigned char**)malloc(info->numberOfImage);
    for(i = 0; i < info->numberOfImage; i++)
    {
        info->data[i] = (unsigned char*)malloc(info->numberOfRows * info->numberOfColumns);
        fread(info->data[i],sizeof(unsigned char), info->numberOfRows * info->numberOfColumns, pf);
    }

    fclose(pf);
}


void readLabelSetFile(char *path, pLabelSetFile info)
{
    FILE * pf = NULL;

    pf = fopen(path, "rb");

    fread(info, sizeof(LabelSetFile) - 4, 1, pf);
    if(info->magicNumber != 0x00000801) // 2049 magic number MSB
    {
        return;
    }
    info->label = (unsigned char*)malloc(info->numberOfItems);

    fread(info->label, sizeof(unsigned char), info->numberOfItems, pf);

    fclose(pf);
}