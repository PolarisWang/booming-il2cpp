# ── System.Private.CoreLib native verification skeletons ────────────
# Each family subdirectory with a CMakeLists.txt is included here.
# Families without native skeletons are skipped gracefully.

file(GLOB family_entries LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/*")
foreach(family_dir ${family_entries})
    if(EXISTS "${family_dir}/CMakeLists.txt")
        get_filename_component(family_name "${family_dir}" NAME)
        add_subdirectory("${family_name}")
    endif()
endforeach()
