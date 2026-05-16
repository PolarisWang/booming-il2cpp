get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_common STATIC
    chaos/common.h
    chaos/native_types.h
    chaos/checked_conv.h
    chaos/arithmetic.h
    chaos/config.h
    chaos/finally.h
    chaos/format.h
    chaos/json_reader.h
    chaos/load_store.h
    chaos/log.h
    chaos/monitor.h
    chaos/profile.h
    chaos/ptr_tag.h
    chaos/span.h
    chaos/static_init.h
    chaos/status.h
    chaos/thread.h
    chaos/trace.h
    chaos/type_info.h
    chaos/profile_globals.cpp
    chaos/collection.h
    chaos/unordered_dense.h
    chaos/async.h
    span.cpp
    collection.cpp
    monitor.cpp
    thread.cpp
    async.cpp)

target_compile_features(chaos_common PUBLIC cxx_std_17)

# chaos_common uses fmtlib for type-safe string formatting.
# Also exposes ankerl::unordered_dense include path for opt-in use.
target_link_libraries(chaos_common PUBLIC chaos_fmt chaos_unordered_dense)

# chaos_common is a dependency-free foundation library.
# It does NOT depend on chaos_runtime_core, BDWGC, or any other module.
# All types are self-contained; opaque handles use CHAOS_IL2CPP_INTPTR.

target_include_directories(
    chaos_common
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}")
