cmake_minimum_required(VERSION 3.23...3.28 FATAL_ERROR)

project(139_floyd_seq LANGUAGES C)

option(FLOYD_SEQ_WITH_ASAN "Whether to enable address sanitizing for executable 'floyd-seq'" OFF)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (APPLE)
    list(APPEND CMAKE_INSTALL_RPATH @loader_path/../lib)
elseif(UNIX)
    list(APPEND CMAKE_INSTALL_RPATH $ORIGIN/../lib)
endif()

add_subdirectory(third_party/libidx)
add_subdirectory(src/floyd_seq)
