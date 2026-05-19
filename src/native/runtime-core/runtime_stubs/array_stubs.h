// ── Array stub declarations ────────────────────────────────────
#pragma once

#include <chaos/native_types.h>

extern "C" {

CHAOS_IL2CPP_INTPTR ChaosArrayEmpty(void) noexcept;
void ChaosArrayCopy(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 sourceIndex, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 destIndex, CHAOS_IL2CPP_INT32 count) noexcept;
CHAOS_IL2CPP_INTPTR ChaosArrayCreateInstance(CHAOS_IL2CPP_INTPTR elementType, CHAOS_IL2CPP_INT32 length) noexcept;
CHAOS_IL2CPP_INTPTR ChaosArrayCreateInstance2D(CHAOS_IL2CPP_INTPTR elementType, CHAOS_IL2CPP_INT32 length1, CHAOS_IL2CPP_INT32 length2) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayBinarySearch(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayBinarySearchRange(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 length, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayIndexOf(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayLastIndexOf(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept;
void ChaosArraySort(CHAOS_IL2CPP_INTPTR array) noexcept;
void ChaosArrayReverse(CHAOS_IL2CPP_INTPTR array) noexcept;
CHAOS_IL2CPP_INTPTR ChaosArrayGetValue(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index) noexcept;
CHAOS_IL2CPP_INTPTR ChaosBitConverterGetBytes(CHAOS_IL2CPP_INTPTR unused, CHAOS_IL2CPP_INT32 value) noexcept;

}  // extern "C"