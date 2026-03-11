add_executable(tgt_exe_floyd_seq)

set_property(TARGET tgt_exe_floyd_seq PROPERTY OUTPUT_NAME floyd-seq)

target_compile_definitions(
    tgt_exe_floyd_seq
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_exe_floyd_seq
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_floyd_seq
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
    tgt_exe_floyd_seq
    PRIVATE
        ../../third_party/libidx/include
)

target_link_libraries(
    tgt_exe_floyd_seq
    PRIVATE
        m
        tgt_lib_idx
)

target_link_options(
    tgt_exe_floyd_seq
    PRIVATE
        -pie
        $<$<BOOL:${FLOYDSEQ_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_floyd_seq
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_floyd_seq)
