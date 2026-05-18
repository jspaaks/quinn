cmake_minimum_required(VERSION 3.23...3.28 FATAL_ERROR)

project(185_allgatherv LANGUAGES C)

find_package(MPI REQUIRED)

option(ALLGATHERV_DEMO_TRAP_DBG "Whether to enable trapping execution of the program at the start in order to facilitate attaching a debugger" OFF)
option(ALLGATHERV_DEMO_WITH_ASAN "Whether to enable address sanitizing for executable 'allgatherv-demo'" OFF)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (APPLE)
    list(APPEND CMAKE_INSTALL_RPATH @loader_path/../lib)
elseif(UNIX)
    list(APPEND CMAKE_INSTALL_RPATH $ORIGIN/../lib)
endif()

add_subdirectory(src/allgatherv_demo)
