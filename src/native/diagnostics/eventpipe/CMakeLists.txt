# CMakeLists.txt — EventPipe diagnostic event transport (chaos_eventpipe)
#
# Builds a static library providing lightweight IPC-based diagnostic events
# (GC, ThreadPool, Exception).  Uses Windows Named Pipes for transport.
#
# This is a platform-specific module gated on _WIN32.  On unsupported
# platforms, add_subdirectory is skipped (see root CMakeLists.txt).

cmake_minimum_required(VERSION 3.20)
project(chaos_eventpipe LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)

# ── Static library ───────────────────────────────────────────────────────
add_library(chaos_eventpipe STATIC
    ep_config.h
    ep_protocol.h
    ep_platform.h
    ep_transport.h
    ep_transport.cpp
    ep_event.h
    ep_event.cpp
    ep_gc_bridge.h
    ep_gc_bridge.cpp
    ep_threadpool_bridge.h
    ep_threadpool_bridge.cpp
    ep_exception_bridge.h
    ep_exception_bridge.cpp
    ep_hotupdate_bridge.h
    ep_hotupdate_bridge.cpp
    ep_reflection_bridge.h
    ep_reflection_bridge.cpp
    ep_memorydomain_bridge.h
    ep_memorydomain_bridge.cpp
    ep_instance.h
    ep_instance.cpp
)

target_include_directories(chaos_eventpipe PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/.."          # eventpipe/... from diagnostics root
    "${REPO_ROOT}/src/native/runtime-core"    # gc/gc_events.h
    "${REPO_ROOT}/src/native/common"          # chaos/native_types.h
)

target_compile_options(chaos_eventpipe PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /EHa- /GS- /W3>)

target_compile_features(chaos_eventpipe PUBLIC cxx_std_17)

# ── Receiver CLI (separate executable) ───────────────────────────────────
add_subdirectory(receiver)
