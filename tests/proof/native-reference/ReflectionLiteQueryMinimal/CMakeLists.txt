cmake_minimum_required(VERSION 3.20)

project(chaos_stage5b_reflection_lite_query_minimal_proof LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(STAGE5B_PROOF_INPUT_PROJECT "${REPO_ROOT}/tests/proof/input/ReflectionLiteQueryMinimal/ReflectionLiteQueryMinimal.csproj")
set(STAGE5B_PROOF_INPUT_DLL "${REPO_ROOT}/tests/proof/input/ReflectionLiteQueryMinimal/bin/Release/net8.0/ReflectionLiteQueryMinimal.dll")
set(STAGE5B_DRIVER_PROJECT "${REPO_ROOT}/src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj")
set(STAGE5B_DRIVER_DLL "${REPO_ROOT}/src/managed/Chaos.IL2CPP.Driver/bin/Release/net8.0/Chaos.IL2CPP.Driver.dll")
set(STAGE5B_MANAGED_CLOSURE_ROOT "${REPO_ROOT}/artifacts/proof/managed-closure/ReflectionLiteQueryMinimal")
set(STAGE5B_MANAGED_CLOSURE_TYPED_IL "${STAGE5B_MANAGED_CLOSURE_ROOT}/typed-il-ir.json")
set(STAGE5B_MANAGED_CLOSURE_AOT_MANIFEST "${STAGE5B_MANAGED_CLOSURE_ROOT}/aot-manifest.json")
set(STAGE5B_MANAGED_CLOSURE_METADATA "${STAGE5B_MANAGED_CLOSURE_ROOT}/metadata-registration.json")
set(STAGE5B_MANAGED_CLOSURE_CODE "${STAGE5B_MANAGED_CLOSURE_ROOT}/code-registration.json")
set(STAGE5B_MANAGED_CLOSURE_MANIFEST "${STAGE5B_MANAGED_CLOSURE_ROOT}/closure.manifest.json")
set(STAGE5B_OUTPUT_ROOT "${REPO_ROOT}/artifacts/proof/native-reference/ReflectionLiteQueryMinimal")
set(STAGE5B_GENERATED_SOURCE "${STAGE5B_OUTPUT_ROOT}/generated/native-reference.generated.cpp")
set(STAGE5B_PROOF_PLAN "${STAGE5B_OUTPUT_ROOT}/native-proof.plan.json")
set(STAGE5B_PROOF_MANIFEST "${STAGE5B_OUTPUT_ROOT}/native-proof.manifest.json")
set(STAGE5B_RUN_OUTPUT_ROOT "${STAGE5B_OUTPUT_ROOT}/run")
set(STAGE5B_RUN_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/RunNativeReferenceProof.cmake")
set(STAGE5B_RUN_STDOUT_LOG "${STAGE5B_RUN_OUTPUT_ROOT}/stdout.log")
set(STAGE5B_RUN_STDERR_LOG "${STAGE5B_RUN_OUTPUT_ROOT}/stderr.log")
set(STAGE5B_RUN_EXIT_CODE_PATH "${STAGE5B_RUN_OUTPUT_ROOT}/exit-code.txt")
set(STAGE5B_HOST_INPUT_BUILD_STAMP "${STAGE5B_OUTPUT_ROOT}/.host-input-build.stamp")
set(STAGE5B_DRIVER_BUILD_STAMP "${STAGE5B_OUTPUT_ROOT}/.driver-build.stamp")
set(STAGE5B_MANAGED_CLOSURE_STAMP "${STAGE5B_OUTPUT_ROOT}/.managed-closure.stamp")
set(STAGE5B_CODEGEN_STAMP "${STAGE5B_OUTPUT_ROOT}/.codegen.stamp")

set_source_files_properties("${STAGE5B_GENERATED_SOURCE}" PROPERTIES GENERATED TRUE)

add_custom_command(
    OUTPUT "${STAGE5B_HOST_INPUT_BUILD_STAMP}"
    BYPRODUCTS
        "${STAGE5B_PROOF_INPUT_DLL}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE5B_OUTPUT_ROOT}"
    COMMAND dotnet build "${STAGE5B_PROOF_INPUT_PROJECT}" -c Release
    COMMAND "${CMAKE_COMMAND}" -E touch "${STAGE5B_HOST_INPUT_BUILD_STAMP}"
    DEPENDS
        "${STAGE5B_PROOF_INPUT_PROJECT}"
    WORKING_DIRECTORY "${REPO_ROOT}"
    VERBATIM)

add_custom_command(
    OUTPUT "${STAGE5B_DRIVER_BUILD_STAMP}"
    BYPRODUCTS
        "${STAGE5B_DRIVER_DLL}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE5B_OUTPUT_ROOT}"
    COMMAND dotnet build "${STAGE5B_DRIVER_PROJECT}" -c Release
    COMMAND "${CMAKE_COMMAND}" -E touch "${STAGE5B_DRIVER_BUILD_STAMP}"
    DEPENDS
        "${STAGE5B_DRIVER_PROJECT}"
    WORKING_DIRECTORY "${REPO_ROOT}"
    VERBATIM)

add_custom_command(
    OUTPUT "${STAGE5B_MANAGED_CLOSURE_STAMP}"
    BYPRODUCTS
        "${STAGE5B_MANAGED_CLOSURE_TYPED_IL}"
        "${STAGE5B_MANAGED_CLOSURE_AOT_MANIFEST}"
        "${STAGE5B_MANAGED_CLOSURE_METADATA}"
        "${STAGE5B_MANAGED_CLOSURE_CODE}"
        "${STAGE5B_MANAGED_CLOSURE_MANIFEST}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE5B_OUTPUT_ROOT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE5B_MANAGED_CLOSURE_ROOT}"
    COMMAND dotnet "${STAGE5B_DRIVER_DLL}" "${STAGE5B_PROOF_INPUT_DLL}" "${STAGE5B_MANAGED_CLOSURE_ROOT}"
    COMMAND "${CMAKE_COMMAND}" -E touch "${STAGE5B_MANAGED_CLOSURE_STAMP}"
    DEPENDS
        "${STAGE5B_HOST_INPUT_BUILD_STAMP}"
        "${STAGE5B_DRIVER_BUILD_STAMP}"
    WORKING_DIRECTORY "${REPO_ROOT}"
    VERBATIM)

add_custom_command(
    OUTPUT "${STAGE5B_CODEGEN_STAMP}"
    BYPRODUCTS
        "${STAGE5B_GENERATED_SOURCE}"
        "${STAGE5B_PROOF_PLAN}"
        "${STAGE5B_PROOF_MANIFEST}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE5B_OUTPUT_ROOT}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE5B_OUTPUT_ROOT}/generated"
    COMMAND dotnet "${STAGE5B_DRIVER_DLL}" emit-native-reference "${STAGE5B_MANAGED_CLOSURE_ROOT}" "${STAGE5B_OUTPUT_ROOT}"
    COMMAND "${CMAKE_COMMAND}" -E touch "${STAGE5B_CODEGEN_STAMP}"
    DEPENDS
        "${STAGE5B_MANAGED_CLOSURE_STAMP}"
        "${STAGE5B_MANAGED_CLOSURE_TYPED_IL}"
        "${STAGE5B_MANAGED_CLOSURE_AOT_MANIFEST}"
        "${STAGE5B_MANAGED_CLOSURE_METADATA}"
        "${STAGE5B_MANAGED_CLOSURE_CODE}"
        "${STAGE5B_MANAGED_CLOSURE_MANIFEST}"
    WORKING_DIRECTORY "${REPO_ROOT}"
    VERBATIM)

add_custom_target(chaos_stage5b_reflection_lite_query_minimal_host_input_build
    DEPENDS
        "${STAGE5B_HOST_INPUT_BUILD_STAMP}")

add_custom_target(chaos_stage5b_reflection_lite_query_minimal_driver_build
    DEPENDS
        "${STAGE5B_DRIVER_BUILD_STAMP}")

add_custom_target(chaos_stage5b_reflection_lite_query_minimal_managed_closure
    DEPENDS
        "${STAGE5B_MANAGED_CLOSURE_STAMP}")

add_dependencies(chaos_stage5b_reflection_lite_query_minimal_managed_closure
    chaos_stage5b_reflection_lite_query_minimal_host_input_build
    chaos_stage5b_reflection_lite_query_minimal_driver_build)

add_custom_target(chaos_stage5b_reflection_lite_query_minimal_codegen
    DEPENDS
        "${STAGE5B_CODEGEN_STAMP}")

add_dependencies(chaos_stage5b_reflection_lite_query_minimal_codegen
    chaos_stage5b_reflection_lite_query_minimal_managed_closure)

add_executable(chaos_stage5b_reflection_lite_query_minimal_proof EXCLUDE_FROM_ALL
    main.cpp
    "${STAGE5B_GENERATED_SOURCE}")

add_dependencies(chaos_stage5b_reflection_lite_query_minimal_proof
    chaos_stage5b_reflection_lite_query_minimal_codegen)

add_custom_target(chaos_stage5b_reflection_lite_query_minimal_proof_run
    BYPRODUCTS
        "${STAGE5B_RUN_STDOUT_LOG}"
        "${STAGE5B_RUN_STDERR_LOG}"
        "${STAGE5B_RUN_EXIT_CODE_PATH}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${STAGE5B_RUN_OUTPUT_ROOT}"
    COMMAND "${CMAKE_COMMAND}"
        "-DCHAOS_STAGE5B_PROOF_EXE=$<TARGET_FILE:chaos_stage5b_reflection_lite_query_minimal_proof>"
        "-DCHAOS_STAGE5B_RUN_OUTPUT_ROOT=${STAGE5B_RUN_OUTPUT_ROOT}"
        "-DCHAOS_STAGE5B_STDOUT_PATH=${STAGE5B_RUN_STDOUT_LOG}"
        "-DCHAOS_STAGE5B_STDERR_PATH=${STAGE5B_RUN_STDERR_LOG}"
        "-DCHAOS_STAGE5B_EXIT_CODE_PATH=${STAGE5B_RUN_EXIT_CODE_PATH}"
        -P "${STAGE5B_RUN_SCRIPT}"
    DEPENDS
        chaos_stage5b_reflection_lite_query_minimal_proof
    VERBATIM)

target_compile_features(chaos_stage5b_reflection_lite_query_minimal_proof PRIVATE cxx_std_17)
target_include_directories(
    chaos_stage5b_reflection_lite_query_minimal_proof
    PRIVATE
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/src/native/runtime-core"
        "${REPO_ROOT}/src/native/bootstrap"
        "${REPO_ROOT}/src/native/support")
target_link_libraries(
    chaos_stage5b_reflection_lite_query_minimal_proof
    PRIVATE
        chaos_runtime_core
        chaos_bootstrap
        chaos_support)

set_target_properties(
    chaos_stage5b_reflection_lite_query_minimal_proof
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${STAGE5B_RUN_OUTPUT_ROOT}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${STAGE5B_RUN_OUTPUT_ROOT}")
