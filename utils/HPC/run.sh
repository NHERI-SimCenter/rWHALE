#!/bin/sh

for node in $(scontrol show hostnames $SLURM_NODELIST) ; do
  srun -N 1-1 -n 1 -w $node cp -rf $PWD/WorkflowRegionalEarthquake /tmp &
done
wait
