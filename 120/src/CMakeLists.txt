cmake_minimum_required(VERSION 3.23...3.28 FATAL_ERROR)

project(project LANGUAGES C)

include(GenerateExportHeader)
include(GNUInstallDirs)
include(CTest)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "../dist")
endif()

if (APPLE)
    list(APPEND CMAKE_INSTALL_RPATH @loader_path/../lib)
elseif(UNIX)
    list(APPEND CMAKE_INSTALL_RPATH $ORIGIN/../lib)
endif()

if (CMAKE_PROJECT_NAME)
    # I'm the top-most project
    option(BLKDCMP_BUILD_TESTING "Whether to build the libblkdcmp tests" ${BUILD_TESTING})
else()
    # I'm not the top-most project, set default BLKDCMP_BUILD_TESTING OFF
    option(BLKDCMP_BUILD_TESTING "Whether to build the libblkdcmp tests" OFF)
endif()

add_subdirectory(lib/blkdcmp)

if(BLKDCMP_BUILD_TESTING)
    add_subdirectory(bin/test_blkdcmp)
endif()
