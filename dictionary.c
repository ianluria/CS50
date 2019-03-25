// Implements a dictionary's functionality

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"

// Represents number of children for each node in a trie
#define N 27

// Represents a node in a trie
typedef struct node
{
    bool is_word;
    struct node *children[N];
} node;

int hashChar(char character);
bool load(const char *dictionary);
bool freeNodes(node *node);

// Represents a trie
node *root;

unsigned int wordCount = 0;

// Keeps track of whether the dictionary has been sucessfully loaded
bool loaded = false;

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    // Initialize trie
    root = malloc(sizeof(node));

    if (root == NULL)
    {
        return false;
    }

    root->is_word = false;

    for (int i = 0; i < N; i++)
    {
        root->children[i] = NULL;
    }

    // Open dictionary
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        unload();
        return false;
    }

    // *wordCount = 0;

    // Buffer for a word
    char word[LENGTH + 1];

    // Insert words into trie
    while (fscanf(file, "%s", word) != EOF)
    {
        int len = strlen(word);

        node *nodeTracker = root;

        for (int i = 0; i < len; i++)
        {
            char letter = word[i];
            int hash = hashChar(letter);

            if (hash == 1000)
            {
                //check for break out of for loop only
                break;
            }

            // Pointer to the node of this hash's index in the node nodeTracker is pointing to
            node *thisCharIndex = nodeTracker->children[hash];

            if (thisCharIndex == NULL)
            {
                // Allocate space for a new node at that index
                thisCharIndex = malloc(sizeof(node));

                if (thisCharIndex == NULL)
                {
                    return false;
                }

                thisCharIndex->is_word = false;

                for (int newIndex = 0; newIndex < N; newIndex++)
                {
                    thisCharIndex->children[newIndex] = NULL;
                }

                // Pointer at hash index now points to new node
                nodeTracker->children[hash] = thisCharIndex;
            }

            // Last char of word reached
            if (i == len - 1)
            {
                thisCharIndex->is_word = true;
                wordCount += 1;
            }
            else
            {
                // Point nodeTracker to the next node
                nodeTracker = thisCharIndex;
            }
        }
    }

    // Close dictionary
    fclose(file);

    // Indicate success
    loaded = true;
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    if (loaded)
    {
        return wordCount;
    }

    return 0;
}

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    int len = strlen(word);
    node *tracker = root;

    //loop through each char
    for (int i = 0; i < len; i++)
    {
        char character = word[i];

        int hashed = hashChar(character);

        // Character not valid
        if (hashed == 1000)
        {
            return false;
        }

        // Array index hasn't been created
        if (tracker->children[hashed] == NULL)
        {
            return false;
        }
        else
        {
            // Tracker now points to the node at the index
            tracker = tracker->children[hashed];
        }

        // End of string reached
        if (i == len - 1)
        {
            if (tracker->is_word)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    return false;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    freeNodes(root);

    free(root);

    return true;
}

//can only delete nodes that have blank arrays

bool freeNodes(node *nodeIn)
{

    for (int i = 0; i < N; i++)
    {
        node *thisNode = nodeIn->children[i];

        if (thisNode != NULL)
        {
            // Check if all of thisNode's children are NULL pointers
            bool isBlank = freeNodes(thisNode);

            if (isBlank)
            {
                // thisNode is now NULL;
                free(thisNode);
            }
        }
    }

    return true;
}

// Takes a character and returns an integer between 0-26 if the character is [a-z\']
// Else, returns 1000
int hashChar(char character)
{
    // Apostrophe
    if (character == 39)
    {
        return 26;
    }

    if (character > 64 && character < 123)
    {
        if (character < 91 || character > 96)
        {
            // Make uppercase letter lowercase
            if (character < 91)
            {
                character = character + 32;
            }
        }

        // a = 0 ... z = 25;
        return character - 97;
    }

    return 1000;
}
