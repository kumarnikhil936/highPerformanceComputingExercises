#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

const unsigned char LIVE = 1;
const unsigned char DEAD = 0;

double
get_time ()
{
  struct timeval tv;
  if (gettimeofday (&tv, NULL) == 0)
    return (double) tv.tv_sec + ((double) tv.tv_usec / (double) 1000000);
  else
    return 0.0;
}

/* data structure for two-dimensional array */
typedef struct arr
{
  int rows;
  int cols;
  int **elems;
} arr_t;


/* function declarations */
arr_t *create_board (arr_t * a, int rows, int cols);

void freeup_memory (arr_t * a);

int read_ppm (FILE * infile, arr_t * board);

void update (arr_t * board, arr_t * new_board);

void write_ppm (char *filename, arr_t * board);

void transform_border (arr_t * board);


/* main */
int
main (int argc, char *argv[])
{
  int steps = 0, print_interval = 0, size = 0;

  arr_t board1, board2;
  arr_t *board = &board1;
  arr_t *new_board = &board2;

  double start_time, init_done_time, end_time;

  char *args_message = "infile num_steps";
  int num_threads;

  /* using the number of threads set in the environment variable OMP_NUM_THREADS */
#pragma omp parallel
  {
#pragma omp master
    {
      num_threads = omp_get_num_threads ();
    }
  }

  start_time = get_time ();

  if (argc < 3)
    {
      fprintf (stderr, "usage:  %s %s\n", argv[0], args_message);
      return EXIT_FAILURE;
    }

  /* initialize board */
  FILE *infile;

  infile = fopen (argv[1], "r");

  if (infile == NULL)
    {
      fprintf (stderr, "unable to open input file %s\n", argv[1]);
      return EXIT_FAILURE;
    }

  steps = atoi (argv[2]);

  if (steps <= 0)
    {
      fprintf (stderr, "usage:  %s %s\n. Steps should be greater than 0.", argv[0], args_message);
      return EXIT_FAILURE;
    }

  if (read_ppm (infile, board) != 0)
    {
      fclose (infile);
      return EXIT_FAILURE;
    }

  size = board->rows - 2;

  fclose (infile);

  /* print input information: */
  fprintf (stderr, "\nInput:  board read from file %s\n", argv[1]);

  fprintf (stderr, "\n%d steps, %d thread(s)\n", steps, num_threads);

  /* create "new board" and clear borders */
  if (create_board (new_board, size + 2, size + 2) == NULL)
    {
      fprintf (stderr, "unable to allocate space for board of size %d\n",
	       size);
      return EXIT_FAILURE;
    }

  transform_border (new_board);

  /* print initial configuration */
  write_ppm ("output-0.ppm", board);

  init_done_time = get_time ();

  /* loop to update board and print */
  for (int step = 0; step < steps; ++step)
    {

      /* update (results in new_board) */
      update (board, new_board);

      /* print */
      char filename[64];
      sprintf (filename, "output-%d.ppm", step + 1);

      fprintf (stderr, "\nWriting output ppm file after iteration %d\n",
	       step + 1);

      write_ppm (filename, board);

      /* swap old and new boards */
      {
	arr_t *temp = board;
	board = new_board;
	new_board = temp;
      }
    }

  // write_ppm("output.ppm", board);
  end_time = get_time ();

  /* print timing information. */
  fprintf (stderr, "\nTotal time %gs\n", end_time - start_time);

  fprintf (stderr, "\nTime excluding initialization %gs\n",
	   end_time - init_done_time);

  /* tidy up and return */
  freeup_memory (board);
  freeup_memory (new_board);

  return EXIT_SUCCESS;
}

/* constructs arr structure. */
arr_t *
create_board (arr_t * a, int rows, int cols)
{
  int *temp;

  a->rows = rows;
  a->cols = cols;

  if ((a->elems = malloc (rows * sizeof (int *))) == NULL)
    {
      return NULL;
    }

  if ((temp = malloc (rows * cols * sizeof (int))) == NULL)
    {
      free (a->elems);
      return NULL;
    }

  for (int row = 0; row < rows; ++row, temp += cols)
    {
      a->elems[row] = temp;
    }

  return a;
}

/* frees space pointed to by arr structure */
void
freeup_memory (arr_t * a)
{
  free (a->elems[0]);
  free (a->elems);
}

/* sets unused "edge" cells to 0 */
void
transform_border (arr_t * board)
{

  for (int c = 0; c < board->cols; ++c)
    {
      board->elems[0][c] = 0;
      board->elems[board->rows - 1][c] = 0;
    }

  for (int r = 0; r < board->rows; ++r)
    {
      board->elems[r][0] = 0;
      board->elems[r][board->cols - 1] = 0;
    }
}

/* reads initial configuration from infile. */
int
read_ppm (FILE * infile, arr_t * board)
{
  int i, j, size, temp;
  size_t size1, size2;

  fscanf (infile, "P6\n%zd %zd\n255\n", &size1, &size2);

  size = (int) (size1);

  if (create_board (board, size + 2, size + 2) == NULL)
    {
      fprintf (stderr, "unable to allocate space for board of size %d\n",
	       size);
      return 2;
    }

  for (i = 1; i <= size; ++i)
    {
      for (j = 1; j <= size; ++j)
	{

	  unsigned char buff[3];
	  fread (buff, 1, 3, infile);

	  if (buff[0] == 0 && buff[1] == 0 && buff[2] == 0)	//black pixel
	    board->elems[i][j] = LIVE;
	  else
	    board->elems[i][j] = DEAD;
	}
    }
  transform_border (board);
  return 0;
}


/* updates board configuration */
void
update (arr_t * board, arr_t * new_board)
{
  int i, j;

#pragma omp parallel for private(j) schedule(static)
  for (i = 1; i <= board->rows - 2; ++i)
    {
      for (j = 1; j <= board->cols - 2; ++j)
	{

	  int neighbs = 0;
	  int mycell = board->elems[i][j];
	  int k, l;

	  /*count neighbors */
	  for (k = i - 1; k <= i + 1; ++k)
	    {
	      for (l = j - 1; l <= j + 1; ++l)
		{

		  if (!(k == i && l == j))
		    {
		      neighbs += board->elems[k][l];

		    }
		}
	    }

	  /*Logic of game */
	  if (mycell)
	    {
	      if (!(neighbs == 2 || neighbs == 3))
		{
		  mycell = 0;
		}
	    }
	  else
	    {
	      if (neighbs == 3)
		{
		  mycell = 1;
		}
	    }

	  /*Update board */
	  new_board->elems[i][j] = mycell;
	}
    }
}


/* prints current board configuration. */
void
write_ppm (char *filename, arr_t * board)
{

  FILE *outfile = fopen (filename, "wb");
  fprintf (outfile, "P6\n%zd %zd\n255\n", board->rows - 2, board->cols - 2);

  const unsigned char _black[] = { 0x00, 0x00, 0x00 };
  const unsigned char _white[] = { 0xff, 0xff, 0xff };

  for (int i = 1; i <= board->rows - 2; ++i)
    {
      for (int j = 1; j <= board->cols - 2; ++j)
	{
	  if (board->elems[i][j] == 1)
	    fwrite (_black, 1, 3, outfile);
	  else
	    fwrite (_white, 1, 3, outfile);
	}
    }

  fclose (outfile);
}

