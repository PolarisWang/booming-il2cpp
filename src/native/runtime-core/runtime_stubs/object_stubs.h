// ── Object / RuntimeHelpers stub declarations ─────────────────
#pragma once

CHAOS_IL2CPP_INTPTR ChaosObjectEqualsStatic(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept;
void    ChaosObjectCtor(CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INT32 ChaosObjectGetHashCode(CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INTPTR ChaosObjectToString(CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INTPTR ChaosObjectGetType(CHAOS_IL2CPP_INTPTR obj) noexcept;

CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersEquals(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept;
CHAOS_IL2CPP_INT32  ChaosRuntimeHelpersGetHashCode(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersGetObjectValue(CHAOS_IL2CPP_INTPTR value) noexcept;

CHAOS_IL2CPP_INTPTR ChaosObjectMemberwiseClone(CHAOS_IL2CPP_INTPTR obj) noexcept;

/// RuntimeHelpers.GetUninitializedObject(Type) — allocates uninitialized
/// object via ChaOS GC (GcAllocateFast), bypassing constructor call.
CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersGetUninitializedObject(CHAOS_IL2CPP_INTPTR type_handle) noexcept;