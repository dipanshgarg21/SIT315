#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define MAX 8

// OpenCL variables
cl_device_id device_id;
cl_context context;
cl_program program;
cl_kernel kernel;
cl_command_queue queue;
cl_event event = NULL;
int err;

// Matrices
int a[MAX][MAX], b[MAX][MAX], c[MAX][MAX];

// OpenCL buffer objects
cl_mem bufA, bufB, bufC;

// Constants
const int max = MAX;
const int TS = 4;
const size_t local[2] = {TS, TS};
const size_t global[2] = {max, max};

// Function prototypes
void init(int a[MAX][MAX]);
void matrix_mul(int a[MAX][MAX], int b[MAX][MAX], int c[MAX][MAX]);
void print_matrix(int a[MAX][MAX]);

// OpenCL function prototypes
cl_device_id create_device();
cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename);
void setup_openCL_device_context_queue_kernel();
void setup_kernel_memory();
void copy_kernel_args();
void free_memory();

int main()
{
    // Initialize matrices
    init(a);
    init(b);

    // Perform matrix multiplication without OpenCL
    matrix_mul(a, b, c);

    // Print the result matrix before OpenCL execution
    printf("Before OpenCL execution:\n");
    print_matrix(c);
    printf("\n........\n");

    // Set up OpenCL device, context, queue, and kernel
    setup_openCL_device_context_queue_kernel();

    // Set up memory buffers on the device
    setup_kernel_memory();

    // Copy matrix data from host to device
    copy_kernel_args();

    // Execute the OpenCL kernel
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, NULL, &event);
    clWaitForEvents(1, &event);

    // Copy the result matrix from the device to host
    clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0, MAX * MAX * sizeof(int), c, 0, NULL, NULL);

    // Print the result matrix after OpenCL execution
    printf("After OpenCL execution:\n");
    print_matrix(c);

    // Free memory and resources
    free_memory();

    return 0;
}

// Free OpenCL memory and resources
void free_memory()
{
    clReleaseKernel(kernel);
    clReleaseMemObject(bufA);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufC);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
}

// Set OpenCL kernel arguments
void copy_kernel_args()
{
    clSetKernelArg(kernel, 0, sizeof(int), (void *)&max);
    clSetKernelArg(kernel, 1, sizeof(int), (void *)&max);
    clSetKernelArg(kernel, 2, sizeof(int), (void *)&max);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&bufA);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&bufB);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&bufC);

    if (err < 0)
    {
        perror("Couldn't create a kernel argument");
        printf("error = %d", err);
        exit(1);
    }
}

// Set up memory buffers on
void setup_kernel_memory()
{
    // Create OpenCL buffers for matrices A, B, and C
    bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, MAX * MAX * sizeof(int), NULL, NULL);
    bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, MAX * MAX * sizeof(int), NULL, NULL);
    bufC = clCreateBuffer(context, CL_MEM_READ_WRITE, MAX * MAX * sizeof(int), NULL, NULL);

    // Copy matrices A, B, and C from the host to the device
    clEnqueueWriteBuffer(queue, bufA, CL_TRUE, 0, MAX * MAX * sizeof(int), a, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufB, CL_TRUE, 0, MAX * MAX * sizeof(int), b, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufC, CL_TRUE, 0, MAX * MAX * sizeof(int), c, 0, NULL, NULL);
}

// Set up OpenCL device, context, queue, and kernel
void setup_openCL_device_context_queue_kernel()
{
    // Create an OpenCL device
    device_id = create_device();

    // Create an OpenCL context
    cl_int err;
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    if (err < 0)
    {
        perror("Couldn't create a context");
        exit(1);
    }

    // Build the OpenCL program
    program = build_program(context, device_id, "matrix_mul.cl");

    // Create an OpenCL command queue
    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    if (err < 0)
    {
        perror("Couldn't create a command queue");
        exit(1);
    }

    // Create an OpenCL kernel
    kernel = clCreateKernel(program, "matrix_mult", &err);
    if (err < 0)
    {
        perror("Couldn't create a kernel");
        printf("error = %d", err);
        exit(1);
    }
}

// Build the OpenCL program from the source code file
cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename)
{
    cl_program program;
    FILE *program_handle;
    char *program_buffer;
    char *program_log;
    size_t program_size, log_size;

    // Read the OpenCL program file and store its content in a buffer
    program_handle = fopen(filename, "r");
    if (program_handle == NULL)
    {
        perror("Couldn't find the program file");
        exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char *)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);

    // Create an OpenCL program from the source code
    program = clCreateProgramWithSource(ctx, 1, (const char **)&program_buffer, &program_size, &err);
    if (err < 0)
    {
        perror("Couldn't create the program");
        exit(1);
    }
    free(program_buffer);

    // Build the OpenCL program
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err < 0)
    {
        // If there is an error, retrieve the build log and print it
        // Get the size of the build log
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        // Allocate memory for the program build log
        program_log = (char *)malloc(log_size + 1);
        program_log[log_size] = '\0';

        // Get the program build log
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);

        // Print the program build log
        printf("Program build log:\n%s\n", program_log);

        // Free the allocated memory
        free(program_log);

        // Check for any build errors
        if (err < 0)
        {
            perror("Couldn't build the program");
            exit(1);
        }
    }

    return program;
}

// Create an OpenCL device
cl_device_id create_device()
{
    cl_platform_id platform;
    cl_device_id dev;
    cl_int err;

    // Get the first available platform
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err < 0)
    {
        perror("Couldn't identify a platform");
        exit(1);
    }

    // Get the first available device on the platform
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
    if (err == CL_DEVICE_NOT_FOUND)
    {
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
    }
    if (err < 0)
    {
        perror("Couldn't access any devices");
        exit(1);
    }

    return dev;
}

// Initialize the matrix with random values
void init(int a[MAX][MAX])
{
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            a[i][j] = rand() % 10;
        }
    }
}

// Perform matrix multiplication without OpenCL
void matrix_mul(int a[MAX][MAX], int b[MAX][MAX], int c[MAX][MAX])
{
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            c[i][j] = 0;
            for (int k = 0; k < MAX; k++)
            {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// Print the matrix
void print_matrix(int a[MAX][MAX])
{
    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; j < MAX; j++)
        {
            printf("%d\t", a[i][j]);
        }
        printf("\n");
    }
}
