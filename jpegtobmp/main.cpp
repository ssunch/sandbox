#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/jpeglib.h"
#include "include/bmpinfo.h"

/* we will be using this uninitialized pointer later to store raw, uncompressd image */
unsigned char *raw_image = NULL;

/* dimensions of the image we want to write */
int width;
int height;
int bytes_per_pixel;   /* or 1 for GRACYSCALE images */
int color_space; /* or JCS_GRAYSCALE for grayscale images */

char *get_filename_ext(char *filename)
{
	char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) {
		return "";
	}
	return dot + 1;
}

int write_bmp_file( char *filename )
{
    BMPFileHeader bfile;
    BMPInfoHeader binfo;

    int bytesPerLine;
    int padding = getPadding(width, 24);

    bfile.bfType = ((DWORD)'M' << 8);
    bfile.bfType |= 'B';

    bfile.bfSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + ((width + padding) * height * 3);
    bfile.bfOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

    binfo.biSize = sizeof(BMPInfoHeader);
    binfo.biWidth = width;
    binfo.biHeight = height;
    binfo.biplanes = 1;
    binfo.biBitCount = 24;
    binfo.biCompression = 0;
    binfo.biSizeImage = ((width + padding) * height * 3);



    bytesPerLine = width * 3;  /* (for 24 bit images) */
    /* round up to a dword boundary */
    if (bytesPerLine & 0x0003) 
    {
        bytesPerLine |= 0x0003;
        ++bytesPerLine;
    }

    FILE * fp;

    printf("Bytes per line : %d\n", bytesPerLine);

    fp = fopen(filename, "wb");
    if (fp == NULL)
        errorReturn(fp);

    if(fwrite(&bfile, 1, sizeof(BMPFileHeader), fp) < 1)
            errorReturn(fp);
    if(fwrite(&binfo, 1, sizeof(BMPInfoHeader), fp) < 1)
            errorReturn(fp);
    
    char *linebuf;

    linebuf = (char *) calloc(1, bytesPerLine);
    if (linebuf == NULL)
    {
        printf ("Error allocating memory\n");
        free(raw_image);
        /* -- close all open files and free any allocated memory -- */
        exit (1);   
    }


    int line,x;

    for (line = height-1; line >= 0; line --)
    {
        /* fill line linebuf with the image data for that line */
        for( x =0 ; x < width; x++ )
        {
            *(linebuf+x*bytes_per_pixel) = *(raw_image+(x+line*width)*bytes_per_pixel+2);
            *(linebuf+x*bytes_per_pixel+1) = *(raw_image+(x+line*width)*bytes_per_pixel+1);
            *(linebuf+x*bytes_per_pixel+2) = *(raw_image+(x+line*width)*bytes_per_pixel+0);
        }

        /* remember that the order is BGR and if width is not a multiple
            of 4 then the last few bytes may be unused
        */
        fwrite(linebuf, 1, bytesPerLine, fp);
    }
    free(linebuf);
    fclose(fp);



}



/**
 * read_jpeg_file Reads from a jpeg file on disk specified by filename and saves into the 
 * raw_image buffer in an uncompressed format.
 * 
 * \returns positive integer if successful, -1 otherwise
 * \param *filename char string specifying the file name to read from
 *
 */

int read_jpeg_file( char *filename )
{
    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW row_pointer[1];

    FILE *infile = fopen( filename, "rb" );
    unsigned long location = 0;
    int i = 0;

    if ( !infile )
    {
        printf("Error opening jpeg file %s\n!", filename );
        return -1;
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

    printf( "JPEG File Information: \n" );
    printf( "Image width and height: %d pixels and %d pixels.\n", width=cinfo.image_width, height=cinfo.image_height );
    printf( "Color components per pixel: %d.\n", bytes_per_pixel = cinfo.num_components );
    printf( "Color space: %d.\n", cinfo.jpeg_color_space );

    /* Start decompression jpeg here */
    jpeg_start_decompress( &cinfo );

    /* allocate memory to hold the uncompressed image */
    raw_image = (unsigned char*)malloc( cinfo.output_width*cinfo.output_height*cinfo.num_components );
    /* now actually read the jpeg into the raw buffer */
    row_pointer[0] = (unsigned char *)malloc( cinfo.output_width*cinfo.num_components );
    /* read one scan line at a time */
    while( cinfo.output_scanline < cinfo.image_height )
    {
        jpeg_read_scanlines( &cinfo, row_pointer, 1 );
        for( i=0; i<cinfo.image_width*cinfo.num_components;i++) 
            raw_image[location++] = row_pointer[0][i];
    }
    /* wrap up decompression, destroy objects, free pointers and close open files */
    jpeg_finish_decompress( &cinfo );
    jpeg_destroy_decompress( &cinfo );
    free( row_pointer[0] );
    fclose( infile );

    return 1;
}


int main(int argc,char **argv)
{
    char outFilename[FILENAME_MAX] = "";

	strcpy(outFilename, argv[1]);
	strcpy(get_filename_ext(outFilename), "bmp");
    
    if(argc != 2)
    {
        printf("Usage: %s source.jpg\n",argv[0]); return -1; 
    }
    /* Try opening a jpeg*/

    if(read_jpeg_file( argv[1]) > 0)
        write_bmp_file(outFilename);

    else return -1;

    free(raw_image);

    return 0;
}