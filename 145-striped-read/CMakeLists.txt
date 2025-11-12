cmake_minimum_required(VERSION 3.23...3.28 FATAL_ERROR)

project(145_striped_read LANGUAGES C)

find_package(MPI REQUIRED)

option(STRIPED_READ_TRAP_DBG "Whether to enable trapping each process for debugger attachment" OFF)
option(STRIPED_READ_WITH_ASAN "Whether to enable address sanitizing for executable 'striped-read'" OFF)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (APPLE)
    list(APPEND CMAKE_INSTALL_RPATH @loader_path/../lib)
elseif(UNIX)
    list(APPEND CMAKE_INSTALL_RPATH $ORIGIN/../lib)
endif()

add_subdirectory(third-party/blkdcmp)
add_subdirectory(third-party/idx)
add_subdirectory(src/striped-read)
