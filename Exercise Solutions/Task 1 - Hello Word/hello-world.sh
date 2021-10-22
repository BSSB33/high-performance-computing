#!/bin/bash

#PBS -l select=2:ncpus=2:mem=1gb

#PBS -l walltime=00:05:00

#PBS -q short_cpuQ

module load mpich-3.2
mpirun.actual -n 2 ./hello-world/processes