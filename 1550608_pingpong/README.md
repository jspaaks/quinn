# 1550608_pingpong

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
$ ./dist/bin/pingpong
Usage: mpirun -np 2 ./dist/bin/pingpong N
    Multiprocess program that goes back and forth between its 2 processes
    for a total of N times, subsequently reporting average latency and
    bandwidth statistics
--------------------------------------------------------------------------
MPI_ABORT was invoked on rank 0 in communicator MPI_COMM_WORLD
with errorcode -1.

NOTE: invoking MPI_ABORT causes Open MPI to kill all MPI processes.
You may or may not see output from other processes, depending on
exactly when Open MPI kills them.
--------------------------------------------------------------------------
$ mpirun -np 2 ./dist/bin/pingpong 10000
10000 ping-pongs of 1 byte took 0.002910 seconds
10000 ping-pongs of 1 MB took 1.231166 seconds
Estimated latency: 1.45439e-07 [s]
Estimated bandwidth: 1.62832e+10 [bytes/s]
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `PINGPONG_WITH_ASAN` can be used to enable address sanitizing on the
executable `pingpong`. `PINGPONG_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DPINGPONG_WITH_ASAN=ON ..
```

## Debugging

The CMake variable `PINGPONG_TRAP_DBG` can be used to trap execution of each spawned process,
such that a debugger may be attached to it. `PINGPONG_TRAP_DBG`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DPINGPONG_TRAP_DBG=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
