#include <stdlib.h>
#include <string.h>
#include "include/jpginfo.h"

static Image jpgImage;

ErrorState openJPGFile(char *file)
{
    ErrorState err = ErrorNone;
    unsigned char* image;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen( file, "rb" );
    unsigned long location = 0;
    int i = 0;

    if ( !infile )
    {
        printf("Error opening jpeg file %s\n!", file );
        err = ErrorOpen;
        return err;
    }
    /* here we set up the standard libjpeg error handler */
    cinfo.err = jpeg_std_error( &jerr );
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress( &cinfo );
    /* this makes the library read from infile */
    jpeg_stdio_src( &cinfo, infile );
    /* reading the image header which contains image information */
    jpeg_read_header( &cinfo, TRUE );
    /* Uncomment the following to output image information, if needed. */

    jpgImage.bitCount = cinfo.num_components * 8;
    jpgImage.width = cinfo.image_width;
    jpgImage.height = cinfo.image_height;

    printf( "JPEG File Information: \n" );
    printf( "Image width and height: %d pixels and %d pixels.\n", cinfo.image_width, cinfo.image_height );
    printf( "Color components per pixel: %d.\n", cinfo.num_components * 8 );
    printf( "Color space: %d.\n", cinfo.jpeg_color_space );

    /* Start decompression jpeg here */
    jpeg_start_decompress( &cinfo );

    /* allocate memory to hold the uncompressed image */
    image = (unsigned char*)malloc( cinfo.output_width*cinfo.output_height*cinfo.num_components );
    jpgImage.image = (pRGB)malloc( cinfo.output_width*cinfo.output_height);
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *)malloc( cinfo.output_width*cinfo.num_components );
    /* read one scan line at a time */
    while( cinfo.output_scanline < cinfo.image_height )
    {
        jpeg_read_scanlines( &cinfo, row_pointer, 1 );
        for( i=0; i<cinfo.image_width*cinfo.num_components;i++) 
            image[location++] = row_pointer[0][i];
    }
    memcpy(jpgImage.image, image, cinfo.output_width*cinfo.output_height*cinfo.num_components );
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress( &cinfo );
    jpeg_destroy_decompress( &cinfo );
    free( row_pointer[0] );
    fclose( infile );

    return err;
}

Image *getJPGInfo(void)
{
    return &jpgImage;
}