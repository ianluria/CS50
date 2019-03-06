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

        if (jpeg.jpegArray[0] == 0){
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











void function FindJPEG(JpegStorage jpegArrayPointer)
{
    BYTE testByte1;
    BYTE testByte2;
    BYTE testByte3;
    BYTE testByte4;

    size_t read1 = fread(&testByte1, sizeof(BYTE), 1, inptr);

    if (read1 == 0)
    {
        jpegArrayPointer[0] = 0;
        return;
    }

    if (testByte1 == 255)
    {
        size_t read2 = fread(&testByte2, sizeof(BYTE), 1, inptr);

        if (read2 == 0)
        {
            jpegArrayPointer[0] = 0;
            return;
        }

        if (testByte2 == 216)
        {
            size_t read3 = fread(&testByte3, sizeof(BYTE), 1, inptr);

            if (read3 == 0)
            {
                jpegArrayPointer[0] = 0;
                return;
            }

            if (testByte3 == 255)
            {
                size_t read4 = fread(&testByte4, sizeof(BYTE), 1, inptr);

                if (read4 == 0)
                {
                    jpegArrayPointer[0] = 0;
                    return;
                }

                if (testByte4 & 240 == 224)
                {
                    //go back 4 spaces to the beginning of jpeg and add 512 bytes to array
                    fseek(inptr, -4, SEEK_CUR);

                    size_t success = fread(jpegArrayPointer, sizeof(JPEGARRAY), 1, inptr);

                    if (success < 512)
                    {
                        // The bytes read were less than 512, which means end of file was reached
                        jpegArrayPointer[0] = 0;
                    }

                    return;
                }
                else
                {
                    // Go back three pixels and rerun findJPEG
                    fseek(inptr, -3, SEEK_CUR);
                    //findJPG()
                }
            }
            else
            {
                // Go back two pixels and rerun findJPEG
                fseek(inptr, -2, SEEK_CUR);
                //findJPG()
            }
        }
        else
        {
            // Go back one pixel and rerun findJPEG
            fseek(inptr, -1, SEEK_CUR);
            //findJPG()
        }
    }
    else
    {
        // Run findJPG on the next pixel
        //findJPG()
    }

    return array[0] = 0;
}
