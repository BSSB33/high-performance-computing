#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <limits.h>
#include "../headers/uthash.h"

struct dict_item
{
    char word[255]; // we use this as key for the hash table
    int nr;
    UT_hash_handle hh; // makes the structure hashable
};

typedef struct
{
    long tv_sec;
    long tv_usec;
} timeval;

void add_item_to_dict(struct dict_item *dict, char *word)
{
    struct dict_item *s;
    HASH_FIND_STR(dict, word, s);
    if (s == NULL)
    {
        s = (struct dict_item *)malloc(sizeof *s);
        strcpy(s->word, word);
        HASH_ADD_STR(dict, word, s);
        s->nr = 1;
    }
    else
    {
        s->nr++;
    }
}

void print_dict(struct dict_item *dict, int max_items)
{
    struct dict_item *s;
    int j = 1;
    for (s = dict; s != NULL && max_items > 0; s = (struct dict_item *)(s->hh.next))
    {
        printf("%d. word: %s - nr: %d\n", j, s->word, s->nr);
        max_items--;
        j++;
    }
}

/*
    Comparison function for sorting by number of occurences
*/
int nr_sort(struct dict_item *a, struct dict_item *b)
{
    return (b->nr - a->nr);
}

void sort_by_nr(struct dict_item *dict)
{
    HASH_SORT(dict, nr_sort);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: <program> <input-file> <0|1>"); // 0 - words, 1 - sentences
        exit(EXIT_FAILURE);
    }

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    size_t read;
    timeval t1, t2;
    struct dict_item *my_dict = NULL;

    int comm_sz;
    int my_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_sz(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank != 0)
    {
        // start = my_rank * (nr_of_lines / comm_sz)
        // process starts reading file from start line 
            // just as in sequential code, create dict from words, reading line by line
            // until (my_rank + 1) * (nr_of_lines / comm_sz) && getline() != -1
        
        
        // send created dict to main process
    }
    else
    {
        // open file for reading, read number of lines
        int nr_of_lines;
        // broadcast number of lines to all processes, so they can calculate where to start reading from based on comm_sz and rank

        // first process starts from first line until nr_of_lines/comm_sz
            // just as in sequential code, create dict from words, reading line by line
            // until (my_rank + 1) * (nr_of_lines / comm_sz) && getline() != -1
        
        // receive created dictionaries from processes and create one master dict for output
    }

    gettimeofday(&t1, NULL);

    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        exit(EXIT_FAILURE);
    }

    if (atoi(argv[2]) == 0)
    {
        while ((read = getline(&line, &len, fp)) != -1)
            add_item_to_dict(my_dict, line);
    }
    else
    {
        while ((read = getline(&line, &len, fp)) != -1)
        {
            char *word = strtok(line, " ");
            while (word)
            {
                add_item_to_dict(my_dict, word);
                word = strtok(NULL, " ");
            }
        }
    }

    sort_by_nr(my_dict);
    gettimeofday(&t2, NULL);
    float elapsed_time = (t2.tv_sec - t1.tv_sec) + 1e-6 * (t2.tv_usec - t1.tv_usec);
    printf("\nCompleted in %0.6f seconds", elapsed_time);
    printf("\n\n Most popular words from resulting dictionary:\n");
    print_dict(my_dict, 30);

    fclose(fp);
    free(line);
    return 0;
}