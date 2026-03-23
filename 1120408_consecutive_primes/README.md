# exercise 4.8 adjacent primes

## Requirements

```
# install openmpi, cmake, and some compilation tools from ubuntu repositories
$ sudo apt install libopenmpi3t64  \
                   libopenmpi-dev  \
                   cmake           \
                   build-essential
```

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
```

Subsequently running the programs should output something like:

```text
$ bash ../comparison.sh
+ let val=1000000
+ ./dist/bin/serial 1000000
Found 8170 occurrences of adjacent primes in range [2..1000000].

real    1m32.203s
user    1m32.198s
sys     0m0.001s
+ mpirun -np 0 ./dist/bin/parallel1 1000000
(2) range [500003,750003)
(3) range [750003,1000001)
(0) range [3,250003)
(1) range [250003,500003)
(0) found 2588 occurrences of adjacent primes in range [3,250003)
(1) found 1977 occurrences of adjacent primes in range [250003,500003)
(2) found 1849 occurrences of adjacent primes in range [500003,750003)
(3) found 1755 occurrences of adjacent primes in range [750003,1000001)
(0) there are 8170 adjacent primes in range [2,1000000]

real    1m29.902s
user    5m33.940s
sys     0m0.262s
+ mpirun -np 0 ./dist/bin/parallel2 1000000
(3) found 2036 occurrences of adjacent primes
(0) found 2064 occurrences of adjacent primes
(2) found 2059 occurrences of adjacent primes
(1) found 2010 occurrences of adjacent primes
(0) there are 8170 adjacent primes in range [2,1000000]

real    1m32.159s
user    6m5.881s
sys     0m0.235s
```

Not sure what's going on with the timing. parallel1 could be improved by calculating the primes in
blocks, because higher primes are more expensive to evaluate. parallel2 could be improved by
avoiding calculating a prime and its neighbor and instead store one of the primes so you can use it
for the next evaluation.

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._

[![Copier](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/copier-org/copier/master/img/badge/badge-grayscale-inverted-border-orange.json)](https://github.com/copier-org/copier)
