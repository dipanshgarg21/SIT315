#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

#define MAX 1000000

int data_sum[MAX];

int main(int argc, char* argv[])
{
    int size, rank, start, end;
    long local_sum = 0, global_sum = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL));
    clock_t initial = clock();

    if (rank == 0) {

        for (int i = 0; i < MAX; i++) {
            data_sum[i] = rand() % 20;
        }

        // Distribute data using MPI_Scatter
        MPI_Scatter(data_sum, MAX/size, MPI_INT, MPI_IN_PLACE, MAX/size, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        // Receive data using MPI_Scatter
        MPI_Scatter(data_sum, MAX/size, MPI_INT, data_sum, MAX/size, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Calculate local sum
    start = rank * (MAX/size);
    end = start + (MAX/size);
    for (int i = start; i < end; i++) {
        local_sum += data_sum[i];
    }

    // Gather local sums using MPI_Gather
    MPI_Gather(&local_sum, 1, MPI_LONG, &global_sum, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        clock_t complete = clock();
        double total_time = ((complete - initial) / (double)CLOCKS_PER_SEC) * 1000;

        std::cout << "The final sum = " << global_sum << std::endl;
        std::cout << "Total time taken = " << total_time << " ms" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
