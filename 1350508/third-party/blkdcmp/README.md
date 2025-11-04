# block decomposition

Library with reusable functions to determine block size, block start index, block last index, etc.--useful when parallel programming.

## Requirements

Install compilation tools and CMake from Ubuntu repos:

```console
sudo apt install build-essential
sudo apt install cmake
```

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

## Testing

Building and running the tests requires that [Criterion](https://github.com/Snaipe/Criterion) is
installed on the system, e.g. with

```console
$ sudo apt install libcriterion3 libcriterion-dev
```

The CMake variable `BLKDCMP_BUILD_TESTING` can be used to control building the
tests.

- When this project is the top project, `BLKDCMP_BUILD_TESTING` inherits the value of
  CTest's `BUILD_TESTING`, which is set to ON by default.
- When this project is not the top project but instead it is used as a dependency to a parent
  project, the default is to not build the tests. However, building the tests is still possible by
  setting the `BLKDCMP_BUILD_TESTING` to `ON`, e.g like so:

```console
$ cmake -DBLKDCMP_BUILD_TESTING=ON ..
```

Run the tests with

```console
$ ./dist/bin/test_blkdcmp -j1 --verbose
```

Should yield something like:

```console
└─ $ ./dist/bin/test_blkdcmp -j1 --verbose
[----] Criterion v2.4.1
[====] Running 2 tests from blkdcmp_get_blk_owner:
[RUN ] blkdcmp_get_blk_owner::100_4
[PASS] blkdcmp_get_blk_owner::100_4: (0.00s)
[RUN ] blkdcmp_get_blk_owner::10_4
[PASS] blkdcmp_get_blk_owner::10_4: (0.00s)
[====] Running 8 tests from blkdcmp_get_blk_sz:
[RUN ] blkdcmp_get_blk_sz::100_4_0
[PASS] blkdcmp_get_blk_sz::100_4_0: (0.00s)
[RUN ] blkdcmp_get_blk_sz::100_4_1
[PASS] blkdcmp_get_blk_sz::100_4_1: (0.00s)
[RUN ] blkdcmp_get_blk_sz::100_4_2
[PASS] blkdcmp_get_blk_sz::100_4_2: (0.00s)
[RUN ] blkdcmp_get_blk_sz::100_4_3
[PASS] blkdcmp_get_blk_sz::100_4_3: (0.00s)
[RUN ] blkdcmp_get_blk_sz::10_4_0
[PASS] blkdcmp_get_blk_sz::10_4_0: (0.00s)
[RUN ] blkdcmp_get_blk_sz::10_4_1
[PASS] blkdcmp_get_blk_sz::10_4_1: (0.00s)
[RUN ] blkdcmp_get_blk_sz::10_4_2
[PASS] blkdcmp_get_blk_sz::10_4_2: (0.00s)
[RUN ] blkdcmp_get_blk_sz::10_4_3
[PASS] blkdcmp_get_blk_sz::10_4_3: (0.00s)
[====] Running 8 tests from blkdcmp_get_idx_blk_e:
[RUN ] blkdcmp_get_idx_blk_e::100_4_0
[PASS] blkdcmp_get_idx_blk_e::100_4_0: (0.00s)
[RUN ] blkdcmp_get_idx_blk_e::100_4_1
[PASS] blkdcmp_get_idx_blk_e::100_4_1: (0.00s)
[RUN ] blkdcmp_get_idx_blk_e::100_4_2
[PASS] blkdcmp_get_idx_blk_e::100_4_2: (0.00s)
[RUN ] blkdcmp_get_idx_blk_e::100_4_3
[PASS] blkdcmp_get_idx_blk_e::100_4_3: (0.00s)
[RUN ] blkdcmp_get_idx_blk_e::10_4_0
[PASS] blkdcmp_get_idx_blk_e::10_4_0: (0.00s)
[RUN ] blkdcmp_get_idx_blk_e::10_4_1
[PASS] blkdcmp_get_idx_blk_e::10_4_1: (0.00s)
[RUN ] blkdcmp_get_idx_blk_e::10_4_2
[PASS] blkdcmp_get_idx_blk_e::10_4_2: (0.00s)
[RUN ] blkdcmp_get_idx_blk_e::10_4_3
[PASS] blkdcmp_get_idx_blk_e::10_4_3: (0.00s)
[====] Running 8 tests from blkdcmp_get_idx_blk_s:
[RUN ] blkdcmp_get_idx_blk_s::100_4_0
[PASS] blkdcmp_get_idx_blk_s::100_4_0: (0.00s)
[RUN ] blkdcmp_get_idx_blk_s::100_4_1
[PASS] blkdcmp_get_idx_blk_s::100_4_1: (0.00s)
[RUN ] blkdcmp_get_idx_blk_s::100_4_2
[PASS] blkdcmp_get_idx_blk_s::100_4_2: (0.00s)
[RUN ] blkdcmp_get_idx_blk_s::100_4_3
[PASS] blkdcmp_get_idx_blk_s::100_4_3: (0.00s)
[RUN ] blkdcmp_get_idx_blk_s::10_4_0
[PASS] blkdcmp_get_idx_blk_s::10_4_0: (0.00s)
[RUN ] blkdcmp_get_idx_blk_s::10_4_1
[PASS] blkdcmp_get_idx_blk_s::10_4_1: (0.00s)
[RUN ] blkdcmp_get_idx_blk_s::10_4_2
[PASS] blkdcmp_get_idx_blk_s::10_4_2: (0.00s)
[RUN ] blkdcmp_get_idx_blk_s::10_4_3
[PASS] blkdcmp_get_idx_blk_s::10_4_3: (0.00s)
[====] Synthesis: Tested: 26 | Passing: 26 | Failing: 0 | Crashing: 0
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `BLKDCMP_WITH_ASAN` can be used to enable address sanitizing on the
library `libblkdcmp.so`. `BLKDCMP_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DBLKDCMP_WITH_ASAN=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
