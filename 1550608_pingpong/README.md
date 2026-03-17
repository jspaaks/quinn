# 1550608_pingpong

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
$ mpirun -np 2 ./dist/bin/pingpong 10000
```

Should output something like:

```text
10000 ping-pongs of 1 byte took 0.003404 seconds
10000 ping-pongs of 1 MB took 1.253736 seconds
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `PINGPONG_WITH_ASAN` can be used to enable address sanitizing on the
executable `pingpong`. `PINGPONG_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DPINGPONG_WITH_ASAN=ON ..
```

## Debugging

The CMake variable `PINGPONG_TRAP_DBG` can be used to trap execution of each spawned process,
such that a debugger may be attached to it. `PINGPONG_TRAP_DBG`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DPINGPONG_TRAP_DBG=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
