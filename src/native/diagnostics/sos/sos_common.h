/// sos_common.h — Shared DbgEng infrastructure for chaos_gc_sos.dll
///
/// Both gc_sos.cpp and jit_sos.cpp are compiled into chaos_gc_sos.dll.
/// The DbgEng interface pointers, helpers, and DebugExtensionInitialize/
/// Uninitialize are shared.  gc_sos.cpp owns the lifecycle; jit_sos.cpp
/// consumes the exported globals and helpers via this header.
///
/// Convention: gc_sos.cpp defines the globals (without static) and the
/// helper functions (without static).  Both .cpp files include this header.

#ifndef CHAOS_IL2CPP_SOS_COMMON_H_
#define CHAOS_IL2CPP_SOS_COMMON_H_

#include <windows.h>
#include <dbgeng.h>

#include <cstdarg>
#include <cstdio>

// ── DbgEng interface pointers (defined in gc_sos.cpp) ──────────────────────
extern IDebugClient*    g_sos_client;
extern IDebugControl*   g_sos_control;
extern IDebugSymbols*   g_sos_symbols;
extern IDebugDataSpaces* g_sos_data;

// ── Helper functions (defined in gc_sos.cpp) ───────────────────────────────
void SosPrint(PCSTR fmt, ...) noexcept;
bool SosReadTarget(ULONG64 offset, void* buf, ULONG size) noexcept;
ULONG64 SosResolvePointer(PCSTR name) noexcept;

#endif  // CHAOS_IL2CPP_SOS_COMMON_H_
