# asan.cmake — AddressSanitizer configuration for chaos-il2cpp native tests.
#
# Include this file to enable ASan for a specific test target.
# Usage in add_chaos_test:
#   add_chaos_test(test_name test.cpp LIBS ... SANITIZE "asan")
#
# Or include once in the root CMakeLists.txt to enable globally:
#   include(config/sanitizers/asan.cmake)
#   set(CHAOS_SANITIZE_ASAN ON)

if(NOT COMMAND chaos_enable_asan)
    macro(chaos_enable_asan target)
        target_compile_options(${target} PRIVATE /fsanitize=address)
        target_link_options(${target} PRIVATE /fsanitize=address /FORCE:MULTIPLE)
        target_compile_definitions(${target} PRIVATE
            _DISABLE_VECTOR_ANNOTATION
            _DISABLE_STRING_ANNOTATION)

        # Locate ASan runtime DLL and copy to output directory.
        find_file(CHAOS_ASAN_DLL clang_rt.asan_dynamic-x86_64.dll
            PATHS
                "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64"
                "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64"
                "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64"
                "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64"
        )
        if(CHAOS_ASAN_DLL)
            message(STATUS "ASan DLL found for ${target}: ${CHAOS_ASAN_DLL}")
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${CHAOS_ASAN_DLL}" "$<TARGET_FILE_DIR:${target}>/"
                COMMENT "Copy ASan DLL to $<TARGET_FILE_DIR:${target}>"
            )
        else()
            message(WARNING "ASan DLL not found — ${target} may fail with 0xc0000135")
        endif()

        # Copy suppressions file.
        get_filename_component(CHAOS_PROJECT_ROOT
            "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CHAOS_PROJECT_ROOT}/asan-suppressions.txt"
                "$<TARGET_FILE_DIR:${target}>/"
            COMMENT "Copy asan-suppressions.txt to $<TARGET_FILE_DIR:${target}>"
        )
    endmacro()
endif()
