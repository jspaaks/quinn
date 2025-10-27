add_executable(tgt_exe_reference)

set_property(TARGET tgt_exe_reference PROPERTY OUTPUT_NAME reference)

target_compile_definitions(
    tgt_exe_reference
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_exe_reference
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_reference
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${REFERENCE_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_link_libraries(
    tgt_exe_reference
    PRIVATE
        m
        MPI::MPI_C
)

target_link_options(
    tgt_exe_reference
    PRIVATE
        -pie
        $<$<BOOL:${REFERENCE_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_reference
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_reference)
