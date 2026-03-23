add_executable(tgt_exe_sieve_reference)

set_property(TARGET tgt_exe_sieve_reference PROPERTY OUTPUT_NAME sieve-reference)

target_compile_definitions(
    tgt_exe_sieve_reference
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_exe_sieve_reference
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_sieve_reference
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${SIEVE_REFERENCE_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-mtune=native>
        $<$<CONFIG:Release>:-march=native>
)

target_link_libraries(
    tgt_exe_sieve_reference
    PRIVATE
        m
        MPI::MPI_C
)

target_link_options(
    tgt_exe_sieve_reference
    PRIVATE
        -pie
        $<$<BOOL:${SIEVE_REFERENCE_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_sieve_reference
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_sieve_reference)
