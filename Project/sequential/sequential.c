#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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

struct dict_item *my_dict = NULL;

void add_item(char *word)
{
    struct dict_item *s;

    HASH_FIND_STR(my_dict, word, s);
    if (s == NULL)
    {
        s = (struct dict_item *)malloc(sizeof *s);
        // s->word = word;
        strcpy(s->word, word);
        HASH_ADD_STR(my_dict, word, s);
        s->nr = 1;
    }
    else
    {
        s->nr++;
    }
}

void print_dict(int max_items)
{
    struct dict_item *s;
    if (max_items == -1)
    {

        for (s = my_dict; s != NULL; s = (struct dict_item *)(s->hh.next))
        {
            printf("word: %s - nr: %d\n", s->word, s->nr);
        }
    }
    else
    {
        int j = 1;
        for (s = my_dict; s != NULL && max_items > 0; s = (struct dict_item *)(s->hh.next))
        {
            printf("%d. word: %s - nr: %d\n", j, s->word, s->nr);
            max_items--;
            j++;
        }
    }
}

/*
    Comparison function for sorting by number of occurences
*/
int nr_sort(struct dict_item *a, struct dict_item *b)
{
    return (b->nr - a->nr);
}

void sort_by_nr()
{
    HASH_SORT(my_dict, nr_sort);
}

int main(int argc, char *argv[])
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
    gettimeofday(&t1, NULL);

    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        exit(EXIT_FAILURE);
    }

    if (atoi(argv[2]) == 0)
    {
        while ((read = getline(&line, &len, fp)) != -1)
            add_item(line);
    }
    else
    {
        char last_word[20];
        while ((read = getline(&line, &len, fp)) != -1)
        {
            char *word = strtok(line, " \t\r\n\v\f");
            while (word)
            {
                add_item(word);
                strcpy(last_word, word);
                word = strtok(NULL, " \t\r\n\v\f");
            }
        }
        printf("last word: %s\n", last_word);
    }

    sort_by_nr();
    gettimeofday(&t2, NULL);
    float elapsed_time = (t2.tv_sec - t1.tv_sec) + 1e-6 * (t2.tv_usec - t1.tv_usec);
    printf("\nCompleted in %0.6f seconds\n", elapsed_time);
    printf("Final dic length: %d\n", HASH_COUNT(my_dict));
    printf("\n\n Most popular words from resulting dictionary:\n");
    print_dict(10);

    fclose(fp);
    free(line);

    return 0;
}