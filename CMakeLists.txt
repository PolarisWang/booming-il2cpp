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

if(ROADMAP0_PRESET_TARGET STREQUAL "windows-x64-reference")
    add_subdirectory(src/native/runtime-core)
    add_subdirectory(src/native/support)
    add_subdirectory(src/native/bootstrap)
    add_subdirectory(subjects/HelloWorldObject/validation/proof/native-reference)
    add_subdirectory(tests/contracts/native/abi)
elseif(ROADMAP0_PRESET_TARGET STREQUAL "macos-reference")
    add_subdirectory(tests/contracts/native/bridge)
elseif(ROADMAP0_PRESET_TARGET STREQUAL "android-arm64-smoke")
    add_subdirectory(tests/gate/android-smoke)
elseif(ROADMAP0_PRESET_TARGET STREQUAL "ios-arm64-packaging")
    enable_language(OBJCXX)
    add_subdirectory(tests/gate/ios-smoke)
elseif(ROADMAP0_PRESET_TARGET STREQUAL "linux-x64-packaging")
    add_subdirectory(tests/gate/linux-packaging)
else()
    message(FATAL_ERROR "Unsupported ROADMAP0_PRESET_TARGET='${ROADMAP0_PRESET_TARGET}'")
endif()
