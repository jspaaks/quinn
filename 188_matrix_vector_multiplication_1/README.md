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
