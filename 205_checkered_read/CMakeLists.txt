cmake_minimum_required(VERSION 3.23...3.28 FATAL_ERROR)

project(205_checkered_read LANGUAGES C)

find_package(MPI REQUIRED)

option(READ_TRAP_DBG "Whether to enable trapping the debugger when executing 'read'" OFF)
option(READ_WITH_ASAN "Whether to enable address sanitizing for executable 'read'" OFF)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (APPLE)
    list(APPEND CMAKE_INSTALL_RPATH @loader_path/../lib)
elseif(UNIX)
    list(APPEND CMAKE_INSTALL_RPATH $ORIGIN/../lib)
endif()

add_subdirectory(third_party/libblkdcmp)
add_subdirectory(src/read)
