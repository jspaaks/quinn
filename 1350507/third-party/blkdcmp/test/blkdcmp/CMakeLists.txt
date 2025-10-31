add_executable(tgt_exe_test_blkdcmp)

set_property(TARGET tgt_exe_test_blkdcmp PROPERTY OUTPUT_NAME test_blkdcmp)

target_compile_definitions(
    tgt_exe_test_blkdcmp
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_exe_test_blkdcmp
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_test_blkdcmp
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Release>:-Werror>
)

target_include_directories(
    tgt_exe_test_blkdcmp
    PRIVATE
        ../include
)

target_link_libraries(
    tgt_exe_test_blkdcmp
    PRIVATE
        criterion
        tgt_lib_blkdcmp
)

target_sources(
    tgt_exe_test_blkdcmp
    PRIVATE
        test_blkdcmp_get_blk_owner.c
        test_blkdcmp_get_blk_sz.c
        test_blkdcmp_get_idx_blk_e.c
        test_blkdcmp_get_idx_blk_s.c
)

add_test(
    NAME
        test_blkdcmp
    COMMAND
        ${CMAKE_INSTALL_BINDIR}/test_blkdcmp
)

install(TARGETS tgt_exe_test_blkdcmp)
