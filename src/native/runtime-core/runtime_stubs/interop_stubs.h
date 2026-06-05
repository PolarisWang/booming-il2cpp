// ── Interop stub declarations ──────────────────────────────────
// Simple interop stubs for Marshal/COM methods that don't need
// prebuilt library linkage. Compiled from source via TPG.
#pragma once

#include <chaos/native_types.h>

// Marshal P/Invoke error helpers
void    ChaosMarshalSetLastPInvokeError(CHAOS_IL2CPP_INT32 error) noexcept;
CHAOS_IL2CPP_INT32 ChaosMarshalGetLastPInvokeError(void) noexcept;

// Exception code/pointers stubs
CHAOS_IL2CPP_INT32 ChaosMarshalGetExceptionCode(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosMarshalGetExceptionPointers(void) noexcept;

// GetHRForLastWin32Error: converts last P/Invoke error to HRESULT
CHAOS_IL2CPP_INT32 ChaosMarshalGetHRForLastWin32Error(void) noexcept;

// COM cleanup check stub
CHAOS_IL2CPP_INT32 ChaosMarshalAreComObjectsAvailableForCleanup(void) noexcept;
