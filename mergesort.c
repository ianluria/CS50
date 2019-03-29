#include <stdio.h>
#include <string.h>

void merge(int low, int high, int *array, int *aux);

int main(void)
{

    int array[] = {25, 10, 8, 6};

    int length = (sizeof(array) / sizeof(array[0]));

    int aux[length];

    memcpy(aux, array, sizeof(array));

    merge(0, length - 1, array, aux);

    for (int i = 0; i < length; i++)
    {
        printf("%d = %d\n", i, array[i]);
    }

    return 0;
}

void merge(int low, int high, int *array, int *aux)
{

    if (low == high)
    {
        return;
    }

    int midpoint = ((high - low) / 2) + low;

    merge(low, midpoint, array, aux);
    merge(midpoint + 1, high, array, aux);

    //combine the two arrays

    int leftPosition = low;
    int rightPosition = midpoint + 1;

    for (int i = low; i <= high; i++)
    {
        if (leftPosition > midpoint)
        {
            aux[i] = array[rightPosition];
            rightPosition += 1;
        }
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

    for (int i = low; i <= high; i++)
    {
        array[i] = aux[i];
    }

    return;
}
