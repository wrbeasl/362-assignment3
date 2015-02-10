#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

int main(int argc, char **argv){
	int queue[1024];
	int rank, size, i = 0, j = 0, Curr_Robin = 0, tag = 0;
	int Proc_Work_Counter[size];
	float Max_Times[5] = { 3.0, 5.0, 6.0, 7.5, 9.0 };

	clock_t Begin, End;
	double Execution_time;

	MPI_Init(&argc, &argv);
	Begin = clock();

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	srand(time(NULL));

	MPI_Status status;

	float Work_Sleep[size -1];
	float Work_Sleep_Avg[size- 1];
	float Type_Sleep[5] = { 0.0 };
	float Type_Sleep_Avg[5] = { 0.0 };

	int Types[5] = { 0 };

	for(i = 0; i < size; i++){
		Proc_Work_Counter[i] = 0;
	}

	if(rank == 0){
		float temp_sleep, temp_total;

		/* This populates the queue and incriments the Types array */
		for(i = 0; i < 1024; i++){
			queue[i] = rand() % 5;
			Types[queue[i]]++;
		}

		for(i = 0; i < 1024; ++ i){
			if(Curr_Robin == size) Curr_Robin = 1;

			MPI_Send(&queue[i], 1, MPI_INT, Curr_Robin, i, MPI_COMM_WORLD);
			Proc_Work_Counter[Curr_Robin]++;

			Curr_Robin++;

			MPI_Recv(&temp_sleep, 1, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			Type_Sleep[status.MPI_TAG] += temp_sleep;
			Work_Sleep[status.MPI_SOURCE] += temp_sleep;
		}

		/* Print outs go here */


	} else {
		int temp = 0;
		float Sleep_total = 0.0;

		while(status.MPI_TAG <= 1024){
			MPI_Recv(&temp, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

			printf("Process %d recieved value %d from master\n", rank, temp);

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
				MPI_Send(&sleepTime, 1, MPI_FLOAT, 0, tag, MPI_COMM_WORLD);
		}
	}


	return 0;

}