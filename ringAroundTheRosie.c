#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {

	const int MILLION = 1000000;
	const int SIZE = 1000;

    // Initialize MPI
	MPI_Init(NULL, NULL);

	// Get rank and size
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// If size is less than 2
	if (world_size < 2) {
		fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	double* message = (double*)malloc(MILLION * sizeof(double));


	if (world_rank == 0) {
		for (int i = 0; i < SIZE; i++) {
			message[i] = 0.00;
		}

		// Time variables
		double start, stop, total;

		start = MPI_Wtime();

		MPI_Send(
			&message, // data
			sizeof(message), // count
			MPI_INT, // datatype
			1, // destination
			0, // tag
			MPI_COMM_WORLD); // communicator

		MPI_Recv(
			&message, // data
			sizeof(message), // count
			MPI_INT, // datatype
			(world_size-1), // source
			0, // tag
			MPI_COMM_WORLD, // communicator
			MPI_STATUS_IGNORE);

		stop = WPI_Wtime();
		total = stop - start;

		printf(total);

	}
	else {

		MPI_Recv(
			&message, // data
			sizeof(message), // count
			MPI_INT, // datatype
			world_rank-1, // source
			0, // tag
			MPI_COMM_WORLD, // communicator
			MPI_STATUS_IGNORE);

		MPI_Send(
			&message, // data
			sizeof(message), // count
			MPI_INT, // datatype
			world_rank % world_size, // destination
			0, // tag
			MPI_COMM_WORLD); // communicator

	}

	free(message);
	MPI_Finalize();
}