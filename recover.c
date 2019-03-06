#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

typedef uint8_t BYTE;
typedef struct
{
    // Array of 512 bytes used to store a full jpeg
    BYTE jpegArray[512];
} JpegStorage;

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
    char jpegFilename[8];

    while (nextJpegBlock)
    {
        JpegStorage jpeg;

        bool foundJPEG = findJPEG(&jpeg, 0);

        if (foundJPEG)
        {
            //write jpegArray into new file
            sprintf(jpegFilename, "%03i.jpg", newJpegCounter);
        }
        else
        {
            //write jpegArray into new file
            // End of file reached
            nextJpegBlock = false;
        }
    }

    return 0;

    //while input has another 512 byte block

    // read bytes until a jpeg signature is found
    // write 512 bytes into a new jpg file
    // advance 512 bytes in input

    // if 512 block is less than 512 bytes, return false -- end of file
}

bool function findJPEG(JpegStorage *jpegStoragePointer, int counter)
{
    BYTE testByte;

    const int JPEGSIGNATUREBYTES[] = {255, 216, 255};

    size_t read = fread(&testByte, sizeof(BYTE), 1, inptr);

    // End of file or error reached while reading the next byte from file
    if (read == 0)
    {
        return false;
    }

    // End of JPEGSIGNATUREBYTES array reached
    if (counter == 3)
    {
        if (testByte & 240 == 224)
        {
            //go back 4 spaces to the beginning of jpeg and add 512 bytes to array
            fseek(inptr, -4, SEEK_CUR);

            size_t success = fread(jpegStoragePointer->jpegArray, sizeof(JpegStorage), 1, inptr);

            if (success < 512)
            {
                // The bytes read were less than 512, which means end of file was reached
                return false;
            }
            return true;
        }
    }
    else if (testByte == JPEGSIGNATUREBYTES[counter])
    {
        counter += 1;

        findJPEG(jpegStoragePointer, counter);
    }

    if (counter > 0)
    {
        int negativeCounter = counter * -1;

        // Go back in file negativeCounter spaces
        fseek(inptr, negativeCounter, SEEK_CUR);
    }

    findJPEG(jpegStoragePointer, 0);
}
