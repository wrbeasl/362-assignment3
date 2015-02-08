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
	int i, j;
	float maxtime, perTime;
	int types[5];
	float timeMax[5] = {3.0, 5.0, 6.0, 7.5, 9.0};
	float *times;
	
	srand(time(NULL));
	
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	
	int counts[size-1];
	int disp[size-1];

	if(rank == 0){
		int i = 0;
		for(i = 0; i < MAX_LOAD; ++i){
			queue[i] = rand() % 5;
			switch(queue[i]){
				case 0:
					maxtime+=timeMax[0];
					types[0]++;
					break;
				case 1:
					maxtime+=timeMax[1];
					types[1]++;
					break;
				case 2:
					maxtime+=timeMax[2];
					types[2]++;
					break;
				case 3:
					maxtime+=timeMax[3];
					types[3]++
					break;
				case 4:
					maxtime+=timeMax[4];
					types[4]++;
					break;
			}
		}

		//printf("Curr total time: %lf\n", sum);
		
		perTime = floor(maxtime/((float)size-1));
		
		j = 0;
		float time;
		int total = 0;
		for(i = 1; i < size; i++){
			time = 0;
			disp[i-1] = j;
			while(time < perTime && j < MAX_LOAD){
				time += timeMax[queue[j]];
				j++;
				counts[i-1]++;
			}
		}
		
		//this is where we begin setting up the sending process to the slave processes. I haven't figured a way to do this, but I think a 'for' loop might work. I'm not sure how arrays are sent beyond using scatterv, gatherv
		//What about a for loop that sends based on the process number and sends a preliminary ID-start code and a # of processes code?
		for(i = 1; i < size; i++){
			MPI_Send(&counts[i-1], 1, MPI_INT, i, 2, MPI_COMM_WORLD);
			MPI_Send(&disp[i-1], 1, MPI_INT, i, 3, MPI_COMM_WORLD);
		}
		
		for(i = 1; i < size; i++){
			MPI_Send(&queue[disp[i-1]], counts[i-1], MPI_INT, i, 4, MPI_COMM_WORLD);
		}
	}
	
	else{
		//This is subprocessor work area. This will need to be sent using a object of some sort, maybe. Could use tags 2 and 3 to send times and ID codes?
		int numElem, startPoint;
		MPI_Recv(&numElem, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
		MPI_Recv(&startPoint, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
		
		int jobs[numElem];
		
		MPI_Recv(&jobs, numElem, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);
		
		for(i = 0; i < numElem; ++i){
			//do work and such
		}
	}
	
	if(rank == 0){
		//Receiving area. Letting everyone know what to do here.
	}


        MPI_Finalize():

	return 0;
}
