#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv)
{
    int rank = 0;
    int size = 0;

	double PI = 3.14;
	double time = 0.0;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank( MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD, &size);

	int series_size = 1000;

    if(rank == 0)
    {
        int segment_size = series_size / (size - 1);
        double answer = 0;
		double send = 0;

        if(series_size > segment_size * (size - 1))
            segment_size++;

        MPI_Barrier( MPI_COMM_WORLD);
		time = MPI_Wtime();

		//Send length of piece of series to compute to all processes
        MPI_Bcast( &segment_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

		//Collect all data
        MPI_Reduce( &send, &answer, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        MPI_Barrier( MPI_COMM_WORLD);
		time = MPI_Wtime() - time;

		//Print answer and work time
        printf("Answer = %lf,\twork time = %1.3lf ms\n", answer, time * 1000);
    }
    else
    {
        double answer = 0;
		double recv = 0;

        MPI_Barrier( MPI_COMM_WORLD);

		//Receive length of piece of series to compute
        int segment_size = 0;
        MPI_Bcast( &segment_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

        int i = (rank - 1) * segment_size;
        if(i == 0)
            i++;

		//Compute piece of series
        while( (i != rank * segment_size) && (i < series_size))
        {
            answer += 6.0 / ((PI * PI) * (double) (i * i));
            i++;
        }

		//Send computed answer to the root
        MPI_Reduce( &answer, &recv, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        MPI_Barrier( MPI_COMM_WORLD);
    }


    MPI_Finalize();

    return 0;
}
