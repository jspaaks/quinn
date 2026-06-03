add_executable(tgt_exe_multiply)

set_property(TARGET tgt_exe_multiply PROPERTY OUTPUT_NAME multiply)

target_compile_definitions(
    tgt_exe_multiply
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<BOOL:${MULTIPLY_TRAP_DBG}>:-DMULTIPLY_TRAP_DBG>
)

target_compile_features(
    tgt_exe_multiply
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_multiply
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${MULTIPLY_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_include_directories(
    tgt_exe_multiply
    PRIVATE
        ../../third_party/libblkdcmp/include
        ../../third_party/libcla/include
)

target_link_libraries(
    tgt_exe_multiply
    PRIVATE
        tgt_lib_blkdcmp
        tgt_lib_cla
        MPI::MPI_C
)

target_link_options(
    tgt_exe_multiply
    PRIVATE
        -pie
        $<$<BOOL:${MULTIPLY_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_multiply
    PRIVATE
        main.c
        matrix.c
        vector.c
)

install(TARGETS tgt_exe_multiply)
