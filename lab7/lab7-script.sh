#!/bin/sh
module load intel/18.0.3
srun -t1 -N1 make compile run
