// ── Misc stub declarations ─────────────────────────────────────
// Assorted stubs without a clear single domain: Array, Buffer,
// Type marshalling, Culture, GC, Environment, Console, Delegate.
#pragma once

// Array
void    ChaosArrayClear(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayGetLength(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 dimension) noexcept;

// Type marshalling
CHAOS_IL2CPP_INTPTR ChaosStoreInt64(CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStoreFloat32(CHAOS_IL2CPP_FLOAT32 value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosStoreFloat64(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64  ChaosLoadInt64(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_FLOAT64 ChaosLoadFloat64(CHAOS_IL2CPP_INTPTR value) noexcept;

// Buffer
CHAOS_IL2CPP_INT32  ChaosBufferByteLength(CHAOS_IL2CPP_INTPTR array) noexcept;

// Culture
CHAOS_IL2CPP_INTPTR ChaosCultureGetCurrent(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetInvariant(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetCompareInfo(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetDateTimeFormat(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetDisplayName(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetName(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetNumberFormat(CHAOS_IL2CPP_INTPTR culture) noexcept;

// GC
void    ChaosGcCollect(CHAOS_IL2CPP_INT32 generation) noexcept;
CHAOS_IL2CPP_INT32  ChaosGcGetGeneration(CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INT32  ChaosGcGetMaxGeneration(void) noexcept;

// Environment / Console
CHAOS_IL2CPP_INTPTR ChaosEnvironmentGetStackTrace(void) noexcept;
void    ChaosConsoleWriteLine(CHAOS_IL2CPP_INTPTR value) noexcept;

// Delegate
void    ChaosDelegateInitialize(CHAOS_IL2CPP_INTPTR delegate_obj, CHAOS_IL2CPP_INTPTR target, CHAOS_IL2CPP_INTPTR method_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDelegateGetTarget(CHAOS_IL2CPP_INTPTR delegate_obj) noexcept;