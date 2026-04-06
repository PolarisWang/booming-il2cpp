cmake_minimum_required(VERSION 3.20)

project(chaos_stage4_hello_world_object_proof LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(CHAOS_HELLOWORLD_GENERATED_ROOT
    "${REPO_ROOT}/artifacts/subjects/HelloWorldObject/shared/generated"
    CACHE PATH "Generated bucket root for the HelloWorldObject subject")
set(CHAOS_HELLOWORLD_BUILD_OUT_ROOT
    "${REPO_ROOT}/artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/build/out"
    CACHE PATH "Build output root for the HelloWorldObject subject")
set(CHAOS_HELLOWORLD_RUNTIME_ROOT
    "${REPO_ROOT}/artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/runtime"
    CACHE PATH "Runtime evidence root for the HelloWorldObject subject")

set(STAGE4_GENERATED_SOURCE "${CHAOS_HELLOWORLD_GENERATED_ROOT}/generated/native-reference.generated.cpp")
set(STAGE4_RUN_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/RunNativeReferenceProof.cmake")
set(STAGE4_RUN_STDOUT_LOG "${CHAOS_HELLOWORLD_RUNTIME_ROOT}/stdout.log")
set(STAGE4_RUN_STDERR_LOG "${CHAOS_HELLOWORLD_RUNTIME_ROOT}/stderr.log")
set(STAGE4_RUN_EXIT_CODE_PATH "${CHAOS_HELLOWORLD_RUNTIME_ROOT}/exit-code.txt")

set_source_files_properties("${STAGE4_GENERATED_SOURCE}" PROPERTIES GENERATED TRUE)

add_executable(chaos_stage4_hello_world_object_proof EXCLUDE_FROM_ALL
    main.cpp
    "${STAGE4_GENERATED_SOURCE}")

add_custom_target(chaos_stage4_hello_world_object_proof_run
    BYPRODUCTS
        "${STAGE4_RUN_STDOUT_LOG}"
        "${STAGE4_RUN_STDERR_LOG}"
        "${STAGE4_RUN_EXIT_CODE_PATH}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CHAOS_HELLOWORLD_RUNTIME_ROOT}"
    COMMAND "${CMAKE_COMMAND}"
        "-DCHAOS_STAGE4_PROOF_EXE=$<TARGET_FILE:chaos_stage4_hello_world_object_proof>"
        "-DCHAOS_STAGE4_RUN_OUTPUT_ROOT=${CHAOS_HELLOWORLD_RUNTIME_ROOT}"
        "-DCHAOS_STAGE4_STDOUT_PATH=${STAGE4_RUN_STDOUT_LOG}"
        "-DCHAOS_STAGE4_STDERR_PATH=${STAGE4_RUN_STDERR_LOG}"
        "-DCHAOS_STAGE4_EXIT_CODE_PATH=${STAGE4_RUN_EXIT_CODE_PATH}"
        -P "${STAGE4_RUN_SCRIPT}"
    DEPENDS
        chaos_stage4_hello_world_object_proof
    VERBATIM)

target_compile_features(chaos_stage4_hello_world_object_proof PRIVATE cxx_std_17)
target_include_directories(
    chaos_stage4_hello_world_object_proof
    PRIVATE
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/src/native/runtime-core"
        "${REPO_ROOT}/src/native/bootstrap"
        "${REPO_ROOT}/src/native/support")
target_link_libraries(
    chaos_stage4_hello_world_object_proof
    PRIVATE
        chaos_runtime_core
        chaos_bootstrap
        chaos_support)

set_target_properties(
    chaos_stage4_hello_world_object_proof
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CHAOS_HELLOWORLD_BUILD_OUT_ROOT}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CHAOS_HELLOWORLD_BUILD_OUT_ROOT}")
