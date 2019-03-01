// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize f infile outfile\n");
        return 1;
    }

    // Get floating point number for factor
    float factor = 0;
    sscanf(argv[1], "%f", &factor);

    // A factor of 0 is the same as a factor of 1: return an exact copy of the image
    // A factor less than 1 is 0.5
    // A factor 1 or greater is rounded to nearest integer
    if (factor <= 0)
        factor = 1;
    else if (factor < 1)
        factor = 0.5;
    else if (factor > 100.0)
        factor = 100.0;
    else
        factor = roundf(factor);

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

    // Round width and height if factor is less than 1
    if (factor == 0.5)
    {
        tempInfoHeader.biWidth = round(tempInfoHeader.biWidth * factor);
        tempInfoHeader.biHeight = round(tempInfoHeader.biHeight * factor);
    }
    else
    {
        tempInfoHeader.biWidth *= factor;
        tempInfoHeader.biHeight *= factor;
    }

    // Determine padding for scanlines
    int oldPadding = (4 - (infoHeader.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int newPadding = (4 - (tempInfoHeader.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    tempInfoHeader.biSizeImage = ((sizeof(RGBTRIPLE) * tempInfoHeader.biWidth) + newPadding) * abs(tempInfoHeader.biHeight);

    tempFileHeader.bfSize = tempInfoHeader.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&tempFileHeader, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&tempInfoHeader, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Iterate over original infile's scanlines
    for (int scanline = 0, biHeight = abs(infoHeader.biHeight); scanline < biHeight; scanline++)
    {
        // Array used to hold a new scanline of pixels
        RGBTRIPLE scanlineArray[tempInfoHeader.biWidth];

        int scanlineArrayIndex = 0;

        // iterate over pixels in original scanline
        for (int pixel = 0; pixel < infoHeader.biWidth; pixel++)
        {
            // temporary storage
            RGBTRIPLE triple;
            RGBTRIPLE evenTriple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // Write the pixel factor times into new scanline array
            for (int k = 0; k < factor; k++)
            {
                bool addPixel = true;

                if (factor == 0.5)
                {
                    if ((pixel + 1) % 2 == 0)
                    {
                        evenTriple = triple;
                        addPixel = false;
                    }
                    // Do not average pixel colors if it is the first pixel in the scanline, just add it directly to array
                    else if (pixel + 1 != 1)
                    {
                        // Average the color of the pixel before the current pixel and the current pixel
                        triple.rgbtBlue = (evenTriple.rgbtBlue + triple.rgbtBlue) / 2;
                        triple.rgbtGreen = (evenTriple.rgbtGreen + triple.rgbtGreen) / 2;
                        triple.rgbtRed = (evenTriple.rgbtRed + triple.rgbtRed) / 2;
                    }
                }

                if (addPixel)
                {
                    // Add triple into the array
                    scanlineArray[scanlineArrayIndex] = triple;
                    scanlineArrayIndex += 1;
                }
            }
        }

        // skip over padding in original file, if any
        fseek(inptr, oldPadding, SEEK_CUR);

        RGBTRIPLE evenScanlineArray[tempInfoHeader.biWidth];

        // Write a new scanline by factor to output
        for (int j = 0; j < factor; j++)
        {
            bool writeToFile = true;

            if (factor == 0.5)
            {
                if ((scanline + 1) % 2 == 0)
                {

                    memcopy(evenScanlineArray, scanlineArray, sizeof(RGBTRIPLE) * tempInfoHeader.biwidth);
                    writeToFile = false;
                }
                else if (scanline + 1 != 1)
                {

                    for (size_t k = 0, len = sizeof(scanlineArray) / sizeof(scanlineArray[0]); k < len; k +)
                    {
                        scanlineArray[k] = triple;
                        evenScanlineArray[k] = evenTriple;

                        triple.rgbtBlue = (evenTriple.rgbtBlue + triple.rgbtBlue) / 2;
                        triple.rgbtGreen = (evenTriple.rgbtGreen + triple.rgbtGreen) / 2;
                        triple.rgbtRed = (evenTriple.rgbtRed + triple.rgbtRed) / 2;
                    }

                    //for each pixel in evanScanlineArray
                    //match it with pixel in scanlineArray
                    // average the colors and store it back in scanlineArray
                }
            }
            //if factor is 0.5
            //If first line, write to file
            //If even numbered line, store to temporary storage and do not write to file
            //If odd numbered line, average each pixel from even with odd and add averaged pixels to an array

            // write each pixel from scanlineArray to output
            for (size_t k = 0, len = sizeof(scanlineArray) / sizeof(scanlineArray[0]); k < len; k++)
            {
                const RGBTRIPLE *pixel = &scanlineArray[k];

                fwrite(pixel, sizeof(RGBTRIPLE), 1, outptr);
            }

            // Add newPadding
            for (int k = 0; k < newPadding; k++)
            {
                fputc(0x00, outptr);
            }
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
