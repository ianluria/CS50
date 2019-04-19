from sys import argv
import crypt

# Program which takes a user's hashed password and brute force test it through every letter combination to find a matching unencrypted password. Max five characters. 

def main():
    if len(argv) !=2:
        print("Usage: crack.py hash")
        return

    hashedPassword = argv[1]

    salt = hashedPassword[0] + hashedPassword[1]
    
    password = ["A"]

    length = len(password)

    while True:
    
        count = 1
        
        # Hashes the letter combination and returns true if its hash matches hashedPassword 
        matchingHashes = cryptPassword(password, salt, hashedPassword)

        if matchingHashes:
            password = ''.join(password)
            print(password)
            return 0

        # Beyond this point, a match was not found and the next character combination needs to be prepared

        # When 'z' is reached, special treatment is required as it is the last character
        if password[length - count] == "z":
            while True:

                if password[length - count] != "z":
                    password[length - count] = advanceCharacter(password[length - count])
                    break

                # If the zero index is reached, the password needs to be appended a new character, or if it has reached five characters already, the program must terminate
                if length - count == 0:
                    if length + 1 < 6:
                        password.append("A")
                        password[length - count] = "A"
                        # Update length
                        length = len(password)
                        break
                    else:
                        # Match not found in first five characters
                        return 1
                else:
                    password[length-count] = "A"
                    count += 1
        else:
            password[length - count] = advanceCharacter(password[length - count])
    return 2

def advanceCharacter(character):
    if character == "Z":
        return "a"
    else:
        return chr(ord(character) + 1)

def cryptPassword(password, salt, hashedPassword):

    password = ''.join(password)

    testHash = crypt.crypt(password, salt)

    if testHash == hashedPassword:
        return True
    else:
        return False

if __name__ == "__main__":
    main()




    