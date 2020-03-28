#include<stdio.h>
#include<mpi.h>

int main(int argc, char* argv[])
{
    //Change this variable to gain more/less statistics
    int measurements  = 2048;

    int rank =   0;
    int size =   0;

    int buf[16] = {};
    int send_buf = 0;
    int recv_buf = 0;
    int gather_buf[256] = {};

    double bcast_start  = 0;
    double bcast_finish = 0;
    double bcast_time   = 0;

    double reduce_start  = 0;
    double reduce_finish = 0;
    double reduce_time   = 0;

    double gather_start  = 0;
    double gather_finish = 0;
    double gather_time   = 0;

    double scatter_start  = 0;
    double scatter_finish = 0;
    double scatter_time   = 0;


    MPI_Init( &argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    int i = 0;

    //Measure time of MPI_Bcast()
    for(i = 0; i < measurements; i++)
    {
        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            bcast_start = MPI_Wtime();
        }

        MPI_Bcast( &buf, 16, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            bcast_finish = MPI_Wtime();
            bcast_time += bcast_finish - bcast_start;
        }
    }


    //Measure time of MPI_Reduce()
    for(i = 0; i < measurements; i++)
    {
        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            reduce_start = MPI_Wtime();
        }

        MPI_Reduce( &send_buf, &recv_buf, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);        

        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            reduce_finish = MPI_Wtime();
            reduce_time += reduce_finish - reduce_start;
        }
    }


    //Measure time of MPI_Gather()
    for(i = 0; i < measurements; i++)
    {
        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            gather_start = MPI_Wtime();
        }

        MPI_Gather( &send_buf, 1, MPI_INT, &gather_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            gather_finish = MPI_Wtime();
            gather_time += gather_finish - gather_start;
        }
    }


    //Measure time of MPI_Scatter()
    for(i = 0; i < measurements; i++)
    {
        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            scatter_start = MPI_Wtime();
        }

        MPI_Scatter( &send_buf, 1, MPI_INT, &recv_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);        

        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            scatter_finish = MPI_Wtime();
            scatter_time += scatter_finish - scatter_start;
        }
    }


    //Print section
    if(rank == 0)
    {
        //Convert results from seconds to milliseconds
        bcast_time = 1000 * (bcast_time / ((float) measurements));
        reduce_time = 1000 * (reduce_time / ((float) measurements));
        gather_time = 1000 * (gather_time / ((float) measurements));
        scatter_time = 1000 * (scatter_time / ((float) measurements));

        printf("MPI_Bcast work time:   %0.3lf ms, accuracy: %0.3lf ms\n", bcast_time,   1000 * MPI_Wtick());
        printf("MPI_Reduce work time:  %0.3lf ms, accuracy: %0.3lf ms\n", reduce_time,  1000 * MPI_Wtick());
        printf("MPI_Gather work time:  %0.3lf ms, accuracy: %0.3lf ms\n", gather_time,  1000 * MPI_Wtick());
        printf("MPI_Scatter Work time: %0.3lf ms, accuracy: %0.3lf ms\n", scatter_time, 1000 * MPI_Wtick());
    }

    MPI_Finalize();

    return 0;
}
