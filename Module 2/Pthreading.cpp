#include <iostream>
#include <fstream>
#include <pthread.h>

const int N = 100;
const int TOTAL_THREADS = 4;

class ThreadData {
public:
   int thread_id;
   int (*A)[N];
   int (*B)[N];
   int (*C)[N];
};

void *multiply(void *thread_arg)
{
   
   ThreadData *data = static_cast<ThreadData*>(thread_arg);
   
   int thread_id = data->thread_id;
   int (*A)[N] = data->A;
   int (*B)[N] = data->B;
   int (*C)[N] = data->C;
   
   int start_row = thread_id * N / TOTAL_THREADS;
   int end_row = (thread_id + 1) * N / TOTAL_THREADS;
   
   for (int i = start_row; i < end_row; i++) {
       for (int j = 0; j < N; j++) {
           C[i][j] = 0;
           for (int k = 0; k < N; k++) {
               C[i][j] += A[i][k] * B[k][j];
           }
       }
   }
   pthread_exit(NULL);

   return C;
}

int main()
{
   clock_t start_time = clock();
   int A[N][N], B[N][N];
   for (int i = 0; i < N; i++) {
       for (int j = 0; j < N; j++) {
           A[i][j] = rand();
           B[i][j] = rand();
       }
   }
   
   int C[N][N];
   pthread_t threads[TOTAL_THREADS];
   ThreadData thread_data[TOTAL_THREADS];
   for (int i = 0; i < TOTAL_THREADS; i++) {
       thread_data[i].thread_id = i;
       thread_data[i].A = A;
       thread_data[i].B = B;
       thread_data[i].C = C;
       pthread_create(&threads[i], NULL, multiply, static_cast<void*>(&thread_data[i]));
   }

   for (int i = 0; i < TOTAL_THREADS; i++) {
       pthread_join(threads[i], NULL);
   }

   clock_t end_time = clock();
   double execution_time = (static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC) * 1000000000;
   std::cout << "--> Execution time: " << execution_time << " nanoseconds\n" << std::endl;
   
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