cmake_minimum_required(VERSION 3.15)
project(chaos_l3_verify LANGUAGES CXX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/reflection-binding/native_test/l2-verify/build")

add_executable(l3_verify_reflection-binding
    "D:/agent/booming-il2cpp/src/native/benchmark-host/l3_verify_main.cpp"
    "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/reflection-binding/il2cpp_dist/genuine/generated/native-aot.generated.cpp"
)

target_include_directories(l3_verify_reflection-binding PRIVATE
    "D:/agent/booming-il2cpp/src/native/common"
    "D:/agent/booming-il2cpp/src/native/common/chaos"
    "D:/agent/booming-il2cpp/contracts/native/v0"
    "D:/agent/booming-il2cpp/src/native/runtime-core"
    "D:/agent/booming-il2cpp/src/native/bootstrap"
    "D:/agent/booming-il2cpp/third_party/fmt/include"
    "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/reflection-binding/native_test/l2-verify/build"
)

target_compile_definitions(l3_verify_reflection-binding PRIVATE
    CHAOS_IL2CPP_CHECK
    CHAOS_IL2CPP_TRACE_ENABLED
    CHAOS_IL2CPP_VERIFY_MODE
    GC_NOT_DLL
)

target_link_libraries(l3_verify_reflection-binding PRIVATE
    "D:/agent/booming-il2cpp/build/native-runtime/Release/chaos_runtime_core.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/bootstrap/Release/chaos_bootstrap.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/interpreter/Release/chaos_interpreter.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/common/Release/chaos_common.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/support/Release/chaos_support.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/hot-update/Release/chaos_hot_update.lib"
    "D:/agent/booming-il2cpp/build/native/bdwgc_build/Release/chaos_bdwgc.lib"
    ole32
    user32
)

target_compile_features(l3_verify_reflection-binding PUBLIC cxx_std_20)
