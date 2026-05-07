cmake_minimum_required(VERSION 3.15)
project(chaos_l3_verify LANGUAGES CXX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/io-streams-basics/native_test/l2-verify/build")

add_compile_options(/utf-8)

add_executable(l3_verify_io-streams-basics
    "D:/agent/booming-il2cpp/src/native/benchmark-host/l3_verify_main.cpp"
    "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/io-streams-basics/il2cpp_dist/genuine/generated/native-aot.generated.cpp"
    "D:/agent/booming-il2cpp/src/native/runtime-core/runtime_instantiation.cpp"
)

target_include_directories(l3_verify_io-streams-basics PRIVATE
    "D:/agent/booming-il2cpp/src/native/common"
    "D:/agent/booming-il2cpp/src/native/common/chaos"
    "D:/agent/booming-il2cpp/contracts/native/v0"
    "D:/agent/booming-il2cpp/src/native/runtime-core"
    "D:/agent/booming-il2cpp/src/native/bootstrap"
    "D:/agent/booming-il2cpp/src/native/interpreter"
    "D:/agent/booming-il2cpp/third_party/bdwgc/include"
    "D:/agent/booming-il2cpp/third_party/fmt/include"
    "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/io-streams-basics/native_test/l2-verify/build"
)

target_compile_definitions(l3_verify_io-streams-basics PRIVATE
    CHAOS_IL2CPP_CHECK
    CHAOS_IL2CPP_TRACE_ENABLED
    CHAOS_IL2CPP_VERIFY_MODE
    GC_NOT_DLL
    CHAOS_RUNTIME_ABI_STATIC
)

target_link_libraries(l3_verify_io-streams-basics PRIVATE
    "D:/agent/booming-il2cpp/build/native/src/native/runtime-core/Release/chaos_runtime_core.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/bootstrap/Release/chaos_bootstrap.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/interpreter/Release/chaos_interpreter.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/common/Release/chaos_common.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/support/Release/chaos_support.lib"
    "D:/agent/booming-il2cpp/build/native/src/native/hot-update/Release/chaos_hot_update.lib"
    "D:/agent/booming-il2cpp/build/native/bdwgc_build/Release/chaos_bdwgc.lib"
    "D:/agent/booming-il2cpp/build/native/fmt_build/Release/chaos_fmt.lib"
    ole32
    user32
)

target_compile_features(l3_verify_io-streams-basics PUBLIC cxx_std_20)
