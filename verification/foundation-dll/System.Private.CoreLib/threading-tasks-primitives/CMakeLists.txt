# ── threading-tasks-primitives native skeleton ──────────────────
# This wrapper delegates to the native/ subdirectory so that the
# presets build system picks it up and inherits all project-level
# compiler/linker settings (EH mode, defines, etc.).

add_subdirectory(native)
