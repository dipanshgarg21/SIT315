#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define VECTOR_SIZE 1024

const char *programSource =
    "__kernel void vecAdd(__global const float* a, __global const float* b, __global float* c) {\n"
    "    int gid = get_global_id(0);\n"
    "    c[gid] = a[gid] + b[gid];\n"
    "}\n";

int main()
{
    // Create the OpenCL context
    cl_context context = NULL;
    cl_device_id device = NULL;
    cl_command_queue commandQueue = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    cl_mem aMemObj = NULL;
    cl_mem bMemObj = NULL;
    cl_mem cMemObj = NULL;
    cl_int errNum;

    // Create the context
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &errNum);
    if (errNum != CL_SUCCESS)
    {
        printf("Failed to create OpenCL context\n");
        return 1;
    }

    // Create the command queue
    commandQueue = clCreateCommandQueue(context, device, 0, &errNum);
    if (errNum != CL_SUCCESS)
    {
        printf("Failed to create command queue\n");
        clReleaseContext(context);
        return 1;
    }

    // Create the program
    program = clCreateProgramWithSource(context, 1, (const char **)&programSource, NULL, &errNum);
    if (errNum != CL_SUCCESS)
    {
        printf("Failed to create program\n");
        clReleaseCommandQueue(commandQueue);
        clReleaseContext(context);
        return 1;
    }

    // Build the program
    errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (errNum != CL_SUCCESS)
    {
        printf("Failed to build program\n");

        // Get the build log size
        size_t logSize;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);

        // Allocate memory for the log
        char *log = (char *)malloc(logSize);

        // Get the build log
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log, NULL);

        // Print the build log
        printf("Build log:\n%s\n", log);

        free(log);

        clReleaseProgram(program);
        clReleaseCommandQueue(commandQueue);
        clReleaseContext(context);
        return 1;
    }

    // Create the kernel
    kernel = clCreateKernel(program, "vecAdd", &errNum);
    if (errNum != CL_SUCCESS)
    {
        printf("Failed to create kernel\n");
        clReleaseProgram(program);
        clReleaseCommandQueue(commandQueue);
        clReleaseContext(context);
        return 1;
    }

    // Allocate memory for the vectors
    float *a = (float *)malloc(sizeof(float) * VECTOR_SIZE);
    float *b = (float *)malloc(sizeof(float) * VECTOR_SIZE);
    float *c = (float *)malloc(sizeof(float) * VECTOR_SIZE);

    // Initialize vectors with random values
    srand(time(NULL));
    for (int i = 0; i < VECTOR_SIZE; i++)
    {
        a[i] = (float)rand() / (float)RAND_MAX;
        b[i] = (float)rand() / (float)RAND_MAX;
    }

    // Create memory buffers
    aMemObj = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * VECTOR_SIZE, a, &errNum);
    if (errNum != CL_SUCCESS)
    {
        printf("Failed to create buffer for vector a\n");
        free(a);
        free(b);
        free(c);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(commandQueue);
        clReleaseContext(context);
        return 1;
    }

    bMemObj = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * VECTOR_SIZE, b, &errNum);
    if (errNum != CL_SUCCESS)
    {
        printf("Failed to create buffer for vector b\n");
        free(a);
        free(b);
        free(c);
        clReleaseMemObject(aMemObj);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(commandQueue);
        clReleaseContext(context);
        return 1;
    }

    cMemObj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * VECTOR_SIZE, NULL, &errNum);
    if (errNum != CL_SUCCESS)
    {
        printf("Failed to create buffer for vector c\n");
        free(a);
        free(b);
        free(c);
        clReleaseMemObject(aMemObj);
        clReleaseMemObject(bMemObj);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(commandQueue);
        clReleaseContext(context);
        return 1;
    }

    // Set the kernel arguments
    errNum = clSetKernelArg(kernel, 0, sizeof(cl_mem), &aMemObj);
    errNum |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &bMemObj);
    errNum |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &cMemObj);
    if (errNum != CL_SUCCESS)
    {
        printf("Failed to set kernel arguments\n");
        free(a);
        free(b);
        free(c);
        clReleaseMemObject(aMemObj);
        clReleaseMemObject(bMemObj);
        clReleaseMemObject(cMemObj);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(commandQueue);
        clReleaseContext(context);
        return 1;
    }

    // Execute the kernel
    size_t globalWorkSize[1] = {VECTOR_SIZE};
    size_t localWorkSize[1] = {1};
    errNum = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    if (errNum != CL_SUCCESS)
    {
        printf("Failed to enqueue kernel\n");
        free(a);
        free(b);
        free(c);
        clReleaseMemObject(aMemObj);
        clReleaseMemObject(bMemObj);
        clReleaseMemObject(cMemObj);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(commandQueue);
        clReleaseContext(context);
        return 1;
    }

    // Read the output buffer
    errNum = clEnqueueReadBuffer(commandQueue, cMemObj, CL_TRUE, 0, sizeof(float) * VECTOR_SIZE, c, 0, NULL, NULL);
    if (errNum != CL_SUCCESS)
    {
        printf("Failed to read buffer\n");
        free(a);
        free(b);
        free(c);
        clReleaseMemObject(aMemObj);
        clReleaseMemObject(bMemObj);
        clReleaseMemObject(cMemObj);
        clReleaseKernel(kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(commandQueue);
        clReleaseContext(context);
        return 1;
    }

    // Verify the result
    for (int i = 0; i < VECTOR_SIZE; i++)
    {
        if (c[i] != a[i] + b[i])
        {
            printf("Error: Mismatch at index %d\n", i);
            free(a);
            free(b);
            free(c);
            clReleaseMemObject(aMemObj);
            clReleaseMemObject(bMemObj);
            clReleaseMemObject(cMemObj);
            clReleaseKernel(kernel);
            clReleaseProgram(program);
            clReleaseCommandQueue(commandQueue);
            clReleaseContext(context);
            return 1;
        }
    }

    printf("Vector addition successful\n");

    // Cleanup
    free(a);
    free(b);
    free(c);
    clReleaseMemObject(aMemObj);
    clReleaseMemObject(bMemObj);
    clReleaseMemObject(cMemObj);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(commandQueue);
    clReleaseContext(context);

    return 0;
}