add_executable(tgt_exe_sieve)

set_property(TARGET tgt_exe_sieve PROPERTY OUTPUT_NAME sieve)

target_compile_definitions(
    tgt_exe_sieve
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<BOOL:${SIEVE_TRAP_DBG}>:-DSIEVE_TRAP_DBG>
)

target_compile_features(
    tgt_exe_sieve
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_sieve
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${SIEVE_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-mtune=native>
        $<$<CONFIG:Release>:-march=native>
)

target_include_directories(
    tgt_exe_sieve
    PRIVATE
        ../../third-party/blkdcmp/include
        ${CMAKE_BINARY_DIR}/include
)

target_link_directories(
    tgt_exe_sieve
    PRIVATE
        ${CMAKE_BINARY_DIR}/third-party/blkdcmp/src/blkdcmp/
)

target_link_libraries(
    tgt_exe_sieve
    PRIVATE
        m
        blkdcmp
        MPI::MPI_C
)

target_link_options(
    tgt_exe_sieve
    PRIVATE
        -pie
        $<$<BOOL:${SIEVE_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_sieve
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_sieve)
