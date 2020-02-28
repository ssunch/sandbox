#include <stdio.h>
#include <stdlib.h>

//#include "include/linkedlist.h"
#include "include/mnist_hwn.h"

#define errorReturn(FILE_POINTER) \
do{ \
printf("*error occured in %s:%d\n", __FILE__ , __LINE__); \
fclose(FILE_POINTER); \
return; \
} while(0);

void readImageSetFile(char *path, pImageSetFile _info);
void readLabelSetFile(char *path, pLabelSetFile _info);

int main(char argc, char *argv[])
{
    ImageSetFile trainningData;
    LabelSetFile trainningLabel;



    printf("Starting '%s'\n", (argv[0][0] == '.') ? &argv[0][2]: argv[0]);
    readImageSetFile(argv[1], &trainningData);
    readLabelSetFile(argv[2], &trainningLabel);

    return 0;
}

void readImageSetFile(char *path, pImageSetFile _info)
{
    FILE *pf = NULL;
    int i;
    int sum = 0;

    pf = fopen(path, "rb");

    if(fread(&_info->info, sizeof(ImageSetFileInfo), 1, pf) < 0)
        errorReturn(pf);
    _info->info.magicNumber = be32toh(_info->info.magicNumber);
    _info->info.numberOfImage = be32toh(_info->info.numberOfImage);
    _info->info.numberOfRows = be32toh(_info->info.numberOfRows);
    _info->info.numberOfColumns = be32toh(_info->info.numberOfColumns);

    if(_info->info.magicNumber != 0x00000803) // 2051 magic number MSB
    {
        return;
    }

    _info->data = (unsigned char**)malloc(_info->info.numberOfImage);
    for(i = 0; i < _info->info.numberOfImage; i++)
    {
        _info->data[i] = (unsigned char*)malloc(_info->info.numberOfRows * _info->info.numberOfColumns);
        if(fread(_info->data[i],sizeof(unsigned char), _info->info.numberOfRows * _info->info.numberOfColumns, pf) < 0)
            errorReturn(pf);
        sum += _info->info.numberOfRows * _info->info.numberOfColumns;
    }

    fclose(pf);
    pf = NULL;
}


void readLabelSetFile(char *path, pLabelSetFile _info)
{
    FILE * pf = NULL;

    pf = fopen(path, "rb");

    fread(&_info->info, sizeof(LabelSetFile), 1, pf);
    _info->info.magicNumber = be32toh(_info->info.magicNumber);
    _info->info.numberOfItems = be32toh(_info->info.numberOfItems);
    if(_info->info.magicNumber != 0x00000801) // 2049 magic number MSB
    {
        return;
    }
    _info->label = (unsigned char*)malloc(_info->info.numberOfItems);

    fread(_info->label, sizeof(unsigned char), _info->info.numberOfItems, pf);

    fclose(pf);
}