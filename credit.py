from cs50 import get_int

# Returns digit at tenCounter's place in userCCNumber  
def getDigit(tenCounter, userCCNumber):
    return (userCCNumber % tenCounter) // (tenCounter * 0.1)

def main():

    userCCNumber = -1

    # Get credit card number from user  
    while userCCNumber < 0:
        userCCNumber = get_int("Your card number: ")
    
    # Keeps track of which digit in userCCNumber is being examined
    counter = 1

    # Used to isolate an integer value in userCCNumber
    tenCounter = 10

    # Sum of all examined digits in userCCNumber 
    grandSum = 0
    
    # Go through each digit in userCCNumber and add to grandSum depending on its position
    while tenCounter <= userCCNumber * 10:

        digit = getDigit(tenCounter, userCCNumber)

        # If counter is at an even numbered place
        if counter % 2 == 0:

            # Double the digit
            doubled = digit * 2
            
            remainder = doubled % 10

            # Is there now more than one digit in digit? 
            if doubled > 9:

               # Add both digits together  
               remainder = ((doubled - remainder) * 0.1) + remainder

            grandSum += remainder

        else:
            grandSum += digit

        counter += 1
        tenCounter *= 10

    # Counter overshot and needs to be rewound one
    counter -= 1

    # CCNumber can only be valid if grandSum is evenly divisible by 10
    if grandSum % 10 == 0:

        firstTwoDigits = int((getDigit(tenCounter * 0.1, userCCNumber) * 10)+(getDigit(tenCounter * 0.01, userCCNumber)))

        if firstTwoDigits == 34 or firstTwoDigits == 37:
            # Counter is equal to the number of digits in userCCNumber
            if counter == 15:
                print("AMEX")
                return
        elif int(firstTwoDigits * 0.1) == 4:
            if counter == 16 or counter == 13:
                print("VISA")
                return
        elif counter == 16:
            for i in range(51, 56):
                if i == firstTwoDigits:
                    print("MASTERCARD")
                    return
            
    print("INVALID")
    return

if __name__ == "__main__":
    main()