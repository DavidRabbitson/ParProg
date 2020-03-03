#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
    int rank =  0;
    int size =  0;
    int buf  = -1;

    MPI_Init( &argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank != 0)
        MPI_Recv( &buf, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);

    printf("I'm [%d]\n", rank);
    fflush(stdout);

    if (rank != (size - 1))
        MPI_Send( &rank, 1, MPI_INT, rank + 1, rank, MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}

