#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <float.h>
#include <string.h>

int butterfly(int value, int my_rank, int world_rank){
    int level = 1;
    int rcvd_num;
    int partner;

    do{
	// The partner of the node should differ only at one of the bit locations, 
	// so we do a xor operation to calculate the partner node, and later shift 
	// the level right by 1 to get the next partner node for the next level. "
        partner = my_rank ^ level; 

        MPI_Send(&value, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
        MPI_Recv(&rcvd_num, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        value = value + rcvd_num;
        level = level << 1;

    } while(world_rank > level);
    return value;
}

int main (int argc, char *argv[])
{
    int world_rank, my_rank;
    int num, butterfly_sum, all_reduce_sum;

    MPI_Init(&argc,&argv);

    MPI_Comm_size(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Seeding the random function according to the time and rank 
    srand(time(NULL)+my_rank);

    // Get a random value between 0 - 99
    num = rand() % 100;

    printf("\nRank = %d :: Random value = %d", my_rank, num);

    MPI_Barrier(MPI_COMM_WORLD);
    
    // Calculate the butterfly sum using the method defined above    
    butterfly_sum = butterfly(num, my_rank, world_rank);
    
    // Using the MPI_Allreduce collective routine to calculate sum
    MPI_Allreduce(&num, &all_reduce_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    
    // Compare the output of the two different sums
    if (my_rank==0) 
        printf("\nButterfly sum = %d  ::  Reduce sum = %d \n", butterfly_sum, all_reduce_sum);

    MPI_Finalize();
    return 0;
}

