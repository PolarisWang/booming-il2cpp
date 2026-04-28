get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_common STATIC
    chaos/common.h
    chaos/checked_conv.h
    chaos/arithmetic.h
    chaos/load_store.h
    chaos/finally.h
    chaos/ptr_tag.h
    chaos/status.h
    chaos/static_init.h
    chaos/span.h
    chaos/collection.h
    chaos/monitor.h
    chaos/thread.h
    chaos/async.h
    span.cpp
    collection.cpp
    monitor.cpp
    thread.cpp
    async.cpp)

target_compile_features(chaos_common PUBLIC cxx_std_17)

# chaos_common is a dependency-free foundation library.
# It does NOT depend on chaos_runtime_core, BDWGC, or any other module.
# All types are self-contained; opaque handles use std::intptr_t.

target_include_directories(
    chaos_common
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}")
