add_executable(tgt_exe_shade_par)

set_property(TARGET tgt_exe_shade_par PROPERTY OUTPUT_NAME shade-par)

target_compile_definitions(
    tgt_exe_shade_par
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<BOOL:${SHADE_PAR_TRAP_DBG}>:-DSHADE_PAR_TRAP_DBG>
)

target_compile_features(
    tgt_exe_shade_par
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_shade_par
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${SHADE_PAR_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_include_directories(
    tgt_exe_shade_par
    PRIVATE
        ../../third_party/libblkdcmp/include
        ../../third_party/libcla/include
        ../../third_party/libidx/include
)

target_link_libraries(
    tgt_exe_shade_par
    PRIVATE
        tgt_lib_blkdcmp
        tgt_lib_cla
        tgt_lib_idx
        MPI::MPI_C
)

target_link_options(
    tgt_exe_shade_par
    PRIVATE
        -pie
        $<$<BOOL:${SHADE_PAR_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_shade_par
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_shade_par)
