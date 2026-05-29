# ── Foundation DLL verification native skeletons ────────────────────
# Each assembly subdirectory adds its family native skeleton targets.
# These are EXCLUDE_FROM_ALL and only build on explicit request.
# New assemblies are auto-discovered — just drop a CMakeLists.txt in the
# assembly directory to include it.

file(GLOB ASSEMBLY_DIRS LIST_DIRECTORIES true RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" */CMakeLists.txt)
foreach(ASSEMBLY_CMAKELIST ${ASSEMBLY_DIRS})
    get_filename_component(ASSEMBLY_DIR ${ASSEMBLY_CMAKELIST} DIRECTORY)
    add_subdirectory(${ASSEMBLY_DIR})
endforeach()
