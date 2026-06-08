add_executable(tgt_exe_gatherv)

set_property(TARGET tgt_exe_gatherv PROPERTY OUTPUT_NAME gatherv)

target_compile_definitions(
    tgt_exe_gatherv
    PRIVATE
        $<$<BOOL:${GATHERV_TRAP_DBG}>:-DGATHERV_TRAP_DBG>
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_exe_gatherv
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_gatherv
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${GATHERV_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_include_directories(
    tgt_exe_gatherv
    PRIVATE
        ../../third_party/libblkdcmp/include
)

target_link_libraries(
    tgt_exe_gatherv
    PRIVATE
        tgt_lib_blkdcmp
        MPI::MPI_C
)

target_link_options(
    tgt_exe_gatherv
    PRIVATE
        -pie
        $<$<BOOL:${GATHERV_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_gatherv
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_gatherv)
