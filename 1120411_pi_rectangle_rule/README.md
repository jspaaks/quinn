# exercise 4.11 approximation of pi using rectangle rule

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
$ mkdir build && cd build/

# generate the build files
$ cmake ..

# build the project
$ cmake --build .

# install the project to <repo>/build/dist
$ cmake --install .
```

Subsequently running the programs should output something like:

```text
$ time ./dist/bin/serial
Area under the curve is: 3.1415926536

real    0m37.829s
user    0m37.819s
sys     0m0.008s
$ time mpirun -np 1 ./dist/bin/parallel
Area under the curve is: 3.1415926536

real    0m40.168s
user    0m39.605s
sys     0m0.059s
$ time mpirun -np 4 ./dist/bin/parallel
Area under the curve is: 3.1415926536

real    0m23.735s
user    1m27.213s
sys     0m0.208s
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._

[![Copier](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/copier-org/copier/master/img/badge/badge-grayscale-inverted-border-orange.json)](https://github.com/copier-org/copier)
