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

// Represents a trie
node *root;

unsigned int *wordCount;
bool loaded = false;

// Loads dictionary into memory, returning true if successful else false
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

    *wordCount = 0;

    // Buffer for a word
    char word[LENGTH + 1];

    // Insert words into trie
    while (fscanf(file, "%s", word) != EOF)
    {
        // TODO
        //check for lowercase

        *wordCount += 1;

        int len = strlen(word);

        node *nodeTracker = root;

        for (int i = 0; i < len; i++)
        {
            char letter = word[i];
            int hash = 0;

            // Apostrophe
            if (letter = 39)
            {
                hash = 26;
            }
            // Letter at 123 will produce a false positive
            else if (letter = 123)
            {
                letter = 94;
            }
            else
            {
                // a = 0 ... z = 25;
                //to lower case
                hash = letter - 97;
            }

            if (hash < 0 || hash > 26)
            {
                break;
            }

            // Pointer to the node of this hash's index in the node nodeTracker is pointing to
            node *thisCharIndex = nodeTracker->children[hash];

            if (thisCharIndex = NULL)
            {
                // Allocate space for a new node at that index
                thisCharIndex = malloc(sizeof(node));

                if (thisCharIndex == NULL)
                {
                    return false;
                }

                thisCharIndex->is_word = false;

                for (int newIndex = 0; newIndex < newIndex++)
                {
                    thisCharIndex->children[i] = NULL;
                }
            }

            // Last char of word reached
            if (i == len - 1)
            {
                thisCharIndex->is_word = true;
            }
            else
            {
                // Point nodeTracker to the next node
                nodeTracker = thisCharIndex;
            }

            
        }

        //for loop through word
        //hash char
        //if char's index's pointer is null, malloc a new node
        //if last char is reached make is_word true
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
    // TODO
    if (loaded)
    {
        return *wordCount;
    }

    return 0;
}

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    // TODO
    //loop through each char
    //starting at root, if the node's children's index is not null for char
    //assign tracker to that node
    //if last char reached, check if make_is_word is true

    return false;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    // TODO
    return false;
}
