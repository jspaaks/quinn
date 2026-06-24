# 205_checkered_read

Exploring data distribution patterns in a Cartesian communicator.

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

# run the program to see if it works
$ ./dist/bin/read
```

The program should generate a grid of 7x11 integers randomly draw from the interval [0,9]. Each row then
gets distributed to the first first process in each column of the Cartesian communicator, where it is
further distributed along the members of that row in the Cartesian communcicator. Each process then reports
its chunk of the original matrix to the terminal, something like:

```text
$ mpirun -np 4 ./dist/bin/read
(1,1) has submatrix of size 4x6
(0,0) topology: 2x2
(0,0) has submatrix of size 3x5
(0,0) 6 4 9 8 6 6 3 1 5 6 2
(0,0) 5 4 7 9 1 1 1 1 9 0 0
(0,0) 9 2 6 3 1 4 8 2 5 4 8
(0,0) 6 4 6 4 8 7 1 6 2 8 2
(0,0) 9 8 4 2 9 5 1 2 7 0 4
(0,0) 5 6 7 2 4 1 9 0 1 8 5
(0,0) 8 2 5 7 6 1 9 4 4 0 2
(0,1) has submatrix of size 3x6
(1,0) has submatrix of size 4x5
(0,0) lengths: 5 6
(0,0) offsets: 0 5
(1,0): 6 4 6 4 8
(1,0): 9 8 4 2 9
(1,0): 5 6 7 2 4
(1,0): 8 2 5 7 6
(0,1): 6 3 1 5 6 2
(0,1): 1 1 1 9 0 0
(0,1): 4 8 2 5 4 8
(0,0): 6 4 9 8 6
(0,0): 5 4 7 9 1
(0,0): 9 2 6 3 1
(1,1): 7 1 6 2 8 2
(1,1): 5 1 2 7 0 4
(1,1): 1 9 0 1 8 5
(1,1): 1 9 4 4 0 2
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `READ_WITH_ASAN` can be used to enable address sanitizing on the
executable `read`. `READ_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DREAD_WITH_ASAN=ON ..
```

## Debugging 

The CMake variable `READ_TRAP_DBG` can be used to enable trapping the debugger when
executing `read`. `READ_TRAP_DBG`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DREAD_TRAP_DBG=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
