add_executable(tgt_exe_blkdcmp_demo)

set_property(TARGET tgt_exe_blkdcmp_demo PROPERTY OUTPUT_NAME blkdcmp-demo)

target_compile_definitions(
    tgt_exe_blkdcmp_demo
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_exe_blkdcmp_demo
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_blkdcmp_demo
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Release>:-Werror>
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-mtune=native>
        $<$<CONFIG:Release>:-march=native>
)

target_include_directories(
    tgt_exe_blkdcmp_demo
    PRIVATE
        ../../third-party/blkdcmp/include
        ${CMAKE_BINARY_DIR}/include
)

target_link_directories(
    tgt_exe_blkdcmp_demo
    PRIVATE
        ${CMAKE_BINARY_DIR}/third-party/blkdcmp/src/blkdcmp/
)

target_link_libraries(
    tgt_exe_blkdcmp_demo
    PRIVATE
        m
        blkdcmp
)

target_link_options(
    tgt_exe_blkdcmp_demo
    PRIVATE
        -pie
        $<$<BOOL:${SIEVE_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_blkdcmp_demo
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_blkdcmp_demo)
