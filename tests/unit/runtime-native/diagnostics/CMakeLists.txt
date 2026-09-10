# CMakeLists.txt — Diagnostics test suite
#
# Unit tests for the diagnostics subsystem: EventPipe protocol, layout,
# and bridge modules (P3-C, P3-D).
#
# Built via the unified chaos_native_add_test factory. The factory's default
# lib set (chaos_test_libs_v0) includes chaos_eventpipe + chaos_debugger +
# chaos_runtime_core + chaos_fmt, so no explicit LIBS lists are needed here.

# ── EventPipe protocol unit tests ────────────────────────────────────
chaos_native_add_test(test_ep_protocol
    ep_protocol_test.cpp
    LABELS "unit;diagnostics;eventpipe"
)

chaos_native_add_test(test_ep_event_layout
    ep_event_layout_test.cpp
    LABELS "unit;diagnostics;eventpipe"
)

# ── EventPipe GC bridge tests ───────────────────────────────────────
chaos_native_add_test(test_ep_gc_bridge
    ep_gc_bridge_test.cpp
    LABELS "unit;diagnostics;eventpipe"
)

# ── DAP debugger unit tests ──────────────────────────────────────────
chaos_native_add_test(test_dbg_protocol
    dbg_protocol_test.cpp
    LABELS "unit;diagnostics;debugger"
)

chaos_native_add_test(test_dbg_breakpoint
    dbg_breakpoint_test.cpp
    LABELS "unit;diagnostics;debugger"
)

chaos_native_add_test(test_dbg_stepping
    dbg_stepping_test.cpp
    LABELS "unit;diagnostics;debugger"
)

chaos_native_add_test(test_dbg_stack
    dbg_stack_test.cpp
    LABELS "unit;diagnostics;debugger"
)

chaos_native_add_test(test_dbg_variables
    dbg_variables_test.cpp
    LABELS "unit;diagnostics;debugger"
)

# ── DAP server dispatch unit tests ────────────────────────────────────────
chaos_native_add_test(test_dbg_server
    dbg_server_test.cpp
    LABELS "unit;diagnostics;debugger"
)

# ── DAP integration tests ─────────────────────────────────────────────
chaos_native_add_test(test_dbg_integration
    dbg_integration_test.cpp
    LABELS "integration;diagnostics;debugger"
)

# ── SOS extension unit tests ─────────────────────────────────────────
# Unity-build: sos_test.cpp includes gc_sos.cpp directly.
# CHAOS_SOS_UNIT_TEST is defined to exclude DbgEng-dependent code.
set(SOS_DIR "${CHAOS_CMAKE_ROOT}/src/native/diagnostics/sos")
chaos_native_add_test(test_sos
    sos_test.cpp
    LABELS "unit;diagnostics;sos"
)
target_compile_definitions(test_sos PRIVATE CHAOS_SOS_UNIT_TEST)
target_include_directories(test_sos PRIVATE
    "${SOS_DIR}"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core"
)

# ── EventPipe negative/edge tests (P3-G) ────────────────────────────────
chaos_native_add_test(test_ep_negative
    ep_negative_test.cpp
    LABELS "unit;diagnostics;eventpipe"
)

# ── DAP stress/no-crash tests (P3-G) ────────────────────────────────────
chaos_native_add_test(test_dbg_stress
    dbg_stress_test.cpp
    LABELS "stress;diagnostics;debugger"
)
