#include <stdio.h>
#include <cs50.h>

//Ian Luria cs50

// Returns a specific digit based on multiples of ten
int getDigit(long tenCounter, long userCCNumber)
{
    int digit = (userCCNumber % tenCounter) /(tenCounter*0.1);
    return digit;
}

int main(void)
{
    long userCCNumber;
    
    do
    {
        userCCNumber = get_long("Your card number: ");
    } while (userCCNumber < 1000000000000);
    
    int grandSum = 0;
    int underlinedSum = 0;
    int notUnderlinedSum = 0;
    int counter = 1;
    long tenCounter = 10;
    
    while (tenCounter <= userCCNumber * 10)
    {   
        int digit = getDigit(tenCounter, userCCNumber);
        
        if (counter % 2 == 0)
        {
            int product = digit * 2;
            int underlined = product % 10;
            if (product > 9)
            {
                // Add the two digits together i.e. 16 = 7 
                underlined = ((product - underlined) * 0.1) + underlined;
            }
            
            underlinedSum += underlined;
        } 
        else
        {
            notUnderlinedSum += digit;
        }
        
        counter+=1;
        tenCounter *= 10;
    }
    
   
    counter -= 1;
    grandSum = underlinedSum + notUnderlinedSum;
    
    if (grandSum % 10 == 0)
    {
       char creditCard;
       int firstTwoDigits = (getDigit(tenCounter *0.1, userCCNumber)*10)+(getDigit(tenCounter*0.01, userCCNumber));
       int firstDigit = firstTwoDigits * 0.1; 
 
        if (firstTwoDigits == 34 || firstTwoDigits == 37)
        {
            if (counter == 15)
            {
                creditCard = 'A'; //AmEx
            } 
            else
            {
                creditCard = 'X'; //Fail
            }
        } 
        else if (firstDigit == 4)
        {  
            if (counter == 16 || counter == 13)
            {
                creditCard = 'V'; //Visa
            } 
            else
            {
                creditCard = 'X';
            }
        } 
        else if (counter == 16)
        {
            bool masterCard = false;
            
            for (int i = 51; i < 56; i++)
            {
                if (i == firstTwoDigits)
                {
                    masterCard = true;
                }    
            }
            
            if (masterCard)
            {
                creditCard = 'M'; //Mastercard
            } 
            else
            {
                creditCard = 'X';
            }
            
        } 
        else
        {
            creditCard = 'Z'; 
        }
   
        if (creditCard == 'Z')
        {
            printf("INVALID\n");
        } 
        else if (creditCard == 'V')
        {
            printf("VISA\n");
        } 
        else if (creditCard == 'M')
        {
            printf("MASTERCARD\n");
        } 
        else if (creditCard == 'A')
        {
            printf("AMEX\n");
        }
       
    } 
    else
    {
        printf("INVALID\n");
    }
}
