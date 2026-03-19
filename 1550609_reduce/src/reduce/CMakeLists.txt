add_executable(tgt_exe_reduce)

set_property(TARGET tgt_exe_reduce PROPERTY OUTPUT_NAME reduce)

target_compile_definitions(
    tgt_exe_reduce
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<BOOL:${REDUCE_TRAP_DBG}>:-DREDUCE_TRAP_DBG>
)

target_compile_features(
    tgt_exe_reduce
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_reduce
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${REDUCE_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_include_directories(
    tgt_exe_reduce
    PRIVATE
        ../../third_party/libblkdcmp/include
)

target_link_libraries(
    tgt_exe_reduce
    PRIVATE
        tgt_lib_blkdcmp
        MPI::MPI_C
)

target_link_options(
    tgt_exe_reduce
    PRIVATE
        -pie
        $<$<BOOL:${REDUCE_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_reduce
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_reduce)
