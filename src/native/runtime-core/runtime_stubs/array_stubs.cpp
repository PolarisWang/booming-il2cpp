// ── Array stub implementations (all no-op stubs) ───────────────
#include "array_stubs.h"

CHAOS_IL2CPP_INTPTR ChaosArrayEmpty(void) noexcept
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

void ChaosArrayCopy(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INT32 sourceIndex, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT32 destIndex, CHAOS_IL2CPP_INT32 count) noexcept
{
    (void)source;
    (void)sourceIndex;
    (void)dest;
    (void)destIndex;
    (void)count;
}

CHAOS_IL2CPP_INTPTR ChaosArrayCreateInstance(CHAOS_IL2CPP_INTPTR elementType, CHAOS_IL2CPP_INT32 length) noexcept
{
    (void)elementType;
    (void)length;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}

CHAOS_IL2CPP_INT32 ChaosArrayBinarySearch(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    (void)array;
    (void)value;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

CHAOS_IL2CPP_INT32 ChaosArrayIndexOf(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    (void)array;
    (void)value;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

CHAOS_IL2CPP_INT32 ChaosArrayLastIndexOf(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INTPTR value) noexcept
{
    (void)array;
    (void)value;
    return static_cast<CHAOS_IL2CPP_INT32>(0);
}

void ChaosArraySort(CHAOS_IL2CPP_INTPTR array) noexcept
{
    (void)array;
}

void ChaosArrayReverse(CHAOS_IL2CPP_INTPTR array) noexcept
{
    (void)array;
}

CHAOS_IL2CPP_INTPTR ChaosArrayGetValue(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index) noexcept
{
    (void)array;
    (void)index;
    return static_cast<CHAOS_IL2CPP_INTPTR>(0);
}