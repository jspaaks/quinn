# 204_cart_create

Exploring `MPI_Cart_create`, `MPI_Cart_coords`, and `MPI_Cart_shift` functions.

Program creates a 2 dimensional cartesian topology of processes, then lets each process
print some diagnostic information about itself and its neighbors.

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
$ mpirun -np 12 --oversubscribe ./dist/bin/cart_create
Using a Cartesian topology communicator with 4 rows and 3 columns of processes
with a periodic boundary horizontally, but not vertically.

row  |  column  |  rank  |  top  |  right  |  bottom  |  left
   2 |        0 |      6 |     3 |       7 |        9 |      8 
   2 |        1 |      7 |     4 |       8 |       10 |      6 
   2 |        2 |      8 |     5 |       6 |       11 |      7 
   3 |        2 |     11 |     8 |       9 |       -2 |     10 
   0 |        0 |      0 |    -2 |       1 |        3 |      2 
   1 |        0 |      3 |     0 |       4 |        6 |      5 
   1 |        1 |      4 |     1 |       5 |        7 |      3 
   0 |        2 |      2 |    -2 |       0 |        5 |      1 
   3 |        1 |     10 |     7 |      11 |       -2 |      9 
   1 |        2 |      5 |     2 |       3 |        8 |      4 
   3 |        0 |      9 |     6 |      10 |       -2 |     11 
   0 |        1 |      1 |    -2 |       2 |        4 |      0 
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `CART_CREATE_WITH_ASAN` can be used to enable address sanitizing on the
executable `cart_create`. `CART_CREATE_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DCART_CREATE_WITH_ASAN=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
