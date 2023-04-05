#include <iostream>
#include <mpi.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

#define VEC_SIZE 1000000

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // allocate memory for sub-vectors
    int sub_size = VEC_SIZE / size;
    vector<int> vec(sub_size);
    vector<int> sub_vec(sub_size);

    // generate random values for vectors
    srand(time(NULL));
    clock_t start = clock();
    for (int i = 0; i < sub_size; i++) {
        vec[i] = rand() % 20;
        sub_vec[i] = rand() % 20;
    }

    // distribute sub-vectors to nodes
    MPI_Scatter(vec.data(), sub_size, MPI_INT, sub_vec.data(), sub_size, MPI_INT, 0, MPI_COMM_WORLD);

    // perform addition on sub-vector
    int sub_sum = 0;
    for (int i = 0; i < sub_size; i++) {
        sub_sum += sub_vec[i];
    }

    // send partial sums to central node
    int global_sum;
    MPI_Reduce(&sub_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    clock_t end = clock();
    float total_time = (end - start)/double(CLOCKS_PER_SEC) * 1000;
    if (rank == 0) {
        cout << "Final sum = " << global_sum << endl;
        cout << "Total execution time = " << total_time << " ms" << endl;
    }

    MPI_Finalize();
    return 0;
}
