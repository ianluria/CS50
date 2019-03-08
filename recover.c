#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

typedef char BYTE;
typedef struct
{
    // Array of 512 bytes used to store a full jpeg
    BYTE jpegArray[512];
} JpegStorage;

bool findJPEG(JpegStorage *jpegStoragePointer, int counter, FILE *inptr);

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

        bool foundJPEG = findJPEG(&jpeg, 0, inptr);

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

            fwrite(jpeg.jpegArray, sizeof(jpeg.jpegArray), 1, jpegImg);

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

bool findJPEG(int *jpegBlockCounter, int counter, FILE *inptr)
{
    BYTE testByte;

    const int JPEGSIGNATUREBYTES[] = {255, 216, 255};

    while (testByte == 0)
    {
        size_t read = fread(&testByte, sizeof(BYTE), 1, inptr);

        // End of file or error reached while reading the next byte from file
        if (read == 0)
        {
            return false;
        }
    }

    // End of JPEGSIGNATUREBYTES array reached
    if (counter == 3)
    {
        if ((testByte & 240) == 224)
        {
            //go back 4 spaces to the beginning of jpeg and add 512 bytes to array
            fseek(inptr, -4, SEEK_CUR);

            *jpegBlockCounter = 1;

            bool anotherBlock = true;

            while (anotherBlock)
            {
                fseek(inptr, 512, SEEK_CUR);

                //seek to end of block
                //check if block signautre is present
                //if so, increment blockcounter
                //else end function and return to main
            }

            return true;
        }
    }
    else if (testByte == JPEGSIGNATUREBYTES[counter])
    {
        counter += 1;

        findJPEG(jpegStoragePointer, counter, inptr);
    }

    if (counter > 0)
    {
        int negativeCounter = counter * -1;

        // Go back in file negativeCounter spaces
        fseek(inptr, negativeCounter, SEEK_CUR);
    }

    findJPEG(jpegStoragePointer, 0, inptr);

    return false;
}

//Given the current location in file, check if the next four bytes are a jpeg signature












// size_t success = fread(jpegStoragePointer->jpegArray, sizeof(jpegStoragePointer->jpegArray), 1, inptr);

//             if (success < 512)
//             {
//                 // The bytes read were less than 512, which means end of file was reached
//                 return false;
//             }