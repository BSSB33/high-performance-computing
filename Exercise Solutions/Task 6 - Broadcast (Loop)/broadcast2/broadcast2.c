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
        if (my_rank != 0) { // Childs
                MPI_Recv(greeting, MAX_STRING, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                printf("Process %d - MESSAGE RECIEVED: %s!\n", my_rank, greeting);
        } 
        else { // Main
                for (int q = 1; q < comm_sz; q++) { 
                        sprintf(greeting, "MESSAGE"); 
                        MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, q, 0, MPI_COMM_WORLD);
                        printf("Process %d - Sending the message: %s, to process %d\n", my_rank, greeting, q);
                }
        }
        MPI_Finalize(); 
        return 0; 
} /* main */ 