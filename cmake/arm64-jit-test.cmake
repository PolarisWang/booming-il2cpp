    # ── ARM64 JIT encoder smoke test ──────────────────────────────────
    message(STATUS "Checking arm64_jit_smoke_test.cpp...")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/contracts/native/runtime-core/arm64_jit_smoke_test.cpp")
        message(STATUS "Found arm64_jit_smoke_test.cpp, adding chaos_arm64_jit_smoke target")
        add_executable(chaos_arm64_jit_smoke
            tests/contracts/native/runtime-core/arm64_jit_smoke_test.cpp)
        target_compile_features(chaos_arm64_jit_smoke PRIVATE cxx_std_17)
        target_include_directories(chaos_arm64_jit_smoke PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/jit"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/common"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/runtime-core"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/runtime-core/gc"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/runtime-core/runtime_stubs"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/interpreter"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/interpreter/generated"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/diagnostics"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/pal"
            "${CMAKE_CURRENT_SOURCE_DIR}/contracts/native/v0"
            "${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmt/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/third_party/unordered_dense/include")
        target_link_libraries(chaos_arm64_jit_smoke PRIVATE
            "-Wl,--start-group"
            chaos_jit
            chaos_runtime_core
            chaos_interpreter
            chaos_bootstrap
            chaos_hot_update
            chaos_support
            chaos_common
            chaos_pal
            chaos_eventpipe
            chaos_fmt
            "-Wl,--end-group"
            "-lnuma")
        set_target_properties(chaos_arm64_jit_smoke PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib")
    else()
        message(STATUS "NOT FOUND: arm64_jit_smoke_test.cpp")
    endif()

    # ── ARM64 JIT code execution test ───────────────────────────────────
    message(STATUS "Checking arm64_jit_exec_test.cpp...")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/contracts/native/runtime-core/arm64_jit_exec_test.cpp")
        message(STATUS "Found arm64_jit_exec_test.cpp, adding chaos_arm64_jit_exec target")
        add_executable(chaos_arm64_jit_exec
            tests/contracts/native/runtime-core/arm64_jit_exec_test.cpp
            src/native/jit/jit_helpers.cpp)
        target_compile_features(chaos_arm64_jit_exec PRIVATE cxx_std_17)
        target_include_directories(chaos_arm64_jit_exec PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/jit"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/common"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/runtime-core"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/runtime-core/gc"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/runtime-core/runtime_stubs"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/interpreter"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/interpreter/generated"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/diagnostics"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/pal"
            "${CMAKE_CURRENT_SOURCE_DIR}/contracts/native/v0"
            "${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmt/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/third_party/unordered_dense/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/runtime-native/runtime-core/gc"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/runtime-native")
        target_link_libraries(chaos_arm64_jit_exec PRIVATE
            "-Wl,--start-group"
            chaos_jit
            chaos_runtime_core
            chaos_interpreter
            chaos_bootstrap
            chaos_hot_update
            chaos_support
            chaos_common
            chaos_pal
            chaos_eventpipe
            chaos_fmt
            "-Wl,--end-group"
            "-lnuma")
        set_target_properties(chaos_arm64_jit_exec PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib")
        target_compile_definitions(chaos_arm64_jit_exec PRIVATE
            CHAOS_IL2CPP_LOG_LEVEL=3)

        # ── CTest integration (QEMU user-mode) ──────────────────────────
        find_program(QEMU_AARCH64 qemu-aarch64)
        if(QEMU_AARCH64)
            enable_testing()
            add_test(NAME chaos_arm64_jit_smoke
                COMMAND ${QEMU_AARCH64} $<TARGET_FILE:chaos_arm64_jit_smoke>)
            add_test(NAME chaos_arm64_jit_exec
                COMMAND ${QEMU_AARCH64} $<TARGET_FILE:chaos_arm64_jit_exec>)
            set_tests_properties(chaos_arm64_jit_smoke chaos_arm64_jit_exec PROPERTIES
                LABELS "arm64;jit"
                ENVIRONMENT "QEMU_LD_PREFIX=/usr/aarch64-linux-gnu")
            message(STATUS "ARM64 CTest tests registered (qemu-aarch64 found)")
        else()
            message(WARNING "qemu-aarch64 not found — ARM64 tests will not be registered with CTest")
        endif()

        # ── GoogleTest for ARM64 cross-compilation ──────────────────────────
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
            URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/_deps")
        FetchContent_MakeAvailable(googletest)

        function(add_arm64_jit_test name source_file)
            add_executable(chaos_arm64_jit_${name}
                ${source_file}
                tests/unit/runtime-native/runtime-core/gc/gc_test_stubs.cpp)
            target_compile_features(chaos_arm64_jit_${name} PRIVATE cxx_std_17)
            target_include_directories(chaos_arm64_jit_${name} PRIVATE
                "${CMAKE_CURRENT_SOURCE_DIR}/src/native/jit"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/native/common"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/native/runtime-core"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/native/runtime-core/gc"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/native/runtime-core/runtime_stubs"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/native/interpreter"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/native/interpreter/generated"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/native/diagnostics"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/native/pal"
                "${CMAKE_CURRENT_SOURCE_DIR}/contracts/native/v0"
                "${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmt/include"
                "${CMAKE_CURRENT_SOURCE_DIR}/third_party/unordered_dense/include"
                "${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/runtime-native")
            target_link_libraries(chaos_arm64_jit_${name} PRIVATE
                gtest_main
                "-Wl,--start-group"
                chaos_jit
                chaos_runtime_core
                chaos_interpreter
                chaos_bootstrap
                chaos_hot_update
                chaos_support
                chaos_common
                chaos_pal
                chaos_eventpipe
                chaos_fmt
                "-Wl,--end-group"
                "-lnuma")
            target_compile_definitions(chaos_arm64_jit_${name} PRIVATE
                CHAOS_IL2CPP_JIT_TEST_ACTIVE=1)
            target_link_options(chaos_arm64_jit_${name} PRIVATE
                -Wl,--allow-multiple-definition
                -Wl,--undefined=JitRecompileToTier1)
            if(QEMU_AARCH64)
                add_test(NAME chaos_arm64_jit_${name}
                    COMMAND ${QEMU_AARCH64} $<TARGET_FILE:chaos_arm64_jit_${name}>)
                set_tests_properties(chaos_arm64_jit_${name} PROPERTIES
                    LABELS "arm64;jit"
                    ENVIRONMENT "QEMU_LD_PREFIX=/usr/aarch64-linux-gnu")
            endif()
        endfunction()

        add_arm64_jit_test(il_smoke     tests/unit/runtime-native/jit/jit_il_smoke_test.cpp)
        set_tests_properties(chaos_arm64_jit_il_smoke PROPERTIES TIMEOUT 120)

        add_arm64_jit_test(simd         tests/unit/runtime-native/jit/jit_simd_test.cpp)
        set_tests_properties(chaos_arm64_jit_simd PROPERTIES TIMEOUT 60)

        add_arm64_jit_test(abi          tests/unit/runtime-native/jit/jit_abi_test.cpp)

        add_arm64_jit_test(gc_slot_map  tests/unit/runtime-native/jit/jit_gc_slot_map_test.cpp)
        set_tests_properties(chaos_arm64_jit_gc_slot_map PROPERTIES TIMEOUT 30)

        add_arm64_jit_test(osr          tests/unit/runtime-native/jit/jit_osr_test.cpp)
        set_tests_properties(chaos_arm64_jit_osr PROPERTIES TIMEOUT 30)

        add_arm64_jit_test(binary_ir    tests/unit/runtime-native/jit/jit_binary_ir_test.cpp)
        set_tests_properties(chaos_arm64_jit_binary_ir PROPERTIES TIMEOUT 30)

        add_arm64_jit_test(mode         tests/unit/runtime-native/jit/jit_mode_test.cpp)
        set_tests_properties(chaos_arm64_jit_mode PROPERTIES TIMEOUT 30)

        add_arm64_jit_test(pgo          tests/unit/runtime-native/jit/jit_pgo_test.cpp)
        set_tests_properties(chaos_arm64_jit_pgo PROPERTIES TIMEOUT 30)

        add_arm64_jit_test(debug_info   tests/unit/runtime-native/jit/jit_debug_info_test.cpp)
        set_tests_properties(chaos_arm64_jit_debug_info PROPERTIES TIMEOUT 30)

        add_arm64_jit_test(bench tests/unit/runtime-native/jit/bench_jit_vs_aot.cpp)
        set_tests_properties(chaos_arm64_jit_bench PROPERTIES TIMEOUT 120)

        message(STATUS "ARM64 JIT: 10 architecture-neutral test targets registered")
    else()
        message(STATUS "NOT FOUND: arm64_jit_exec_test.cpp")
    endif()
