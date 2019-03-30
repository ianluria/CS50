#include <stdio.h>
#include <string.h>

void merge(int low, int high, int *array, int *aux);

int main(void)
{
    // Array that will be sorted
    int array[] = {25, 10, 8, 6};

    int length = (sizeof(array) / sizeof(array[0]));

    // Temporary array used to store values during sorting
    int aux[length];

    // Copy contents of array
    memcpy(aux, array, sizeof(array));

    // Begin recursive process
    merge(0, length - 1, array, aux);

    for (int i = 0; i < length; i++)
    {
        printf("%d = %d\n", i, array[i]);
    }

    return 0;
}

// Takes min and max indexes and writes ascendingly the corresponding array values.
void merge(int low, int high, int *array, int *aux)
{
    // Base case is reached when there is only one value
    if (low == high)
    {
        return;
    }

    int midpoint = ((high - low) / 2) + low;

    // Break down range by midpoint until base case is reached
    merge(low, midpoint, array, aux);
    merge(midpoint + 1, high, array, aux);

    // Combine the two ranges in ascending order

    //Index at which range begins
    int leftPosition = low;
    int rightPosition = midpoint + 1;

    //For each index in range, insert the value by order from array into aux
    for (int i = low; i <= high; i++)
    {
        // leftPosition has passed its limit
        if (leftPosition > midpoint)
        {
            aux[i] = array[rightPosition];
            rightPosition += 1;
        }
        // rightPosition has passed its limit
        else if (rightPosition > high)
        {
            aux[i] = array[leftPosition];
            leftPosition += 1;
        }
        else if (array[leftPosition] <= array[rightPosition])
        {
            aux[i] = array[leftPosition];
            leftPosition += 1;
        }
        else
        {
            aux[i] = array[rightPosition];
            rightPosition += 1;
        }
    }

    // Adjust the values in array once new positions in aux are settled
    for (int i = low; i <= high; i++)
    {
        array[i] = aux[i];
    }

    return;
}
