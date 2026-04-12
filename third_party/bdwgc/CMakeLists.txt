cmake_minimum_required(VERSION 3.20)

project(chaos_bdwgc LANGUAGES C)

add_library(chaos_bdwgc STATIC
    alloc.c
    allchblk.c
    blacklst.c
    checksums.c
    dbg_mlc.c
    dyn_load.c
    finalize.c
    fnlz_mlc.c
    gc_dlopen.c
    gcj_mlc.c
    headers.c
    mach_dep.c
    malloc.c
    mallocx.c
    mark.c
    mark_rts.c
    misc.c
    new_hblk.c
    obj_map.c
    os_dep.c
    ptr_chck.c
    reclaim.c
    specific.c
    thread_local_alloc.c
    typd_mlc.c
    vector_mlc.c)

if(WIN32)
    target_sources(chaos_bdwgc PRIVATE win32_threads.c)
    target_compile_definitions(chaos_bdwgc PRIVATE
        GC_WIN32_THREADS
        _CRT_SECURE_NO_DEPRECATE)
elseif(APPLE)
    target_sources(chaos_bdwgc PRIVATE
        darwin_stop_world.c
        pthread_support.c
        pthread_stop_world.c
        pthread_start.c)
    target_compile_definitions(chaos_bdwgc PRIVATE GC_PTHREADS)
    target_link_libraries(chaos_bdwgc PRIVATE pthread)
else()
    target_sources(chaos_bdwgc PRIVATE
        pthread_support.c
        pthread_stop_world.c
        pthread_start.c)
    target_compile_definitions(chaos_bdwgc PRIVATE GC_PTHREADS)
    target_link_libraries(chaos_bdwgc PRIVATE pthread)
endif()

target_compile_definitions(chaos_bdwgc PUBLIC
    GC_THREADS
    ALL_INTERIOR_POINTERS
    GC_NOT_DLL
    PARALLEL_MARK
    THREAD_LOCAL_ALLOC
    DONT_ADD_BYTE_AT_END
    NO_DEBUGGING)

# GC_BUILTIN_ATOMIC uses C11 __atomic builtins — only available on GCC/Clang
if(NOT MSVC)
    target_compile_definitions(chaos_bdwgc PUBLIC GC_BUILTIN_ATOMIC)
else()
    # MSVC: use libatomic_ops fallback (uses _InterlockedXxx internally)
    target_include_directories(chaos_bdwgc PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/libatomic_ops/src")
    target_sources(chaos_bdwgc PRIVATE
        libatomic_ops/src/atomic_ops.c)
endif()

# Suppress warnings from BDWGC internals
if(MSVC)
    target_compile_options(chaos_bdwgc PRIVATE /w)
else()
    target_compile_options(chaos_bdwgc PRIVATE -w)
endif()

target_include_directories(chaos_bdwgc PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include")
