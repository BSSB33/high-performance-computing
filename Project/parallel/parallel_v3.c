#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include "../headers/uthash.h"

/**
 * Hashable version of the dictionary item struct
 * 
 */
struct dict_item_hh
{
    int nr;
    char word[25];     // we use this as key for the hash table
    UT_hash_handle hh; // makes the structure hashable
};

/**
 * Data structure for storing word and number of occurences
 * 
 */
struct dict_item
{
    int nr;
    char word[25];
};

typedef struct
{
    long tv_sec;
    long tv_usec;
} timeval;

const int READ_BY_WORDS = 0;
const int READ_BY_SENTENCES = 1;
const int STRING_MAX = 10000000;

void add_item_to_dict(struct dict_item_hh **dict, char *word)
{
    struct dict_item_hh *s;
    HASH_FIND_STR(*dict, word, s);
    if (s == NULL)
    {
        s = (struct dict_item_hh *)malloc(sizeof *s);
        strcpy(s->word, word);
        HASH_ADD_STR(*dict, word, s);
        s->nr = 1;
    }
    else
    {
        s->nr++;
    }
}

void add_item_to_master_dict(struct dict_item_hh **master_dict, char *word, int nr)
{
    struct dict_item_hh *s;
    HASH_FIND_STR(*master_dict, word, s);
    if (s == NULL)
    {
        s = (struct dict_item_hh *)malloc(sizeof *s);
        strcpy(s->word, word);
        HASH_ADD_STR(*master_dict, word, s);
        s->nr = nr;
    }
    else
    {
        s->nr += nr;
    }
}

void print_dict(struct dict_item_hh *dict, int max_items)
{
    struct dict_item_hh *s;
    int j = 1;
    for (s = dict; s != NULL && max_items > 0; s = (struct dict_item_hh *)(s->hh.next))
    {
        printf("%d. word: %s - nr: %d\n", j, s->word, s->nr);
        max_items--;
        j++;
    }
}

/*
    Comparison function for sorting by number of occurences
*/
int nr_sort(struct dict_item_hh *a, struct dict_item_hh *b)
{
    return (b->nr - a->nr);
}

void sort_by_nr(struct dict_item_hh **dict)
{
    HASH_SORT(*dict, nr_sort);
}


/* following code assumes all file operations succeed. In practice,
 * return codes from open, close, fstat, mmap, munmap all need to be
 * checked for error.
*/
int read_file_line(int fd, struct stat s, int line_no, char *output)
{
	char *buf;
	off_t start = -1, end = -1;
	size_t i;
	int ln, ret = 1;
 
	if (line_no == 1) start = 0;
	else if (line_no < 1){
		warn("line_no too small");
		return 0; /* line_no starts at 1; less is error */
	}
 
	line_no--; /* back to zero based, easier */
 
	/* Map the whole file.  If the file is huge (up to GBs), OS will swap
	 * pages in and out, and because search for lines goes sequentially
	 * and never accesses more than one page at a time, penalty is low.
	 * If the file is HUGE, such that OS can't find an address space to map
	 * it, we got a real problem.  In practice one would repeatedly map small
	 * chunks, say 1MB at a time, and find the offsets of the line along the
	 * way.  Although, if file is really so huge, the line itself can't be
	 * guaranteed small enough to be "stored in memory", so there.
	 */
	buf = mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
 
	/* optional; if the file is large, tell OS to read ahead */
	//madvise(buf, s.st_size, MADV_SEQUENTIAL);
 
	for (i = ln = 0; i < s.st_size && ln <= line_no; i++) {
		if (buf[i] != '\n') continue;
 
		if (++ln == line_no) start = i + 1;
		else if (ln == line_no + 1) end = i + 1;
	}
 
	if (start >= s.st_size || start < 0) {
		warn("file does not have line %d", line_no + 1);
		ret = 0;
	} else {
		/* 	do something with the line here, like
		write(STDOUT_FILENO, buf + start, end - start);
			or copy it out, or something
		*/
        strncpy(output, buf + start, end - start);
	}
 
	munmap(buf, s.st_size);
	close(fd);
 
	return ret;
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Usage: <program> <input-file> <0|1> <number_of_lines>"); // 0 - words, 1 - sentences
        exit(EXIT_FAILURE);
    }

    // FILE *fp;

    // struct dict_item_hh *my_dict = NULL;

    // fp = fopen(argv[1], "r");
    // if (fp == NULL)
    // {
    //     exit(EXIT_FAILURE);
    // }

    // int FILE_MODE = atoi(argv[2]);
    // if (FILE_MODE != READ_BY_SENTENCES || FILE_MODE != READ_BY_WORDS)
    // {
    //     printf("%d\n", FILE_MODE);

    //     exit(EXIT_FAILURE);
    // }

    int NR_OF_LINES = atoi(argv[3]);

    int comm_sz;
    int my_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int LINES_PER_PROCESS = NR_OF_LINES / comm_sz;

    /**
     * creating custom struct MPI datatype 
     */
    struct dict_item s;

    MPI_Datatype DictType;
    MPI_Datatype type[2] = {MPI_INT, MPI_CHAR};
    int blocklen[2] = {1, 25};
    MPI_Aint disp[2];
    MPI_Aint nr_addr, w_addr;
    MPI_Get_address(&s.nr, &nr_addr);
    MPI_Get_address(&s.word, &w_addr);
    disp[0] = 0;
    disp[1] = w_addr - nr_addr;
    MPI_Type_create_struct(2, blocklen, disp, type, &DictType);
    MPI_Type_commit(&DictType);

    int fd;
    struct stat st;
    fd = open(argv[1], O_RDONLY);
    fstat(fd, &st);

    if (my_rank != 0) // Childs
    {
        struct dict_item_hh *small_dict = NULL;
        char buf[STRING_MAX];
        int start = my_rank * LINES_PER_PROCESS + 1;
        int end;
        if (my_rank < comm_sz - 1)
        {
            end = start + LINES_PER_PROCESS - 1;
        }
        else {
            end = NR_OF_LINES;
        }
        printf("start: %d - end: %d - length: %d\n", start, end, end - start);
        
        for (int i = start; i <= end; i++)
        {
            read_file_line(fd, st, i, buf);
            char *word = strtok(buf, " \t\r\n\v\f");

            while (word)
            {
                add_item_to_dict(&small_dict, word);
                word = strtok(NULL, " \t\r\n\v\f");
            }
            printf("%d process reading them lines %d\n", my_rank, i);
        }

        //Sening the length of the small dictionary
        unsigned int small_dict_len = HASH_COUNT(small_dict);
        MPI_Send(&small_dict_len, 1, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);

        struct dict_item_hh *s;
        struct dict_item tmp[small_dict_len];
        int i = 0;
        for (s = small_dict; s != NULL; s = (struct dict_item_hh *)(s->hh.next))
        {
            struct dict_item k;
            k.nr = s->nr;
            strcpy(k.word, s->word);
            tmp[i] = k;
            i++;
        }
        MPI_Send(&tmp, small_dict_len, DictType, 0, 0, MPI_COMM_WORLD);
    }
    else // Main
    {
        struct timeval t1, t2;
        gettimeofday(&t1, NULL);

        struct dict_item_hh *MASTER_DICT = NULL;

        char buf[STRING_MAX];
        int start = my_rank * LINES_PER_PROCESS + 1;
        int end = start + LINES_PER_PROCESS - 1;
        //printf("start: %d - end: %d\n", start, end);
        
        for (int i = start; i <= end; i++)
        {
            read_file_line(fd, st, i, buf);
            char *word = strtok(buf, " \t\r\n\v\f");

            while (word)
            {
                add_item_to_dict(&MASTER_DICT, word);
                word = strtok(NULL, " \t\r\n\v\f");
            }
        }

        unsigned int dic_len[comm_sz];
        unsigned int i, j;
        for (i = 1; i < comm_sz; i++)
        {
            MPI_Recv(&dic_len[i], 1, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        
        // Recieving a small dictionary from every child and addint every record of it to the master dictionary
        for (i = 1; i < comm_sz; i++)
        {
            struct dict_item received_dict[dic_len[i]];
            MPI_Recv(&received_dict, dic_len[i], DictType, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (j = 0; j < dic_len[i]; j++)
            {                
                add_item_to_master_dict(&MASTER_DICT, received_dict[j].word, received_dict[j].nr);
            }
        }
        
        printf("Finalized dict\n");
        gettimeofday(&t2, NULL);
        printf("Final dic length: %d\n", HASH_COUNT(MASTER_DICT));
        float elapsed_time = (t2.tv_sec - t1.tv_sec) + 1e-6 * (t2.tv_usec - t1.tv_usec);

        printf("\nCompleted in %0.6f seconds\n", elapsed_time);
        printf("\n\nMost popular words from resulting dictionary:\n");
        sort_by_nr(&MASTER_DICT);
        print_dict(MASTER_DICT, 10);
    }
    // if (atoi(argv[2]) == 0)
    // {
    //     while ((read = getline(&line, &len, fp)) != -1)
    //         add_item_to_dict(my_dict, line);
    // }
    // else
    // {
    //     while ((read = getline(&line, &len, fp)) != -1)
    //     {
    //         char *word = strtok(line, " ");
    //         while (word)
    //         {
    //             add_item_to_dict(my_dict, word);
    //             word = strtok(NULL, " ");
    //         }
    //     }
    // }

    // sort_by_nr(my_dict);
    // gettimeofday(&t2, NULL);
    // printf("\nCompleted in %0.6f seconds", elapsed_time);
    // printf("\n\n Most popular words from resulting dictionary:\n");
    // print_dict(my_dict, 30);

    // fclose(fp);
    // free(line);
    MPI_Finalize();
    return 0;
}
