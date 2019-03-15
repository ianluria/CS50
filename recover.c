#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

typedef unsigned char BYTE;

int createNewJpegFile(int newJpegCounter, FILE *jpegImageFile);
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
    FILE *jpegImageFile;

    while (nextJpegBlock)
    {

        int rewindCounter = 0;
        bool appendToFile = false;

        // Test for signature sequence in current position of inptr
        int signatureFound = testForJPEGSignature(0, inptr, &rewindCounter);

        // Error found in inptr
        if (signatureFound == 0)
        {
            return 5;
        }

        // End of file reached
        if (signatureFound == 3)
        {
            break;
        }

        rewindCounter *= -1;

        //return to beginning of block
        int rewindSeek = fseek(inptr, rewindCounter, SEEK_CUR);

        // End of file or error
        if (rewindSeek < 0)
        {
            int end = feof(inptr);
            int error = ferror(inptr);

            if (end > 0)
            {
                break;
            }

            if (error > 0)
            {
                return 5;
            }
        }

        // Signature found!
        if (signatureFound == 2)
        {
            printf("Signature found:  %lu\n", ftell(inptr));

            // if a new jpeg signature was found while processing an existing jpeg close current file
            if (jpegBlockCounter > 0)
            {
                int jpegClose = fclose(jpegImageFile);

                if (jpegClose != 0)
                {
                    return 5;
                }

                jpegBlockCounter = 0;
            }

            //create new jpeg file
            int newFile = createNewJpegFile(newJpegCounter, jpegImageFile);

            // Error creating a new jpeg file
            if (newFile == 2)
            {
                return 2;
            }

            newJpegCounter += 1;

            appendToFile = true;
        }
        else if (signatureFound == 1)
        {
            // If a jpeg file is being built, append this block to it
            if (jpegBlockCounter > 0)
            {
                appendToFile = true;
            }
        }

        // Read block into array while advancing inptr
        BYTE blockArray[512];

        size_t read = fread(blockArray, sizeof(blockArray) / sizeof(BYTE), 1, inptr);

        // End of file or error reached while reading the next byte from file
        if (read < 1)
        {
            int end = feof(inptr);
            int error = ferror(inptr);

            if (end > 0)
            {
                break;
            }

            if (error > 0)
            {
                return 3;
            }
        }

        if (appendToFile)
        {
            size_t writeCount = fwrite(blockArray, 512, 1, &jpegImageFile);

            // Error
            if (writeCount < sizeof(blockArray) / sizeof(blockArray[0]))
            {
                int end = feof(&jpegImageFile);
                int error = ferror(&jpegImageFile);

                if (end > 0)
                {
                    return 3;
                }

                if (error > 0)
                {
                    return 3;
                }
            }

            jpegBlockCounter += 1;
        }
    }

    int inptrClose = fclose(inptr);

    if (inptrClose != 0)
    {
        return 5;
    }

    return 0;
}

// Returns 1 for success
// Returns 2 for failure
int createNewJpegFile(int newJpegCounter, FILE *jpegImageFile)
{

    char jpegFilename[8];

    sprintf(jpegFilename, "%03i.jpg", newJpegCounter);

    *jpegImageFile = fopen(jpegFilename, "a");

    if (jpegImg == NULL)
    {
        fprintf(stderr, "Could not open jpeg.\n");
        return 2;
    }

    return 1;
}

//Given the current location in file, check if the next four bytes are a jpeg signature
//Returns 0 for error
//Returns 1 for byte does not match signature
//Returns 2 for complete signature found
//Returns 3 for end of file

int testForJPEGSignature(int index, FILE *inptr, int *rewindCounter)
{
    BYTE testByte = 0;

    const int JPEGSIGNATUREBYTES[] = {255, 216, 255};

    // Test the parameter testByte before testing a byte from file

    size_t read = fread(&testByte, sizeof(BYTE), 1, inptr);

    // End of file or error reached while reading the next byte from file
    if (read < 1)
    {
        int end = feof(inptr);
        int error = ferror(inptr);

        if (end > 0)
        {
            return 3;
        }

        if (error > 0)
        {
            return 0;
        }
    }

    *rewindCounter += 1;

    // Passed last index in JPEGSIGNATUREBYTES
    if (index == 3)
    {
        // Complete match of signature
        if ((testByte & 240) == 224)
        {
            //*rewindCounter += 1;
            return 2;
        }
        else
        {
            //*rewindCounter += 1;
            return 1;
        }
    }

    if (testByte == JPEGSIGNATUREBYTES[index])
    {

        // Tally how many bytes need to be rewound

        //*rewindCounter += 1;

        index += 1;

        int foundNextSignatureByte = testForJPEGSignature(index, inptr, rewindCounter);

        if (foundNextSignatureByte == 2)
        {
            return 2;
        }
        else if (foundNextSignatureByte == 0)
        {
            return 0;
        }
        else if (foundNextSignatureByte == 3)
        {
            return 3;
        }
    }

    return 1;
}