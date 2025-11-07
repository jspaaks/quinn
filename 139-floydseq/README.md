# 139-floydseq

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

Running the program should output something like:

```text
$ ./dist/bin/floydseq
Usage: ./dist/bin/floydseq FILEPATH
    Read a directed acyclic graph's adjacency matrix from FILEPATH
    and use Floyd's algorithm to determine the shortest-path
    matrix. FILEPATH should point to a binary file in IDX format.
$ ./dist/bin/floydseq ../data/adjacency.idx

Example of Floyd's algorithm

adjacency matrix:
    0     2     5   Inf   Inf   Inf 
  Inf     0     7     1   Inf     8 
  Inf   Inf     0     4   Inf   Inf 
  Inf   Inf   Inf     0     3   Inf 
  Inf   Inf     2   Inf     0     3 
  Inf     5   Inf     2     4     0 

shortest-path matrix:
    0     2     5     3     6     9 
  Inf     0     6     1     4     7 
  Inf    15     0     4     7    10 
  Inf    11     5     0     3     6 
  Inf     8     2     5     0     3 
  Inf     5     6     2     4     0 

```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `FLOYDSEQ_WITH_ASAN` can be used to enable address sanitizing on the
executable `floydseq`. `FLOYDSEQ_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DFLOYDSEQ_WITH_ASAN=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
