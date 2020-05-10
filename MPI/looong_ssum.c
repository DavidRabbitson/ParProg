#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mpi.h>

int number_of_blocks(int len)
{
    int cond = 0;
    if(len % 9 != 0)
        cond = 1;

    int blocks = (int)(len / 9) + cond;

    return blocks;
}

int* long_to_blocks(int len, char* str)
{
    int blocks = number_of_blocks(len);
     
    int* num = (int*)calloc(blocks, sizeof(int));
    char tmp[9];

    int i;
    int offset = 0;
    for(i = 0; i < blocks; i++)
    {
        int char_count = 9;

        if((i == 0) && (len % 9 != 0))
            char_count = len % 9;

        num[i] = atoi( strncpy( tmp, &(str[offset]), char_count));
        offset += char_count;
    }
    return num;
}

int main(int argc, char* argv[])
{
    int rank = 0;
    int size = 0;

    MPI_Init( &argc, &argv);

    MPI_Comm_rank( MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD, &size);

    if(rank == 0)
    {
        int len = 0;
        char buf[10];

        double time = MPI_Wtime();


        FILE* fp = fopen(argv[1], "r+");
        if(fp == NULL)
            return 1;
        
        fscanf(fp, "%s", buf);
        len = atoi(buf);


        char digits1[len];
        char digits2[len];

        fscanf(fp, "%s", digits1);
        fscanf(fp, "%s", digits2);


        int* num1 = long_to_blocks(len, digits1);
        int* num2 = long_to_blocks(len, digits2);

        int rbuf[1];

        MPI_Scatter( num1 - 1, 1, MPI_INT, rbuf, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter( num2 - 1, 1, MPI_INT, rbuf, 1, MPI_INT, 0, MPI_COMM_WORLD);


        int* res = (int*)calloc(size, sizeof(int));
        MPI_Gather( rbuf, 1, MPI_INT, res, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        fseek(fp, 0, SEEK_END);

        fprintf(fp, "%d", res[1]);
        int i;
        for(i = 2; i < size; i++)
            fprintf(fp, "%09d", res[i]);

        time = MPI_Wtime() - time;
        printf("Work time = %.3fms\n", time*1000);

        fclose(fp);

        free(res);
        free(num1);
        free(num2);
    }
    else
    {
        int result = 0;
        int num1 = 0;
        int num2 = 0;

        MPI_Scatter( &result, 1, MPI_INT, &num1, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter( &result, 1, MPI_INT, &num2, 1, MPI_INT, 0, MPI_COMM_WORLD);
 

        int overflow = 0;
    
        result += num1 + num2;

        if(rank != size - 1)
            MPI_Recv( &overflow, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, NULL);

        result += overflow;

        if(rank != 1 && result >= 1000000000)
        {
            result -= 1000000000;
            overflow = 1;
        }
        else
            overflow = 0;
        
        if(rank != 1)
            MPI_Send( &overflow, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);

 
        int kek[size - 1];
        MPI_Gather( &result, 1, MPI_INT, kek, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0; 
}
