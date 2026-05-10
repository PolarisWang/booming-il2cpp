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

CHAOS_IL2CPP_INT64 ChaosInterlockedReadInt64(CHAOS_IL2CPP_INT64* location) noexcept
{
    return std::atomic_load_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT64>*>(location),
        std::memory_order_seq_cst);
}

CHAOS_IL2CPP_INT64 ChaosInterlockedIncrementInt64(CHAOS_IL2CPP_INT64* location) noexcept
{
    return std::atomic_fetch_add_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT64>*>(location),
        CHAOS_IL2CPP_INT64(1),
        std::memory_order_seq_cst) + 1;
}

CHAOS_IL2CPP_INT64 ChaosInterlockedDecrementInt64(CHAOS_IL2CPP_INT64* location) noexcept
{
    return std::atomic_fetch_sub_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT64>*>(location),
        CHAOS_IL2CPP_INT64(1),
        std::memory_order_seq_cst) - 1;
}

CHAOS_IL2CPP_INT64 ChaosInterlockedExchangeInt64(CHAOS_IL2CPP_INT64* location, CHAOS_IL2CPP_INT64 value) noexcept
{
    return std::atomic_exchange_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT64>*>(location),
        value,
        std::memory_order_seq_cst);
}

CHAOS_IL2CPP_INT32 ChaosInterlockedCompareExchangeInt32(CHAOS_IL2CPP_INT32* location, CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 comparand) noexcept
{
    return std::atomic_compare_exchange_strong_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT32>*>(location),
        &comparand,
        value,
        std::memory_order_seq_cst,
        std::memory_order_seq_cst);
}

CHAOS_IL2CPP_INT64 ChaosInterlockedCompareExchangeInt64(CHAOS_IL2CPP_INT64* location, CHAOS_IL2CPP_INT64 value, CHAOS_IL2CPP_INT64 comparand) noexcept
{
    std::atomic_compare_exchange_strong_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT64>*>(location),
        &comparand,
        value,
        std::memory_order_seq_cst,
        std::memory_order_seq_cst);
    return comparand;
}

void ChaosInterlockedStoreNoBarrier(CHAOS_IL2CPP_INT64* location, CHAOS_IL2CPP_INT64 value) noexcept
{
    std::atomic_store_explicit(
        reinterpret_cast<std::atomic<CHAOS_IL2CPP_INT64>*>(location),
        value,
        std::memory_order_relaxed);
}

CHAOS_IL2CPP_INT32 ChaosVolatileRead(CHAOS_IL2CPP_INTPTR ptr) noexcept
{
    return *reinterpret_cast<volatile CHAOS_IL2CPP_INT32*>(ptr);
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core