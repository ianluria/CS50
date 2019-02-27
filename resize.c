// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

float roundDecimal(float factorDecimal);

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize f infile outfile\n");
        return 1;
    }

    // Get floating point number for f
    float factor = 0;
    sscanf(argv, "%*s %f", &factor);

    // A factor of 0 is the same as a factor of 1: return an exact copy of the image 
    if (factor <= 0)
        factor = 1;

    // Rounds any decimal to a quarter
    float factorDecimal = roundDecimal(factor % 1);

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER fileHeader;
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER infoHeader;
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (fileHeader.bfType != 0x4d42 || fileHeader.bfOffBits != 54 || infoHeader.biSize != 40 ||
        infoHeader.biBitCount != 24 || infoHeader.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // Make a temporary copy of fileHeader
    BITMAPFILEHEADER tempFileHeader;
    tempFileHeader = fileHeader;

    // Make a temporary copy of infoHeader
    BITMAPINFOHEADER tempInfoHeader;
    tempInfoHeader = infoHeader;

    // Change the width and height of image by factor
    tempInfoHeader.biWidth *= factor;
    tempInfoHeader.biHeight *= factor;

    // Determine padding for scanlines
    int padding = (4 - (tempInfoHeader.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    tempInfoHeader.biSizeImage = ((sizeof(RGBTRIPLE) * tempInfoHeader.biWidth) + padding) * abs(tempInfoHeader.biHeight);

    tempFileHeader.bfSize = tempInfoHeader.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&tempFileHeader, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&tempInfoHeader, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over original infile's scanlines
    for (int i = 0, biHeight = abs(infoHeader.biHeight); i < biHeight; i++)
    {

        //write each scanline * the factor
        for (int j = 0; j < factor)

            // iterate over pixels in scanline
            for (int pixel = 0; pixel < infoHeader.biWidth; pixel++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // Try changing size_t nmemb to factor variable

                // write RGB triple to outfile
                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}

float roundDecimal(float factorDecimal)
{
    if (factorDecimal > 0.75)
        return 1.0; 
    else if (factorDecimal > 0.5)
        return 0.75;
    else if (factorDecimal > 0.25)
        return 0.50;
    else if (factorDecimal > 0)
        return 0.25;
    else
        return 0;
}