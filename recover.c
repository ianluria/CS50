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

    while (nextJpegBlock)
    {
        JpegStorage jpeg;

        FindJPEG(&jpeg);

        if (jpeg.jpegArray[0] == 0)
        {
            // End of file reached
            nextJpegBlock = false;
        }
        else
        {
            //write jpegArray into new file
        }
    }

    return 0;

    //while input has another 512 byte block

    // read bytes until a jpeg signature is found
    // write 512 bytes into a new jpg file
    // advance 512 bytes in input

    // if 512 block is less than 512 bytes, return false -- end of file
}

void function FindJPEG(JpegStorage *jpegArrayPointer, int counter)
{
    BYTE testByte;

    const int jpegSignatureBytes[] = {255, 216, 255};

    size_t read = fread(&testByte, sizeof(BYTE), 1, inptr);

    // End of file or error reached while reading the next byte from file
    if (read == 0)
    {
        *jpegArrayPointer[0] = 0;
        return;
    }

    // End of jpegSignatureBytes array reached
    if (counter == 3)
    {
        if (testByte & 240 == 224)
        {
            //go back 4 spaces to the beginning of jpeg and add 512 bytes to array
            fseek(inptr, -4, SEEK_CUR);

            size_t success = fread(jpegArrayPointer, sizeof(JPEGARRAY), 1, inptr);

            if (success < 512)
            {
                // The bytes read were less than 512, which means end of file was reached
                *jpegArrayPointer[0] = 0;
            }

            return;
        }
    }
    else if (testByte == jpegSignatureBytes[counter])
    {
        counter += 1;

        FindJPEG(jpegArrayPointer, counter);
    }

    if (counter > 0)
    {
        int negativeCounter = counter * -1;

        // Go back in file negativeCounter spaces
        fseek(inptr, negativeCounter, SEEK_CUR);
    }

    FindJPEG(jpegArrayPointer, 0);
}

