# 185_allgatherv

MPI_Allgatherv demo

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

Output should be like:

```text
$ ./dist/bin/allgatherv-demo
Program should have 4 ranks, aborting.
$ mpirun -np 2 ./dist/bin/allgatherv-demo
Program should have 4 ranks, aborting.
--------------------------------------------------------------------------
Primary job  terminated normally, but 1 process returned
a non-zero exit code. Per user-direction, the job has been aborted.
--------------------------------------------------------------------------
--------------------------------------------------------------------------
mpirun detected that one or more processes exited with non-zero status, thus causing
the job to be terminated. The first process to do so was:

  Process name: [[9692,1],0]
  Exit code:    42
--------------------------------------------------------------------------
$ mpirun -np 4 ./dist/bin/allgatherv-demo
MPI_Allgatherv demo
expected:  100  101  102  103  104  105  106  107  108  109  110  111  112  113
actual  :  100  101  102  103  104  105  106  107  108  109  110  111  112  113
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

## CMake variables

| CMake variable name | Description |
| --- | --- |
| `ALLGATHERV_DEMO_WITH_ASAN` | enables address sanitizing on the executable `allgatherv-demo` |
| `ALLGATHERV_DEMO_TRAP_DBG`  | enables trapping execution of the program at the start in order to facilitate attaching a debugger |

Configure the build via `ccmake ..`, or via a command line argument with e.g.:

```console
$ cmake -DALLGATHERV_DEMO_WITH_ASAN=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
