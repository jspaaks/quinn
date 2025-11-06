cmake_minimum_required(VERSION 3.23...3.28 FATAL_ERROR)

project(139 LANGUAGES C)

option(SEQUENTIAL_WITH_ASAN "Whether to enable address sanitizing for executable 'sequential'" OFF)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (APPLE)
    list(APPEND CMAKE_INSTALL_RPATH @loader_path/../lib)
elseif(UNIX)
    list(APPEND CMAKE_INSTALL_RPATH $ORIGIN/../lib)
endif()

add_subdirectory(src/sequential)
add_subdirectory(third_party/libidx)

