#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

typedef char BYTE;

// get rid of this

bool findJPEG(int counter, FILE *inptr);
int testForJPEGSignature(int index, FILE *inptr, int *rewindCounter);

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover file.jpg\n");
        return 1;
    }

    char *infile = argv[1];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    bool nextJpegBlock = true;
    int newJpegCounter = 0;
    int jpegBlockCounter = 0;
    char jpegFilename[8];

    while (nextJpegBlock)
    {

        bool foundJPEG = findJPEG(&jpegBlockCounter, inptr);

        if (foundJPEG)
        {
            sprintf(jpegFilename, "%03i.jpg", newJpegCounter);

            newJpegCounter += 1;

            FILE *jpegImg = fopen(jpegFilename, "w");

            //jpegs can take up for than one block
            //test first bytes of sequential block for jpeg signature
            //if no signature is found, add it on block before
            //keep testing until a new signature is found, or end of file is reached
            //write complete jpeg to new file

            //FIX THIS!!!

            fwrite(, sizeof(jpeg.jpegArray), *jpegBlockCounter, jpegImg);

            *jpegBlockCounter = 0;

            fclose(jpegImg);
        }
        else
        {
            // End of file reached
            nextJpegBlock = false;
        }
    }

    fclose(inptr);

    return 0;
}

bool findJPEG(int *jpegBlockCounter, FILE *inptr)
{
    BYTE testByte;
    int rewindCounter = 0;

    // Only look for non-zero byte if function is not currently processing an existing jpeg
    if (*jpegBlockCounter == 0)
    {
        // Read through file until a non-zero byte is found
        while (testByte == 0)
        {
            size_t read = fread(&testByte, sizeof(BYTE), 1, inptr);

            // End of file or error reached while reading the next byte from file
            if (read == 0)
            {
                return false;
            }
        }
    }
    // Else, just test the first byte for a signature.
    else
    {
        size_t read = fread(&testByte, sizeof(BYTE), 1, inptr);

        // End of file or error reached while reading the next byte from file
        if (read == 0)
        {
            return false;
        }
    }

    // Test testByte for signature
    int signatureFound = testForJPEGSignature(testByte, 0, *inptr, &rewindCounter);

    // Reached end of file or error
    if (signatureFound == 0)
    {
        return false;
    }

    //rewind file
    rewindCounter *= -1;

    fseek(inptr, rewindCounter, SEEK_CUR);

    // Signature found!
    if (signatureFound == 2)
    {

        *jpegBlockCounter += 1;

        // Move forward 512 bytes in file to next block
        int seek = fseek(inptr, 512, SEEK_CUR);

        // End of file or error
        if (seek < 0)
        {
            return false;
        }

        findJPEG(jpegBlockCounter, inptr);

        return true;
        // Run findJPEG again to see if next block is part of same jpeg
    }
    else if (signatureFound == 1)
    {
        // Return true if at least one block was found
        if (*jpegBlockCounter > 0)
        {
            return true;
        }

        // Run function again for next byte
        findJPEG(jpegBlockCounter, inptr);
    }

    return false;
}

//Given the current location in file, check if the next four bytes are a jpeg signature
//Returns 0 for end of file or read error
//Returns 1 for byte does not match signature
//Returns 2 for complete signature found

int testForJPEGSignature(BYTE testByte, int index, FILE *inptr, int *rewindCounter)
{
    const int JPEGSIGNATUREBYTES[] = {255, 216, 255};

    // Test the parameter testByte before testing a byte from file
    if (index > 0)
    {
        size_t read = fread(&testByte, sizeof(BYTE), 1, inptr);

        // End of file or error reached while reading the next byte from file
        if (read == 0)
        {
            return 0;
        }
    }

    // Passed last index in JPEGSIGNATUREBYTES
    if (index == 3)
    {
        // Complete match of signature
        if ((testByte & 240) == 224)
        {
            *rewindCounter = 4;
            return 2;
        }
        else
        {
            *rewindCounter += 1;
            return 1;
        }
    }

    if (testByte == JPEGSIGNATUREBYTES[index])
    {

        // Tally how many bytes need to be rewound
        if (index > 0)
        {
            *rewindCounter += 1;
        }

        index += 1;

        int foundNextSignatureByte = testForJPEGSignature(testByte, index, inptr, rewindCounter);

        if (foundNextSignatureByte == 2)
        {
            return 2;
        }
        else if (foundNextSignatureByte == 0)
        {
            return 0;
        }
    }

    return 1;
}