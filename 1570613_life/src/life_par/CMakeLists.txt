add_executable(tgt_exe_life_par)

set_property(TARGET tgt_exe_life_par PROPERTY OUTPUT_NAME life_par)

target_compile_definitions(
    tgt_exe_life_par
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<BOOL:${LIFE_PAR_TRAP_DBG}>:-DLIFE_PAR_TRAP_DBG>
)

target_compile_features(
    tgt_exe_life_par
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_life_par
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${LIFE_PAR_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_include_directories(
    tgt_exe_life_par
    PRIVATE
        ../../third_party/libblkdcmp/include
        ../../third_party/libcla/include
)

target_link_libraries(
    tgt_exe_life_par
    PRIVATE
        tgt_lib_blkdcmp
        tgt_lib_cla
        MPI::MPI_C
)

target_link_options(
    tgt_exe_life_par
    PRIVATE
        -pie
        $<$<BOOL:${LIFE_PAR_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_life_par
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_life_par)
