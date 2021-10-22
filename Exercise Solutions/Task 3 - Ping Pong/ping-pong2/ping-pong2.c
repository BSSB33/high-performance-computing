#include <stdio.h> 
#include <string.h> /* For strlen */ 
#include <mpi.h> /* For WI functions, etc */ 
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
const int MAX_STRING = 100; 

int main(void) { 
        //char greeting[MAX_STRING];
        int comm_sz; /* Number of processes */ 
        int my_rank; /* My process rank */ 

        int count_0 = 30;
        int count_1 = 30;
        int end = -100;
        
        /*Random Number Generation*/
        static int r[100];
        int i;

        srand((unsigned)time( NULL ));
        for ( i = 0; i < 100; ++i) {
            r[i] = rand()%(1-11);
        }
        
        MPI_Init(NULL, NULL); 
        MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
        
        if (my_rank != 0) { 
            int random_index = 0;
            while(count_0 > 0){
                int a = r[random_index];
                printf("Process 0 generated the random number of: %d\n", a);
                random_index += 2;

                MPI_Send(&a, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("0: Number sent: 0 -> 1 (%d)\n", a);

                int numberByChild;
                MPI_Recv(&numberByChild, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                printf("0: Number recieved: 1 -> 0 (%d)\n", numberByChild);

                //Chekc if other process won:
                if(numberByChild == -100){
                    printf("Process 1 ended, finishing Main");
                    break;
                }

                count_0 -= numberByChild;
                printf("count_0: %d\n", count_0);
            }
    
            if(count_0 <= 0){
                MPI_Send(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("Process 0 WON!\n");    
            }
        }
        else {
            int random_index = 1;
            while(count_1 > 0){
                int b = r[random_index];
                printf("Process 1 generated the random number of: %d\n", b);
                random_index += 2;

                int numberByMain;
                MPI_Recv(&numberByMain, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                printf("1: Number recieved: 0 -> 1 (%d)\n", numberByMain);

                //Chekc if other process won:
                if(numberByMain == -100){
                    printf("Process 0 ended, finishing child");
                    break;
                }

                MPI_Send(&b, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("1: Number sent: 0 -> 1 (%d)\n", b);

                count_1 -= numberByMain;
                printf("count_1: %d\n", count_1);
            }
            
            if(count_1 <= 0){
                MPI_Send(&end, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("Process 1 WON!\n");
            }
        }

        MPI_Finalize(); 
        return 0; 
} /* main */ 

