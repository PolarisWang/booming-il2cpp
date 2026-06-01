# ── Diagnostics umbrella ──────────────────────────────────────────────────
# This CMakeLists.txt serves as a single entry point for all diagnostics
# sub-modules. Each sub-module maintains its own independent CMakeLists.txt.
#
# Sub-modules:
#   debugger/   — DAP (Debug Adapter Protocol) debugger (v2)
#   eventpipe/  — EventPipe diagnostic IPC (Windows named pipes)
#   sos/        — SOS Debugger Extension (WinDbg, Windows only, requires DbgEng SDK)
#
# v1 debug_agent (binary protocol) has been removed. All consumers should
# link directly against: chaos_debugger, chaos_eventpipe, chaos_gc_sos.
#
# Legacy: hotupdate_events.h is header-only; included here for discovery.
# Consumers include it via <diagnostics/hotupdate_events.h> or path.

add_subdirectory(debugger)
add_subdirectory(eventpipe)

# SOS requires DbgEng SDK — the root CMakeLists.txt checks this conditionally.
# This umbrella defers the check to the parent scope.
if(COMMAND add_subdirectory AND EXISTS "C:/Program Files (x86)/Windows Kits/10/Debuggers/inc/dbgeng.h")
    add_subdirectory(sos)
endif()
