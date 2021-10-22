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
        int backup_rank = my_rank;

        if (backup_rank == 0) { // Main
                int a = 3; 
                printf("Process %d - Sending original message: %d, to process %d\n", backup_rank, a, 1);
                MPI_Send(&a, sizeof(a), MPI_INT, backup_rank + 1, 0, MPI_COMM_WORLD);
        } 
        else if(backup_rank < comm_sz - 1){
                int b;
                MPI_Recv(&b, sizeof(b), MPI_INT, backup_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                printf("Process %d - MESSAGE RECIEVED: %d!\n", backup_rank, b);
                MPI_Send(&b, sizeof(b), MPI_INT, backup_rank + 1, 0, MPI_COMM_WORLD);
        }
        else{
                int c;
                MPI_Recv(&c, sizeof(c), MPI_INT, backup_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                printf("Process %d - MESSAGE RECIEVED: %d!\n", backup_rank, c);
        }



        MPI_Finalize(); 
        return 0; 
} /* main */ 