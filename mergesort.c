#include <stdio.h>

int main(void)
{
}

bool sort(int start, int length) //7 ,2
{
    const int MASTERARRAY[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    int arrayIn[length]; //2

    for (int i = 0; i < length; i++)
    {
        arrayIn[i] = MASTERARRAY[start + i];
    }

    if (length == 1)
    {
        printf("single element: %i\n", arrayIn[0]);
        return true;
    }
    else
    {
        int midpoint = length / 2; //1 midpoint will always be part of high

        int lowLen = midpoint; //1

        int lowStart = start; //7

        int highLen = length - midpoint; //1

        int highStart = start + lowLen; //8

        int arrayLow[lowLen];

        int arrayHigh[highLen];

        for (int i = 0; i < length; i++)
        {
            if (i < lowLen)
            {
                arrayLow[i] = TESTARRAY[i + start];
                //printf("low: index: %i  value: %i\n", i, arrayLow[i]);
            }
            else
            {
                int index = i - midpoint;
                arrayHigh[index] = TESTARRAY[i + start];
                //printf("high: index: %i  value: %i\n", index, arrayHigh[index]);
            }
        }

        // Both low and high arrays have only one element
        if (sort(lowLen, lowStart) && sort(highLen, highStart))
        {
            //merge low with high

            //low index is sorted 
            // for each index in low array
            //if that index is less than each in high array, place in 0 array
            // else place lesser element from high array in 0 array

            //  

            return true;
        }

        return 0;
    }

    // const int MASTERLENGTH = sizeof(MASTERARRAY) / sizeof(MASTERARRAY[0]);

    // const int MASTERMIDPOINT = MASTERLENGTH / 2;