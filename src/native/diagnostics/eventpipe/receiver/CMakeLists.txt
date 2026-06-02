# CMakeLists.txt — chaos-diag EventPipe receiver CLI
#
# Standalone executable that connects to a running Chaos IL2CPP runtime
# via named pipe (Windows) or Unix domain socket (Linux) and displays
# diagnostic events in real-time.

cmake_minimum_required(VERSION 3.20)
project(chaos_diag LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)

# ── Executable ───────────────────────────────────────────────────────────
get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)

if(WIN32)
    add_executable(chaos_diag chaos_diag_win32.cpp)
    target_link_libraries(chaos_diag PRIVATE kernel32.lib)
else()
    add_executable(chaos_diag chaos_diag_posix.cpp)
endif()

target_include_directories(chaos_diag PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.."          # ep_receiver.h
    "${REPO_ROOT}/src/native/common"          # chaos/native_types.h
)

target_compile_options(chaos_diag PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /EHa- /GS- /W3>)

target_compile_features(chaos_diag PUBLIC cxx_std_17)
