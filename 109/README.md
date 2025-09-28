# circuit satisfiability

## Requirements

```
# install openmpi, cmake, and some compilation tools from ubuntu repositories
$ sudo apt install libopenmpi3t64  \
                   libopenmpi-dev  \
                   cmake           \
                   build-essential
```

## CMake

The project has been initialized with a [CMakeLists.txt](CMakeLists.txt)-based
configuration for building with CMake:

```console
# change into the build directory
$ cd build/

# generate the build files
$ cmake ..

# build the project
$ cmake --build .

# install the project to <repo>/build/dist
$ cmake --install .
```

Subsequently running the programs should output something like:

```text
$ ./dist/bin/serial
serial version:
0) 1010 1111 1001 1001 (39413)
0) 0110 1111 1001 1001 (39414)
0) 1110 1111 1001 1001 (39415)
0) 1010 1111 1101 1001 (39925)
0) 0110 1111 1101 1001 (39926)
0) 1110 1111 1101 1001 (39927)
0) 1010 1111 1011 1001 (40437)
0) 0110 1111 1011 1001 (40438)
0) 1110 1111 1011 1001 (40439)
Found 9 ways to satisfy the circuit
$ ./dist/bin/serial 39413
0) 1010 1111 1001 1001 (39413)
Circuit was satisfied for input 39413
$ ./dist/bin/serial 77
Circuit was not satisfied for input 77
$ mpirun -np 2 ./dist/bin/parallel
0) 0110 1111 1001 1001 (39414)
0) 0110 1111 1101 1001 (39926)
0) 0110 1111 1011 1001 (40438)
1) 1010 1111 1001 1001 (39413)
1) 1110 1111 1001 1001 (39415)
1) 1010 1111 1101 1001 (39925)
1) 1110 1111 1101 1001 (39927)
1) 1010 1111 1011 1001 (40437)
1) 1110 1111 1011 1001 (40439)
Process 0 is done, identified 3 solutions
There are 9 solutions in total
Walltime: 2958.758000 microseconds
Process 1 is done, identified 6 solutions
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._

[![Copier](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/copier-org/copier/master/img/badge/badge-grayscale-inverted-border-orange.json)](https://github.com/copier-org/copier)
