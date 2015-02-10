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
#include <sys/types.h>

int main(int argc, char **argv){

	int queue[1024];
	int rank, size;
	int i, j;
	float maxtime, perTime;
	int Types[5];
	float timeMax[5] = {3.0, 5.0, 6.0, 7.5, 9.0};
	float *times;
	
	clock_t start, end;
	
	srand(time(NULL));
	
	
	MPI_Init(&argc, &argv);
	start = clock();
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	
	int workCount[size-1];
	int Proc_Count[size-1];
	int Proc_Displacement[size-1];
	
	float workSleep[size - 1];
	float workSleepAvg[size - 1];
	float typeSleep[5];
	float typeSleepAvg[5];
	
	for(i = 0; i < size-1; i++){
		Proc_Count[i] = 0;
		Proc_Displacement[i] = 0;
	}

	if(rank == 0){
		int i = 0;
		for(i = 0; i < 1024; ++i){
			queue[i] = rand() % 5;
			switch(queue[i]){
				case 0:
					maxtime+=timeMax[0];
					Types[0]++;
					break;
				case 1:
					maxtime+=timeMax[1];
					Types[1]++;
					break;
				case 2:
					maxtime+=timeMax[2];
					Types[2]++;
					break;
				case 3:
					maxtime+=timeMax[3];
					Types[3]++;
					break;
				case 4:
					maxtime+=timeMax[4];
					Types[4]++;
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
			Proc_Displacement[i-1] = j;
			while(time < perTime && j < 1024){
				time += timeMax[queue[j]];
				j++;
				Proc_Count[i-1]++;
			}
		}
		
		//this is where we begin setting up the sending process to the slave processes. I haven't figured a way to do this, but I think a 'for' loop might work. I'm not sure how arrays are sent beyond using scatterv, gatherv
		//What about a for loop that sends based on the process number and sends a preliminary ID-start code and a # of processes code?
		for(i = 1; i < size; i++){
			MPI_Send(&Proc_Count[i-1], 1, MPI_INT, i, 2, MPI_COMM_WORLD);
			MPI_Send(&Proc_Displacement[i-1], 1, MPI_INT, i, 3, MPI_COMM_WORLD);
		}
		
		for(i = 1; i < size; i++){
			MPI_Send(&queue[Proc_Displacement[i-1]], Proc_Count[i-1], MPI_INT, i, 4, MPI_COMM_WORLD);
			workCount[i-1] = Proc_Count[i-1];
			//printf("%d\n", workCount[i-1]);
		}
		
		float tempSleep;
		
		for(i = 0; i < 1024; i++){
			MPI_Recv(&tempSleep, 1, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			typeSleep[queue[status.MPI_TAG]] += tempSleep;
			workSleep[status.MPI_SOURCE-1] += tempSleep;
		}
		
		for(i = 0; i < 5; i++){
			typeSleepAvg[i] = typeSleep[i]/Types[i];
			printf("Total time slept for type %d is %f\n", i, typeSleep[i]);
			printf("Average time slept for type %d is %f\n\n", i, typeSleepAvg[i]);
		}
		
		int ID;
		
		for(i = 0; i < size-1; i++){
			ID = i + 1;
			workSleepAvg[i] = workSleep[i]/Proc_Count[i];
			printf("Total time slept for process %d is %f\n", ID, workSleep[i]);
			printf("Average time slept for process %d is %f\n\n", ID, workSleepAvg[i]);
		}

		for(i = 1; i < size; ++i){
			printf("Total number of workloads for process %d : %d\n", i, workCount[i-1]);
			printf("\n");
		}

		for(i = 0; i < 5; ++i){
			printf("Number of workloads for Type %d : %d\n\n", i, Types[i]);
		}


	}
	
	else{
		// The working processes area
		int Number_Of_Elements, Starting_Point;
		MPI_Recv(&Number_Of_Elements, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
		MPI_Recv(&Starting_Point, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
		
		int jobs[Number_Of_Elements];
		int tag;
		
		MPI_Recv(&jobs, Number_Of_Elements, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);
		
		float sleepTime;
		
		for(i = 0; i < Number_Of_Elements; ++i){
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
			tag = Starting_Point + i;
			MPI_Send(&sleepTime, 1, MPI_FLOAT, 0, tag, MPI_COMM_WORLD);
			}
	}

		end = clock();
		if(rank == 0) printf("Total execution time: %lf\n",(double)(end - start)/CLOCKS_PER_SEC);
        MPI_Finalize();

	return 0;
}
