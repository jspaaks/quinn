add_library(tgt_lib_blkdcmp SHARED)

set_property(TARGET tgt_lib_blkdcmp PROPERTY C_VISIBILITY_PRESET hidden)
set_property(TARGET tgt_lib_blkdcmp PROPERTY OUTPUT_NAME blkdcmp)
set_property(TARGET tgt_lib_blkdcmp PROPERTY VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(
    tgt_lib_blkdcmp
    BASE_NAME blkdcmp
    EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/blkdcmp/export.h
)

target_compile_definitions(
    tgt_lib_blkdcmp
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_lib_blkdcmp
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_lib_blkdcmp
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIC
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${BLKDCMP_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_include_directories(
    tgt_lib_blkdcmp
    PRIVATE
        ../../include
)

target_link_options(
    tgt_lib_blkdcmp
    PRIVATE
        $<$<BOOL:${BLKDCMP_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_lib_blkdcmp
    PRIVATE
        blkdcmp.c
    PUBLIC
        FILE_SET fset_lib_blkdcmp_headers
            TYPE HEADERS
            BASE_DIRS
                ../../include
                ${CMAKE_BINARY_DIR}/include  # contains cmake-generated file
            FILES
                ../../include/blkdcmp/blkdcmp.h
                ${CMAKE_BINARY_DIR}/include/blkdcmp/export.h  # cmake-generated file
)

install(TARGETS tgt_lib_blkdcmp
    LIBRARY
    FILE_SET fset_lib_blkdcmp_headers
)
