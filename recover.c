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

    //bool foundJPEG = true;

    

    while (testByte == 0)
    {
        size_t read = fread(&testByte, sizeof(BYTE), 1, inptr);

        // End of file or error reached while reading the next byte from file
        if (read == 0)
        {
            return false;
        }
    }

        //advanced to a non-zero testByte

        bool signatureFound = testForJPEGSignature(0, *inptr);

       

        if (signatureFound)
        {

            
             fseek(inptr, -4, SEEK_CUR);

        }

        bool nextBlock = true;


        while (nextBlock)
        {
            if (testForJPEGSignature(0, *inptr))
            {   
                //fseek back to beginning of block
                nextBlock = false;
            }
            else
            {
                //fseek back to beginning of block
                //read 512
            }
            




        }

        //check to see if the next block begins with a signature
        //if it does, stop
        //else, advance block count


 //fread
            fseek(inptr, 512, SEEK_CUR);
*jpegBlockCounter += 1;

        //test if next block begins with signature

        bool anotherBlock = true;

        while (anotherBlock)
        {
           if (testForJPEGSignature(0, *inptr))
           {
            
           }

            //need to change these to fread and not fseek

           else
           {
            *jpegBlockCounter += 1;

            fseek(inptr, -4, SEEK_CUR);
           
           //fread
            fseek(inptr, 512, SEEK_CUR);
           }
           

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

//NEED TO ADD ERROR CHECKING FOR FREAD!!!!!!!!!!

bool testForJPEGSignature(int index, FILE *inptr)
{
    const int JPEGSIGNATUREBYTES[] = {255, 216, 255};

    BYTE testByte;

    size_t read = fread(&testByte, sizeof(BYTE), 1, inptr);

    if (index == 3)
    {
        if ((testByte & 240) == 224)
        {

            return true;
        }
        else
        {
            return false;
        }
    }

    if (testByte == JPEGSIGNATUREBYTES[index])
    {
        index += 1;
        bool foundNextSignatureByte = testForJPEGSignature(index, inptr);

        if (foundNextSignatureByte)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    // Go back in file and try again
    fseek(inptr, (index * -1), SEEK_CUR);
    
    return false;
}

// size_t success = fread(jpegStoragePointer->jpegArray, sizeof(jpegStoragePointer->jpegArray), 1, inptr);

//             if (success < 512)
//             {
//                 // The bytes read were less than 512, which means end of file was reached
//                 return false;
//             }