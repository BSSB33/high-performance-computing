#include <stdio.h> 
#include <string.h> /* For strlen */ 
#include <mpi.h> /* For WI functions, etc */ 
const int MAX_STRING = 100; 
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
int read_file_line(const char *path, int line_no, int end_line_no)
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
                /* 	do something with the line here, like
		write(STDOUT_FILENO, buf + start, end - start);
                printf("%s\n", buf);    // Buffer, content of the whole text from the chunk
                printf("%i\n", start);  // Start of the chunk
                printf("%i\n", end);    // End of the chunk
			or copy it out, or something
		*/
                if(end_line_no <= line_no){
                        warn("ERROR: Ending line number can not be smaller then beginning line number!");
                        printf("ERROR: Ending line number can not be smaller then beginning line number!");
                        ret = 0;
                }
                else {
                        write(STDOUT_FILENO, buf + start, end - start);
                }
        }       
 
	munmap(buf, s.st_size);
	close(fd);
 
	return ret;
}

int main(void) { 
        int comm_sz; /* Number of processes */ 
        int my_rank; /* My process rank */ 

        MPI_Init(NULL, NULL); 
        MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
        if (my_rank == 0) { // Main
        //TODO Broadcast first line of the files and open every file from n+1 (skipping the first line - alias number of rows)
                //printf("Process %i out of %i - Main\n", my_rank, comm_sz);
                read_file_line("/home/gabor.vitrai/proj/alice/alice_sentences.txt", 900, 902);
        } 
        else { // Childs
                //printf("Process %i out of %i - Child\n", my_rank, comm_sz);
        }
        MPI_Finalize(); 
        return 0; 
} /* main */ 

