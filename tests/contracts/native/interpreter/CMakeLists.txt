# chaos_interpreter.lib and chaos_runtime_core.lib both carry compiled copies of
# instantiation_engine symbols; linking them together (with chaos_codegen) otherwise
# fails with LNK2005. /FORCE:MULTIPLE tolerates the duplicates like add_chaos_test.
if(MSVC)
    add_link_options(/FORCE:MULTIPLE)
endif()

add_executable(chaos_interpreter_smoke
    interpreter_smoke.cpp)

target_compile_features(chaos_interpreter_smoke PRIVATE cxx_std_17)
target_link_libraries(
    chaos_interpreter_smoke
    PRIVATE
        chaos_interpreter
        chaos_codegen)

add_executable(chaos_interpreter_new_features_test
    interpreter_new_features_test.cpp)

target_compile_features(chaos_interpreter_new_features_test PRIVATE cxx_std_17)
target_link_libraries(
    chaos_interpreter_new_features_test
    PRIVATE
        chaos_interpreter
        chaos_runtime_core
        chaos_codegen)

add_executable(chaos_interpreter_integration
    interpreter_integration.cpp
    method_replacement_test.cpp)

target_compile_features(chaos_interpreter_integration PRIVATE cxx_std_17)
target_compile_options(chaos_interpreter_integration PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_include_directories(chaos_interpreter_integration PRIVATE
    ${CMAKE_SOURCE_DIR}/src/native/hot-update)
target_link_libraries(
    chaos_interpreter_integration
    PRIVATE
        chaos_interpreter
        chaos_bootstrap
        chaos_fmt
        chaos_common
        chaos_support
        chaos_hot_update
        chaos_codegen)

add_executable(chaos_interpreter_bridge_test
    interpreter_bridge_test.cpp)

target_compile_features(chaos_interpreter_bridge_test PRIVATE cxx_std_17)
if(MSVC)
    target_compile_options(chaos_interpreter_bridge_test PRIVATE /utf-8)
endif()
target_link_libraries(
    chaos_interpreter_bridge_test
    PRIVATE
        chaos_interpreter
        chaos_runtime_core
        chaos_bootstrap
        chaos_codegen
        chaos_common
        chaos_support
        chaos_fmt)

add_executable(chaos_interpreter_entry_test
    interpreter_entry_test.cpp)

target_compile_features(chaos_interpreter_entry_test PRIVATE cxx_std_17)
target_include_directories(chaos_interpreter_entry_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/native/interpreter/generated)
target_link_libraries(
    chaos_interpreter_entry_test
    PRIVATE
        chaos_runtime_core
        chaos_interpreter
        chaos_bootstrap
        chaos_fmt
        chaos_common
        chaos_support
        chaos_codegen)

add_executable(chaos_interpreter_profile_test
    interpreter_profile_test.cpp)

target_compile_features(chaos_interpreter_profile_test PRIVATE cxx_std_17)
target_include_directories(chaos_interpreter_profile_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src/native/interpreter/generated)
target_link_libraries(
    chaos_interpreter_profile_test
    PRIVATE
        chaos_runtime_core
        chaos_interpreter
        chaos_bootstrap
        chaos_fmt
        chaos_common
        chaos_support
        chaos_codegen)

add_executable(chaos_tiering_benchmark
    tiering_benchmark.cpp)

target_compile_features(chaos_tiering_benchmark PRIVATE cxx_std_17)
target_include_directories(chaos_tiering_benchmark PRIVATE
    ${CMAKE_SOURCE_DIR}/src/native/interpreter/generated)
target_link_libraries(
    chaos_tiering_benchmark
    PRIVATE
        chaos_runtime_core
        chaos_interpreter
        chaos_bootstrap
        chaos_fmt
        chaos_common
        chaos_support
        chaos_codegen)
