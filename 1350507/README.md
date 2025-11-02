# 1350507 Parallel sieve of Erathostenes

Modifications:

- don't reserve space in the sieve for even integers
- determine list of primes 2,3,..sqrt(N) sequentially in each process

## CMake

The project has been initialized with a [CMakeLists.txt](CMakeLists.txt)-based
configuration for building with CMake:

```console
# change into the build directory
$ cd build/

# generate the build files
$ cmake -DCMAKE_BUILD_TYPE=Debug ..

# build the project
$ cmake --build .

# install the project to <repo>/build/dist
$ cmake --install . --prefix dist/
```

Output should be something like:

```text
$ ./dist/bin/blkdcmp-demo
Usage: blkdcmp-demo N NRANKS
    Illustrate the partitioning of [3,5,..,N] over NRANKS.

$ ./dist/bin/blkdcmp-demo 12 2
idx      value    owner
        0        3        0
        1        5        0
        2        7        1
        3        9        1
        4       11        1
---------------------------

0: 0..1 (2)
1: 2..4 (3)
$ rm -r ../build/*
$ cmake -DCMAKE_BUILD_TYPE=Release -DSIEVE_TRAP_DBG=OFF ..
$ cmake --build .
$ cmake --install . --prefix dist
$ mpirun -np 0 ./dist/bin/sieve 1000000
78498 primes are less than or equal to 1000000
Total elapsed time:   0.000406
```

## Testing

Building and running the tests requires that [Criterion](https://github.com/Snaipe/Criterion) is
installed on the system, e.g. with

```console
$ sudo apt install libcriterion3 libcriterion-dev
```

Run the tests with

```console
$ ./dist/bin/test_blkdcmp -j1 --verbose
```

The CMake variable `BLKDCMP_BUILD_TESTING` can be used to build the tests.

- When this project is the top project, `BLKDCMP_BUILD_TESTING` inherits the value of
  CTest's `BUILD_TESTING`, which is set to ON by default.
- When this project is not the top project but instead it is used as a dependency to a parent
  project, the default is to not build the tests. However, building the tests is still possible by
  setting the `BLKDCMP_BUILD_TESTING` to `ON`, e.g like so:

```console
$ cmake -DBLKDCMP_BUILD_TESTING=ON ..
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `SIEVE_WITH_ASAN` can be used to enable address sanitizing on the
executable `sieve`. `SIEVE_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DSIEVE_WITH_ASAN=ON ..
```

The CMake variable `BLKDCMP_WITH_ASAN` can be used to enable address sanitizing on the
library `libblkdcmp.so`. `BLKDCMP_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DBLKDCMP_WITH_ASAN=ON ..
```

## Debugging

The CMake variable `SIEVE_TRAP_DBG` can be used to trap execution of each spawned process, such that a debugger may be attached to it. `SIEVE_TRAP_DBG`'s value is `OFF` by default. To enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DSIEVE_TRAP_DBG=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
