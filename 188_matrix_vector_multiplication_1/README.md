# 188_matrix_vector_multiplication_1

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
$ ./dist/bin/multiply -h
Usage mpirun -np NP ./dist/bin/multiply [REQUIREDS]

  Matrix-vector multiplication using NP processes (row-striped matrix implementation).

  The matrix is the left hand side operand; the vector is the right hand side
  operand. The program will initialize the operands with random integers 0..9,
  calculate the result, then print both operands and the resulting vector.

  Required arguments
    --ncols, -c   Number of columns in the matrix (number of elements in the vector).
    --nrows, -r   Number of rows in the matrix.

$ mpirun -np 4 ./dist/bin/multiply --nrows 10 --ncols 5
matrix (left hand operand):
    4     4     0     8     8
    5     6     2     8     3
    4     0     0     1     8
    0     5     8     4     5
    3     8     4     9     7
    5     1     7     4     8
    3     7     1     0     4
    3     8     5     3     1
    7     5     0     2     5
    3     7     3     8     8
vector (right hand operand):
    5
    3
    1
    4
    5
vector (result):
  104
   92
   64
   64
  114
   91
   57
   61
   83
  111
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

## CMake variables

| CMake variable name | description |
|---|---|
| `MULTIPLY_WITH_ASAN` | enables address sanitizing on the executable `multiply` |
| `MULTIPLY_TRAP_DBG`  | enables trapping execution of the program at the start in order to facilitate attaching a debugger |

Configure the build via `ccmake ..`, or via a command line argument with e.g.:

```console
$ cmake -DMULTIPLY_WITH_ASAN=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
