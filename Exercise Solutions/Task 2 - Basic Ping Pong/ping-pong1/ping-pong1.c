#include <stdio.h> 
#include <string.h> /* For strlen */ 
#include <mpi.h> /* For WI functions, etc */ 
const int MAX_STRING = 100; 

int main(void) { 
        char greeting[MAX_STRING];
        int comm_sz; /* Number of processes */ 
        int my_rank; /* My process rank */ 

        
        MPI_Init(NULL, NULL); 
        MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
        if (my_rank != 0) { 
                sprintf(greeting, "MESSAGE"); 
                for (int q = 1; q < 5; q++) { 
                        MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
                        printf("Message sent: 0 -> 1\n");
                        MPI_Recv(greeting, MAX_STRING, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                        printf("Message from 1: %s\n", greeting);
                }
                
        }
        else {
                //printf("Process 1: From process td of %d of %d \n", my_rank, comm_sz); 
                for (int q = 1; q < 5; q++) { 
                        MPI_Recv(greeting, MAX_STRING, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                        printf("Message from 0: %s\n", greeting);
                        MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                        printf("Message sent: 1 -> 0\n");
                }
                
        }
        MPI_Finalize(); 
        return 0; 
} /* main */ 