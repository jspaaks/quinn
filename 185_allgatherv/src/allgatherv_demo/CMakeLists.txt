add_executable(tgt_exe_allgatherv_demo)

set_property(TARGET tgt_exe_allgatherv_demo PROPERTY OUTPUT_NAME allgatherv-demo)

target_compile_definitions(
    tgt_exe_allgatherv_demo
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<BOOL:${ALLGATHERV_DEMO_TRAP_DBG}>:-DALLGATHERV_DEMO_TRAP_DBG>
)

target_compile_features(
    tgt_exe_allgatherv_demo
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_allgatherv_demo
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${ALLGATHERV_DEMO_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_link_libraries(
    tgt_exe_allgatherv_demo
    PRIVATE
        MPI::MPI_C
)

target_link_options(
    tgt_exe_allgatherv_demo
    PRIVATE
        -pie
        $<$<BOOL:${ALLGATHERV_DEMO_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_allgatherv_demo
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_allgatherv_demo)
