add_executable(ir_precompile
    main.cpp)

target_compile_features(ir_precompile PRIVATE cxx_std_17)
if(MSVC)
    target_compile_options(ir_precompile PRIVATE /utf-8)
endif()

target_link_libraries(ir_precompile
    PRIVATE
        chaos_interpreter
        chaos_runtime_core
        chaos_bootstrap
        chaos_common)

target_include_directories(ir_precompile PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}")