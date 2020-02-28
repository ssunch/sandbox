#ifndef _MNIST_HWN_H_
#define _MNIST_HWN_H_

typedef struct _IMAGESET_FILE_
{
    int magicNumber;
    int numberOfImage;
    int numberOfRows;
    int numberOfColumns;
    unsigned char **data;
}ImageSetFile, *pImageSetFile;

typedef struct _LABELSET_FILE_
{
    int magicNumber;
    int numberOfItems;
    unsigned char *label;
}LabelSetFile, *pLabelSetFile;

#endif