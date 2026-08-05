# Interpreter contracts targets — factory (cmake/chaos_native_test.cmake) provides
# the runtime lib set, /FORCE:MULTIPLE + /utf-8 flags, include dirs (incl.
# src/native/interpreter/generated and src/native/hot-update), and the codegen-symbol stub.

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/interpreter_smoke.cpp")
    chaos_native_add_test(chaos_interpreter_smoke interpreter_smoke.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/interpreter_new_features_test.cpp")
    chaos_native_add_test(chaos_interpreter_new_features_test interpreter_new_features_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/interpreter_integration.cpp")
    chaos_native_add_test(chaos_interpreter_integration interpreter_integration.cpp method_replacement_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/interpreter_bridge_test.cpp")
    chaos_native_add_test(chaos_interpreter_bridge_test interpreter_bridge_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/interpreter_entry_test.cpp")
    chaos_native_add_test(chaos_interpreter_entry_test interpreter_entry_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/interpreter_profile_test.cpp")
    chaos_native_add_test(chaos_interpreter_profile_test interpreter_profile_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tiering_benchmark.cpp")
    chaos_native_add_test(chaos_tiering_benchmark tiering_benchmark.cpp)
endif()
