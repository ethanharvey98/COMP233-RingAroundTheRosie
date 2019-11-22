// Ethan Harvey ~ COMP 233 ~ Ring Around the Rosie
// MPI program that passes messages from node to node

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MILLION 1000000
#define TRIALS 10

int main(int argc, char** argv) {

	// List of sizes
	const int SIZES[12] = {1000, 50000, 100000, 200000, 300000,
	400000, 500000, 600000, 700000, 800000, 900000, 1000000};

	// Rank
	int world_rank;
	// Size
	int world_size;
	// Time variables
	double start, stop, total;
	// Fastest and average time
	double fastest, average = 0;
	// Slowest time
	double slowest;
	// Array of times
	double raw[TRIALS];
	// Array of calculated times
	double times[TRIALS];
	// Message to be passed around
	double* message = (double*)malloc(MILLION * sizeof(double));

    // Initialize MPI
	MPI_Init(NULL, NULL);

	// Get rank and size
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// If size is less than 2
	if (world_size < 2) {
		fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	// If master print header
	if (world_rank == 0) {
		printf("Ethan Harvey ~ COMP 233 ~ Ring Around the Rosie\n\n");
		printf("Bytes,Fastest,Slowest,Average\n");
	}

	// Loop through each size
	int size;
	for (size = 0; size < 12; size++) {

		// Run number of trials specified
		int trial;
		for (trial = 0; trial < TRIALS; trial++) {

			// If master
			if (world_rank == 0) {
				int i;
				for (i = 0; i < SIZES[size]; i++) {
					message[i] = SIZES[size] + trial;
				}

				// Start timer
				start = MPI_Wtime();

				MPI_Send(
					message, // data
					SIZES[size]+1, // count
					MPI_DOUBLE, // datatype
					world_rank+1, // destination
					0, // tag
					MPI_COMM_WORLD); // communicator

				MPI_Recv(
					message, // data
					SIZES[size]+1, // count
					MPI_DOUBLE, // datatype
					world_size-1, // source
					0, // tag
					MPI_COMM_WORLD, // communicator
					MPI_STATUS_IGNORE);

				// End timer
				stop = MPI_Wtime();
				total = stop - start;

				for (i = 0; i < SIZES[size]; i++) {
					// Check to make sure the message is the same
					if (message[i] != SIZES[size] + trial) {
						printf("Messages are not the same");
					}
				}

				raw[trial] = total;
			}
			else {

				MPI_Recv(
					message, // data
					SIZES[size]+1, // count
					MPI_DOUBLE, // datatype
					(world_rank-1) % world_size, // source
					0, // tag
					MPI_COMM_WORLD, // communicator
					MPI_STATUS_IGNORE);

				MPI_Send(
					message, // data
					SIZES[size]+1, // count
					MPI_DOUBLE, // datatype
					(world_rank+1) % world_size, // destination
					0, // tag
					MPI_COMM_WORLD); // communicator

			}
		}

		if (world_rank == 0) {
			// Initialize slowest to the first time
			slowest = times[0];

			// Set fastest, slowest, and average
			int i;
			for (i = 0; i < TRIALS; i++) {
				times[i] = raw[i]/world_size;
				if (times[i] > fastest) {
					fastest = times[i];
				}
				if (times[i] < slowest) {
					slowest = times[i];
				}
				average += times[i];
			}
			// Divide total by number of trials
			average = average/TRIALS;

			// Print 
			printf("%d,%f,%f,%f\n", 8*SIZES[size], fastest, slowest, average);
		}
	}
	
	// Free message
	free(message);

	// If master print footer
	if (world_rank == 0) {
		printf("\n<<< Normal Termination >>>");
	}

	MPI_Finalize();
	return 0;
}