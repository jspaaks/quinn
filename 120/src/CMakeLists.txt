cmake_minimum_required(VERSION 3.23)

project(project LANGUAGES C)
include(CTest)

set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "${CMAKE_BINARY_DIR}/../dist")
endif()

if(BUILD_TESTING)
    add_subdirectory(${PROJECT_ROOT}/src/bin/test_blkdecomp)
endif()
add_subdirectory(${PROJECT_ROOT}/src/lib/blkdecomp)
