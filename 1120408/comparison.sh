#! /bin/bash
set -x
let val=1000000
time ./dist/bin/serial $val
time mpirun -np 0 ./dist/bin/parallel1 $val
time mpirun -np 0 ./dist/bin/parallel2 $val

