#!/bin/bash

#PBS -l select=1:ncpus=1:mem=1gb

#PBS -l walltime=00:00:20

#PBS -q short_cpuQ

module load mpich-3.2

cd project/high-performance-computing/Project/parallel
rm job.sh.*
mpicc -g -Wall -o parallel_v1 parallel_v1.c
mpirun.actual -n 2 ./parallel_v1 ../alice/alice_sentences.txt 1 988