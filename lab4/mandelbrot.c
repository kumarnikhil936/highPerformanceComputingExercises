#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



/*
	A block struct contains one "task" to do.
	It represents a square area of the fractal to be rendered with the upper left point at x,y and lengths of size.
	it also contains information as to where to insert the calculated pixels as a starting shift (target_pos) and a size in pixels (target_size)
*/
typedef struct Block_{
	//Input
	float x;//Upper left point
	float y;//Upper left point
	float size;
	//Output pixels
	int target_pos;
	int target_size;//Assumes a square!
} Block;



int checkMandelbrot(float real, float imag, int cutoff){
/*
	float x_init = 0;
	float y_init = 0;
	float xtemp = 0;
	float ytemp = 0;
	int iter = 0;
	float x = real;
	float y = imag;

	while ((x*x + y*y < 2*2) & (iter < cutoff)) {
		xtemp = x*x - y*y + x_init;
		ytemp = 2*x*y + y_init;
		if (x == xtemp & y == ytemp) {
			iter = cutoff;
			break;
		}
		x = xtemp;
		y = ytemp;
		iter = iter + 1;
	}

	return iter;
	*/ 
	/*
		Task 2
		------
		Implement the code for determining whether the point c (in the complex plane, i.e. c = real + i*imag) is in the Mandelbrot
                set (https://en.wikipedia.org/wiki/Mandelbrot_set).

		The point (c = real + i*imag) is in the Mandelbrot set if the following sequence of complex numbers z_n is bounded
                (i.e. |z_n| <= 2  which means  sqrt(Real(z_n)^2 + Imag(z_n)^2) <=2 )

                 z_0 = 0
                 z_1 = z_0^2 + c
                 z_2 = z_1^2 + c
                
                 ... 

		 i.e. z_{n+1} = (z_n)^2 + c

                Consider that z_n are complex numbers!

                 
		Perform the iteration up to as many times as the cutoff states, at which point you assume the point is in the set.
		If the absolute value (|z_n|) is ever greater than 2, the series will diverge for sure and you can conclude that the point is not in the set.

                This function returns the number of iterations n after that sqrt(Re(z_n)^2 + Im(z_n)^2) > 2.


		Notes:
			- You can do this test without calling the expensive square root function.

		Solution:
			- Looks like the set in Wikipedia.
	*/


	if(real > imag) return cutoff; //<-- This needs to be modified.
	return 0;                      //<-- This needs to be modified.

}



void HandleBlock(int my_rank, Block block, MPI_Win* window, int total_size_x, int**my_local_results, int max_number_iterations){
	int v, b;
	for(v=0;v<block.target_size;++v){
			for(b=0;b<block.target_size;++b){

				int result = checkMandelbrot(
					block.x + block.size * b / block.target_size
					,block.y + block.size * v / block.target_size
					, max_number_iterations
				);


				(*my_local_results)[b+v*block.target_size] = result; 
			}
	}

	for(v=0;v<block.target_size;++v){

		 /*
			Task 1
			------
			Transfer the pixels calculated above to the memory exposed by Process 0 using MPI_Put

			Notes:
				- Due to the different layouts, you need to do this line by line
				- Target location for each line is the base position(block.target_pos) + an offset(v*total_size_x) that depends on the dimensions of the target matrix
				- Lock and unlock. This causes the write to happen before we overwrite our local results with the next block. (Passive Target Synchronization).
			Solution:
				- A triangular shape appears as the output
		*/
	MPI_Win_lock( MPI_LOCK_EXCLUSIVE, 0, 0, *window);

        MPI_Put( (*my_local_results) + v*block.target_size, block.target_size, MPI_INT, 0, (v * total_size_x) + block.target_pos, block.target_size, MPI_INT, *window);

        MPI_Win_unlock(0, *window);
		
	}
}




int main(int argc, char *argv[]){
	/*
		We render a square area with side lengths size (3rd command line parameter) with its upper left point given by the first two parameters.
		A fourth parameter sets the maximum number of iterations, to possibly bring out finer details.
	*/
	int output_size_pixels = 2000;
	float pos_x = -1.80f;
	float pos_y = -1.0f;
	float size = 2.0f;
	int max_number_iterations = 100;

	//MPI seems to do fine with providing these to all participating processes
	if(argc >= 4){
		pos_x = atof(argv[1]);
		pos_y = atof(argv[2]);
		size = atof(argv[3]);
	}
	if(argc >= 5){
		max_number_iterations = atoi(argv[4]);
	}


 	static unsigned char white[3];
	white[0]=255; white[1]=255; white[2]=255;
 	static unsigned char black[3];
	black[0]=0; black[1]=0; black[2]=0;
 	static unsigned char greyscale[3];
	greyscale[0]=0; greyscale[1]=0; greyscale[2]=0;



	MPI_Init(&argc,&argv);
	int my_rank,total_ranks;
	MPI_Comm_size(MPI_COMM_WORLD,&total_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);


	MPI_Win window;
	int * shared_data;

	/*
			Task 3
			------
			Create and distribute multiple Blocks in such a way, that more than one process can work at a time and the likelihood of load imbalances is low

			Notes:
				- Each of the n processes should work on n blocks, which ideally are not oriented as a row or column to reduce the likelihood of load imbalances

			Solution:
				- Should yield a speedup over the previous version, scaling with the number of processes used.
				- Try higher numbers of iterations.
	*/


	if(my_rank == 0){
		//Allocate memory and make available to others

		MPI_Aint siz = output_size_pixels * output_size_pixels * sizeof(int);


		MPI_Alloc_mem(siz, MPI_INFO_NULL, &shared_data);
		MPI_Win_create(shared_data, siz, sizeof(int),MPI_INFO_NULL, MPI_COMM_WORLD, &window);


	} else {
		//All other ranks make no memory available for remote access
		MPI_Win_create(shared_data, 0,sizeof(int),MPI_INFO_NULL, MPI_COMM_WORLD, &window);
	}

	//See below
	int * my_result_vals;

	int blocks_per_direction = 1;
	int total_blocks = blocks_per_direction * blocks_per_direction;
	int block_pixel_size = output_size_pixels / blocks_per_direction;
	//Allocate a block of memory to keep our local results until sending
	my_result_vals = malloc(sizeof(int)*(block_pixel_size  * block_pixel_size));



	if(my_rank == 1){
		int block_coord_x = 0; //In full blocks
		int block_coord_y = 0;
		//Make a block and fill it with the neccessary information
		Block block;
		block.x = pos_x + size / blocks_per_direction * block_coord_x;//upper left corner of the block we want to manage
		block.y = pos_y + size / blocks_per_direction * block_coord_y;
		block.size = size / blocks_per_direction;

		//in pixels
		block.target_size = block_pixel_size;
		//Start of the first row in our target matrix.
		//Second row will start with an offset of +output_size_pixels, etc.
		block.target_pos = block.target_size * (block_coord_x + block_coord_y * output_size_pixels);


		HandleBlock(my_rank,block,&window,output_size_pixels, &my_result_vals, max_number_iterations);
	}


	//Before outputting the result we wait for all the values
	MPI_Barrier(MPI_COMM_WORLD);

	//Temporary storage no longer needed
	free(my_result_vals);


	if(my_rank == 0){
		char filename[100];
		sprintf(filename,"Mandelbrot_x%f y%f size %f.ppm",pos_x,pos_y,size);
		FILE *fp = fopen(filename, "wb"); /* b - binary mode */
		fprintf(fp, "P6\n%d %d\n255\n", output_size_pixels,output_size_pixels);


		int i, b;
		for(i=0;i<output_size_pixels;++i){
			for(b=0;b<output_size_pixels;++b){
				int res = shared_data[i*output_size_pixels + b];
				if( res > 0){
					greyscale[0]=250-200*res/max_number_iterations;
					greyscale[1]=250-200*res/max_number_iterations;
					greyscale[2]=250-200*res/max_number_iterations;
					fwrite(greyscale, 1 , 3 , fp);
				} else {
					fwrite(black, 1 , 3 , fp);
				}

			}
		}

		fclose(fp);
	}





	MPI_Win_free(&window);
	MPI_Finalize();

	return 0;
}
