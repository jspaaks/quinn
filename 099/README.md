# circuit satisfiability

## CMake

The project has been initialized with a [CMakeLists.txt](CMakeLists.txt)-based
configuration for building with CMake:

```console
# change into the build directory
$ cd build/

# generate the build files
$ cmake ..

# build the project
$ cmake --build .

# install the project to <repo>/build/dist
$ cmake --install .

# run the program to see if it works
$ ./dist/bin/serial

# run the mpi program as follows (-np 0 means use all cores)
$ mpirun -np 0 ./dist/bin/parallel
```

Should output something like:                                                                                                               

```text
$ ./dist/bin/serial
serial version:
0) 1010 1111 1001 1001
0) 0110 1111 1001 1001
0) 1110 1111 1001 1001
0) 1010 1111 1101 1001
0) 0110 1111 1101 1001
0) 1110 1111 1101 1001
0) 1010 1111 1011 1001
0) 0110 1111 1011 1001
0) 1110 1111 1011 1001
```

```
$ mpirun -np 2 ./dist/bin/parallel
parallel version:
1) 1010 1111 1001 1001
1) 1110 1111 1001 1001
1) 1010 1111 1101 1001
1) 1110 1111 1101 1001
1) 1010 1111 1011 1001
1) 1110 1111 1011 1001
0) 0110 1111 1001 1001
0) 0110 1111 1101 1001
0) 0110 1111 1011 1001
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._

[![Copier](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/copier-org/copier/master/img/badge/badge-grayscale-inverted-border-orange.json)](https://github.com/copier-org/copier)
