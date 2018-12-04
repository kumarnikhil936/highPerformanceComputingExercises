
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

/* Global variables */
long long int number_of_tosses_so_far1;
long long int number_of_tosses_so_far2;

long long int global_num_in_circle1;
long long int global_num_in_circle2;

long long int number_of_tosses_so_far3;
long long int global_num_in_circle3;

/* declare the mutex */
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

// generate toss without threads
void toss_generator_no_threads(long long int n_toss){
    long long int inside_circle = 0;
    double rand_x, rand_y;
    double sq_dist;
    long long int t;
    long long int local_num_tosses;

    local_num_tosses = (long long int) n_toss;

    // seeding the random based on time and n_toss
    srand(local_num_tosses * time(NULL));

    inside_circle = 0;

    for (t = 0; t < local_num_tosses; t++) {

	// keep the values between -1 and 1
        rand_x = (((double)rand()/RAND_MAX) * 2 ) - 1;

        rand_y = (((double)rand()/RAND_MAX) * 2 ) - 1;

    	sq_dist = (rand_x * rand_x) + (rand_y * rand_y);
        
        if(sq_dist <= 1)
            inside_circle++;
    }

    number_of_tosses_so_far3 = t;
    global_num_in_circle3 = inside_circle;
}

/* generate toss */
void *toss_generator_update_each_step(void* n_toss){
    /* long long int local_inside_circle = 0;*/
    double rand_x, rand_y;
    double sq_dist;
    long long int t;
    long long int local_num_tosses;

    local_num_tosses = (long long int) n_toss;
    
    /* seeding the random based on time and n_toss */
    uint32_t seed_value = local_num_tosses * time(NULL);

    for (t = 0; t < local_num_tosses; t++) {

	    /* keep the values between -1 and 1 */
        rand_x = (((double)rand_r(&seed_value)/RAND_MAX) * 2 ) - 1;
        rand_y = (((double)rand_r(&seed_value)/RAND_MAX) * 2 ) - 1;
    	sq_dist = (rand_x * rand_x) + (rand_y * rand_y);
        
        if(sq_dist <= 1) {
            pthread_mutex_lock(&m1);
            global_num_in_circle1++;
            pthread_mutex_unlock(&m1);
        }
    
        pthread_mutex_lock(&m2);
        number_of_tosses_so_far1++;
        pthread_mutex_unlock(&m2);
    } 
    /*return local_inside_circle;*/
    return NULL;
}

/* generate toss */
void *toss_generator_update_at_end(void* n_toss){
    long long int local_inside_circle = 0;
    double rand_x, rand_y;
    double sq_dist;
    long long int t;
    long long int local_num_tosses;

    local_num_tosses = (long long int) n_toss;
    
    /* seeding the random based on time and n_toss */
    uint32_t seed_value = local_num_tosses * time(NULL);

    for (t = 0; t < local_num_tosses; t++) {

	/* keep the values between -1 and 1 */
        rand_x = (((double)rand_r(&seed_value)/RAND_MAX) * 2 ) - 1;
        rand_y = (((double)rand_r(&seed_value)/RAND_MAX) * 2 ) - 1;
    	sq_dist = (rand_x * rand_x) + (rand_y * rand_y);
        
        if(sq_dist <= 1)
            local_inside_circle++;
    }
    
    /* update the global variables */
    pthread_mutex_lock(&m1);
    global_num_in_circle2 += local_inside_circle;
    number_of_tosses_so_far2 += local_num_tosses;
    pthread_mutex_unlock(&m1);
    
    /*return local_inside_circle;*/
    return NULL;
}

struct thread_st {
	int id;
	long long int n_toss;
};

int main (int argc, char *argv[])
{
    long long int n_toss;
    double pi_val1, pi_val2, pi_val3;

    /*clock_t begin_time_1, finish_time_1;
    clock_t begin_time_2, finish_time_2;*/
    struct timeval t1, t2, t3, t4, t5, t6;

    long long int *num_toss_in_thread;
    struct thread_st *myThread;
    pthread_t *thread_handles;

    number_of_tosses_so_far1 = 0;
    global_num_in_circle1 = 0;

    number_of_tosses_so_far2 = 0;
    global_num_in_circle2 = 0;

    number_of_tosses_so_far3 = 0;
    global_num_in_circle3 = 0;

    long double time_1, time_2, time_3; 
    /*double efficiency_1, efficiency_2;*/
    
    long int n_threads;
    
    /* Obtain the values form command line arguments and convert them from string to long integer */
    n_threads = strtol(argv[1], NULL, 10);
    n_toss = strtol(argv[2], NULL, 10);

    num_toss_in_thread = malloc(n_threads * sizeof(n_toss));
  
    /* Divide tosses among threads */
    long long int toss_per_thread;
    long long int leftover_threads;
    
    toss_per_thread = n_toss/n_threads;
    leftover_threads = n_toss%n_threads;    

    int i;
    for ( i = 0; i < (n_threads - leftover_threads); i++ ){
	num_toss_in_thread[i] = toss_per_thread; 
    }

    for ( i = (n_threads - leftover_threads); i < n_threads; i++ ){
	num_toss_in_thread[i] = toss_per_thread + 1; 
    }
 
    /* Allocate handles */
    thread_handles = malloc(n_threads * sizeof(pthread_t));
    myThread = malloc(n_threads * sizeof(struct thread_st));
    
    /* Task1 */

    /* Start the time */
    /*begin_time_1 = clock();*/
    gettimeofday(&t1, NULL);

    long int t;
    
    /* Create threads */
    for (t = 0; t < n_threads; t++) {
        myThread[t].id = t;
        myThread[t].n_toss = num_toss_in_thread[t];
        pthread_create(&thread_handles[t], NULL, toss_generator_update_each_step, (void*) (myThread[t].n_toss));
	}

    /* Join threads */
    for (t = 0; t < n_threads; t++) {
        pthread_join(thread_handles[t], NULL);
    }

    pi_val1 = (double)(4 * global_num_in_circle1)/number_of_tosses_so_far1;

    /*finish_time_1 = clock();*/
    gettimeofday(&t2, NULL);

    time_1 = ((t2.tv_sec*1000000 - t1.tv_sec*1000000) + t2.tv_usec + t1.tv_usec);
    /* time_with_mutex = (double)(finish_time_1 - begin_time_1)/CLOCKS_PER_SEC; */
    
    /* Stats */
    printf("\nTask1: Num threads: %ld :: Num tosses: %lld ::  Num tosses in circle: %lld ::  Time elapsed: %Lf :: Pi: %lf \n", n_threads, number_of_tosses_so_far1, global_num_in_circle1, time_1/1000, pi_val1);

    
    /* Task2 */
    
    /* Start the time */
    /*begin_time_2 = clock();*/
    gettimeofday(&t3, NULL);
   
    /* Create threads */
    for (t = 0; t < n_threads; t++) {
        /*myThread[t].id = t;
        myThread[t].n_toss = num_toss_in_thread[t];*/
        pthread_create(&thread_handles[t], NULL, toss_generator_update_at_end, (void*) (myThread[t].n_toss));
	}

    /* Join threads */
    for (t = 0; t < n_threads; t++) {
        pthread_join(thread_handles[t], NULL);
    }

    pi_val2 = (double)(4 * global_num_in_circle2)/number_of_tosses_so_far2;

    /*finish_time_2 = clock();*/
    gettimeofday(&t4, NULL);

    time_2 = ((t4.tv_sec*1000000 - t3.tv_sec*1000000) + t4.tv_usec + t3.tv_usec);
    /* time_with_mutex = (double)(finish_time - begin_time)/CLOCKS_PER_SEC; */
    
    /* Stats */
    printf("\nTask2: Num threads: %ld :: Num tosses: %lld ::  Num tosses in circle: %lld ::  Time elapsed: %Lf :: Pi: %lf \n", n_threads, number_of_tosses_so_far2, global_num_in_circle2, time_2/1000, pi_val2);

 
    /* Without using any threads */
    
    /* Start the time */
    /*begin_time_2 = clock();*/
    gettimeofday(&t5, NULL);
   
    toss_generator_no_threads(n_toss);    

    pi_val3 = (double)(4 * global_num_in_circle3)/number_of_tosses_so_far3;

    /*finish_time_2 = clock();*/
    gettimeofday(&t6, NULL);

    time_3 = ((t6.tv_sec*1000000 - t5.tv_sec*1000000) + t6.tv_usec + t5.tv_usec);
    
    /* Stats */
    printf("\nNo threads: Num tosses: %lld ::  Num tosses in circle: %lld ::  Time elapsed: %Lf :: Pi: %lf \n", number_of_tosses_so_far3, global_num_in_circle3, time_3/1000, pi_val3);

    double efficiency_1, efficiency_2;
    efficiency_1 = time_3 / ( n_threads * time_1); 
    efficiency_2 = time_3 / ( n_threads * time_2);
    
    // Write stats to a file
    FILE *file = fopen("plot.dat","a");
    if(file == NULL){
        printf("Error while opening the file for writing the data!\n");
        exit(1);
    }
    fprintf(file,"%ld\t%Lf\t%Lf\t%f\t%f\n",n_threads, time_1, time_2, efficiency_1, efficiency_2); 
    fclose(file);

    free(thread_handles);
    free(myThread);
    free(num_toss_in_thread);
    
    return 0;
}
