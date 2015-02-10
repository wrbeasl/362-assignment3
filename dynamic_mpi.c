/* 
	William Beasley
	wrbeasl
	CPSC 3620 Assignment 3
	static_mpi.c
	Description: This is a dynamic mpi workload program.

*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#define MAX_LOAD 1024

/*
	References:
	Type 0: .1 - 3.0 Seconds
	Type 1: 2.0 - 5.0
	Type 2: 1.0 6.0
	Type 3: 5.0 7.5
	Type 4: 7.0 9.0
*/

/** 

	BASE THE VALUES TO EACH PROCESS BY THE MAX TIMES


**/


int main(int argc, char **argv){

		int queue[MAX_LOAD];
		int rank, size;
		int proc_count[size-1];
		int workLoadcount[size-1];
		float timeMax[5] = {3.0, 5.0, 6.0, 7.5, 9.0};

		clock_t begin, end;
		double time_spent;

		srand(time(NULL));

		MPI_Init(&argc, &argv);
		begin = clock();
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		MPI_Status status;

		float W_Sleep[size - 1];
		float W_Sleep_AVG[size - 1];
		float T_Sleep[5] = { 0.0 };
		float T_Sleep_AVG[5] = {0.0};

		int Types[5];

		int j = 0, curr_rob = 1;
		int counts[size-1];
		int disp[size-1];

		// Populate the size specified arrays
		for(j = 1; j < size ; ++j){
			counts[j] = 0;
			disp[j] = 0;
			proc_count[j] = 0;
		}

		// Begin the masters work
		if(rank == 0){
			int i = 0, tag = 0; 
			float tempSleep, tempTotal;

			// Populate the queue and incriment the Types array depending on the random value
			for( i = 0; i < MAX_LOAD; ++i){
				queue[i] = rand() % 5;
				Types[queue[i]]++;
			}

			// Send one queue value to a worker and await a response from said worker
			// Save the response in the appropriate locations
			for(i = 0; i < MAX_LOAD; ++i){
				if(curr_rob > size-1) curr_rob = 1;

				MPI_Send(&queue[i], 1, MPI_INT, curr_rob, i, MPI_COMM_WORLD);
				proc_count[curr_rob]++;
				curr_rob++;

				MPI_Recv(&tempSleep, 1, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				T_Sleep[status.MPI_TAG] += tempSleep;
				W_Sleep[status.MPI_SOURCE] += tempSleep;
			}

			int t = 1024;
			for(i = 1; i < size; ++i){
				MPI_Send(&t, 1, MPI_INT, i, i, MPI_COMM_WORLD);
			}

			/* printing for loops */

			for(i = 0; i < 5; ++i){
				T_Sleep_AVG[i] = T_Sleep[i]/Types[i];
				printf("Total time slept for type %d is %lf\n", i, T_Sleep[i]);
				printf("Average time slept for type %d is %lf\n", i, T_Sleep_AVG[i]);
				printf("\n");
			}

			for(i = 0; i < 5; ++i){
				printf("Number of workloads for Type %d : %d\n", i, Types[i]);

			}

			printf("\n");

			for(i = 1; i < size; ++i){
				printf("Total number of workloads for process %d : %d\n", i, proc_count[i]);
				printf("\n");
			}

			for(i = 1; i < size; ++i){
				W_Sleep_AVG[i] = W_Sleep[i]/proc_count[i];
				printf("Total time slept for process %d is %lf\n", i, W_Sleep[i]);
				printf("Average time slept for process %d is %lf\n", i, W_Sleep_AVG[i]);
				printf("\n");
			}
		} else {
			int temp = 0, currTag = 0;
			int i = 0;
			float total_sleep_time = 0.0;
			while(status.MPI_TAG <= MAX_LOAD-1){
				MPI_Recv(&temp, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				if(temp == 1024) break;

				float sleepTime = ((float)rand()/(float)(RAND_MAX));

				switch(temp){
					case 0:
						sleepTime = (.1 + (sleepTime * (3.0 - .1)));
						break;
					case 1:
						sleepTime = (2.0 + (sleepTime * (5.0 - 2.0)));
						break;
					case 2:
						sleepTime = (1.0 + (sleepTime * (6.0 - 1.0)));
						break;
					case 3:
						sleepTime = (5.0 + (sleepTime * (7.5 - 5.0)));
						break;
					case 4:
						sleepTime = (7.0 + (sleepTime * (9.0 - 7.0)));
						break;
				}

				usleep((useconds_t) (sleepTime * 1000));
				int tag = temp;
				MPI_Send(&sleepTime, 1, MPI_FLOAT, 0, tag, MPI_COMM_WORLD);

			}

		}
        end = clock();

        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        if(rank == 0) printf("Total execution time: %lf\n", time_spent);
        MPI_Finalize();
	return 0;
}
