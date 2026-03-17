add_executable(tgt_exe_pingpong)

set_property(TARGET tgt_exe_pingpong PROPERTY OUTPUT_NAME pingpong)

target_compile_definitions(
    tgt_exe_pingpong
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<BOOL:${PINGPONG_TRAP_DBG}>:-DPINGPONG_TRAP_DBG>
)

target_compile_features(
    tgt_exe_pingpong
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_pingpong
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${PINGPONG_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_link_libraries(
    tgt_exe_pingpong
    PRIVATE
        MPI::MPI_C
)

target_link_options(
    tgt_exe_pingpong
    PRIVATE
        -pie
        $<$<BOOL:${PINGPONG_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_pingpong
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_pingpong)
