cmake_minimum_required(VERSION 3.20)
project(test_crash CXX)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(/utf-8)
set(CHAOS_PROJECT_ROOT "D:/agent/booming-il2cpp")
set(CHAOS_NATIVE_BUILD "D:/agent/booming-il2cpp/build/native")
set(CHAOS_ENTRY_INCLUDES
    "${CHAOS_PROJECT_ROOT}/src/native/common"
    "${CHAOS_PROJECT_ROOT}/src/native/runtime-core"
    "${CHAOS_PROJECT_ROOT}/src/native"
    "${CHAOS_PROJECT_ROOT}/contracts/native/v0"
    "${CHAOS_PROJECT_ROOT}/third_party/fmt/include"
)
set(CHAOS_LIB_DIRS
    "${CHAOS_NATIVE_BUILD}/src/native/runtime-core/RelWithDebInfo"
    "${CHAOS_NATIVE_BUILD}/src/native/bootstrap/RelWithDebInfo"
    "${CHAOS_NATIVE_BUILD}/src/native/common/RelWithDebInfo"
    "${CHAOS_NATIVE_BUILD}/src/native/interpreter/RelWithDebInfo"
    "${CHAOS_NATIVE_BUILD}/src/native/support/RelWithDebInfo"
    "${CHAOS_NATIVE_BUILD}/src/native/hot-update/RelWithDebInfo"
    "${CHAOS_NATIVE_BUILD}/bdwgc_build/RelWithDebInfo"
    "${CHAOS_NATIVE_BUILD}/fmt_build/RelWithDebInfo"
)
set(CHAOS_RUNTIME_LIBS
    chaos_runtime_core chaos_bootstrap chaos_common
    chaos_interpreter chaos_support chaos_hot_update
    chaos_bdwgc chaos_fmt
)

file(GLOB CHAOS_CODEGEN_CPP "${CHAOS_PROJECT_ROOT}/verification/foundation-dll/System.Private.CoreLib/convert-char/codegen/*/generated/native-aot.generated.cpp")
add_executable(test_bridge test_bridge.cpp ${CHAOS_CODEGEN_CPP})
target_include_directories(test_bridge PRIVATE ${CHAOS_ENTRY_INCLUDES})
target_link_directories(test_bridge PRIVATE ${CHAOS_LIB_DIRS})
target_compile_options(test_bridge PRIVATE /EHa)
target_link_libraries(test_bridge PRIVATE ${CHAOS_RUNTIME_LIBS})