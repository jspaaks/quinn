add_executable(tgt_exe_floydseq)

set_property(TARGET tgt_exe_floydseq PROPERTY OUTPUT_NAME floydseq)

target_compile_definitions(
    tgt_exe_floydseq
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_exe_floydseq
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_floydseq
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${FLOYDSEQ_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_include_directories(
    tgt_exe_floydseq
    PRIVATE
        ../../third_party/libidx/include
)

target_link_libraries(
    tgt_exe_floydseq
    PRIVATE
        m
        tgt_lib_idx
)

target_link_options(
    tgt_exe_floydseq
    PRIVATE
        -pie
        $<$<BOOL:${FLOYDSEQ_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_floydseq
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_floydseq)
