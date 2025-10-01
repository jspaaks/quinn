# exercise 4.9 max gap between primes

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
$ time ./dist/bin/serial 1000000
Max gap between adjacent primes in range [2..1000000] was 114.

real    1m43.198s
user    1m43.185s
sys     0m0.005s
$ time mpirun -np 0 ./dist/bin/parallel 1000 1000000
The maximum gap between two adjancent primes in interval [2..1000000] is: 114

real    0m42.024s
user    2m45.162s
sys     0m0.131s
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._

[![Copier](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/copier-org/copier/master/img/badge/badge-grayscale-inverted-border-orange.json)](https://github.com/copier-org/copier)
