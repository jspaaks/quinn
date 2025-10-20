# block decomposition

Library with reusable functions to determine block size, block start index, block last index, etc -- useful when parallel programming.

## Requirements

```
# install cmake, criterion and its developement headers, as well as some compilation
# tools from ubuntu repositories
$ sudo apt install cmake            \
                   libcriterion3    \
                   libcriterion-dev \
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

Subsequently running the test program should output something like:

```text
$ ctest
    Start 1: test_blkdcmp
1/1 Test #1: test_blkdcmp .....................   Passed    0.60 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.60 sec
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._

[![Copier](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/copier-org/copier/master/img/badge/badge-grayscale-inverted-border-orange.json)](https://github.com/copier-org/copier)
