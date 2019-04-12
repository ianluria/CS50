from cs50 import get_int

# Returns digit at tenCounter's place
def getDigit(tenCounter, userCCNumber):
    return (userCCNumber % tenCounter) // (tenCounter*0.1)

def main():

    userCCNumber = 0

    # Get credit card number from user
    while userCCNumber < 1000000000000:
        userCCNumber = get_int("Your card number: ")
    
    # Keeps track of which digit is being examined
    counter = 1

    # Used to isolate an integer value
    tenCounter = 10

    grandSum = 0
    #notUnderlinedSum = 0

    # Go through each digit in userCCNumber and add to grandSum depending on position
    while tenCounter <= userCCNumber * 10:
        digit = getDigit(tenCounter, userCCNumber)

        # If counter is at an even numbered place
        if counter % 2 == 0:

            # Double the digit
            product = digit * 2

            
            underlined = product % 10

            # Is there more than one digit in digit? 
            if product > 9:

               # Add both digits together  
               underlined = ((product - underlined) * 0.1) + underlined

            grandSum += underlined

        else:
            grandSum += digit

        counter += 1
        tenCounter *= 10

    # Counter overshot and need to be rewound one
    counter -= 1

    #grandSum = underlinedSum + notUnderlinedSum

    # CCNumber can only be valid if it is evenly divisible by 10
    if grandSum % 10 == 0:

        firstTwoDigits = (getDigit(tenCounter *0.1, userCCNumber)*10)+(getDigit(tenCounter*0.01, userCCNumber))

        if firstTwoDigits == 34 or firstTwoDigits == 37:
            # Counter is equal to the number of digits in userCCNumber
            if counter == 15:
                #creditCard = "A"
                print("AMEX")
                return
        elif firstTwoDigits * 0.1 == 4:
            if counter == 16 or counter == 13:
                #creditCard = "V"
                print("VISA")
                return
        elif counter == 16:
            for i in range(51, 56):
                if i == firstTwoDigits:
                    #creditCard = "M"
                    print("MASTERCARD")
                    return
            
        # if creditCard == "V":
        #     print("VISA")
        #     return
        # elif creditCard == "M":
        #     print("MASTERCARD")
        #     return
        # elif creditCard == "A":
        #     print("AMEX")
        #     return
   

    print("INVALID")
    return


if __name__ == "__main__":
    main()