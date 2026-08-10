cmake_minimum_required(VERSION 3.20)

project(chaos_method_aot_map_test LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(ARTIFACT_ROOT "${REPO_ROOT}/artifacts/native-runtime-core-test")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

# ── Bulk uniform GC test targets (auto-discovered) ────────────────────────────
# Every gc_*.cpp whose target name is <prefix>_<name_we> and needs no special
# CXX_STANDARD/extra sources is created by the factory via the GLOB helper.
# Special targets are declared explicitly below; additionally the sources below
# are EXCLUDED because they aren't standalone tests (helpers / stubs / not-ready).
chaos_native_glob_add_tests(chaos "gc_*.cpp"
    RESOURCE_LOCK bgc
    LOCK_SOURCES
        gc_bgc_smoke
        gc_bgc_race_test
        gc_bgc_root_scan_test
        gc_bgc_stress_test
        gc_stress_test
    EXCLUDE
        gc_root_scanner_test.cpp  # wants CXX_STANDARD 20 (kept explicit below)
        gc_test_stubs.cpp         # codegen-symbol stub source, NOT a test
        gc_minimal_init.cpp       # non-test init helper
        gc_debug_contract_test.cpp  # built standalone in testing/ (gc_debug_contract.cpp + stub)
        gc_api_test.cpp           # not part of the contracts set
        gc_benchmark.cpp          # not part of the contracts set
        gc_soak_test.cpp          # not part of the contracts set
)

# ── Explicit (non-uniform / special) targets ─────────────────────────────────
# gc_root_scanner_test uses C++20 features.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_root_scanner_test.cpp")
    chaos_native_add_test(chaos_gc_root_scanner_test gc_root_scanner_test.cpp CXX_STANDARD 20)
endif()

# loh_stress_test / oversized_stress_test: sources are NOT gc_-prefixed, so the
# GLOB above skips them; target names carry a gc_ segment.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/loh_stress_test.cpp")
    chaos_native_add_test(chaos_gc_loh_stress_test loh_stress_test.cpp LABELS "stress")
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/oversized_stress_test.cpp")
    chaos_native_add_test(chaos_gc_oversized_stress_test oversized_stress_test.cpp LABELS "stress")
endif()

# method_aot_map_test: reduced lib set.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/method_aot_map_test.cpp")
    chaos_native_add_test(chaos_method_aot_map_test method_aot_map_test.cpp
        LIBS chaos_runtime_core chaos_bootstrap chaos_interpreter chaos_codegen)
endif()

# Non-GC subsystem targets (threading / domain / memory / marshal / misc).
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/domain_unload_test.cpp")
    chaos_native_add_test(chaos_domain_unload_test domain_unload_test.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/memory_domain_test.cpp")
    chaos_native_add_test(chaos_memory_domain_test memory_domain_test.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/threading_smoke.cpp")
    chaos_native_add_test(chaos_threading_smoke threading_smoke.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/execution_context_smoke.cpp")
    chaos_native_add_test(chaos_execution_context_smoke execution_context_smoke.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/synchronization_smoke.cpp")
    chaos_native_add_test(chaos_synchronization_smoke synchronization_smoke.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/monitor_stress_test.cpp")
    chaos_native_add_test(chaos_monitor_stress_test monitor_stress_test.cpp LABELS "stress")
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/threading_benchmark.cpp")
    chaos_native_add_test(chaos_threading_benchmark threading_benchmark.cpp
    LABELS "benchmark")
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/blittable_benchmark.cpp")
    chaos_native_add_test(chaos_blittable_benchmark blittable_benchmark.cpp
    LABELS "benchmark")
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/delegate_stress_test.cpp")
    chaos_native_add_test(chaos_delegate_stress_test delegate_stress_test.cpp LABELS "stress")
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/exception_sentinel_test.cpp")
    chaos_native_add_test(chaos_exception_sentinel_test exception_sentinel_test.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/capacity_stress_test.cpp")
    chaos_native_add_test(chaos_capacity_stress_test capacity_stress_test.cpp LABELS "stress")
endif()
# NOTE: codegen_native_test was EXCLUDED because it #included code_generator.h
# from a "Phase 3d" JIT/codegen module that was renamed codegen/ -> jit/ without
# migrating this test.  It has now been re-rewritten to the current jit API
# (jit_engine.h / Compile() / JitMethod / RegisterNativeCodeSection etc.) so it
# compiles again, and is re-enabled here.  T4 code uses the JIT register
# convention (RBX=args, RSI=ret) rather than the Win64 C ABI, so the target
# links t4_jit_call.asm to invoke compiled entries with the correct thunk.
#
# unwind_info_test is ENABLED: it now exercises the real
# chaos::il2cpp::jit::EmitUnwindInfo (src/native/jit/jit_unwind.h), which is in
# CHAOS_NATIVE_INCLUDE_DIRS. Replaced the old speculative codegen:: binding.
#
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/codegen_native_test.cpp")
    if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
        # x64 Windows: T4 thunk requires an x64 assembly shim (ASM_MASM).
        enable_language(ASM_MASM)
        set_source_files_properties(
            "${CMAKE_CURRENT_SOURCE_DIR}/t4_jit_call.asm"
            PROPERTIES LANGUAGE ASM_MASM)
        chaos_native_add_test(chaos_codegen_native_test
            codegen_native_test.cpp
            "${CMAKE_CURRENT_SOURCE_DIR}/t4_jit_call.asm")
    else()
        # Non-x64 / non-MSVC: T4 thunk assembly is x64-only; still compile the
        # C++ test, but without native codegen execution coverage.
        chaos_native_add_test(chaos_codegen_native_test codegen_native_test.cpp)
    endif()
endif()
# unwind_info_test asserts x86-64 UWOP/RBP/mov-rax-imm64 byte layout — only valid
# on x64. On non-x64 (arm64) the bytes don't exist, so exclude the target there.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/unwind_info_test.cpp" AND
   (MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8 OR CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$"))
    chaos_native_add_test(chaos_unwind_info_test unwind_info_test.cpp)
endif()
