#ifndef _MNIST_HWN_H_
#define _MNIST_HWN_H_

typedef struct _IMAGESET_FILE_INFO_
{
    int magicNumber;
    int numberOfImage;
    int numberOfRows;
    int numberOfColumns;
}ImageSetFileInfo;

typedef struct _IMAGESET_FILE_
{
    ImageSetFileInfo info;
    unsigned char **data;
}ImageSetFile, *pImageSetFile;

typedef struct _LABELSET_FILE_INFO_
{
    int magicNumber;
    int numberOfItems;
}LabelSetFileInfo;

typedef struct _LABELSET_FILE_
{
    LabelSetFileInfo info;
    unsigned char *label;
}LabelSetFile, *pLabelSetFile;

#endif