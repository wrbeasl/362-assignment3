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
	int i, j;
	float maxtime, perTime;
	int types[5];
	float timeMax[5] = {3.0, 5.0, 6.0, 7.5, 9.0};
	float *times;
	
	double start, end;
	
	srand(time(NULL));
	
	
	MPI_Init(&argc, &argv);
	start = MPI_Wtime();
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	
	int counts[size-1];
	int disp[size-1];
	
	float workSleep[size - 1];
	float workSleepAvg[size - 1];
	float typeSleep[5];
	float typeSleepAvg[5];
	
	for(i = 0; i < size-1; i++){
		counts[i] = 0;
		disp[i] = 0;
	}

	// Populate the master processes queue with workload values
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
					types[3]++;
					break;
				case 4:
					maxtime+=timeMax[4];
					types[4]++;
					break;
			}
		}
		//printf("Curr total time: %lf\n", sum);
		
		perTime = floor(maxtime/((float)size-1));
		
		// calculate the maximum time 
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


		int rr = 1;
		for(i = 1; i < size; i++){
			if(rr > size) rr = 1;
			MPI_Send(&queue[disp[i-1]], counts[i-1], MPI_INT, i, rr, MPI_COMM_WORLD);
			rr++;
		}
		
		float tempSleep;
		
		for(i = 0; i < MAX_LOAD; i++){
			MPI_Recv(&tempSleep, 1, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			typeSleep[queue[status.MPI_TAG]] += tempSleep;
			workSleep[status.MPI_SOURCE-1] += tempSleep;
		}
		
		for(i = 0; i < 5; i++){
			typeSleepAvg[i] = typeSleep[i]/types[i];
			printf("Total time slept for type %d is %f\n", i, typeSleep[i]);
			printf("Average time slept for type %d is %f\n", i, typeSleepAvg[i]);
		}
		
		int ID;
		
		for(i = 0; i < size-1; i++){
			ID = i + 1;
			workSleepAvg[i] = workSleep[i]/counts[i];
			printf("Total time slept for process %d is %f\n", ID, workSleep[i]);
			printf("Average time slept for process %d is %f\n", ID, workSleepAvg[i]);
		}
	}
	
	else{
		//This is subprocessor work area. This will need to be sent using a object of some sort, maybe. Could use tags 2 and 3 to send times and ID codes?
		int numElem, startPoint;
		MPI_Recv(&numElem, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
		MPI_Recv(&startPoint, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
		
		int jobs[numElem];
		int tag;
		
		MPI_Recv(&jobs, numElem, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);
		
		float sleepTime;
		
		for(i = 0; i < numElem; ++i){
			sleepTime = ((float)rand()/(float)(RAND_MAX));
			switch(jobs[i]){
				case 0:
					sleepTime *= 3.0;
					break;
				case 1:
					sleepTime *= 3.0;
					sleepTime += 2.0;
					break;
				case 2:
					sleepTime *= 5.0;
					sleepTime += 1.0;
					break;
				case 3:
					sleepTime *= 2.5;
					sleepTime += 5.0;
					break;
				case 4:
					sleepTime *= 2.0;
					sleepTime += 7.0;
					break;
			}
			
			usleep((useconds_t) (sleepTime * 1000));
			tag = startPoint + i;
			MPI_Send(&sleepTime, 1, MPI_FLOAT, 0, tag, MPI_COMM_WORLD);
			}
	}


        MPI_Finalize();

	return 0;
}
