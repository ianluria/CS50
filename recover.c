#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

typedef unsigned char BYTE;

bool findJPEG(int *counter, FILE *inptr);
int testForJPEGSignature(BYTE testByte, int index, FILE *inptr, int *rewindCounter);

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

            printf("jpeg: %i\n", newJpegCounter);

            newJpegCounter += 1;

            FILE *jpegImg = fopen(jpegFilename, "w");

            if (jpegImg == NULL)
            {
                fprintf(stderr, "Could not open jpeg.\n");
                return 2;
            }

            //jpegs can take up for than one block
            //test first bytes of sequential block for jpeg signature
            //if no signature is found, add it on block before
            //keep testing until a new signature is found, or end of file is reached
            //write complete jpeg to new file

            int number = 512 * jpegBlockCounter;

            int writeCount = fwrite(inptr, 1, number, jpegImg);

            if (writeCount < number)
            {
                return 3;
            }

            jpegBlockCounter = 0;

            int close = fclose(jpegImg);

            if (close != 0)
            {
                return 5;
            }
        }
        else
        {
            // End of file reached
            nextJpegBlock = false;
        }
    }

    int inptrClose = fclose(inptr);

    if (inptrClose != 0)
    {
        return 5;
    }

    return 0;
}

bool findJPEG(int *jpegBlockCounter, FILE *inptr)
{
    bool creatingJpeg = true;

    while (creatingJpeg)
    {
        BYTE testByte = 0;
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
        int signatureFound = testForJPEGSignature(testByte, 0, inptr, &rewindCounter);

        // Reached end of file or error
        if (signatureFound == 0)
        {
            return false;
        }

        // If jpegBlockCounter is greater than zero, make sure file rewinds to beginning of block
        if (jpegBlockCounter > 0)
        {
            if (rewindCounter < 4)
            {
                rewindCounter += 1;
            }
        }

        //rewind file if found more than one part of signature
        rewindCounter *= -1;

        fseek(inptr, rewindCounter, SEEK_CUR);

        // Signature found!
        if (signatureFound == 2)
        {
            // Return true if a new jpeg signature was found while processing an existing jpeg
            if (*jpegBlockCounter > 0)
            {
                return true;
            }

            *jpegBlockCounter += 1;

            // Move forward 512 bytes in file to next block
            int seek = fseek(inptr, 512, SEEK_CUR);

            // End of file or error
            if (seek < 0)
            {
                return false;
            }
        }
        else if (signatureFound == 1)
        {
            // Keep incrementing jpegBlockCounter until another jpeg signature is found
            if (*jpegBlockCounter > 0)
            {
                // Move forward 512 bytes in file to next block
                int seek = fseek(inptr, 512, SEEK_CUR);

                // End of file or error
                if (seek < 0)
                {
                    return false;
                }
            }
        }
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