add_executable(tgt_exe_read)

set_property(TARGET tgt_exe_read PROPERTY OUTPUT_NAME read)

target_compile_definitions(
    tgt_exe_read
    PRIVATE
        $<$<BOOL:${READ_TRAP_DBG}>:-DREAD_TRAP_DBG>
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_exe_read
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_read
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${READ_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_include_directories(
    tgt_exe_read
    PRIVATE
        ../../third_party/libblkdcmp/include
)

target_link_libraries(
    tgt_exe_read
    PRIVATE
        MPI::MPI_C
        tgt_lib_blkdcmp
)

target_link_options(
    tgt_exe_read
    PRIVATE
        -pie
        $<$<BOOL:${READ_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_read
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_read)
