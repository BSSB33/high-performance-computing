#!/bin/bash

#PBS -l select=1:ncpus=1:mem=1gb

#PBS -l walltime=00:00:20

#PBS -q short_cpuQ

module load mpich-3.2
cd ping-pong2
mpicc -g -Wall -o ping-pong2 ping-pong2.c -std=c99
mpirun.actual -n 2 ./ping-pong2