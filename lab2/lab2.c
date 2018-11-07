#include <stdio.h>
#include <string.h>
#include <mpi.h>


void
pingpong (int world_rank, int partner_rank, int value, int count)
{
  if (world_rank == count % 2)
    {
      MPI_Send (&value, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
      printf ("Task B  ::  Count = %d  ::  %d  sends to  %d\n", count, world_rank,
	      partner_rank);
    }
  else
    {
      MPI_Recv (&value, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD,
		MPI_STATUS_IGNORE);
      printf ("Task B  ::  Count = %d  ::  %d  receives from  %d\n", count, world_rank,
	      partner_rank);
    }
}

void
largePingPong (int world_rank, int partner_rank, char value[], int count,
	       int length)
{
  if (world_rank == count % 2)
    {
      MPI_Send (value, length, MPI_BYTE, partner_rank, 0, MPI_COMM_WORLD);
      printf ("Task C  ::  Count = %d  ::  %d  sends to  %d\n", count, world_rank,
	      partner_rank);
    }
  else
    {
      MPI_Recv (value, length, MPI_BYTE, partner_rank, 0, MPI_COMM_WORLD,
		MPI_STATUS_IGNORE);
      printf ("Task C  ::  Count = %d  ::  %d  receives from  %d\n", count, world_rank,
	      partner_rank);
    }
}

int
main (int argc, char** argv)
{
  const int MAX_STRING = 100;

  const int MAX_COUNT = 50;

  double startTime0, startTime1, finishTime0, finishTime1, totalTime0,
    totalTime1, averageLatency0, averageLatency1, bandwidth0, bandwidth1;

  char greeting[MAX_STRING];

  int world_size;
  int world_rank;

  MPI_Init (NULL, NULL);
  MPI_Comm_size (MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank (MPI_COMM_WORLD, &world_rank);

  if (world_size != 2)
    {
      fprintf (stderr, "World size must be two for %s\n", argv[0]);
      MPI_Abort (MPI_COMM_WORLD, 1);
    }

  if (world_rank != 0)
    {
      sprintf (greeting,
	       "Task A  ::  My rank -  %d :: Total number of ranks -  %d\n",
	       world_rank, world_size);
      MPI_Send (greeting, strlen (greeting) + 1, MPI_CHAR, 0, 0,
		MPI_COMM_WORLD);
    }
  else
    {
      printf ("Task A  ::  My rank -  %d :: Total number of ranks -  %d\n",
	      world_rank, world_size);
      int q = 1;
      for (q = 1; q < world_size; q++)
	{
	  MPI_Recv (greeting, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD,
		    MPI_STATUS_IGNORE);
	  printf ("%s\n", greeting);
	}
    }

  int partner_rank = 1 - world_rank;

  int i;
  for (i = 0; i <= MAX_COUNT; i++)
    {
      if (i == 1 && world_rank == 0)
	startTime0 = MPI_Wtime () * 1000000;

      if (i == 1 && world_rank == 1)
	startTime1 = MPI_Wtime () * 1000000;

      pingpong (world_rank, partner_rank, 1, i);
    }

  if (world_rank == 0)
    finishTime0 = MPI_Wtime () * 1000000;
  if (world_rank == 1)
    finishTime1 = MPI_Wtime () * 1000000;

  totalTime0 = finishTime0 - startTime0;
  totalTime1 = finishTime1 - startTime1;

  if (world_rank == 0)
    {
      printf ("\nTask B  ::  Total time taken by rank  %d  is  %f us\n",
	      world_rank, totalTime0);
      printf ("\nTask B  ::  Average Latency for rank  %d  is  %f us\n",
	      world_rank, totalTime0 / 50);
    }
  if (world_rank == 1)
    {
      printf ("\nTask B  ::  Total time taken by rank  %d  is  %f us\n",
	      world_rank, totalTime1);
      printf ("\nTask B  ::  Average Latency for rank  %d  is  %f us\n\n",
	      world_rank, totalTime1 / 50);
    }

  int length = MSG_LEN;
  char buf[length];

  for (i = 0; i <= MAX_COUNT; i++)
    {
      if (i == 1 && world_rank == 0)
	startTime0 = MPI_Wtime () * 1000000;

      if (i == 1 && world_rank == 1)
	startTime1 = MPI_Wtime () * 1000000;

      largePingPong (world_rank, partner_rank, buf, i, length);
    }

  if (world_rank == 0)
    finishTime0 = MPI_Wtime () * 1000000;
  if (world_rank == 1)
    finishTime1 = MPI_Wtime () * 1000000;

  MPI_Finalize ();

  totalTime0 = finishTime0 - startTime0;
  totalTime1 = finishTime1 - startTime1;

  averageLatency0 = totalTime0 / 50;
  averageLatency1 = totalTime1 / 50;

  bandwidth0 = (length / (averageLatency0*1000));
  bandwidth1 = (length / (averageLatency1*1000));

  if (world_rank == 0)
    {
      printf ("\nTotal time taken by rank  %d  is  %f us\n", world_rank,
	      totalTime0);
      printf ("\nAverage Latency for rank  %d  is  %f us\n", world_rank,
	      averageLatency0);
      printf ("\nBandwidth for rank  %d  is  %f GB/s\n",
	      world_rank, bandwidth0);
    }
  if (world_rank == 1)
    {
      printf ("\nTotal time taken by rank  %d  is  %f us\n", world_rank,
	      totalTime1);
      printf ("\nAverage Latency for rank  %d  is  %f us\n", world_rank,
	      averageLatency1);
      printf ("\nBandwidth for rank  %d  is  %f GB/s\n",
	      world_rank, bandwidth1);
    }

  return 0;
}

