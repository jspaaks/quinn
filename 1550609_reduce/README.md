# 1550609_reduce

Use N processes to sum integers 0..M using a reduce operation.

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

Expected output:

```text
$ ./dist/bin/reduce --help
Usage: mpirun -np N ./dist/bin/reduce M

    Use N processes to sum integers 0..M using a reduce operation.

$ mpirun -np 4 ./dist/bin/reduce 14354
rank 0 will calculate the sum of 0-3587 (3588)
rank 2 will calculate the sum of 7177-10765 (3589)
rank 3 will calculate the sum of 10766-14354 (3589)
rank 1 will calculate the sum of 3588-7176 (3589)
The sum of [0..14354] is 103025835
```

## Testing

Building and running the tests requires that [Criterion](https://github.com/Snaipe/Criterion) is
installed on the system, e.g. with

```console
$ sudo apt install libcriterion3 libcriterion-dev
```

Run the tests with

```console
$ ./dist/bin/test_operations -j1 --verbose
```

The CMake variable `OPERATIONS_BUILD_TESTING` can be used to build the
tests.

- When this project is the top project, `OPERATIONS_BUILD_TESTING` inherits the value of
  CTest's `BUILD_TESTING`, which is set to ON by default.
- When this project is not the top project but instead it is used as a dependency to a parent
  project, the default is to not build the tests. However, building the tests is still possible by
  setting the `OPERATIONS_BUILD_TESTING` to `ON`, e.g like so:

```console
$ cmake -DOPERATIONS_BUILD_TESTING=ON ..
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `REDUCE_WITH_ASAN` can be used to enable address sanitizing on the
executable `reduce`. `REDUCE_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DREDUCE_WITH_ASAN=ON ..
```

## Debugging

The CMake variable `REDUCE_TRAP_DBG` can be used to trap execution of each spawned process,
such that a debugger may be attached to it. `REDUCE_TRAP_DBG`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DREDUCE_TRAP_DBG=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
