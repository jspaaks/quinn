cmake_minimum_required(VERSION 3.23...3.28 FATAL_ERROR)

project(124 LANGUAGES C)

find_package(MPI REQUIRED)

option(REFERENCE_WITH_ASAN "Whether to enable address sanitizing for executable 'reference'" OFF)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (APPLE)
    list(APPEND CMAKE_INSTALL_RPATH @loader_path/../lib)
elseif(UNIX)
    list(APPEND CMAKE_INSTALL_RPATH $ORIGIN/../lib)
endif()

add_subdirectory(third-party/blkdcmp)
add_subdirectory(src/reference)
add_subdirectory(src/refactored)

