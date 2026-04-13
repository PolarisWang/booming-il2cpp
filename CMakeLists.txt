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
    CHAOS_SUBJECT_PROOF_ROOT
    "subjects/SolutionCorePack/validation/proof/native-reference"
    CACHE PATH
    "Subject-owned proof host directory for windows-x64-reference")
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
    add_subdirectory(third_party/bdwgc bdwgc_build)
    add_subdirectory(src/native/runtime-core)
    add_subdirectory(src/native/hot-update)
    add_subdirectory(src/native/interpreter)
    add_subdirectory(src/native/engine-bridge)
    add_subdirectory(src/native/support)
    add_subdirectory(src/native/bootstrap)
    add_subdirectory("${CHAOS_SUBJECT_PROOF_ROOT}" "${CMAKE_BINARY_DIR}/subject-proof")
    add_subdirectory(tests/contracts/native/abi)
    add_subdirectory(tests/contracts/native/engine-binding)
    add_subdirectory(tests/contracts/native/hot-update)
    add_subdirectory(tests/contracts/native/interpreter)
    add_subdirectory(tests/contracts/native/gc)
elseif(ROADMAP0_PRESET_TARGET STREQUAL "macos-reference")
    add_subdirectory(tests/contracts/native/bridge)
elseif(ROADMAP0_PRESET_TARGET STREQUAL "android-arm64-smoke")
    add_subdirectory("${CHAOS_SUBJECT_ANDROID_HOST_ROOT}" "${CMAKE_BINARY_DIR}/android-host")
elseif(ROADMAP0_PRESET_TARGET STREQUAL "ios-arm64-packaging")
    enable_language(OBJCXX)
    add_subdirectory("${CHAOS_SUBJECT_IOS_HOST_ROOT}" "${CMAKE_BINARY_DIR}/ios-host")
elseif(ROADMAP0_PRESET_TARGET STREQUAL "linux-x64-packaging")
    add_subdirectory(tests/integration/platform-hosts/linux-packaging)
else()
    message(FATAL_ERROR "Unsupported ROADMAP0_PRESET_TARGET='${ROADMAP0_PRESET_TARGET}'")
endif()
