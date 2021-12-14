#!/bin/bash

#PBS -l select=1:ncpus=64:mem=1gb

#nodes=1,2,4
#cpus=1,2,4,8,16

#PBS -l walltime=00:03:00

#PBS -q short_cpuQ

module load mpich-3.2

cd project/high-performance-computing/Project/parallel
rm job.sh.*
# mpicc -g -Wall -o p parallel_v1.c
mpicc -g -Wall -o p parallel_v2.c
# mpicc -g -Wall -fopenmp -o p parallel_v4.c -std=c99
# ../sequential/seq ../got/got_sentences.txt 1
# ../sequential/seq ../alice/alice_sentences.txt 1

echo "Running with 1 nodes 64 cores"
for proc in 1 2 4 8 16 32 64
do
    # mpirun.actual -n $proc ./p ../alice/alice_sentences.txt 1 988
    mpirun.actual -n $proc ./p ../got/got_sentences.txt 1 137781
done
echo "2. run"
for proc in 1 2 4 8 16 32 64
do
    # mpirun.actual -n $proc ./p ../alice/alice_sentences.txt 1 988
    mpirun.actual -n $proc ./p ../got/got_sentences.txt 1 137781
done
echo "3. run"
for proc in 1 2 4 8 16 32 64
do
    # mpirun.actual -n $proc ./p ../alice/alice_sentences.txt 1 988
    mpirun.actual -n $proc ./p ../got/got_sentences.txt 1 137781
done
# echo "Running for GOT"
# for proc in {1, 2, 4, 8, 16, 32}
# do
#     # mpirun.actual -n $proc ./p ../alice/alice_sentences.txt 1 988
#     mpirun.actual -n $proc ./p ../got/got_sentences.txt 1 137781
# done
