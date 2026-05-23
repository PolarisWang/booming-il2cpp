# CMakeLists.txt — chaos-diag EventPipe receiver CLI
#
# Standalone executable that connects to a running Chaos IL2CPP runtime
# via named pipe and displays diagnostic events in real-time.

cmake_minimum_required(VERSION 3.20)
project(chaos_diag LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)

# ── Executable ───────────────────────────────────────────────────────────
add_executable(chaos_diag chaos_diag.cpp)

target_include_directories(chaos_diag PRIVATE
    "${REPO_ROOT}/src/native/common"      # chaos/native_types.h
)

target_compile_options(chaos_diag PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /EHa- /GS- /W3>)

target_link_libraries(chaos_diag PRIVATE
    kernel32.lib
)
