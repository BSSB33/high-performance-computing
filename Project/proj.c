#include <stdio.h> 
#include <string.h> /* For strlen */ 
#include <mpi.h> /* For WI functions, etc */ 
const int MAX_STRING = 100000; 
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
 
/* following code assumes all file operations succeed. In practice,
 * return codes from open, close, fstat, mmap, munmap all need to be
 * checked for error.
*/
int read_file_line(const char *path, int line_no, int end_line_no, char *output)
{
	struct stat s;
	char *buf;
	off_t start = -1, end = -1;
	size_t i;
	int ln, fd, ret = 1;
 
	if (line_no == 1) start = 0;
	else if (line_no < 1){
		warn("Line_no too small");
		return 0; /* line_no starts at 1; less is error */
	}
 
	line_no--; /* back to zero based, easier */
 
	fd = open(path, O_RDONLY);
	fstat(fd, &s);

	if (fd == -1) {
		warn("open");
		printf("%s\n", path);
		return 0;
	}

	if (fd == NULL) {
		warn("Failed to open file");
		return 0;
	}
	if (s.st_size == 0) {
		warn("File is empty");
		return 0;
	}
 
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

    int line_count = (end_line_no - line_no) - 1;
	printf("line_count: %d\n", line_count);
	for (i = ln = 0; i < s.st_size && ln <= line_no + line_count; i++) {
		if (buf[i] != '\n') continue;

		if (++ln == line_no) start = i + 1;
		else if (ln >= line_no + 1) end = i + 1;
	}
 
	if (start >= s.st_size || start < 0) {
		warn("File does not have line %d", line_no + 1);
		ret = 0;
	} 
        else {
				//write(STDOUT_FILENO, buf + start, end - start);
				//strncpy(output, buf + start, end - start);
                if(end_line_no <= line_no) {
					warn("ERROR: Ending line number can not be smaller then beginning line number!");
					printf("ERROR: Ending line number can not be smaller then beginning line number!");
					ret = 1;
                }
                else {
					strncpy(output, buf + start, end - start);
					
					ret = 0;
                }
        }       
 
	munmap(buf, s.st_size);
	close(fd);
 
	return ret;
}

struct node {
	char word[25];	//key
	int nr;			//value
};

int main(void) { 
		char *filename = "./alice/alice_sentences.txt";
		//char *filename = "/home/gabor.vitrai/proj/got/got_sentences.txt";
        int comm_sz; /* Number of processes */ 
        int my_rank; /* My process rank */ 

        MPI_Init(NULL, NULL); 
        MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

		
/* 		struct node s;

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
			MPI_Type_commit(&DictType); */

        if (my_rank == 0) { // Main

			char *line;
			read_file_line(filename, 1, 10, line);
			printf("%s\n", line);
/* 
			struct node data[5] = {{"alice", 5}, {"bob", 10}, {"charlie", 15}, {"david", 20}, {"eve", 25}};

			int size = sizeof(data) / sizeof(data[0]);
			for (int q = 1; q < comm_sz; q++) { 
				MPI_Send(&size, sizeof(size), MPI_INT, q, 0, MPI_COMM_WORLD); 
				printf("MAIN: Process %d - Sending size count: %d, to process %d\n", my_rank, size, q);
				
				MPI_Send(&data, size, DictType, q, 0, MPI_COMM_WORLD);
				printf("MAIN: Process %d - Sending struct to process %d\n", my_rank, q);
			} */
        } 
        else { // Childs
/* 		
			int size;
			MPI_Recv(&size, sizeof(size), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
            printf("CHILD: Process %d - Size recieved: %d!\n", my_rank, size);

			struct node data[size];
			MPI_Recv(&data, size, DictType, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
			pr intf("CHILD: Process %d - Struct recieved from:\n", my_rank);*/
        }
        MPI_Finalize(); 
        return 0; 
} /* main */

// Problems:
// 1. We can not save method output to varibale
// 2. Child rank is always 4
// 3. Big text input is still not working perfectly
