add_executable(tgt_exe_bcast)

set_property(TARGET tgt_exe_bcast PROPERTY OUTPUT_NAME bcast)

target_compile_definitions(
    tgt_exe_bcast
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<BOOL:${BCAST_TRAP_DBG}>:-DBCAST_TRAP_DBG>
)

target_compile_features(
    tgt_exe_bcast
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_bcast
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${BCAST_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_include_directories(
    tgt_exe_bcast
    PRIVATE
)

target_link_libraries(
    tgt_exe_bcast
    PRIVATE
        MPI::MPI_C
)

target_link_options(
    tgt_exe_bcast
    PRIVATE
        -pie
        $<$<BOOL:${BCAST_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_bcast
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_bcast)
