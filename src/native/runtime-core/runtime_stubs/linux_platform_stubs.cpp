// linux_platform_stubs.cpp — Stub definitions for symbols not available on Linux
//
// Provides minimal stub definitions for:
//   - AOT/external runtime symbols (normally from generated code)
//   - Debugger symbols (normally from chaos_debugger, Windows-only)
//   - Bootstrap functions requiring codegen bridge
//
// These stubs allow test executables to link on Linux.  They are NOT
// functionally correct — they are build-compatibility stubs only.

#include <cstdint>

// ════════════════════════════════════════════════════════════════════════════
// AOT hotpatch module symbol
// ════════════════════════════════════════════════════════════════════════════

// Normally defined in generated code (chaos_generated_module.cpp).
// On Linux (reference build), no AOT module is loaded.
// Weak to allow test object files to override with their own definition.
extern "C" __attribute__((weak)) const int chaos_il2cpp_aot_hotpatch_module = 0;

// ════════════════════════════════════════════════════════════════════════════
// External runtime symbols (from external_runtime module or generated code)
// ════════════════════════════════════════════════════════════════════════════

extern "C" __attribute__((weak)) const int kChaosExternalRuntimeCount = 0;
extern "C" __attribute__((weak)) const void* const kChaosExternalRuntimeSubjects[1] = { nullptr };
extern "C" __attribute__((weak)) const void* const kChaosExternalRuntimeFnTable[1] = { nullptr };

// ════════════════════════════════════════════════════════════════════════════
// Debugger stubs (normally in chaos_debugger, Windows-only)
// ════════════════════════════════════════════════════════════════════════════

namespace chaos::il2cpp::diagnostics {

bool g_dbg_any_breakpoints = false;
bool g_dbg_pause_requested = false;

bool DbgIsStepping() noexcept { return false; }
void DbgClearFrameSnapshot() noexcept {}
void* DbgGetFrameSnapshot() noexcept { return nullptr; }
bool DbgCheckBreakpoint(uint32_t, uint32_t) noexcept { return false; }
void DbgNotifyPaused(uint32_t, uint32_t) noexcept {}
bool DbgShouldStopAtCurrentPosition(int) noexcept { return false; }

}  // namespace chaos::il2cpp::diagnostics
