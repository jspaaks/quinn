add_executable(tgt_exe_dims_create)

set_property(TARGET tgt_exe_dims_create PROPERTY OUTPUT_NAME dims_create)

target_compile_definitions(
    tgt_exe_dims_create
    PRIVATE
        $<$<BOOL:${DIMS_CREATE_TRAP_DBG}>:-DDIMS_CREATE_TRAP_DBG>
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_exe_dims_create
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_dims_create
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${DIMS_CREATE_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_link_libraries(
    tgt_exe_dims_create
    PRIVATE
        MPI::MPI_C
)

target_link_options(
    tgt_exe_dims_create
    PRIVATE
        -pie
        $<$<BOOL:${DIMS_CREATE_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_dims_create
    PRIVATE
        main.c
)

install(TARGETS tgt_exe_dims_create)
