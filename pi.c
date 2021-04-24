#include <stdio.h>
#include <math.h>
#include <mpi.h>

int MPI_FlattreeBcast(void *buf, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
    int error, k;
    int numprocs, rank;

    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == root)
    {
        for (k = 0; k < numprocs; k++)
            if (k != root)
            {
                error = MPI_Send(buf, count, datatype, k, 0, comm);
                if (error != MPI_SUCCESS) return error;
            }
    } else
    {
        error = MPI_Recv(buf, count, datatype, root, 0, comm, &status);
        if (error != MPI_SUCCESS) return error;
    }
    return MPI_SUCCESS;
}

int ipow(int base, int exp)
{
    int res = 1;
    for (;;)
    {
        if (exp & 1)
            res *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return res;
}

int MPI_BinomialBcast(void *buf, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
    int error, k;
    int numprocs, rank;
    int receiver, sender;

    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (k = 1; k <= ceil(log2(numprocs)); k++)
        if (rank < ipow(2, k-1))
        {
            receiver = rank + ipow(2, k-1);
            if (receiver < numprocs)
            {
                error = MPI_Send(buf, count, datatype, receiver, 0, comm);
                if (error != MPI_SUCCESS) return error;
            }
        } else
        {
            if (rank < ipow(2, k))
            {
                sender = rank - ipow(2, k-1);
                error = MPI_Recv(buf, count, datatype, sender, 0, comm, &status);
                if (error != MPI_SUCCESS) return error;
            }
        }
    return MPI_SUCCESS;
}

int main(int argc, char *argv[])
{
    int i, done = 0, n;
    double PI25DT = 3.141592653589793238462643;
    double pi, h, sum, x;

    double pirec;
    int numprocs, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while (!done)
    {
        if (rank == 0)
        {
            printf("Enter the number of intervals: (0 quits) \n");
            scanf("%d",&n);
        }
    
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        //MPI_FlattreeBcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        //MPI_BinomialBcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (n == 0) break;
  
        h   = 1.0 / (double) n;
        sum = 0.0;
        for (i = rank + 1; i <= n; i += numprocs) {
            x = h * ((double)i - 0.5);
            sum += 4.0 / (1.0 + x*x);
        }
        pi = h * sum;

        MPI_Reduce(&pi, &pirec, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0)
            printf("pi is approximately %.16f, Error is %.16f\n", pirec, fabs(pirec - PI25DT));
    }
    MPI_Finalize();
}
