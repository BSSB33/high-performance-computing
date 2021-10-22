#!/bin/bash

#PBS -l select=1:ncpus=1:mem=1gb

#PBS -l walltime=00:00:30

#PBS -q short_cpuQ

module load mpich-3.2
cd ping-pong1
mpicc -g -Wall -o ping-pong1 ping-pong1.c -std=c99
mpirun.actual -n 2 ./ping-pong1