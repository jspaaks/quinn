add_executable(tgt_exe_floyd_par)

set_property(TARGET tgt_exe_floyd_par PROPERTY OUTPUT_NAME floyd-par)

target_compile_definitions(
    tgt_exe_floyd_par
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<BOOL:${STRIPED_READ_TRAP_DBG}>:-DSTRIPED_READ_TRAP_DBG>
)

target_compile_features(
    tgt_exe_floyd_par
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_floyd_par
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${STRIPED_READ_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-mtune=native>
        $<$<CONFIG:Release>:-march=native>
)

target_link_libraries(
    tgt_exe_floyd_par
    PRIVATE
        tgt_lib_blkdcmp
        tgt_lib_idx
        MPI::MPI_C
)

target_link_options(
    tgt_exe_floyd_par
    PRIVATE
        -pie
        $<$<BOOL:${STRIPED_READ_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_floyd_par
    PRIVATE
        main.c
        stripe.c
)

install(TARGETS tgt_exe_floyd_par)
