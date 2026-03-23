# exercise 4.6

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

Subsequently running the program should output something like:

```text
$ mpirun -np 1 ./dist/bin/parallel
Hello world from process 0
$ mpirun -np 4 ./dist/bin/parallel
Hello world from process 0
Hello world from process 2
Hello world from process 3
Hello world from process 1
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._

[![Copier](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/copier-org/copier/master/img/badge/badge-grayscale-inverted-border-orange.json)](https://github.com/copier-org/copier)
