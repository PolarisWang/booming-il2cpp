cmake_minimum_required(VERSION 3.20)

project(chaos_stage4_hello_world_object_proof LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(STAGE4_PROOF_INPUT_PROJECT "${REPO_ROOT}/tests/proof/input/HelloWorldObject/HelloWorldObject.csproj")
set(STAGE4_PROOF_INPUT_DLL "${REPO_ROOT}/tests/proof/input/HelloWorldObject/bin/Release/net8.0/HelloWorldObject.dll")
set(STAGE4_DRIVER_PROJECT "${REPO_ROOT}/src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj")
set(STAGE4_DRIVER_DLL "${REPO_ROOT}/src/managed/Chaos.IL2CPP.Driver/bin/Release/net8.0/Chaos.IL2CPP.Driver.dll")
set(STAGE4_MANAGED_CLOSURE_ROOT "${REPO_ROOT}/artifacts/proof/managed-closure/HelloWorldObject")
set(STAGE4_MANAGED_CLOSURE_TYPED_IL "${STAGE4_MANAGED_CLOSURE_ROOT}/typed-il-ir.json")
set(STAGE4_MANAGED_CLOSURE_AOT_MANIFEST "${STAGE4_MANAGED_CLOSURE_ROOT}/aot-manifest.json")
set(STAGE4_MANAGED_CLOSURE_METADATA "${STAGE4_MANAGED_CLOSURE_ROOT}/metadata-registration.json")
set(STAGE4_MANAGED_CLOSURE_CODE "${STAGE4_MANAGED_CLOSURE_ROOT}/code-registration.json")
set(STAGE4_MANAGED_CLOSURE_MANIFEST "${STAGE4_MANAGED_CLOSURE_ROOT}/closure.manifest.json")
set(STAGE4_OUTPUT_ROOT "${REPO_ROOT}/artifacts/proof/native-reference/HelloWorldObject")
set(STAGE4_GENERATED_SOURCE "${STAGE4_OUTPUT_ROOT}/generated/native-reference.generated.cpp")
set(STAGE4_PROOF_MANIFEST "${STAGE4_OUTPUT_ROOT}/native-proof.manifest.json")
set(STAGE4_RUN_OUTPUT_ROOT "${STAGE4_OUTPUT_ROOT}/run")
set(STAGE4_RUN_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/RunNativeReferenceProof.cmake")
set(STAGE4_RUN_STDOUT_LOG "${STAGE4_RUN_OUTPUT_ROOT}/stdout.log")
set(STAGE4_RUN_STDERR_LOG "${STAGE4_RUN_OUTPUT_ROOT}/stderr.log")
set(STAGE4_RUN_EXIT_CODE_PATH "${STAGE4_RUN_OUTPUT_ROOT}/exit-code.txt")
set(STAGE4_HOST_INPUT_BUILD_STAMP "${STAGE4_OUTPUT_ROOT}/.host-input-build.stamp")
set(STAGE4_DRIVER_BUILD_STAMP "${STAGE4_OUTPUT_ROOT}/.driver-build.stamp")
set(STAGE4_MANAGED_CLOSURE_STAMP "${STAGE4_OUTPUT_ROOT}/.managed-closure.stamp")
set(STAGE4_CODEGEN_STAMP "${STAGE4_OUTPUT_ROOT}/.codegen.stamp")

set_source_files_properties("${STAGE4_GENERATED_SOURCE}" PROPERTIES GENERATED TRUE)

add_custom_command(
    OUTPUT "${STAGE4_HOST_INPUT_BUILD_STAMP}"
    BYPRODUCTS
        "${STAGE4_PROOF_INPUT_DLL}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE4_OUTPUT_ROOT}"
    COMMAND dotnet build "${STAGE4_PROOF_INPUT_PROJECT}" -c Release
    COMMAND "${CMAKE_COMMAND}" -E touch "${STAGE4_HOST_INPUT_BUILD_STAMP}"
    DEPENDS
        "${STAGE4_PROOF_INPUT_PROJECT}"
    WORKING_DIRECTORY "${REPO_ROOT}"
    VERBATIM)

add_custom_command(
    OUTPUT "${STAGE4_DRIVER_BUILD_STAMP}"
    BYPRODUCTS
        "${STAGE4_DRIVER_DLL}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE4_OUTPUT_ROOT}"
    COMMAND dotnet build "${STAGE4_DRIVER_PROJECT}" -c Release
    COMMAND "${CMAKE_COMMAND}" -E touch "${STAGE4_DRIVER_BUILD_STAMP}"
    DEPENDS
        "${STAGE4_DRIVER_PROJECT}"
    WORKING_DIRECTORY "${REPO_ROOT}"
    VERBATIM)

add_custom_command(
    OUTPUT "${STAGE4_MANAGED_CLOSURE_STAMP}"
    BYPRODUCTS
        "${STAGE4_MANAGED_CLOSURE_TYPED_IL}"
        "${STAGE4_MANAGED_CLOSURE_AOT_MANIFEST}"
        "${STAGE4_MANAGED_CLOSURE_METADATA}"
        "${STAGE4_MANAGED_CLOSURE_CODE}"
        "${STAGE4_MANAGED_CLOSURE_MANIFEST}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE4_OUTPUT_ROOT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE4_MANAGED_CLOSURE_ROOT}"
    COMMAND dotnet "${STAGE4_DRIVER_DLL}" "${STAGE4_PROOF_INPUT_DLL}" "${STAGE4_MANAGED_CLOSURE_ROOT}"
    COMMAND "${CMAKE_COMMAND}" -E touch "${STAGE4_MANAGED_CLOSURE_STAMP}"
    DEPENDS
        "${STAGE4_HOST_INPUT_BUILD_STAMP}"
        "${STAGE4_DRIVER_BUILD_STAMP}"
    WORKING_DIRECTORY "${REPO_ROOT}"
    VERBATIM)

add_custom_command(
    OUTPUT "${STAGE4_CODEGEN_STAMP}"
    BYPRODUCTS
        "${STAGE4_GENERATED_SOURCE}"
        "${STAGE4_PROOF_MANIFEST}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE4_OUTPUT_ROOT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE4_OUTPUT_ROOT}/generated"
    COMMAND dotnet "${STAGE4_DRIVER_DLL}" emit-native-reference "${STAGE4_MANAGED_CLOSURE_ROOT}" "${STAGE4_OUTPUT_ROOT}"
    COMMAND "${CMAKE_COMMAND}" -E touch "${STAGE4_CODEGEN_STAMP}"
    DEPENDS
        "${STAGE4_MANAGED_CLOSURE_STAMP}"
        "${STAGE4_MANAGED_CLOSURE_TYPED_IL}"
        "${STAGE4_MANAGED_CLOSURE_AOT_MANIFEST}"
        "${STAGE4_MANAGED_CLOSURE_METADATA}"
        "${STAGE4_MANAGED_CLOSURE_CODE}"
        "${STAGE4_MANAGED_CLOSURE_MANIFEST}"
    WORKING_DIRECTORY "${REPO_ROOT}"
    VERBATIM)

add_custom_target(chaos_stage4_hello_world_object_host_input_build
    DEPENDS
        "${STAGE4_HOST_INPUT_BUILD_STAMP}")

add_custom_target(chaos_stage4_hello_world_object_driver_build
    DEPENDS
        "${STAGE4_DRIVER_BUILD_STAMP}")

add_custom_target(chaos_stage4_hello_world_object_managed_closure
    DEPENDS
        "${STAGE4_MANAGED_CLOSURE_STAMP}")

add_dependencies(chaos_stage4_hello_world_object_managed_closure
    chaos_stage4_hello_world_object_host_input_build
    chaos_stage4_hello_world_object_driver_build)

add_custom_target(chaos_stage4_hello_world_object_codegen
    DEPENDS
        "${STAGE4_CODEGEN_STAMP}")

add_dependencies(chaos_stage4_hello_world_object_codegen chaos_stage4_hello_world_object_managed_closure)

add_executable(chaos_stage4_hello_world_object_proof EXCLUDE_FROM_ALL
    main.cpp
    "${STAGE4_GENERATED_SOURCE}")

add_dependencies(chaos_stage4_hello_world_object_proof chaos_stage4_hello_world_object_codegen)

add_custom_target(chaos_stage4_hello_world_object_proof_run
    BYPRODUCTS
        "${STAGE4_RUN_STDOUT_LOG}"
        "${STAGE4_RUN_STDERR_LOG}"
        "${STAGE4_RUN_EXIT_CODE_PATH}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE4_RUN_OUTPUT_ROOT}"
    COMMAND "${CMAKE_COMMAND}"
        "-DCHAOS_STAGE4_PROOF_EXE=$<TARGET_FILE:chaos_stage4_hello_world_object_proof>"
        "-DCHAOS_STAGE4_RUN_OUTPUT_ROOT=${STAGE4_RUN_OUTPUT_ROOT}"
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
        RUNTIME_OUTPUT_DIRECTORY "${STAGE4_RUN_OUTPUT_ROOT}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${STAGE4_RUN_OUTPUT_ROOT}")
