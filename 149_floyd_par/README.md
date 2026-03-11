# 149_floyd_par

## CMake

The project has been initialized with a [CMakeLists.txt](CMakeLists.txt)-based
configuration for building with CMake:

```text
$ cd build
$ rm -r ../build/*
$ cmake -DCMAKE_BUILD_TYPE=Release -DFLOYD_PAR_TRAP_DBG=OFF ..
$ cmake --build .
$ cmake --install . --prefix dist
$ mpirun -np 0 ./dist/bin/floyd-par ../data/adjacency6.idx
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `FLOYD_PAR_WITH_ASAN` can be used to enable address sanitizing on the
executable `floyd-par`. `FLOYD_PAR_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DFLOYD_PAR_WITH_ASAN=ON ..
```

## Debugging

The CMake variable `FLOYD_PAR_TRAP_DBG` can be used to trap execution of each spawned process,
such that a debugger may be attached to it. `FLOYD_PAR_TRAP_DBG`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DFLOYD_PAR_TRAP_DBG=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
