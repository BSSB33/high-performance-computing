#include <stdio.h> 
#include <string.h> /* For strlen */ 
#include <mpi.h> /* For WI functions, etc */ 
const int MAX_STRING = 100; 

int main(void) { 
        int comm_sz; /* Number of processes */ 
        int my_rank; /* My process rank */ 

        MPI_Init(NULL, NULL); 
        MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
        if (my_rank != 0) { // Childs
                MPI_Send(&my_rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("Process %d - Sending the message: %d, to process %d\n", my_rank, my_rank, 0);
        } 
        else { // Main
                int sum = 0;
                for (int q = 1; q < comm_sz; q++) {
                        int recievedFromChild; 
                        MPI_Recv(&recievedFromChild, 1, MPI_INT, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                        printf("Process %d - MESSAGE RECIEVED: %d!\n", my_rank, recievedFromChild);
                        sum += recievedFromChild;        
                }
                printf("All numbers are recieved, sum: %d, avg: %d\n", sum, sum/(comm_sz-1));
        }
        MPI_Finalize(); 
        return 0; 
} /* main */ 