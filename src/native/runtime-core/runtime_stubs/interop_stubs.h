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

// ── JSON string creation helper ──────────────────────────────
// Creates a managed String from UTF-8 bytes. Used by precompiled JSON
// serializer stubs to return formatted JSON strings without going through
// managed JsonSerializer's reflection path.
CHAOS_IL2CPP_INTPTR ChaosStringCreateFromUtf8(const char* utf8, CHAOS_IL2CPP_INT32 length) noexcept;

// ── Precompiled JSON serialization stubs ─────────────────────
// Bypass managed JsonSerializer for primitive types by formatting values
// directly as JSON strings using snprintf, then wrapping the result as a
// managed String via ChaosStringCreateFromUtf8.
CHAOS_IL2CPP_INTPTR ChaosJsonSerializeInt32(CHAOS_IL2CPP_INT32 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosJsonSerializeInt64(CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosJsonSerializeString(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosJsonSerializeBool(CHAOS_IL2CPP_INT32 value) noexcept;

// ── Precompiled JSON deserialization stubs ───────────────────
// Parse JSON strings back to primitive values using atoi/strtol.
CHAOS_IL2CPP_INT32 ChaosJsonDeserializeInt32(CHAOS_IL2CPP_INTPTR jsonStr) noexcept;
CHAOS_IL2CPP_INT64 ChaosJsonDeserializeInt64(CHAOS_IL2CPP_INTPTR jsonStr) noexcept;
CHAOS_IL2CPP_INT32 ChaosJsonDeserializeBool(CHAOS_IL2CPP_INTPTR jsonStr) noexcept;
