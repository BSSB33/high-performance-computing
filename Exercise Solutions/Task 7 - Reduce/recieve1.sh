#!/bin/bash

#PBS -l select=1:ncpus=1:mem=1gb

#PBS -l walltime=00:00:20

#PBS -q short_cpuQ

module load mpich-3.2
cd recieve1
mpicc -g -Wall -o recieve1 recieve1.c -std=c99
mpirun.actual -n 4 ./recieve1