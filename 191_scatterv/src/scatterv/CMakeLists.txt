add_executable(tgt_exe_scatterv)

set_property(TARGET tgt_exe_scatterv PROPERTY OUTPUT_NAME scatterv)

target_compile_definitions(
    tgt_exe_scatterv
    PRIVATE
        $<$<BOOL:${SCATTERV_TRAP_DBG}>:-DSCATTERV_TRAP_DBG>
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_exe_scatterv
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_scatterv
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${SCATTERV_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_include_directories(
    tgt_exe_scatterv
    PRIVATE
        ../../third_party/libblkdcmp/include
)

target_link_libraries(
    tgt_exe_scatterv
    PRIVATE
        tgt_lib_blkdcmp
        MPI::MPI_C
)

target_link_options(
    tgt_exe_scatterv
    PRIVATE
        -pie
        $<$<BOOL:${SCATTERV_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_scatterv
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_scatterv)
