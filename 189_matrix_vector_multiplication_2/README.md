# 189_matrix_vector_multiplication_2

Matrix-vector multiplication using NP processes (column-striped matrix implementation).

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
$ ./dist/bin/multiply
```

Should output something like:

```text
$ ./dist/bin/multiply -h
Usage mpirun -np NP ./dist/bin/multiply [REQUIREDS]

  Matrix-vector multiplication using NP processes (column-striped matrix implementation).

  The matrix is the left hand side operand; the vector is the right hand side
  operand. The program will initialize the operands with random integers 0..9,
  calculate the result, then print both operands and the resulting vector.

  Required arguments
    --ncols, -c   Number of columns in the matrix (number of elements in the vector).
    --nrows, -r   Number of rows in the matrix.

$ mpirun -np 2 ./dist/bin/multiply --nrows 3 --ncols 5
LHS operand (matrix):
 4  0  9  9  4
 5  9  0  8  7
 9  6  8  6  2
RHS operand (column vector):
  7
  1
  4
  2
  7
result (column vector):
   110
   109
   127
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `MULTIPLY_WITH_ASAN` can be used to enable address sanitizing on the
executable `multiply`. `MULTIPLY_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DMULTIPLY_WITH_ASAN=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
