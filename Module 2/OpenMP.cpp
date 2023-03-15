
 

#include <iostream>
#include <fstream>
#include <omp.h>

int N = rand();

int main()
{
    int** A = new int* [N];
    int** B = new int* [N];

    clock_t start_time = clock();
    for (int i = 0; i < N; i++)
    {
        A[i] = new int[N];
        B[i] = new int[N];
    }

    #pragma omp parallel for
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            A[i][j] = rand();
            B[i][j] = rand();
        }

    int** C = new int* [N];
    for (int i = 0; i < N; i++)
        C[i] = new int[N];

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    clock_t end_time = clock();
    double execution_time = (static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC) * 1000000000;
    std::cout << "Execution time: " << execution_time << " nanoseconds" << std::endl;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << C[i][j] << "\t";
        }
        std::cout << "\n" << std::endl;
    }

    std::ofstream outfile("output.txt");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            outfile << C[i][j] << "\t";
        }
        outfile << "\n" << std::endl;
    }
    outfile.close();

    return 0;
}
