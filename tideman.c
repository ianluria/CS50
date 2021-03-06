#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool testLoser(int test, int winner);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new and valid vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            // The rank is the index, and the value is the candidate's array index position
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Add an individual voter's preferences to the global preferences array
void record_preferences(int ranks[])
{
    if (candidate_count == 1)
    {
        return;
    }

    for (int winner = 0; winner < candidate_count; winner++)
    {
        for (int loser = candidate_count - 1; loser > winner; loser--)
        {
            preferences[ranks[winner]][ranks[loser]]++;
        }
    }

    return;
}

// Globally, record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    if (candidate_count == 1)
    {
        return;
    }

    for (int candidate1 = 0; candidate1 < candidate_count; candidate1++)
    {
        for (int candidate2 = 0; candidate2 < candidate_count; candidate2++)
        {
            if (candidate1 != candidate2)
            {
                if (preferences[candidate1][candidate2] > preferences[candidate2][candidate1])
                {
                    pair_count++;

                    pairs[pair_count - 1].winner = candidate1;
                    pairs[pair_count - 1].loser = candidate2;
                }
            }
        }
    }

    return;
}

// Sort pairs in descending order by strength of victory
void sort_pairs(void)
{
    // One pair in array cannot be sorted
    if (pair_count == 1)
    {
        return;
    }

    int pairsSwitched = -1;

    while (pairsSwitched != 0)
    {
        pairsSwitched = 0;

        for (int i = 0; i < pair_count - 1; i++)
        {
            // If the margin of victory of pairs[i] is less than margin of victory of pairs[i+1],
            // switch them so the larger value comes first in array
            if (preferences[pairs[i].winner][pairs[i].loser] < preferences[pairs[i + 1].winner][pairs[i + 1].loser])
            {
                pair tempPair = pairs[i];
                pairs[i] = pairs[i + 1];
                pairs[i + 1] = tempPair;
                pairsSwitched++;
            }
        }
    }

    return;
}

// Lock pairs into the candidate graph in order of strength, without creating cycles
// Test losers to see if they have been winners. If losers can be traced back to the
// original winner and loser pair, a full cycle is possible and edge should not be added
void lock_pairs(void)
{

    if (pair_count == 1)
    {
        return;
    }

    for (int i = 0; i < pair_count; i++)
    {
        int winner = pairs[i].winner;
        int loser = pairs[i].loser;

        // The strongest victory is always added first
        if (i == 0)
        {
            locked[winner][loser] = true;
        }
        else
        {
            int test = loser;

            // Test if a cycle can be created back to the winner and loser pair.
            if (testLoser(test, winner))
            {

                // A full loop back to the origin is possible, therefore this pair should not be added
            }
            else
            {
                locked[winner][loser] = true;
            }
        }
    }
    return;
}

// Test to see if each loser has also been a winner, and can then be traced back to the origin.
bool testLoser(int test, int winner)
{
    if (test == winner)
    {
        return true;
    }

    for (int i = 0; i < candidate_count; i++)
    {
        if (i != test)
        {
            if (locked[test][i])
            {
                if (testLoser(i, winner))
                {
                    return true;
                }
            }
        }
    }

    // No more candidates left to check for winner
    return false;
}

// Print the winner of the election by determining who is the source
void print_winner(void)
{

    if (candidate_count == 1)
    {
        printf("%s\n", candidates[0]);
        return;
    }

    for (int i = 0; i < candidate_count; i++)
    {

        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i])
            {
                continue;
            }
        }

        printf("%s\n", candidates[i]);
    }

    return;
}
