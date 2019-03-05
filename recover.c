#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

typedef uint8_t BYTE;
typedef BYTE JPEGARRAY[512];

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

    bool jpegBlock = true;

    while (jpegBlock)
    {
        BYTE testByte1;
        BYTE testByte2;
        BYTE testByte3;
        BYTE testByte4;

        while (findJPEG)
    }

    //while input has another 512 byte block

    // read bytes until a jpeg signature is found
    // write 512 bytes into a new jpg file
    // advance 512 bytes in input

    // if 512 block is less than 512 bytes, return false -- end of file

    //function findJPEG
    {

        size_t read1 = fread(&testByte1, sizeof(BYTE), 1, inptr);

        if (read1 == 0)
        {
            //return error
        }

        if (testByte1 == 255)
        {
            size_t read2 = fread(&testByte2, sizeof(BYTE), 1, inptr);

            if (read2 == 0)
            {
                //return error
            }

            if (testByte2 == 216)
            {
                size_t read3 = fread(&testByte3, sizeof(BYTE), 1, inptr);

                if (read3 == 0)
                {
                    //return error
                }

                if (testByte3 == 255)
                {
                    size_t read4 = fread(&testByte4, sizeof(BYTE), 1, inptr);

                    if (read4 == 0)
                    {
                        //return error
                    }

                    if (testByte4 & 240 == 224)
                    {
                        //go back 4 spaces and add 512 bytes to array
                        //return array
                    }
                    else
                    {
                        fseek(inptr, 3, SEEK_CUR);
                        //findJPG()
                    }
                }
                else
                {
                    fseek(inptr, 2, SEEK_CUR);
                    //findJPG()
                }
            }
            else
            {
                fseek(inptr, 1, SEEK_CUR);
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
