#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int toss_generator(int rank, long long int n_toss){
    long long int inside_circle;
    double rand_x, rand_y;
    double sq_dist;
    long long int t;

    // seeding the random based on time and rank and n_toss
    srand(rank + n_toss * time(NULL));

    inside_circle = 0;

    for (t = 0; t < n_toss; t++) {

	// keep the values between -1 and 1
        rand_x = (((double)rand()/RAND_MAX) * 2 ) - 1;

        rand_y = (((double)rand()/RAND_MAX) * 2 ) - 1;

    	sq_dist = (rand_x * rand_x) + (rand_y * rand_y);
        
        if(sq_dist <= 1)
            inside_circle++;
        
	// printf("Rank :: %d | x :: %lf | y :: %lf | dist :: %lf \n ", rank, rand_x, rand_y, sq_dist);
    }

    printf("Rank :: %d | Total tosses :: %lld | Inside circle :: %lld \n", rank, n_toss, inside_circle);

    return inside_circle;
}


int main (int argc, char *argv[])
{
    int my_rank, world_rank;
    long long int n_toss, inside_circle;
    long long int sum_inside_circle;
    double pi_val;

    MPI_Init(&argc,&argv);

    MPI_Comm_size(MPI_COMM_WORLD, &world_rank);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if(my_rank == 0 ) {
        printf("Enter number of tosses: \n");

        scanf("%lld", &n_toss);
    }
        
    MPI_Bcast(&n_toss, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    inside_circle = toss_generator(my_rank, n_toss);

    MPI_Reduce(&inside_circle, &sum_inside_circle, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(my_rank == 0) {
        printf("\nTotal number of processes: %d \n",world_rank);

	printf("\nTotal number of tosses inside circle :: %lld \n", sum_inside_circle);

	pi_val = (double)(4*sum_inside_circle)/(n_toss*world_rank);

        printf("\nEstimated value of Pi is %lf \n", pi_val);
    } 

    MPI_Finalize();
    return 0;
}

