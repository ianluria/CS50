#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

typedef unsigned char BYTE;

//scan through card and tally how many signatures are found on block breaks
//make an int array to store each memory address of a new jpeg on card
//read through card and dynamically store each jpegs blocks into an array
//write that array into a new jpeg file


//scan through each block checking for signature

//if signature found
//add that block to a new jpeg file
//if next block is not a signature
//APPEND that block to current jpeg file
//else if next block is a signature
//close current jpeg
//make new jpeg
//add block to new jpeg
//reloop

//if signature 
//read block into array
//append array into jpeg file
//when another signature is found, close existing jpeg, make new file and write into that. 




bool findJPEG(int *counter, FILE *inptr);
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
    int fileCardBlockCounter = 0;



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

            printf("position before rewind: %lu\n", ftell(inptr));

            //rewind number bytes in inptr before writing
            int seek = fseek(inptr, (-1 * number), SEEK_CUR);

            //printf("error: %i\n", ferror(inptr));

            // End of file or error
            if (seek < 0)
            {
                return 5;
            }

            printf("number: %i, blocks: %i\n", number, jpegBlockCounter);
            printf("inptr position before write: %lu\n", ftell(inptr));
            printf("image position before write: %lu\n", ftell(jpegImg));

            size_t writeCount = 0;
            for (int i = 0; i < jpegBlockCounter; i++)
            {
                //writeCount += fwrite(inptr, 1, number, jpegImg);
                writeCount += fwrite(inptr, 512, 1, jpegImg);

                int forward = fseek(inptr, 512, SEEK_CUR);

                // End of file or error
                if (forward < 0)
                {
                    return 5;
                }
            }

            printf("writeCount: %zu\n", writeCount);
            printf("inptr position after write: %lu\n", ftell(inptr));
            printf("image position after write: %lu\n", ftell(jpegImg));

            // int end = feof(inptr);
            // int error = ferror(inptr);

            int end2 = feof(jpegImg);
            int error2 = ferror(jpegImg);

            //printf("eof: %i, error: %i\n", end, error);
            printf("eof: %i, error: %i\n", end2, error2);

            if (writeCount != jpegBlockCounter)
            {
                return 3;
            }

            jpegBlockCounter = 0;

            int close = fclose(jpegImg);

            if (close != 0)
            {
                return 5;
            }

            return 100;
        }
        else
        {
            // End of file reached
            nextJpegBlock = false;
            printf("end of file");
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
    int creatingJpeg = 0;

    while (creatingJpeg < 2000000)
    {
        int rewindCounter = 0;

        // Test for signature sequence in current position of inptr
        int signatureFound = testForJPEGSignature(0, inptr, &rewindCounter);

        // Reached end of file or error
        if (signatureFound == 0)
        {
            return false;
        }

        rewindCounter *= -1;

        //return to beginning of block
        int rewindSeek = fseek(inptr, rewindCounter, SEEK_CUR);

        // End of file or error
        if (rewindSeek < 0)
        {
            return 5;
        }

        // Signature found!
        if (signatureFound == 2)
        {
            printf("Signature found:  %lu\n", ftell(inptr));

            // Return true if a new jpeg signature was found while processing an existing jpeg
            if (*jpegBlockCounter > 0)
            {
                return true;
            }

            *jpegBlockCounter += 1;

            
            //read in block to storage array struct 

            // End of file or error
            
            //printf("ftell from sigFound = 2:  %lu\n", ftell(inptr));
        }
        else if (signatureFound == 1)
        {
            // Keep incrementing jpegBlockCounter until another jpeg signature is found
            if (*jpegBlockCounter > 0)
            {
                *jpegBlockCounter += 1;
            }

            // Move forward 512 bytes in file to next block
            int seek = fseek(inptr, 512, SEEK_CUR);

            // End of file or error
            if (seek < 0)
            {
                return false;
            }

            //printf("ftell from sigFound = 1:  %lu\n", ftell(inptr));
        }

        creatingJpeg += 1;
    }
    return false;
}

//Given the current location in file, check if the next four bytes are a jpeg signature
//Returns 0 for end of file or read error
//Returns 1 for byte does not match signature
//Returns 2 for complete signature found

int testForJPEGSignature(int index, FILE *inptr, int *rewindCounter)
{
    BYTE testByte = 0;

    const int JPEGSIGNATUREBYTES[] = {255, 216, 255};

    // Test the parameter testByte before testing a byte from file

    size_t read = fread(&testByte, sizeof(BYTE), 1, inptr);

    // End of file or error reached while reading the next byte from file
    if (read == 0)
    {
        return 0;
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
    }

    return 1;
}