#!/bin/sh
module load intel/18.0.3
srun -t10 -N1 -A hpclco make default
