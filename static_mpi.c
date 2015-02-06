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

/*
	References:
	Type 0: .1 - 3.0 Seconds
	Type 1: 2.0 - 5.0
	Type 2: 1.0 6.0
	Type 3: 5.0 7.5
	Type 4: 7.0 9.0
*/


int main(int argc, char **argv){

	int queue[1024];
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if(rank == 0){
		int i = 0;
		for(i = 0; i < 1024; ++i){
			queue[i] = rand() % 4;
			printf("%d\n", queue[i]);
		}
	}




	return 0;
}