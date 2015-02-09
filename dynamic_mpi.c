/* 
	William Beasley
	wrbeasl
	CPSC 3620 Assignment 3
	static_mpi.c
	Description: This is a static mpi workload program.

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


		srand(time(NULL));

		MPI_Init(&argc, &argv);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		MPI_Status status;

		int j = 0;
		int counts[size-1];
		int disp[size-1];
		for(j = 0; j < (size - 1); ++j){
			counts[j] = 0;
			disp[j] = 0;
		}

		if(rank == 0){
			int i = 0, rr = 0; 
			for( i = 0; i < MAX_LOAD; ++i){
				queue[i] = rand() % 5;
				printf("Test: %d\n", queue[i]);
			}

			int curr_robin = 1;
			for(i = 1; i < size; ++i){
				if(curr_robin > size) curr_robin = 1;
				MPI_Send(&queue[i], 1, MPI_INT, curr_robin, 4, MPI_COMM_WORLD);
				curr_robin++;
			}


		} else {
			int count = 0;
			while(count < MAX_LOAD){
				int temp = 0;
				MPI_Recv(&temp, 1, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);
				printf("Test2: %d\n", temp);
				count++;
			}
		}

        MPI_Finalize();

	return 0;
}
