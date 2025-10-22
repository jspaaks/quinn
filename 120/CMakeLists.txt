cmake_minimum_required(VERSION 3.23...3.28 FATAL_ERROR)

project(120 LANGUAGES C)

option(BLKDCMP_WITH_ASAN "Whether to enable address sanitizing for library 'libblkdcmp.so'" OFF)

include(CTest)
include(GenerateExportHeader)
include(GNUInstallDirs)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (APPLE)
    list(APPEND CMAKE_INSTALL_RPATH @loader_path/../lib)
elseif(UNIX)
    list(APPEND CMAKE_INSTALL_RPATH $ORIGIN/../lib)
endif()

if (CMAKE_PROJECT_NAME)
    # I'm the top-most project, set BLKDCMP_BUILD_TESTING like BUILD_TESTING by default
    option(BLKDCMP_BUILD_TESTING "Whether to build the libblkdcmp tests" ${BUILD_TESTING})
else()
    # I'm not the top-most project, set BLKDCMP_BUILD_TESTING default OFF
    option(BLKDCMP_BUILD_TESTING "Whether to build the libblkdcmp tests" OFF)
endif()

add_subdirectory(src/blkdcmp)
if (BLKDCMP_BUILD_TESTING)
    add_subdirectory(test/blkdcmp)
endif()
