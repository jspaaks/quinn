# 1560612a_shade

Calculate shade casting based on a map of topographic data, with sun due West.

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

Should output something like:

```text
$ xxd ../data/topography.int8.idx
00000000: 0000 0902 0000 0006 0000 0006 0000 0503  ................
00000010: 0200 0507 0b0a 0804 0f09 1007 0803 160f  ................
00000020: 040b 0702 0a02 0a09 0702 0000 0507 0400  ................
$ ./dist/bin/shade-seq --help
Usage: ./dist/bin/shade-seq FILENAME

    Calculate shading based on topographic data from FILENAME, assuming that the
    sun is due West, and using a sequential implementation.

    Arguments

        FILENAME   IDX formatted, 2-dimensional, square array of int8_t
                   holding the topographic data

$ ./dist/bin/shade-seq ../data/topography.int8.idx 
topo:
   0   0   5   3   2   0
   5   7  11  10   8   4
  15   9  16   7   8   3
  22  15   4  11   7   2
  10   2  10   9   7   2
   0   0   5   7   4   0

shade:
   0   0   0   0   0   0
   0   0   0   0   0   1
   0   1   0   1   1   1
   0   1   1   0   1   1
   0   1   0   0   0   1
   0   0   0   0   0   1
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `SHADE_SEQ_WITH_ASAN` can be used to enable address sanitizing on the
executable `shade-seq`. `SHADE_SEQ_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DSHADE_SEQ_WITH_ASAN=ON ..
```

The CMake variable `SHADE_PAR_WITH_ASAN` can be used to enable address sanitizing on the
executable `shade-par`. `SHADE_PAR_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DSHADE_PAR_WITH_ASAN=ON ..
```

## Debugging

The CMake variable `SHADE_PAR_TRAP_DBG` can be used to trap execution of each spawned process,
such that a debugger may be attached to it. `SHADE_PAR_TRAP_DBG`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DSHADE_PAR_TRAP_DBG=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
