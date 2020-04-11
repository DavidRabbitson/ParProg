#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>



int MY_Bcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
    int rank =  0;
    int size =  0;
    int type_size = 0;

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    MPI_Type_size( datatype, &type_size);

    //MPI_ERR_COMM
    if( !comm)
        return 1;

    //MPI_ERR_COUNT
    if( ( count < 0) || ( count * type_size > sizeof( buffer)))
        return 2;

    //MPI_ERR_BUFFER
    if( !buffer)
        return 4;

    //MPI_ERR_ROOT
    if( root < 0 || root > size)
        return 5;


    if( rank == root)
    {
        int i;
        for(i = 0; i < size; i++)
            if( i != root)
                MPI_Send( buffer, count, datatype, i, 0, comm);
    }
    else
        MPI_Recv( buffer, count, datatype, root, MPI_ANY_TAG, comm, NULL);


    //MPI_SUCCESS
    return 0;
}


int MY_Gather(void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
    int rank =  0;
    int size =  0;

    MPI_Comm_rank( comm, &rank);
    MPI_Comm_size( comm, &size);

    //MPI_ERR_COUNT
    if( !sendcount || !recvcount)
        return 1;

    //MPI_ERR_COMM
    if( !comm)
        return 2;

    //MPI_ERR_ROOT
    if( root < 0 || root > size)
        return 5;

    if( rank != root)
    {
        MPI_Send( sendbuf, sendcount, sendtype, root, 0, comm);
    }
    else
    {
        int data_size;

        MPI_Type_size( recvtype, &data_size);

        int i;
        for(i = 0; i < size; i++)
            if(i < root)
            {
                MPI_Recv( recvbuf + data_size * (recvcount * i), recvcount, recvtype, i, MPI_ANY_TAG, comm, NULL);
            }
            else if(i > root)
            {
                MPI_Recv( recvbuf + data_size * (recvcount * (i - 1)), recvcount, recvtype, i, MPI_ANY_TAG, comm, NULL);
            }
    }

    //MPI_SUCCESS
    return 0;
}


int MY_Scatter(void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
    int rank =  0;
    int size =  0;

    MPI_Comm_rank( comm, &rank);
    MPI_Comm_size( comm, &size);

    //MPI_ERR_COUNT
    if( !sendcount || !recvcount)
        return 1;

    //MPI_ERR_COMM
    if( !comm)
        return 2;

    //MPI_ERR_ROOT
    if( root < 0 || root > size)
        return 5;

    if(rank == root)
    {
        int data_size;

        MPI_Type_size( sendtype, &data_size);

        int i;
        for(i = 0; i < size; i++)
        {
            if(i < root)
            {
                MPI_Send(sendbuf + data_size * (sendcount * i), sendcount, sendtype, i, 0, comm);
            }
            else if(i > root)
            {
                MPI_Send(sendbuf + data_size * (sendcount * (i - 1)), sendcount, sendtype, i, 0, comm);
            }
        }
    }
    else
    {
        MPI_Recv(recvbuf, recvcount, recvtype, root, MPI_ANY_TAG, comm, NULL);
    }

    //MPI_SUCCESS
    return 0;
}


int MY_Reduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
    int rank =  0;
    int size =  0;

    MPI_Comm_rank( comm, &rank);
    MPI_Comm_size( comm, &size);

    //MPI_ERR_COUNT
    if( !count)
        return 1;

    //MPI_ERR_COMM
    if( !comm)
        return 2;

    //MPI_ERR_ROOT
    if( root < 0 || root > size)
        return 5;


    if(rank != root)
    {
        MPI_Send(sendbuf, count, datatype, root, 0, comm);
    }
    else
    {
        int i;
        int j;

        int data_size = 0;
        MPI_Type_size( datatype, &data_size);

        if(datatype == MPI_INT)
        {
            int* values = (int*)calloc(count, data_size);

            for(i = 0; i < size; i++)
            {
                if(i != root)
                {
                    MPI_Recv(recvbuf, count, datatype, i, MPI_ANY_TAG, comm, NULL);
                    if( op == MPI_MAX)
                    {
                        if(i == (int)(!root))
                            for(j = 0; j < count; j++)
                                values[j] = ((int*)recvbuf)[j];
                        else
                            for(j = 0; j < count; j++)
                                if( ((int*)recvbuf)[j] > values[j])
                                    values[j] = ((int*)recvbuf)[j];
                    }
                }
            }

            for(j = 0; j < count; j++)
                ((int*)recvbuf)[j] = values[j];

            free( values);
        }

        if(datatype == MPI_LONG)
        {
            signed long int* values = (signed long int*)calloc(count, data_size);

            for(i = 0; i < size; i++)
            {
                if(i != root)
                {
                    MPI_Recv(recvbuf, count, datatype, i, MPI_ANY_TAG, comm, NULL);
                    if( op == MPI_MAX)
                    {
                        if(i == (int)(!root))
                            for(j = 0; j < count; j++)
                                values[j] = ((signed long int*)recvbuf)[j];
                        else
                            for(j = 0; j < count; j++)
                                if( ((signed long int*)recvbuf)[j] > values[j])
                                    values[j] = ((signed long int*)recvbuf)[j];
                    }
                }
            }

            for(j = 0; j < count; j++)
                ((signed long int*)recvbuf)[j] = values[j];

            free( values);
        }

        if(datatype == MPI_SHORT)
        {
            signed short int* values = (signed short int*)calloc(count, data_size);

            for(i = 0; i < size; i++)
            {
                if(i != root)
                {
                    MPI_Recv(recvbuf, count, datatype, i, MPI_ANY_TAG, comm, NULL);
                    if( op == MPI_MAX)
                    {
                        if(i == (int)(!root))
                            for(j = 0; j < count; j++)
                                values[j] = ((signed short int*)recvbuf)[j];
                        else
                            for(j = 0; j < count; j++)
                                if( ((signed short int*)recvbuf)[j] > values[j])
                                    values[j] = ((signed short int*)recvbuf)[j];
                    }
                }
            }

            for(j = 0; j < count; j++)
                ((signed short int*)recvbuf)[j] = values[j];

            free( values);
        }

        if(datatype == MPI_UNSIGNED_SHORT)
        {
            unsigned short int* values = (unsigned short int*)calloc(count, data_size);

            for(i = 0; i < size; i++)
            {
                if(i != root)
                {
                    MPI_Recv(recvbuf, count, datatype, i, MPI_ANY_TAG, comm, NULL);
                    if( op == MPI_MAX)
                    {
                        if(i == (int)(!root))
                            for(j = 0; j < count; j++)
                                values[j] = ((unsigned short int*)recvbuf)[j];
                        else
                            for(j = 0; j < count; j++)
                                if( ((unsigned short int*)recvbuf)[j] > values[j])
                                    values[j] = ((unsigned short int*)recvbuf)[j];
                    }
                }
            }

            for(j = 0; j < count; j++)
                ((unsigned short int*)recvbuf)[j] = values[j];

            free( values);
        }

        if(datatype == MPI_UNSIGNED)
        {
            unsigned int* values = (unsigned int*)calloc(count, data_size);

            for(i = 0; i < size; i++)
            {
                if(i != root)
                {
                    MPI_Recv(recvbuf, count, datatype, i, MPI_ANY_TAG, comm, NULL);
                    if( op == MPI_MAX)
                    {
                        if(i == (int)(!root))
                            for(j = 0; j < count; j++)
                                values[j] = ((unsigned int*)recvbuf)[j];
                        else
                            for(j = 0; j < count; j++)
                                if( ((unsigned int*)recvbuf)[j] > values[j])
                                    values[j] = ((unsigned int*)recvbuf)[j];
                    }
                }
            }

            for(j = 0; j < count; j++)
                ((unsigned int*)recvbuf)[j] = values[j];

            free( values);
        }

        if(datatype == MPI_UNSIGNED_LONG)
        {
            unsigned long int* values = (unsigned long int*)calloc(count, data_size);

            for(i = 0; i < size; i++)
            {
                if(i != root)
                {
                    MPI_Recv(recvbuf, count, datatype, i, MPI_ANY_TAG, comm, NULL);
                    if( op == MPI_MAX)
                    {
                        if(i == (int)(!root))
                            for(j = 0; j < count; j++)
                                values[j] = ((unsigned long int*)recvbuf)[j];
                        else
                            for(j = 0; j < count; j++)
                                if( ((unsigned long int*)recvbuf)[j] > values[j])
                                    values[j] = ((unsigned long int*)recvbuf)[j];
                    }
                }
            }

            for(j = 0; j < count; j++)
                ((unsigned long int*)recvbuf)[j] = values[j];

            free( values);
        }

        if(datatype == MPI_FLOAT)
        {
            float* values = (float*)calloc(count, data_size);

            for(i = 0; i < size; i++)
            {
                if(i != root)
                {
                    MPI_Recv(recvbuf, count, datatype, i, MPI_ANY_TAG, comm, NULL);
                    if( op == MPI_MAX)
                    {
                        if(i == (int)(!root))
                            for(j = 0; j < count; j++)
                                values[j] = ((float*)recvbuf)[j];
                        else
                            for(j = 0; j < count; j++)
                                if( ((float*)recvbuf)[j] > values[j])
                                    values[j] = ((float*)recvbuf)[j];
                    }
                }
            }
            for(j = 0; j < count; j++)
                ((float*)recvbuf)[j] = values[j];

            free( values);
        }

        if(datatype == MPI_DOUBLE)
        {
            double* values = (double*)calloc(count, data_size);

            for(i = 0; i < size; i++)
            {
                if(i != root)
                {
                    MPI_Recv(recvbuf, count, datatype, i, MPI_ANY_TAG, comm, NULL);
                    if( op == MPI_MAX)
                    {
                        if(i == (int)(!root))
                            for(j = 0; j < count; j++)
                                values[j] = ((double*)recvbuf)[j];
                        else
                            for(j = 0; j < count; j++)
                                if( ((double*)recvbuf)[j] > values[j])
                                    values[j] = ((double*)recvbuf)[j];
                    }
                }
            }
            for(j = 0; j < count; j++)
                ((double*)recvbuf)[j] = values[j];

            free( values);
        }

        if(datatype == MPI_LONG_DOUBLE)
        {
            long double* values = (long double*)calloc(count, data_size);

            for(i = 0; i < size; i++)
            {
                if(i != root)
                {
                    MPI_Recv(recvbuf, count, datatype, i, MPI_ANY_TAG, comm, NULL);
                    if( op == MPI_MAX)
                    {
                        if(i == (int)(!root))
                            for(j = 0; j < count; j++)
                                values[j] = ((long double*)recvbuf)[j];
                        else
                            for(j = 0; j < count; j++)
                                if( ((long double*)recvbuf)[j] > values[j])
                                    values[j] = ((long double*)recvbuf)[j];
                    }
                }
            }
            for(j = 0; j < count; j++)
                ((long double*)recvbuf)[j] = values[j];

            free( values);
        }
 
    }

    MPI_Barrier(comm);

    //MPI_SUCCESS
    return 0;
}

int main(int argc, char* argv[])
{
    int rank =  0;
    int size =  0;
    int buf[5]  = {0, 1, 2, 3, 4};
    int recv_buf[5];

    int send_gather[5] = {4, 3, 2, 1, 0};
    int recv_gather[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    int send_scatter[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int recv_scatter[5];

    int send_reduce[10] = {4, 3, 2, 1, 0, 5, 6, 7, 8, 9};
    int recv_reduce[5];

	MPI_Init( &argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    //Change this variable to gain more/less statistics
    int measurements  = 2048;

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

    int i = 0;

    //Measure time of MY_Bcast()
    for(i = 0; i < measurements; i++)
    {
        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            bcast_start = MPI_Wtime();
        }

        MY_Bcast( buf, 2, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            bcast_finish = MPI_Wtime();
            bcast_time += bcast_finish - bcast_start;
        }
    }


    //Measure time of MY_Reduce()
    for(i = 0; i < measurements; i++)
    {
        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            reduce_start = MPI_Wtime();
        }

        MY_Reduce( send_reduce + rank, recv_reduce, 5, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            reduce_finish = MPI_Wtime();
            reduce_time += reduce_finish - reduce_start;
        }
    }


    //Measure time of MY_Gather()
    for(i = 0; i < measurements; i++)
    {
        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            gather_start = MPI_Wtime();
        }

        MY_Gather( send_gather, 2, MPI_INT, recv_gather, 2, MPI_INT, 1, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            gather_finish = MPI_Wtime();
            gather_time += gather_finish - gather_start;
        }
    }

    //Measure time of MY_Scatter()
    for(i = 0; i < measurements; i++)
    {
        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            scatter_start = MPI_Wtime();
        }

        MY_Scatter( send_scatter, 3, MPI_INT, recv_scatter, 3, MPI_INT, 1, MPI_COMM_WORLD);

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

        printf("MY_Bcast work time:   %0.3lf ms, accuracy: %0.3lf ms\n", bcast_time,   1000 * MPI_Wtick());
        printf("MY_Reduce work time:  %0.3lf ms, accuracy: %0.3lf ms\n", reduce_time,  1000 * MPI_Wtick());
        printf("MY_Gather work time:  %0.3lf ms, accuracy: %0.3lf ms\n", gather_time,  1000 * MPI_Wtick());
        printf("MY_Scatter Work time: %0.3lf ms, accuracy: %0.3lf ms\n", scatter_time, 1000 * MPI_Wtick());
    }

    MPI_Finalize();

    return 0;
}
