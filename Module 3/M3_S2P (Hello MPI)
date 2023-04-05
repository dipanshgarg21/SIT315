//========== This code uses MPI_Send and MPI_Recv ==============
#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int world_size, world_rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    char message[20] = "Hello, world!";
    if (world_rank == 0) {
        // Send message to all other processes
        MPI_Send(&message, 20, MPI_CHAR, 0, MPI_COMM_WORLD);
        printf("Process %d: Sent message '%s'\n", world_rank, message);
    } else {
        // Receive message from process 0
        MPI_Recv(&message, 20, MPI_CHAR, 0, MPI_COMM_WORLD);
        printf("Process %d: Received message '%s'\n", world_rank, message);
    }
    
    MPI_Finalize();
    return 0;
}

// ====================== This code uses MPI_Bcast ====================
#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int world_size, world_rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    char message[20] = "Hello, world!";
    if (world_rank == 0) {
        // Send message to all other processes
        MPI_Bcast(&message, 20, MPI_CHAR, 0, MPI_COMM_WORLD);
        printf("Process %d: Sent message '%s'\n", world_rank, message);
    } else {
        // Receive message from process 0
        MPI_Bcast(&message, 20, MPI_CHAR, 0, MPI_COMM_WORLD);
        printf("Process %d: Received message '%s'\n", world_rank, message);
    }
    
    MPI_Finalize();
    return 0;
}
