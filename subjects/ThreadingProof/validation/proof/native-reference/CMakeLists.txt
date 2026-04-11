cmake_minimum_required(VERSION 3.20)

project(chaos_subject_reference_proof LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../../../" ABSOLUTE)
set(CHAOS_SUBJECT_VARIANT "CHECK" CACHE STRING "Variant selector for the current subject reference fixture")
set_property(CACHE CHAOS_SUBJECT_VARIANT PROPERTY STRINGS CHECK PROFILE SHIP)

set(CHAOS_SUBJECT_GENERATED_ROOT
    "${REPO_ROOT}/artifacts/subjects/ThreadingProof/runs/subject-exec/analysis/generated"
    CACHE PATH "Generated bucket root for the current subject reference fixture")
set(CHAOS_SUBJECT_BUILD_OUT_ROOT
    "${REPO_ROOT}/artifacts/subjects/ThreadingProof/runs/subject-exec/matrices/windows-threading-check/build/out"
    CACHE PATH "Build output root for the current subject reference fixture")
set(CHAOS_SUBJECT_RUNTIME_ROOT
    "${REPO_ROOT}/artifacts/subjects/ThreadingProof/runs/subject-exec/matrices/windows-threading-check/runtime"
    CACHE PATH "Runtime evidence root for the current subject reference fixture")

set(CHAOS_SUBJECT_GENERATED_SOURCE "${CHAOS_SUBJECT_GENERATED_ROOT}/generated/native-reference.generated.cpp")
set(CHAOS_SUBJECT_RUN_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/RunNativeReferenceProof.cmake")
set(CHAOS_SUBJECT_STDOUT_LOG "${CHAOS_SUBJECT_RUNTIME_ROOT}/stdout.log")
set(CHAOS_SUBJECT_STDERR_LOG "${CHAOS_SUBJECT_RUNTIME_ROOT}/stderr.log")
set(CHAOS_SUBJECT_EXIT_CODE_PATH "${CHAOS_SUBJECT_RUNTIME_ROOT}/exit-code.txt")

if(NOT EXISTS "${CHAOS_SUBJECT_GENERATED_SOURCE}")
    message(FATAL_ERROR "Missing generated subject source: ${CHAOS_SUBJECT_GENERATED_SOURCE}")
endif()

function(chaos_apply_subject_variant target_name)
    if(CHAOS_SUBJECT_VARIANT STREQUAL "CHECK")
        target_compile_definitions(
            "${target_name}"
            PRIVATE
                CHAOS_VARIANT_CHECK
                "CHAOS_VARIANT_NAME=\"CHECK\"")
        target_compile_options(
            "${target_name}"
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/Od /Zi>
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O0 -g>)
        return()
    endif()

    if(CHAOS_SUBJECT_VARIANT STREQUAL "PROFILE")
        target_compile_definitions(
            "${target_name}"
            PRIVATE
                CHAOS_VARIANT_PROFILE
                "CHAOS_VARIANT_NAME=\"PROFILE\"")
        target_compile_options(
            "${target_name}"
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/O2 /DNDEBUG>
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3 -DNDEBUG>)
        return()
    endif()

    if(CHAOS_SUBJECT_VARIANT STREQUAL "SHIP")
        target_compile_definitions(
            "${target_name}"
            PRIVATE
                CHAOS_VARIANT_SHIP
                "CHAOS_VARIANT_NAME=\"SHIP\"")
        target_compile_options(
            "${target_name}"
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/O2 /GL /DNDEBUG>
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3 -DNDEBUG>)
        target_link_options(
            "${target_name}"
            PRIVATE
                $<$<CXX_COMPILER_ID:MSVC>:/LTCG>)
        return()
    endif()

    message(FATAL_ERROR "Unsupported CHAOS_SUBJECT_VARIANT='${CHAOS_SUBJECT_VARIANT}'")
endfunction()

add_executable(chaos_subject_reference_proof EXCLUDE_FROM_ALL
    main.cpp
    "${CHAOS_SUBJECT_GENERATED_SOURCE}")

add_custom_target(chaos_subject_reference_proof_run
    BYPRODUCTS
        "${CHAOS_SUBJECT_STDOUT_LOG}"
        "${CHAOS_SUBJECT_STDERR_LOG}"
        "${CHAOS_SUBJECT_EXIT_CODE_PATH}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CHAOS_SUBJECT_RUNTIME_ROOT}"
    COMMAND "${CMAKE_COMMAND}"
        "-DCHAOS_SUBJECT_REFERENCE_PROOF_EXE=$<TARGET_FILE:chaos_subject_reference_proof>"
        "-DCHAOS_SUBJECT_REFERENCE_RUN_OUTPUT_ROOT=${CHAOS_SUBJECT_RUNTIME_ROOT}"
        "-DCHAOS_SUBJECT_REFERENCE_STDOUT_PATH=${CHAOS_SUBJECT_STDOUT_LOG}"
        "-DCHAOS_SUBJECT_REFERENCE_STDERR_PATH=${CHAOS_SUBJECT_STDERR_LOG}"
        "-DCHAOS_SUBJECT_REFERENCE_EXIT_CODE_PATH=${CHAOS_SUBJECT_EXIT_CODE_PATH}"
        -P "${CHAOS_SUBJECT_RUN_SCRIPT}"
    DEPENDS
        chaos_subject_reference_proof
    VERBATIM)

target_compile_features(chaos_subject_reference_proof PRIVATE cxx_std_17)
target_include_directories(
    chaos_subject_reference_proof
    PRIVATE
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/src/native/runtime-core"
        "${REPO_ROOT}/src/native/bootstrap"
        "${REPO_ROOT}/src/native/support")
target_link_libraries(
    chaos_subject_reference_proof
    PRIVATE
        chaos_runtime_core
        chaos_bootstrap
        chaos_support)
chaos_apply_subject_variant(chaos_subject_reference_proof)

set_target_properties(
    chaos_subject_reference_proof
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CHAOS_SUBJECT_BUILD_OUT_ROOT}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CHAOS_SUBJECT_BUILD_OUT_ROOT}")
