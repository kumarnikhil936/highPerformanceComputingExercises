#!/bin/sh
module load intel/18.0.3
srun -t1 --unbuffered -A hpclco make default
