cmake_minimum_required(VERSION 3.15)
project(chaos_l3_verify LANGUAGES CXX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/convert-char/native_test/l2-verify/build")

add_compile_options(/utf-8)

add_executable(l3_verify_convert-char
    "D:/agent/booming-il2cpp/src/native/benchmark-host/l3_verify_main.cpp"
    "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/convert-char/il2cpp_dist/genuine/ConvertCharNativeEntry/generated/native-aot.generated.cpp"
    "D:/agent/booming-il2cpp/src/native/runtime-core/runtime_instantiation.cpp"
)

target_include_directories(l3_verify_convert-char PRIVATE
    "D:/agent/booming-il2cpp/src/native/common"
    "D:/agent/booming-il2cpp/src/native/common/chaos"
    "D:/agent/booming-il2cpp/contracts/native/v0"
    "D:/agent/booming-il2cpp/src/native/runtime-core"
    "D:/agent/booming-il2cpp/src/native/bootstrap"
    "D:/agent/booming-il2cpp/src/native/interpreter"
    "D:/agent/booming-il2cpp/third_party/bdwgc/include"
    "D:/agent/booming-il2cpp/third_party/fmt/include"
    "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/convert-char/native_test/l2-verify/build"
)

target_compile_definitions(l3_verify_convert-char PRIVATE
    CHAOS_IL2CPP_CHECK
    CHAOS_IL2CPP_TRACE_ENABLED
    CHAOS_IL2CPP_VERIFY_MODE
    GC_NOT_DLL
    CHAOS_RUNTIME_ABI_STATIC
)

target_link_libraries(l3_verify_convert-char PRIVATE
    "D:/agent/booming-il2cpp/build/native/src/native/runtime-core/RelWithDebInfo/chaos_runtime_core.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/bootstrap/RelWithDebInfo/chaos_bootstrap.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/interpreter/RelWithDebInfo/chaos_interpreter.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/common/RelWithDebInfo/chaos_common.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/support/RelWithDebInfo/chaos_support.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/hot-update/RelWithDebInfo/chaos_hot_update.lib"
    "D:/agent/booming-il2cpp/build/native/bdwgc_build/RelWithDebInfo/chaos_bdwgc.lib"
    "D:/agent/booming-il2cpp/build/native/fmt_build/RelWithDebInfo/chaos_fmt.lib"
    ole32
    user32
)

target_compile_features(l3_verify_convert-char PUBLIC cxx_std_20)
