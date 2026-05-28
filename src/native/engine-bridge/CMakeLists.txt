get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_engine_bridge STATIC
    engine_bridge.cpp
    engine_bridge.h)

target_compile_features(chaos_engine_bridge PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(chaos_engine_bridge PRIVATE /utf-8)
endif()
target_include_directories(
    chaos_engine_bridge
    PUBLIC
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/contracts/engine/v0"
        "${CMAKE_CURRENT_LIST_DIR}"
    PRIVATE
        "${REPO_ROOT}/src/native/runtime-core"
        "${REPO_ROOT}/src/native/bootstrap")
target_link_libraries(
    chaos_engine_bridge
    PUBLIC
        chaos_runtime_core)
