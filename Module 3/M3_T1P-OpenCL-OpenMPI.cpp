#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <cl.h>
#include <fstream>

void matrixMultiplicationOpenCL(float *A, float *B, float *C, int m, int n, int k) {
    // Initialize the OpenCL platform, device, and context
    cl_platform_id platform;
    cl_device_id device;
    cl_int err;
    err = clGetPlatformIDs(1, &platform, NULL);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);

    // Set up the OpenCL kernel and parameters
    const char *kernelSource = R"(
        __kernel void matrixMultiplicationKernel(__global float *C, __global float *A, __global float *B, int m, int n, int k) {
            int i = get_global_id(0);
            if (i < m * n) {
                C[i] = 0;
                for (int j = 0; j < k; ++j) {
                    C[i] += A[i * k + j] * B[j * n + i];
                }
            }
        }
    )";
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernelSource, NULL, &err);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    err = clCreateKernel(program, "matrixMultiplicationKernel", NULL, &kernel);

    // Set up the data to be sent to the GPU
    float *A_device, *B_device, *C_device;
    err = clEnqueueWriteBuffer(queue, (cl_mem)A, CL_TRUE, 0, sizeof(float) * m * k, A, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, (cl_mem)B, CL_TRUE, 0, sizeof(float) * k * n, B, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, (cl_mem)C, CL_TRUE, 0, sizeof(float) * m * n, C, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, (cl_mem)A_device, CL_TRUE, 0, sizeof(float) * m * k, A, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, (cl_mem)B_device, CL_TRUE, 0, sizeof(float) * k * n, B, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, (cl_mem)C_device, CL_TRUE, 0, sizeof(float) * m * n, C, 0, NULL, NULL);

    // Set up the arguments for the kernel
    size_t global_size[3] = {m * n, k, 1};
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&C_device);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&A_device);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&B_device);
    err = clSetKernelArg(kernel, 3, sizeof(int), (void *)&m);
    err = clSetKernelArg(kernel, 4, sizeof(int), (void *)&n);
    err = clSetKernelArg(kernel, 5, sizeof(int), (void *)&k);

    // Execute the kernel
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, NULL, 0, NULL, NULL);

    // Retrieve the results from the GPU
    err = clEnqueueReadBuffer(queue, (cl_mem)C_device, CL_TRUE, 0, sizeof(float) * m * n, C, 0, NULL, NULL);

    // Release the OpenCL resources
    clReleaseMemObject(A_device);
    clReleaseMemObject(B_device);
    clReleaseMemObject(C_device);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}
void matrixMultiplicationMPI(float *A, float *B, float *C, int m, int n, int k, int rank, int size) {
    int chunk_size = m / size;
    float *A_chunk = new float[chunk_size * k];
    float *C_chunk = new float[chunk_size * n];
    MPI_Scatter(A, chunk_size * k, MPI_FLOAT, A_chunk, chunk_size * k, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, k * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    for (int i = 0; i < chunk_size; ++i) {
        for (int j = 0; j < n; ++j) {
            C_chunk[i * n + j] = 0;
            for (int l = 0; l < k; ++l) {
                C_chunk[i * n + j] += A_chunk[i * k + l] * B[l * n + j];
            }
        }
    }
    MPI_Gather(C_chunk, chunk_size * n, MPI_FLOAT, C, chunk_size * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    delete[] A_chunk;
    delete[] C_chunk;
}
void matrixMultiplicationHybrid(float *A, float *B, float *C, int m, int n, int k, int rank, int size) {
    if (rank == 0) {
        float *A_device, *B_device, *C_device;
        cl_int err;
        cl_platform_id platform;
        cl_device_id device;
        err = clGetPlatformIDs(1, &platform, NULL);
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
        cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
        cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
        const char *kernelSource = R"(
            __kernel void matrixMultiplicationKernel(__global float *C, __global float *A, __global float *B, int m, int n, int k) {
                int i = get_global_id(0);
                if (i < m * n) {
                    C[i] = 0;
                    for (int j = 0; j < k; ++j) {
                        C[i] += A[i * k + j] * B[j * n + i];
                    }
                }
            }
        )";
        cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernelSource, NULL, &err);
        clBuildProgram(program, 1, &device, NULL, NULL, NULL);
        cl_kernel kernel = clCreateKernel(program, "matrixMultiplicationKernel", NULL, &err);
        A_device = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * m * k, NULL, &err);
        B_device = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * k * n, NULL, &err);
        C_device = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * m * n, NULL, &err);
        err = clEnqueueWriteBuffer(queue, A_device, CL_TRUE, 0, sizeof(float) * m * k, A, 0, NULL, NULL);
        err = clEnqueueWriteBuffer(queue, B_device, CL_TRUE, 0, sizeof(float) * k * n, B, 0, NULL, NULL);
err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &C_device);
err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &A_device);
err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &B_device);
err = clSetKernelArg(kernel, 3, sizeof(int), &k);
if (err != CL_SUCCESS) {
    printf("Error: Failed to set kernel arguments! %d\n", err);
    exit(1);
}
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, NULL, 0, NULL, NULL);

    // Retrieve the results from the GPU
    err = clEnqueueReadBuffer(queue, (cl_mem)C_device, CL_TRUE, 0, sizeof(float) * m * n, C, 0, NULL, NULL);

    // Release the OpenCL resources
    clReleaseMemObject(A_device);
    clReleaseMemObject(B_device);
    clReleaseMemObject(C_device);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    // Perform matrix multiplication using MPI
    matrixMultiplicationMPI(A, B, C, m, n, k, rank, size);
}
int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Matrix dimensions
    const int m = 1024;
    const int n = 1024;
    const int k = 1024;

    // Allocate matrices
    float *A = new float[m * k];
    float *B = new float[k * n];
    float *C = new float[m * n];

    // Initialize matrices
    if (rank == 0) {
        for (int i = 0; i < m * k; ++i) {
            A[i] = (float)rand() / RAND_MAX;
        }
        for (int i = 0; i < k * n; ++i) {
            B[i] = (float)rand() / RAND_MAX;
        }
    }

    // Perform matrix multiplication
    matrixMultiplicationHybrid(A, B, C, m, n, k, rank, size);

    // Release memory
    delete[] A;
    delete[] B;
    delete[] C;

    MPI_Finalize();
    return 0;
}
