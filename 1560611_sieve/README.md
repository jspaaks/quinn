# 1560611_sieve

Parallel implementation of the sieve of Eratosthenes, with performance improvements proposed by
Lester, 1993 (replacing the `MPI_BCast` with a series of `MPI_Send`s / `MPI_Recv`'s).

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

Expected output should be something like:

```text
$ ./dist/bin/sieve --help
Usage: mpirun -np P ./dist/bin/sieve N
    Use P processes to determine the number of primes in the
    interval [2, N] using the Sieve of Erathostenes with
    Lester's performance improvements.

$ mpirun -np 4 ./dist/bin/sieve 10000
1229 primes are less than or equal to 10000
Total elapsed time:   0.000110
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

## Debugging

The CMake variable `SIEVE_TRAP_DBG` can be used to trap execution of each spawned process,
such that a debugger may be attached to it. `SIEVE_TRAP_DBG`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DSIEVE_TRAP_DBG=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
