#define _XOPEN_SOURCE
#include <unistd.h>
#include <cs50.h>
#include <stdio.h>
#include <string.h>

void populateAlphabetArray(char alpha[]);
char advanceChar(char character);
void createNewTestPassword(char newPassword[], char password[], char letter);
void findNextNonLastChar(char password[]);
bool loopAlphabet(char *password, char alpha[], const char SALT[], const char HASHEDPASSWORD[]);
bool cryptPassword(char password[], const char SALT[], const char HASHEDPASSWORD[]);

//50zqH8tIlmqO = crypt("AbC", "50")
//505DdIgbrILbY = Ab

int main(int argc, string argv[])
{   
    bool throwError = false;
    
    if (argc != 2)
    {
       throwError = true;      
    }
    
    if (throwError)
    {
       printf("Usage: ./crack hash\n");
       return 1;
    }
    
    const string HASHEDPASSWORD = argv[1];

    const char SALT[] = {HASHEDPASSWORD[0], HASHEDPASSWORD[1], '\0'};

    // Max five char + null for storing evolving password
    char password[6] = "A"; 
    char alpha[53];

    populateAlphabetArray(alpha);

    // First, checks each individual letter for a matching hash 
    for (int i = 0; i < strlen(alpha); i++)
    {
        const char *letter = &alpha[i]; 

        strncpy(password, letter, 1);

        password[1] = '\0';
        
        bool match = cryptPassword(password, SALT, HASHEDPASSWORD);
        
        if (match)
        {
            printf("%s is a password match", password);
            return 0;
        }
      
        printf("New test password: %s\n", password);
    }

    // Reset password to the first letter
    password[0] = 'A';
    
    // Loop will now run until a matching password is found or 'zzzzz' is reached
    bool run = true; 

    while (run)
    {
        
        int passLen = strlen(password);

        if (passLen > 5)
        {
            // Return an error because no hash was found in the first 5 chars
            return 1;
        }
       
        bool foundPassword = loopAlphabet(password, alpha, SALT, HASHEDPASSWORD);

        if(foundPassword)
        {
            break;
        }
    } 

    printf("final returned password: %s\n", password);
    
    return 0;
}

// Appends a letter to the end of password and then tests its hash
bool loopAlphabet(char *password, char alpha[], const char SALT[], const char HASHEDPASSWORD[])
{    

    const int ALPHALEN = strlen(alpha);
    const char ALPHALAST = alpha[ALPHALEN - 1];
    
    for (int i = 0; i < ALPHALEN; i++)
    {   
        char letter = alpha[i];
        
        // Char array used to store a new test password 
        char newPassword[6]; 

        createNewTestPassword(newPassword, password, letter);  
        
        bool match = cryptPassword(newPassword, SALT, HASHEDPASSWORD);

        if (match)
        {
            strncpy(password, newPassword, strlen(newPassword) +1);
            return true;
        }

        //past this point, the hash has failed

        // If last letter of alpha[] has been reached
        if (letter == ALPHALAST)
        {            
            findNextNonLastChar(password);  
        }
    }
    // No matching hash was found for the password passed in as parameter 
    return false;
}

// Copies password into newPassword and concatenates letter to the end 
void createNewTestPassword(char newPassword[], char password[], char letter)
{
    char appendLetter[] = {letter, '\0'};
    
    strcpy(newPassword, password);
    
    strncat(newPassword, appendLetter, 1);
}


// Adjusts password pointer 
void findNextNonLastChar(char password[])
{
    int passLen = strlen(password);
    int countdown = passLen - 1;
    bool lastChar = true;

    while (lastChar)
    {        
        if (countdown == 0)
        {
            if (password[0] != 'z')
            {
                password[0] = advanceChar(password[0]);
            }  
            else 
            {
            // The last char has been reached in the zero index.
            // If all the chars in password are at the end, add a new char and reset
            // Otherwise, run through with the last char in the 0 index

                bool allLastChars = true;

                for (int i = 1; i < passLen; i++)
                {
                    if (password[i] != 'z')
                    {
                        allLastChars = false;
                        i = passLen;
                    }
                }

                if (allLastChars)
                {
                    password[0] = 'A';
                    password[passLen] = 'A';
                    password[passLen + 1] = '\0';
                }
            }

            for (int i = 1; i < strlen(password); i++)
            {
                password[i] = 'A';
            }

            break;
        }

          // Decrease index counter if that index is at the last letter  
        if (password[countdown] == 'z')
        {
            countdown--;
        }
        else
        {
            // Advance the nearest non-last char in password 
            password[countdown] = advanceChar(password[countdown]);
            lastChar = false; 
        } 
    }        
}  

// Advances the character argument by one
char advanceChar(char character)
{
    
    if (character == 'Z')
    {
        return 'a';
    }
     
    return character + 1;
}

void populateAlphabetArray(char alpha[])
{
    int index = 0;

    for (char i = 'A'; i < '['; i++)
    {
        alpha[index] = i;
        index++;
    }

    for (char i = 'a'; i < '{'; i++)
    {
        alpha[index] = i;
        index++;
    }

    alpha[52] = '\0';
}

// Hashes a test password and return true if the hashed password matches the HASHEDPASSWORD
bool cryptPassword(char password[], const char SALT[], const char HASHEDPASSWORD[])
{
    char *testHash = crypt(password, SALT);

    testHash[13] = '\0';
    
    int matchedHashes = strncmp(testHash, HASHEDPASSWORD, 13);
    
    if (matchedHashes == 0)
    {
        return true;
    }
    else
    {
        return false;
    } 
}