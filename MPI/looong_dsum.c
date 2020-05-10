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

        int blocks = number_of_blocks(len);
        int to_scatter = (int)(blocks / (size - 1));

        int* num1 = long_to_blocks(len, digits1);
        int* num2 = long_to_blocks(len, digits2);


        int rbuf[to_scatter];

        MPI_Bcast( &to_scatter, 1, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Scatter( num1 - to_scatter, to_scatter, MPI_INT, rbuf, to_scatter, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter( num2 - to_scatter, to_scatter, MPI_INT, rbuf, to_scatter, MPI_INT, 0, MPI_COMM_WORLD);


        int* res = (int*)calloc(blocks + to_scatter, sizeof(int));
        MPI_Gather( rbuf, to_scatter, MPI_INT, res, to_scatter, MPI_INT, 0, MPI_COMM_WORLD);

        int i;
        for(i = blocks; i >= to_scatter; i--)
        {
            if((i != to_scatter) && (res[i] >= 1000000000))
            {
                res[i] -= 1000000000;
                res[i - 1]++;
            }
            else
                i -= to_scatter - ((blocks - i) % to_scatter);
        }


        fseek(fp, 0, SEEK_END);

        fprintf(fp, "%d", res[to_scatter]);

        for(i = to_scatter + 1; i < blocks + to_scatter; i++)
            fprintf(fp, "%09d", res[i]);

        time = MPI_Wtime() - time;
        printf("Work time = %.3fms\n", time*1000);

        fclose(fp);


        free(num1);
        free(num2);
        free(res);
    }
    else
    {
        int to_get = 0;
        MPI_Bcast( &to_get, 1, MPI_INT, 0, MPI_COMM_WORLD);


        int* result = (int*)calloc(to_get, sizeof(int));
        int* num1 = (int*)calloc(to_get, sizeof(int));
        int* num2 = (int*)calloc(to_get, sizeof(int));

        MPI_Scatter( result, to_get, MPI_INT, num1, to_get, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter( result, to_get, MPI_INT, num2, to_get, MPI_INT, 0, MPI_COMM_WORLD);

 
        int overflow = 0;
        int i;
        for(i = to_get - 1; i >= 0; i--)
        {
            result[i] += num1[i] + num2[i] + overflow;

            if(result[i] >= 1000000000)
            {
                if(i != 0)
                    result[i] -= 1000000000;
                overflow = 1;
            }
            else
                overflow = 0;
        }


        int kek[size - 1];
        MPI_Gather( result, to_get, MPI_INT, kek, to_get, MPI_INT, 0, MPI_COMM_WORLD);

        free(result);
        free(num1);
        free(num2);
    }

    MPI_Finalize();

    return 0; 
}
