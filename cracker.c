#include <cs50.h>
#include <stdio.h>
#include <string.h>

char advanceChar(char character);
void createNewTestPassword(char newPassword[], char password[], char letter);
void findNextNonLastChar(char password[]);
void loopAlphabet(char *password, char alpha[]);

int main(void)
{   
    // Max five char + null for storing evolving password
    char password[6] = "A"; 

    //bool run = true; 
    
    char alpha[]= "ABC";
    
    // Checks each individual letter for matching hash 
    for (int i = 0; i < strlen(alpha); i++)
    {
        /*
         * if string from crypt matches hash
         * return alpha[i] as password 
         */  
        printf("New test password: %c\n", alpha[i]);
    }
    
    // Loop will run until a matching password is found or 'zzzzz' is reached
    // 
    
    int testCount = 0;
    
    while (testCount < 10)
    {
        //printf("\nrun loop: password is now: %s\n", password);
        
        int passLen = strlen(password);

        if (passLen > 2)
        {
            break;
        }
       
        loopAlphabet(password, alpha);
        
        testCount++;
    } 

    printf("final returned password: %s\n", password);
    
    return 0;
}

// Appends a letter to the end of password and tests its hash
void loopAlphabet(char *password, char alpha[])
{    

    const int ALPHALEN = strlen(alpha);
    const char ALPHALAST = alpha[ALPHALEN - 1];
    
    
    for (int i = 0; i < ALPHALEN; i++)
    {   
        //printf("alpha loop: %i\n", i);

        char letter = alpha[i];
        
        // Char array used to store a new test password 
        char newPassword[6]; 

        createNewTestPassword(newPassword, password, letter);  

        printf("New test password: %s\n", newPassword);
        //crypt function
        //return if hashes match
        //past this point, hash has failed

        // If last letter of alpha[] has been reached
        if (letter == ALPHALAST)
        {            
            // If the password has only one character, prepare it so it can have a new char appended
//             if (strlen(password) == 1)
//             { 
//                 password[0] = ALPHALAST;
//                 password[1] = '\0';
//             }
            
            findNextNonLastChar(password);  
        }
    }
//printf("outside alphabet loop\n");
}

// Takes a pointer to newPassword and fills it with chars according to password length
void createNewTestPassword(char newPassword[], char password[], char letter)
{
    char appendLetter[] = {letter, '\0'};
    
//     if (strlen(password) == 1)
//     {   
//         strcpy(newPassword, appendLetter);
//     }
//     else
    
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
         
         //if gets to zero index 
         //advance char in zero index
         //reset all other characters to 'A'
         //if zero index is the last letter
         //exit 
        
          //printf("last char loop: countdown: %i\n", countdown);

          // Password's zero index is at 'z'
          
              
              /*
             //reset password and add a new char
             for (int j = 0; j < passLen; j++)
             {
                 //printf("here4\n");
                 password[j] = 'A';
             }
              
             // Add a new char to end of password
             password[passLen] = 'A';
             password[passLen + 1] = '\0'; 
             //printf("new password at end of findnextlastchar: %s\n", password); 
             break;
             */
          if (countdown == 0)
          {

            for (int i = 1; i < passLen; i++)
                {
                    password[i] = 'A';
                }

            if (password[0] != 'C')
            {
                  password[0] = advanceChar(password[0]);
            }  
            else 
            {
            // The last char has been reached in the zero index.
            // If all the chars in password are at the end, add a new char and reset
            // Otherwise, run through with the last char in the 0 index


                bool allLastChars = false;

                for (int i = 1; i < passLen; i++)
                 {
                     if (password[i] != 'C')
                     {
                         allLastChars = true;
                         i = passLen;
                     }
                 }

                 if (allLastChars)
                 {
                    password[0] = 'A';
                    password[passLen] = 'A';
                    password[passLen + 1] = '\0'; 
                 }



                //check if all chars are lastchar
                //if yes, reset password to all 'A' and then add a new char to end
                //else reset all chars except 0 to 'A' and run again
                
            }
              
              break;
          }

          // Decrease index counter if that index is at the last letter  
          if (password[countdown] == 'C')
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
    
//      if (character == 'Z')
//      {
//      return 'a';
//      }
     
     return character + 1;
}



   









    //if reach last char
        //advance next to last char
        //if --passLast == last char
        //reduce passLast and try again 
        //if reach index 0 and index 0 is at last char
        //reset password with new added char
        //rerun function with new password 
