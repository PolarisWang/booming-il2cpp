# ── Foundation DLL family native skeletons ──────────────────────────
# Each family's native/ directory contains a standalone CMakeLists.txt
# that defines a unique chaos_family_native_<slug> static library target.
# All can coexist in a single build because target names are unique.

add_subdirectory(array-indexing-copy/native)
add_subdirectory(attributes-custom-metadata/native)
add_subdirectory(boxing-unboxing-casts/native)
add_subdirectory(buffer-memory/native)
add_subdirectory(collections-generic-core/native)
add_subdirectory(convert-char/native)
add_subdirectory(delegate-core-invocation/native)
add_subdirectory(enum-parsing/native)
add_subdirectory(exception-throw-diagnostics/native)
add_subdirectory(generic-nullable-value/native)
add_subdirectory(globalization-culture/native)
add_subdirectory(guid-random-hashcode/native)
add_subdirectory(io-streams-basics/native)
add_subdirectory(math-numerics/native)
add_subdirectory(object-equality-identity/native)
add_subdirectory(primitive-numeric-conversions/native)
add_subdirectory(reflection-member-basics/native)
add_subdirectory(runtime-compiler-services/native)
add_subdirectory(span-memory-buffers/native)
add_subdirectory(string-char-text-core/native)
add_subdirectory(threading-monitor-interlocked/native)
add_subdirectory(threading-tasks-primitives/native)
add_subdirectory(time-date-time-timespan/native)
add_subdirectory(type-runtime-handles/native)
