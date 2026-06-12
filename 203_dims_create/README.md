# 203_dims_create

Exploring `MPI_Dims_create`.

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
$ ./dist/bin/dims_create 
Having 1 processes will result in a 1x1 cartesian topology.
$ ./dist/bin/dims_create asdasd
Incorrect number of arguments, aborting.

Usage: mpirun -np NRANKS ./dist/bin/dims_create

  Multiprocess program that illustrates how the number of processes
  NRANKS will be divided over a cartesian grid of 2 dimensions.

$ ./dist/bin/dims_create
Having 1 processes will result in a 1x1 cartesian topology.
$ mpirun -np 12 --oversubscribe ./dist/bin/dims_create                                                                                                                                                                                                                                                       
Having 12 processes will result in a 4x3 cartesian topology.
$ mpirun -np 13 --oversubscribe ./dist/bin/dims_create                                                                                                                                                                                                                                                      
Having 13 processes will result in a 13x1 cartesian topology.
$ mpirun -np 14 --oversubscribe ./dist/bin/dims_create                                                                                                                                                                                                                                                      
Having 14 processes will result in a 7x2 cartesian topology.
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `DIMS_CREATE_WITH_ASAN` can be used to enable address sanitizing on the
executable `dims_create`. `DIMS_CREATE_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DDIMS_CREATE_WITH_ASAN=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
