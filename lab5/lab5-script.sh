#!/bin/sh
module load intel/18.0.3
srun -t15 -N1 -A hpclco make default
