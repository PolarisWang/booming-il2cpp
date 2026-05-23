# CMakeLists.txt — Diagnostics test suite
#
# Unit tests for the diagnostics subsystem: EventPipe protocol, layout,
# and bridge modules (P3-C, P3-D).

# ── Imported library alias ───────────────────────────────────────────
# chaos_eventpipe is prebuilt and imported by the parent CMakeLists.txt.
# GC bridge tests also need chaos_runtime_core for GcRegisterEventCallback.

# ── EventPipe protocol unit tests ────────────────────────────────────
add_chaos_test(test_ep_protocol
    ep_protocol_test.cpp
    LABELS "unit;diagnostics;eventpipe"
)

add_chaos_test(test_ep_event_layout
    ep_event_layout_test.cpp
    LABELS "unit;diagnostics;eventpipe"
)

# ── EventPipe GC bridge tests ───────────────────────────────────────
add_chaos_test(test_ep_gc_bridge
    ep_gc_bridge_test.cpp
    LIBS chaos_eventpipe chaos_runtime_core chaos_fmt
    LABELS "unit;diagnostics;eventpipe"
)

# ── DAP debugger unit tests ──────────────────────────────────────────
add_chaos_test(test_dbg_protocol
    dbg_protocol_test.cpp
    LABELS "unit;diagnostics;debugger"
)

add_chaos_test(test_dbg_breakpoint
    dbg_breakpoint_test.cpp
    LIBS chaos_debugger chaos_runtime_core chaos_fmt
    LABELS "unit;diagnostics;debugger"
)

add_chaos_test(test_dbg_stepping
    dbg_stepping_test.cpp
    LIBS chaos_debugger chaos_runtime_core chaos_fmt
    LABELS "unit;diagnostics;debugger"
)

add_chaos_test(test_dbg_stack
    dbg_stack_test.cpp
    LIBS chaos_debugger chaos_runtime_core chaos_fmt
    LABELS "unit;diagnostics;debugger"
)

add_chaos_test(test_dbg_variables
    dbg_variables_test.cpp
    LIBS chaos_debugger chaos_runtime_core chaos_fmt
    LABELS "unit;diagnostics;debugger"
)

# ── DAP server dispatch unit tests ────────────────────────────────────────
add_chaos_test(test_dbg_server
    dbg_server_test.cpp
    LIBS chaos_debugger chaos_runtime_core chaos_fmt
    LABELS "unit;diagnostics;debugger"
)

# ── DAP integration tests ─────────────────────────────────────────────
add_chaos_test(test_dbg_integration
    dbg_integration_test.cpp
    LIBS chaos_debugger chaos_runtime_core chaos_fmt
    LABELS "integration;diagnostics;debugger"
)

# ── EventPipe receiver unit tests ─────────────────────────────────────
# Compiles chaos_diag.cpp (without main) as part of the test executable.
set(EP_RECEIVER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/native/diagnostics/eventpipe/receiver")
add_chaos_test(test_ep_receiver
    ep_receiver_test.cpp
    "${EP_RECEIVER_DIR}/chaos_diag.cpp"
    LIBS chaos_eventpipe chaos_runtime_core chaos_fmt
    LABELS "unit;diagnostics;eventpipe"
)
target_compile_definitions(test_ep_receiver PRIVATE CHAOS_DIAG_UNIT_TEST)
target_include_directories(test_ep_receiver PRIVATE "${EP_RECEIVER_DIR}")

# ── SOS extension unit tests ─────────────────────────────────────────
# Unity-build: sos_test.cpp includes gc_sos.cpp directly.
# CHAOS_SOS_UNIT_TEST is defined to exclude DbgEng-dependent code.
set(SOS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/native/diagnostics/sos")
add_chaos_test(test_sos
    sos_test.cpp
    LIBS chaos_runtime_core chaos_fmt
    LABELS "unit;diagnostics;sos"
)
target_compile_definitions(test_sos PRIVATE CHAOS_SOS_UNIT_TEST)
target_include_directories(test_sos PRIVATE
    "${SOS_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/native/runtime-core"
)

# ── EventPipe negative/edge tests (P3-G) ────────────────────────────────
add_chaos_test(test_ep_negative
    ep_negative_test.cpp
    LIBS chaos_eventpipe chaos_fmt
    LABELS "unit;diagnostics;eventpipe"
)

# ── DAP stress/no-crash tests (P3-G) ────────────────────────────────────
add_chaos_test(test_dbg_stress
    dbg_stress_test.cpp
    LIBS chaos_debugger chaos_runtime_core chaos_fmt
    LABELS "stress;diagnostics;debugger"
)
