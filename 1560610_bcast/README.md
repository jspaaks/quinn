# 1560610_bcast

Broadcast a message to N processes.

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

The output should be something like:

```text
$ ./dist/bin/bcast --help
Usage: mpirun -np N ./dist/bin/bcast

    Broadcast a message to N processes.

$ mpirun -np 4 ./dist/bin/bcast
before (1) ...........
before (2) ...........
before (3) ...........
before (0) hello bcast
after  (2) hello bcast
after  (0) hello bcast
after  (3) hello bcast
after  (1) hello bcast
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `BCAST_WITH_ASAN` can be used to enable address sanitizing on the
executable `bcast`. `BCAST_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DBCAST_WITH_ASAN=ON ..
```

## Debugging

The CMake variable `BCAST_TRAP_DBG` can be used to trap execution of each spawned process,
such that a debugger may be attached to it. `BCAST_TRAP_DBG`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DBCAST_TRAP_DBG=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
