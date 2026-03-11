cmake_minimum_required(VERSION 3.23...3.28 FATAL_ERROR)

project(149_floyd_par LANGUAGES C)

find_package(MPI REQUIRED)

option(FLOYD_PAR_TRAP_DBG "Whether to enable trapping each process for debugger attachment" OFF)
option(FLOYD_PAR_WITH_ASAN "Whether to enable address sanitizing for executable 'floyd-par'" OFF)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (APPLE)
    list(APPEND CMAKE_INSTALL_RPATH @loader_path/../lib)
elseif(UNIX)
    list(APPEND CMAKE_INSTALL_RPATH $ORIGIN/../lib)
endif()

add_subdirectory(third_party/libblkdcmp)
add_subdirectory(third_party/libidx)
add_subdirectory(src/floyd_par)
