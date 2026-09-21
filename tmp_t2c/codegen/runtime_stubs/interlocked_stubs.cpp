// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// interlocked_stubs.cpp — Interlocked/Volatile implementations
#include <chaos/native_types.h>
#include <atomic>

#include "generated_code_compat.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

void ChaosInterlockedMemoryBarrier(void) noexcept
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
}

CHAOS_IL2CPP_INT64 ChaosInterlockedReadInt64(CHAOS_IL2CPP_INTPTR location) noexcept
{
    auto* typedLocation = reinterpret_cast<CHAOS_IL2CPP_INT64*>(location);
    return std::atomic_load_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT64>*>(typedLocation),
        std::memory_order_seq_cst);
}

CHAOS_IL2CPP_INT64 ChaosInterlockedIncrementInt64(CHAOS_IL2CPP_INTPTR location) noexcept
{
    auto* typedLocation = reinterpret_cast<CHAOS_IL2CPP_INT64*>(location);
    return std::atomic_fetch_add_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT64>*>(typedLocation),
        CHAOS_IL2CPP_INT64(1),
        std::memory_order_seq_cst) + 1;
}

CHAOS_IL2CPP_INT64 ChaosInterlockedDecrementInt64(CHAOS_IL2CPP_INTPTR location) noexcept
{
    auto* typedLocation = reinterpret_cast<CHAOS_IL2CPP_INT64*>(location);
    return std::atomic_fetch_sub_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT64>*>(typedLocation),
        CHAOS_IL2CPP_INT64(1),
        std::memory_order_seq_cst) - 1;
}

CHAOS_IL2CPP_INT64 ChaosInterlockedExchangeInt64(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT64 value) noexcept
{
    auto* typedLocation = reinterpret_cast<CHAOS_IL2CPP_INT64*>(location);
    return std::atomic_exchange_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT64>*>(typedLocation),
        value,
        std::memory_order_seq_cst);
}

CHAOS_IL2CPP_INT32 ChaosInterlockedCompareExchangeInt32(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 comparand) noexcept
{
    auto* typedLocation = reinterpret_cast<CHAOS_IL2CPP_INT32*>(location);
    return std::atomic_compare_exchange_strong_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT32>*>(typedLocation),
        &comparand,
        value,
        std::memory_order_seq_cst,
        std::memory_order_seq_cst);
}

CHAOS_IL2CPP_INT64 ChaosInterlockedCompareExchangeInt64(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT64 value, CHAOS_IL2CPP_INT64 comparand) noexcept
{
    auto* typedLocation = reinterpret_cast<CHAOS_IL2CPP_INT64*>(location);
    std::atomic_compare_exchange_strong_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT64>*>(typedLocation),
        &comparand,
        value,
        std::memory_order_seq_cst,
        std::memory_order_seq_cst);
    return comparand;
}

void ChaosInterlockedStoreNoBarrier(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT64 value) noexcept
{
    auto* typedLocation = reinterpret_cast<CHAOS_IL2CPP_INT64*>(location);
    std::atomic_store_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT64>*>(typedLocation),
        value,
        std::memory_order_relaxed);
}

CHAOS_IL2CPP_INT32 ChaosVolatileRead(CHAOS_IL2CPP_INTPTR ptr) noexcept
{
    return std::atomic_load_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT32>*>(ptr),
        std::memory_order_acquire);
}

void ChaosVolatileWrite(CHAOS_IL2CPP_INTPTR ptr, CHAOS_IL2CPP_INT32 value) noexcept
{
    std::atomic_store_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT32>*>(ptr),
        value,
        std::memory_order_release);
}

// ═══════════════════════════════════════════════════════════════
// Missing Int32 interlocked stubs — 11 methods taking 71-109ms
// each due to interpreter fallback (no native stub available).
// ═══════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INT32 ChaosInterlockedIncrementInt32(CHAOS_IL2CPP_INTPTR location) noexcept
{
    auto* typedLocation = reinterpret_cast<CHAOS_IL2CPP_INT32*>(location);
    return std::atomic_fetch_add_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT32>*>(typedLocation),
        CHAOS_IL2CPP_INT32(1),
        std::memory_order_seq_cst) + 1;
}

CHAOS_IL2CPP_INT32 ChaosInterlockedDecrementInt32(CHAOS_IL2CPP_INTPTR location) noexcept
{
    auto* typedLocation = reinterpret_cast<CHAOS_IL2CPP_INT32*>(location);
    return std::atomic_fetch_sub_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT32>*>(typedLocation),
        CHAOS_IL2CPP_INT32(1),
        std::memory_order_seq_cst) - 1;
}

CHAOS_IL2CPP_INT32 ChaosInterlockedExchangeInt32(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT32 value) noexcept
{
    auto* typedLocation = reinterpret_cast<CHAOS_IL2CPP_INT32*>(location);
    return std::atomic_exchange_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT32>*>(typedLocation),
        value,
        std::memory_order_seq_cst);
}

CHAOS_IL2CPP_INT32 ChaosInterlockedAddInt32(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT32 value) noexcept
{
    auto* typedLocation = reinterpret_cast<CHAOS_IL2CPP_INT32*>(location);
    return std::atomic_fetch_add_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT32>*>(typedLocation),
        value,
        std::memory_order_seq_cst) + value;
}

CHAOS_IL2CPP_INT64 ChaosInterlockedAddInt64(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT64 value) noexcept
{
    auto* typedLocation = reinterpret_cast<CHAOS_IL2CPP_INT64*>(location);
    return std::atomic_fetch_add_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT64>*>(typedLocation),
        value,
        std::memory_order_seq_cst) + value;
}

CHAOS_IL2CPP_UINT32 ChaosInterlockedOrUInt32(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_UINT32 value) noexcept
{
    auto* typedLocation = reinterpret_cast<CHAOS_IL2CPP_UINT32*>(location);
    return std::atomic_fetch_or_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_UINT32>*>(typedLocation),
        value,
        std::memory_order_seq_cst) | value;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core