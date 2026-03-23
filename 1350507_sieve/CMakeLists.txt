cmake_minimum_required(VERSION 3.23...3.28 FATAL_ERROR)

project(1350507 LANGUAGES C)

find_package(MPI REQUIRED)

option(SIEVE_TRAP_DBG "Whether to enable trapping each process for debugger attachment" OFF)
option(SIEVE_WITH_ASAN "Whether to enable address sanitizing for executable 'sieve'" OFF)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (APPLE)
    list(APPEND CMAKE_INSTALL_RPATH @loader_path/../lib)
elseif(UNIX)
    list(APPEND CMAKE_INSTALL_RPATH $ORIGIN/../lib)
endif()

add_subdirectory(third-party/blkdcmp)
add_subdirectory(src/blkdcmp-demo)
add_subdirectory(src/sieve)
