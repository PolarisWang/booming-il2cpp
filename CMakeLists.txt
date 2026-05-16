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

project(roadmap0_preset_router LANGUAGES CXX)

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
    find_package(Python3 QUIET)

    add_subdirectory(third_party/fmt fmt_build)
    add_subdirectory(third_party/unordered_dense unordered_dense_build)
    add_subdirectory(third_party/bdwgc bdwgc_build)
    add_subdirectory(src/native/common)
    add_subdirectory(src/native/runtime-core)
    add_subdirectory(src/native/hot-update)
    add_subdirectory(src/native/interpreter)
    add_subdirectory(src/native/engine-bridge)
    add_subdirectory(src/native/support)
    add_subdirectory(src/native/bootstrap)
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
    add_subdirectory(verification/foundation-dll)

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
    add_subdirectory(tests/contracts/native/bridge)
elseif(ROADMAP0_PRESET_TARGET STREQUAL "android-arm64-smoke")
    add_compile_definitions(CHAOS_IL2CPP_TARGET_PLATFORM_ANDROID=1)
    add_subdirectory("${CHAOS_SUBJECT_ANDROID_HOST_ROOT}" "${CMAKE_BINARY_DIR}/android-host")
elseif(ROADMAP0_PRESET_TARGET STREQUAL "ios-arm64-packaging")
    enable_language(OBJCXX)
    add_compile_definitions(CHAOS_IL2CPP_TARGET_PLATFORM_IOS=1)
    add_subdirectory("${CHAOS_SUBJECT_IOS_HOST_ROOT}" "${CMAKE_BINARY_DIR}/ios-host")
elseif(ROADMAP0_PRESET_TARGET STREQUAL "linux-x64-packaging")
    add_compile_definitions(CHAOS_IL2CPP_TARGET_PLATFORM_LINUX=1)
    add_subdirectory(tests/integration/platform-hosts/linux-packaging)
else()
    message(FATAL_ERROR "Unsupported ROADMAP0_PRESET_TARGET='${ROADMAP0_PRESET_TARGET}'")
endif()
