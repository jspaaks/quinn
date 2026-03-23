# exercise 4.12 approximation of pi using simpson's rule

## CMake

The project has been initialized with a [CMakeLists.txt](CMakeLists.txt)-based
configuration for building with CMake:

```console
# change into the build directory
$ mkdir build && cd build/

# generate the build files
$ cmake ..

# build the project
$ cmake --build .

# install the project to <repo>/build/dist
$ cmake --install .
```

Subsequently running the programs should output something like:

```text
$ ./dist/bin/serial 
Area under the curve is: 3.1415926536
$ mpirun -np 1 ./dist/bin/parallel
Area under the curve is: 3.1415926536
$ mpirun -np 4 ./dist/bin/parallel
Area under the curve is: 3.1415926536
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._

[![Copier](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/copier-org/copier/master/img/badge/badge-grayscale-inverted-border-orange.json)](https://github.com/copier-org/copier)
