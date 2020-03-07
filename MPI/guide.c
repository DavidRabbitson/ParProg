#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
    int rank;
    int size;
    int buf = 0;
    int count = 1;

    MPI_Init( &argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank != 0)
    {
        MPI_Recv( &buf, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
        printf("I'm [%d]\n", rank);
        MPI_Send( &rank, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
    }
    else
    {
        printf("I'm [%d]\n", rank);
        while (count < size)
        {
            MPI_Send( &rank, 1, MPI_INT, count, rank, MPI_COMM_WORLD);
            MPI_Recv( &buf, 1, MPI_INT, count, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
            count++;
        }
    }

    MPI_Finalize();
    return 0;
}

