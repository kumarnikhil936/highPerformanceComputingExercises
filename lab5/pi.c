#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

// generate toss without thread
long long int toss_generator(long long int n_toss){
    long long int inside_circle = 0;
    double rand_x, rand_y;
    double sq_dist;
    long long int t;

    // seeding the random based on time and n_toss
    srand(n_toss * time(NULL));

    inside_circle = 0;

    for (t = 0; t < n_toss; t++) {

	// keep the values between -1 and 1
        rand_x = (((double)rand()/RAND_MAX) * 2 ) - 1;

        rand_y = (((double)rand()/RAND_MAX) * 2 ) - 1;

    	sq_dist = (rand_x * rand_x) + (rand_y * rand_y);
        
        if(sq_dist <= 1)
            inside_circle++;
    }

    return inside_circle;
}

struct thread_st {
	int id;
	long long int n_toss;
};


pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
long long int global_num_in_circle = 0;

// generate toss with thread, without mutex
void *toss_generator_no_mutex(void *thread_passed){
    long long int inside_circle = 0;
    double rand_x, rand_y;
    double sq_dist;
    long long int t;
    
    struct thread_st *my_thread = (struct thread_st *) thread_passed;
    long long int n_toss = my_thread->n_toss;
    int t_id = my_thread->id;

    // seeding the random based on time and thread ID and n_toss
    uint32_t seed_value = t_id + n_toss*time(NULL);

    inside_circle = 0;

    for (t = 0; t < n_toss; t++) {

	// keep the values between -1 and 1
        rand_x = (((double)rand_r(&seed_value)/RAND_MAX) * 2 ) - 1;

        rand_y = (((double)rand_r(&seed_value)/RAND_MAX) * 2 ) - 1;

    	sq_dist = (rand_x * rand_x) + (rand_y * rand_y);
        
        if(sq_dist <= 1)
            inside_circle++;
    }

    // pthread_mutex_lock(&m1);
    global_num_in_circle += inside_circle;
    // pthread_mutex_unlock(&m1);

    return NULL;
}

pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
long long int number_of_tosses_so_far = 0;
long long int num_in_circle = 0;

// generate toss with thread, with mutex
void *toss_generator_with_mutex(void *thread_passed){
    double rand_x, rand_y;
    double sq_dist;
    long long int t;
    
    struct thread_st *my_thread = (struct thread_st *) thread_passed;
    long long int n_toss = my_thread->n_toss;
    int t_id = my_thread->id;

    // seeding the random based on time and thread ID and n_toss
    uint32_t seed_value = t_id + n_toss*time(NULL);

    for (t = 0; t < n_toss; t++) {

	// keep the values between -1 and 1
        rand_x = (((double)rand_r(&seed_value)/RAND_MAX) * 2 ) - 1;
        
	rand_y = (((double)rand_r(&seed_value)/RAND_MAX) * 2 ) - 1;

    	sq_dist = (rand_x * rand_x) + (rand_y * rand_y);

        pthread_mutex_lock(&m1);
        number_of_tosses_so_far++;	// Global variable
        pthread_mutex_unlock(&m1);

        if(sq_dist <= 1) {
            pthread_mutex_lock(&m2);
            num_in_circle++;		// Global Variable
            pthread_mutex_unlock(&m2);
        }
    }

    return NULL;
}

int n_threads;

int main (int argc, char *argv[])
{
    long long int n_toss, in_circle;
    double pi_val;

    clock_t begin_time, finish_time;

    long long int *num_toss_in_thread;
    struct thread_st *myThread;
    pthread_t *t_handle;

    double time_no_threads, time_no_mutex, time_with_mutex; 
    double efficiency_with_mutex, efficiency_no_mutex;

    // Obtain the values form command line arguments and convert them from string to long integer
    n_threads = strtol(argv[1], NULL, 10);
    n_toss = strtol(argv[2], NULL, 10);

    num_toss_in_thread = malloc(n_threads * sizeof(n_toss));
  
    // Divide tosses among threads 
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

    int t;

    // Allocate handles
    t_handle = malloc(n_threads * sizeof(pthread_t));
    myThread = malloc(n_threads * sizeof(struct thread_st));

//    // Start the time 
//    begin_time = clock();
    
//    // Create threads
//    for (t = 0; t < n_threads; t++) {
//        myThread[t].id = t;
//        myThread[t].n_toss = num_toss_in_thread[t];
//        if(pthread_create(&t_handle[t], NULL, toss_generator_with_mutex, (void*) &myThread[t])) {
//	    printf("Error while creating a pthread \n");
//	    return(-1);
//	}
//    }
//
//    // Join threads
//    for (t = 0; t < n_threads; t++) {
//        if(pthread_join(t_handle[t], NULL)) {
//	    printf("Error while joining the pthreads \n");
//	    return(-1);
//	}
//    }
//
//    pi_val = (double)(4 * num_in_circle) / (n_toss);
//
//    finish_time = clock();
//    time_with_mutex = (double)(finish_time - begin_time)/CLOCKS_PER_SEC;
//    
//    // Stats
//    printf("\nWith Mutex :  number_of_tosses_so_far - %lld  ::  total number of tosses in circle - %lld  ::  time taken  - %lf  :: Estimated value of pi - %lf \n", number_of_tosses_so_far, num_in_circle, time_with_mutex, pi_val);

//    // Deallocate memory
//    free(t_handle);
//    free(myThread);

    // Allocate handles
    t_handle = malloc(n_threads * sizeof(pthread_t));
    myThread = malloc(n_threads * sizeof(struct thread_st));

    begin_time = clock();
    printf("Running program without mutex\n");    
    // Create threads
    for (t = 0; t < n_threads; t++) {
        myThread[t].id = t;
	myThread[t].n_toss = num_toss_in_thread[t];
        if(pthread_create(&t_handle[t], NULL, toss_generator_no_mutex, (void*) &myThread[t])) {
	    printf("Error while creating a pthread \n");
	    return(-1);
	}
    }

    // Join threads
    for (t = 0; t < n_threads; t++) {
        if(pthread_join(t_handle[t], NULL)) {
	    printf("Error while joining the pthreads \n");
	    return(-1);
	}
    }

    pi_val = (double)(4 * global_num_in_circle) / (n_toss);

    finish_time = clock();
    time_no_mutex = (double)(finish_time - begin_time)/CLOCKS_PER_SEC;
    
    // Stats
    printf("\nWithout Mutex :  number of tosses in circle - %lld  ::  time taken  - %lf  :: Estimated value of pi - %lf \n", global_num_in_circle, time_no_mutex, pi_val);

    // Deallocate memory
    free(t_handle);
    free(myThread);
    free(num_toss_in_thread);

    // printf("Running program without threads\n");    
    // // Without threads
    // begin_time = clock();

    // in_circle = toss_generator(n_toss);
    // pi_val = (double)(4 * in_circle) / (n_toss);

    // finish_time = clock();
    // time_no_threads = (double)(finish_time - begin_time)/CLOCKS_PER_SEC;
    // 
    // // Stats
    // printf("\nWithout threads :  number of tosses in circle - %lld  ::  time taken  - %lf  :: Estimated value of pi - %lf \n", in_circle, time_no_threads, pi_val);

    // efficiency_with_mutex = time_no_threads / ( n_threads * time_with_mutex); 
    // efficiency_no_mutex = time_no_threads / ( n_threads * time_no_mutex);

//    // Write stats to a file
//    FILE *file = fopen("plotData","a");
//    if(file == NULL){
//        printf("Error while opening the file for writing the data!\n");
//        exit(1);
//    }
//    fprintf(file,"%d\t%lf\t%lf\t%f\t%f\n",n_threads, time_with_mutex, time_no_mutex, efficiency_with_mutex, efficiency_no_mutex); 
//    fclose(file);

    return 0;

}

