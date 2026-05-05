add_executable(chaos_interpreter_smoke
    interpreter_smoke.cpp)

target_compile_features(chaos_interpreter_smoke PRIVATE cxx_std_17)
target_link_libraries(
    chaos_interpreter_smoke
    PRIVATE
        chaos_interpreter)

add_executable(chaos_interpreter_integration
    interpreter_integration.cpp)

target_compile_features(chaos_interpreter_integration PRIVATE cxx_std_17)
target_link_libraries(
    chaos_interpreter_integration
    PRIVATE
        chaos_interpreter
        chaos_bootstrap)

add_executable(chaos_interpreter_bridge_test
    interpreter_bridge_test.cpp)

target_compile_features(chaos_interpreter_bridge_test PRIVATE cxx_std_17)
target_link_libraries(
    chaos_interpreter_bridge_test
    PRIVATE
        chaos_interpreter
        chaos_bootstrap)
