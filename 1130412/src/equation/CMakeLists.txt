cmake_minimum_required(VERSION 3.23...3.28)

set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "${CMAKE_BINARY_DIR}/dist")
endif()

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
if (APPLE)
    list(APPEND CMAKE_INSTALL_RPATH @loader_path/../lib)
elseif(UNIX)
    list(APPEND CMAKE_INSTALL_RPATH $ORIGIN/../lib)
endif()

add_library(tgt_lib_equation SHARED)

set_property(TARGET tgt_lib_equation PROPERTY OUTPUT_NAME equation)

target_compile_definitions(
    tgt_lib_equation
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_lib_equation
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_lib_equation
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIC
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Release>:-Werror>
)

target_include_directories(
    tgt_lib_equation
    PRIVATE
        ${PROJECT_ROOT}/include
)

target_link_options(
    tgt_lib_equation
    PRIVATE
)

target_sources(
    tgt_lib_equation
    PRIVATE
        ${PROJECT_ROOT}/src/equation/equation.c
    PUBLIC
        FILE_SET fset_lib_equation_headers
        TYPE HEADERS
        BASE_DIRS
            ${PROJECT_ROOT}/include
        FILES
            ${PROJECT_ROOT}/include/equation/equation.h
)

install(TARGETS tgt_lib_equation
    LIBRARY
    FILE_SET fset_lib_equation_headers
)
