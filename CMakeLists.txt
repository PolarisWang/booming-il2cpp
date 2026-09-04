cmake_minimum_required(VERSION 3.20)

set(ROADMAP0_PRESET_TARGET "" CACHE STRING "Roadmap 0 preset target selector")

if(ROADMAP0_TOOLCHAIN_VALIDATE_ONLY)
    project(roadmap0_preset_router LANGUAGES NONE)

    if(ROADMAP0_PRESET_TARGET STREQUAL "android-arm64-smoke"
       OR ROADMAP0_PRESET_TARGET STREQUAL "ios-arm64-packaging"
       OR ROADMAP0_PRESET_TARGET STREQUAL "linux-x64-packaging")
        add_custom_target(roadmap0_validate_only ALL)
        return()
    endif()
endif()

# P0.1: SemVer for the whole project — single source of truth in VERSION.
# Keep in sync with src/managed/Directory.Build.props <Version> and tags v*.
set(CHAOS_VERSION "0.2.4")

project(roadmap0_preset_router LANGUAGES CXX ASM VERSION ${CHAOS_VERSION})

# ── clang-tidy static analysis (T-A1) ──────────────────────────────────
# Enable with -DCHAOS_IL2CPP_CLANG_TIDY=ON on GCC/Clang presets (Linux Ninja).
# MSVC: clang-tidy analysis of MSVC dialect is unsupported/noisy — gate OFF.
# The config lives in src/native/.clang-tidy (HeaderFilterRegex src/native/.*\.h$).
# CI invokes `run-clang-tidy` against the `<preset>-clang-tidy` build; CMAKE_CXX_CLANG_TIDY
# is set here so the analyzer runs during the (re)compile pass on changed translation units.
option(CHAOS_IL2CPP_CLANG_TIDY "Enable clang-tidy static analysis during build" OFF)
if(CHAOS_IL2CPP_CLANG_TIDY AND NOT MSVC)
    find_program(CHAOS_IL2CPP_CLANG_TIDY_EXE clang-tidy)
    if(CHAOS_IL2CPP_CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CHAOS_IL2CPP_CLANG_TIDY_EXE}")
        message(STATUS "clang-tidy enabled: ${CHAOS_IL2CPP_CLANG_TIDY_EXE}")
    else()
        message(WARNING "CHAOS_IL2CPP_CLANG_TIDY=ON but clang-tidy not found — static analysis skipped")
    endif()
endif()

# ── MSVC debug info: Embedded (/Z7) avoids shared .pdb contention ─────
# The default /ZI (Edit-and-Continue PDB) crashes on systems where
# mspdbsrv.exe (RPC service) is disabled or blocked.  /Z7 puts debug
# info in each .obj file — no shared PDB, no RPC dependency.
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "Embedded")
endif()

set(
    CHAOS_SUBJECT_ANDROID_HOST_ROOT
    "tests/integration/platform-hosts/android-smoke"
    CACHE PATH
    "Subject-owned host directory for android-arm64-smoke")
set(
    CHAOS_SUBJECT_IOS_HOST_ROOT
    "tests/integration/platform-hosts/ios-smoke"
    CACHE PATH
    "Subject-owned host directory for ios-arm64-packaging")

if(ROADMAP0_PRESET_TARGET STREQUAL "windows-x64-reference")
    add_compile_definitions(CHAOS_IL2CPP_TARGET_PLATFORM_WINDOWS=1)
    include(cmake/config-tier.cmake)
    include(cmake/chaos_native_test.cmake)
    # Register CTest for the native test targets (contracts/native + testing/src/native).
    # Must be at this scope before add_subdirectory so the root CTestTestfile.cmake is
    # generated and ctest --test-dir finds the subdir tests.
    enable_testing()
    find_package(Python3 QUIET)

    # ── GoogleTest (self-contained; the legacy "testing" CMake root that used to
    #    own this FetchContent on the default path is being removed, L11) ─────────
    # Every chaos_native_add_test() target links gtest_main/gtest, so provision
    # googletest here before the native contracts/runtime-native trees.
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
        URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
    )
    if(MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    else()
        set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
    endif()
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(third_party/fmt fmt_build)
    add_subdirectory(third_party/unordered_dense unordered_dense_build)
    add_subdirectory(src/native/pal)
    add_subdirectory(src/native/common)
    add_subdirectory(src/native/diagnostics)
    add_subdirectory(src/native/runtime-core)
    add_subdirectory(src/native/hot-update)
    add_subdirectory(src/native/interpreter)
    add_subdirectory(src/native/jit)
    add_subdirectory(src/native/engine-bridge)
    add_subdirectory(src/native/support)
    add_subdirectory(src/native/bootstrap)
    add_subdirectory(src/native/codegen)
    add_subdirectory(src/native/benchmark-host)
    add_subdirectory(src/native/proof-host)

    add_subdirectory(tests/contracts/native/abi)
    add_subdirectory(tests/contracts/native/engine-binding)
    add_subdirectory(tests/contracts/native/hot-update)
    add_subdirectory(tests/contracts/native/interpreter)
    add_subdirectory(tests/contracts/native/gc)
    add_subdirectory(tests/contracts/native/runtime-core)
    add_subdirectory(tests/contracts/native/common)
    add_subdirectory(tests/perf/unordered-map-bench)

    # ── Foundation DLL native family skeletons ────────────────────
    #add_subdirectory(testing/foundation-dll)

    # ── Native testing suite (GoogleTest-based unit/stress/benchmark) ─────
    # Windows only: imports prebuilt .lib files, uses MSVC-specific flags.
    if(MSVC)
        # GoogleTest is provisioned at root (L11); the legacy "testing" CMake root
        # is removed. Native gtest suites live under tests/unit/runtime-native.
        add_subdirectory(tests/unit/runtime-native)
    endif()

    # ── Architecture check targets ─────────────────────────────────────
    # Each native module gets a per-module check target.
    # Run: cmake --build <build-dir> --target check_arch_<module>
    # Or:  cmake --build <build-dir> --target check_arch  (all modules)
    if(Python3_FOUND)
        set(CHAOS_NATIVE_MODULES
            chaos_common
            chaos_runtime_core
            chaos_hot_update
            chaos_interpreter
            chaos_jit
            chaos_support
            chaos_bootstrap
            chaos_engine_bridge
            chaos_benchmark_host
            chaos_proof_host)
        foreach(MODULE ${CHAOS_NATIVE_MODULES})
            add_custom_target(check_arch_${MODULE}
                COMMAND ${Python3_EXECUTABLE} build/check_arch.py --module ${MODULE}
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                COMMENT "Checking ${MODULE} architecture compliance..."
            )
        endforeach()
        add_custom_target(check_arch
            COMMAND ${Python3_EXECUTABLE} build/check_arch.py --ci
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "Checking all native modules for architecture compliance..."
        )
    endif()
elseif(ROADMAP0_PRESET_TARGET STREQUAL "linux-x64-reference"
       OR ROADMAP0_PRESET_TARGET STREQUAL "linux-x64-native")
    add_compile_definitions(CHAOS_IL2CPP_TARGET_PLATFORM_LINUX=1)
    include(cmake/config-tier.cmake)
    # Register the test helper module (defines chaos_native_test / chaos_compile_only_smoke)
    # used by tests/contracts/native/* added below.  Without it the linux SDK config
    # fails with "Unknown CMake command" — mirrors windows-x64-reference (line 61).
    include(cmake/chaos_native_test.cmake)
    find_package(Python3 QUIET)

    add_subdirectory(third_party/fmt fmt_build)
    add_subdirectory(third_party/unordered_dense unordered_dense_build)
    add_subdirectory(src/native/pal)
    add_subdirectory(src/native/common)
    add_subdirectory(src/native/diagnostics)
    add_subdirectory(src/native/runtime-core)
    add_subdirectory(src/native/hot-update)
    add_subdirectory(src/native/interpreter)
    add_subdirectory(src/native/jit)
    add_subdirectory(src/native/engine-bridge)
    add_subdirectory(src/native/support)
    add_subdirectory(src/native/bootstrap)
    add_subdirectory(src/native/codegen)
    add_subdirectory(src/native/benchmark-host)
    add_subdirectory(src/native/proof-host)

    add_subdirectory(tests/contracts/native/abi)
    add_subdirectory(tests/contracts/native/engine-binding)
    add_subdirectory(tests/contracts/native/hot-update)
    add_subdirectory(tests/contracts/native/interpreter)
    add_subdirectory(tests/contracts/native/gc)
    add_subdirectory(tests/contracts/native/runtime-core)
    add_subdirectory(tests/contracts/native/common)
    add_subdirectory(tests/perf/unordered-map-bench)

    # ── Foundation DLL native family skeletons ────────────────────

    # ── Native testing suite (GoogleTest-based unit/stress/benchmark) ─────
    # Every chaos_native_add_test() target links gtest_main/gtest, so provision
    # googletest here before the unit/runtime-native tree (mirrors the windows
    # branch block at line 72).
    # MSVC only: the native unit suites (log_system_test etc.) include Windows
    # headers (io.h / windows.h) and aren't portable to the linux SDK build,
    # which only needs to emit chaos_*.so/.a libraries.
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
        URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
    )
    if(MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    else()
        set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
    endif()
    FetchContent_MakeAvailable(googletest)

    # Native unit tests (log_system_test etc.) are Windows-only (io.h/windows.h);
    # skip them on the linux SDK release build which only needs chaos_*.so/.a.
    if(MSVC)
        add_subdirectory(tests/unit/runtime-native)
    endif()

    # ── Architecture check targets ─────────────────────────────────────
    if(Python3_FOUND)
        set(CHAOS_NATIVE_MODULES
            chaos_common
            chaos_runtime_core
            chaos_hot_update
            chaos_interpreter
            chaos_jit
            chaos_support
            chaos_bootstrap
            chaos_engine_bridge
            chaos_benchmark_host
            chaos_proof_host)
        foreach(MODULE ${CHAOS_NATIVE_MODULES})
            add_custom_target(check_arch_${MODULE}
                COMMAND ${Python3_EXECUTABLE} build/check_arch.py --module ${MODULE}
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                COMMENT "Checking ${MODULE} architecture compliance..."
            )
        endforeach()
        add_custom_target(check_arch
            COMMAND ${Python3_EXECUTABLE} build/check_arch.py --ci
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "Checking all native modules for architecture compliance..."
        )
    endif()
elseif(ROADMAP0_PRESET_TARGET STREQUAL "macos-reference")
    add_compile_definitions(CHAOS_IL2CPP_TARGET_PLATFORM_MACOS=1)
    include(cmake/config-tier.cmake)
    find_package(Python3 QUIET)

    add_subdirectory(third_party/fmt fmt_build)
    add_subdirectory(third_party/unordered_dense unordered_dense_build)
    add_subdirectory(src/native/pal)
    add_subdirectory(src/native/common)
    add_subdirectory(src/native/diagnostics)
    add_subdirectory(src/native/runtime-core)
    add_subdirectory(src/native/hot-update)
    add_subdirectory(src/native/interpreter)
    add_subdirectory(src/native/jit)
    add_subdirectory(src/native/engine-bridge)
    add_subdirectory(src/native/support)
    add_subdirectory(src/native/bootstrap)
    add_subdirectory(src/native/codegen)
    add_subdirectory(src/native/benchmark-host)
    add_subdirectory(src/native/proof-host)

    add_subdirectory(tests/contracts/native/abi)
    add_subdirectory(tests/contracts/native/engine-binding)
    add_subdirectory(tests/contracts/native/hot-update)
    add_subdirectory(tests/contracts/native/interpreter)
    add_subdirectory(tests/contracts/native/gc)
    add_subdirectory(tests/contracts/native/runtime-core)
    add_subdirectory(tests/contracts/native/common)
    add_subdirectory(tests/perf/unordered-map-bench)

    # Foundation DLL (cross-platform: no MSVC dependency)
    #add_subdirectory(testing/foundation-dll)

    # Architecture check targets
    if(Python3_FOUND)
        set(CHAOS_NATIVE_MODULES
            chaos_common
            chaos_runtime_core
            chaos_hot_update
            chaos_interpreter
            chaos_jit
            chaos_support
            chaos_bootstrap
            chaos_engine_bridge
            chaos_benchmark_host
            chaos_proof_host)
        foreach(MODULE ${CHAOS_NATIVE_MODULES})
            add_custom_target(check_arch_${MODULE}
                COMMAND ${Python3_EXECUTABLE} build/check_arch.py --module ${MODULE}
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                COMMENT "Checking ${MODULE} architecture compliance..."
            )
        endforeach()
        add_custom_target(check_arch
            COMMAND ${Python3_EXECUTABLE} build/check_arch.py --ci
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "Checking all native modules for architecture compliance..."
        )
    endif()
elseif(ROADMAP0_PRESET_TARGET STREQUAL "android-arm64-smoke")
    add_compile_definitions(CHAOS_IL2CPP_TARGET_PLATFORM_ANDROID=1)
    include(cmake/config-tier.cmake)

    add_subdirectory(third_party/fmt fmt_build)
    add_subdirectory(third_party/unordered_dense unordered_dense_build)
    add_subdirectory(src/native/pal)
    add_subdirectory(src/native/common)
    add_subdirectory(src/native/diagnostics)
    add_subdirectory(src/native/runtime-core)
    add_subdirectory(src/native/hot-update)
    add_subdirectory(src/native/interpreter)
    add_subdirectory(src/native/jit)
    add_subdirectory(src/native/engine-bridge)
    add_subdirectory(src/native/support)
    add_subdirectory(src/native/bootstrap)
    add_subdirectory(src/native/codegen)

    # Add the Android smoke host (standalone, does not link runtime modules)
    add_subdirectory("${CHAOS_SUBJECT_ANDROID_HOST_ROOT}" "${CMAKE_BINARY_DIR}/android-host")

    # Contract tests — verify ABI and core functionality on device
    add_subdirectory(tests/contracts/native/abi)
    add_subdirectory(tests/contracts/native/engine-binding)
    add_subdirectory(tests/contracts/native/hot-update)
    add_subdirectory(tests/contracts/native/interpreter)
    add_subdirectory(tests/contracts/native/gc)
    add_subdirectory(tests/contracts/native/runtime-core)
    add_subdirectory(tests/contracts/native/common)
elseif(ROADMAP0_PRESET_TARGET STREQUAL "ios-arm64-packaging")
    enable_language(OBJCXX)
    add_compile_definitions(CHAOS_IL2CPP_TARGET_PLATFORM_IOS=1)
    include(cmake/config-tier.cmake)

    add_subdirectory(third_party/fmt fmt_build)
    add_subdirectory(third_party/unordered_dense unordered_dense_build)
    add_subdirectory(src/native/pal)
    add_subdirectory(src/native/common)
    add_subdirectory(src/native/diagnostics)
    add_subdirectory(src/native/runtime-core)
    add_subdirectory(src/native/hot-update)
    add_subdirectory(src/native/interpreter)
    add_subdirectory(src/native/jit)
    add_subdirectory(src/native/engine-bridge)
    add_subdirectory(src/native/support)
    add_subdirectory(src/native/bootstrap)
    add_subdirectory(src/native/codegen)

    # Add the iOS packaging host (standalone, does not link runtime modules)
    add_subdirectory("${CHAOS_SUBJECT_IOS_HOST_ROOT}" "${CMAKE_BINARY_DIR}/ios-host")

    # Contract tests — verify ABI and core functionality on device
    add_subdirectory(tests/contracts/native/abi)
    add_subdirectory(tests/contracts/native/engine-binding)
    add_subdirectory(tests/contracts/native/hot-update)
    add_subdirectory(tests/contracts/native/interpreter)
    add_subdirectory(tests/contracts/native/gc)
    add_subdirectory(tests/contracts/native/runtime-core)
    add_subdirectory(tests/contracts/native/common)

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
            "${CMAKE_CURRENT_SOURCE_DIR}/src/native/pal"
            "${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmt/include")
        target_link_libraries(chaos_arm64_jit_smoke PRIVATE chaos_jit)
        set_target_properties(chaos_arm64_jit_smoke PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib")
    else()
        message(STATUS "NOT FOUND: arm64_jit_smoke_test.cpp")
    endif()

elseif(ROADMAP0_PRESET_TARGET STREQUAL "linux-x64-packaging")
    add_compile_definitions(CHAOS_IL2CPP_TARGET_PLATFORM_LINUX=1)
    add_subdirectory(tests/integration/platform-hosts/linux-packaging)
elseif(ROADMAP0_PRESET_TARGET STREQUAL "linux-arm64-native")
    message(STATUS "ENTERED linux-arm64-native branch")
    add_compile_definitions(CHAOS_IL2CPP_TARGET_PLATFORM_LINUX=1)
    include(cmake/config-tier.cmake)

    add_subdirectory(third_party/fmt fmt_build)
    add_subdirectory(third_party/unordered_dense unordered_dense_build)
    add_subdirectory(src/native/pal)
    add_subdirectory(src/native/common)
    add_subdirectory(src/native/diagnostics)
    add_subdirectory(src/native/runtime-core)
    add_subdirectory(src/native/hot-update)
    add_subdirectory(src/native/interpreter)
    add_subdirectory(src/native/jit)
    add_subdirectory(src/native/engine-bridge)
    add_subdirectory(src/native/support)
    add_subdirectory(src/native/bootstrap)
    add_subdirectory(src/native/codegen)
    add_subdirectory(src/native/benchmark-host)
    add_subdirectory(src/native/proof-host)

    add_subdirectory(tests/contracts/native/gc)
    add_subdirectory(tests/contracts/native/runtime-core)
    add_subdirectory(tests/contracts/native/common)
    add_subdirectory(tests/contracts/native/abi)
    add_subdirectory(tests/contracts/native/interpreter)

    # ── ARM64 JIT test suite (shared native+smoke) ─────────────────
    include(cmake/arm64-jit-test.cmake)
elseif(ROADMAP0_PRESET_TARGET STREQUAL "linux-arm64-smoke")
    message(STATUS "ENTERED linux-arm64-smoke branch")
    add_compile_definitions(CHAOS_IL2CPP_TARGET_PLATFORM_LINUX=1)
    include(cmake/config-tier.cmake)

    add_subdirectory(third_party/fmt fmt_build)
    add_subdirectory(third_party/unordered_dense unordered_dense_build)
    add_subdirectory(src/native/pal)
    add_subdirectory(src/native/common)
    add_subdirectory(src/native/diagnostics)
    add_subdirectory(src/native/runtime-core)
    add_subdirectory(src/native/hot-update)
    add_subdirectory(src/native/interpreter)
    add_subdirectory(src/native/jit)
    add_subdirectory(src/native/engine-bridge)
    add_subdirectory(src/native/support)
    add_subdirectory(src/native/bootstrap)
    add_subdirectory(src/native/codegen)

    add_subdirectory(tests/contracts/native/gc)
    add_subdirectory(tests/contracts/native/runtime-core)
    add_subdirectory(tests/contracts/native/common)
    add_subdirectory(tests/contracts/native/abi)
    add_subdirectory(tests/contracts/native/interpreter)

    # ── ARM64 JIT test suite (shared native+smoke) ─────────────────
    include(cmake/arm64-jit-test.cmake)
else()
    message(FATAL_ERROR "Unsupported ROADMAP0_PRESET_TARGET='${ROADMAP0_PRESET_TARGET}'")
endif()

