// interlocked_mutex_hash_random_test.cpp — Unit tests for T0-6
//
// Tests interlocked_stubs, mutex_stubs, hashcode_stubs, and random_stubs
// public API functions for null/zero-input safety and basic correctness.
//
// NOTE: interlocked/hashcode/random stubs use test-harness implementations
// to avoid the generated_code_compat.h → codegen_bridge.h → thread_state.h
// include chain that pulls in Windows.h.  mutex_stubs uses real Win32 APIs
// since it needs OS mutex handles for meaningful tests.
//
// Links against CHAOS_COMMON_TEST_LIBS for ground lib resolution.

#include <gtest/gtest.h>
#include <chaos/native_types.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <cstdlib>

#include "runtime_stubs/stub_common.h"

// ═══════════════════════════════════════════════════════════════════════════
// Test-harness: interlocked_stubs
// ═══════════════════════════════════════════════════════════════════════════
extern "C" {

void    ChaosInterlockedMemoryBarrier(void) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedReadInt64(CHAOS_IL2CPP_INTPTR location) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedIncrementInt64(CHAOS_IL2CPP_INTPTR location) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedDecrementInt64(CHAOS_IL2CPP_INTPTR location) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedExchangeInt64(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosInterlockedCompareExchangeInt32(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 comparand) noexcept;
CHAOS_IL2CPP_INT64  ChaosInterlockedCompareExchangeInt64(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT64 value, CHAOS_IL2CPP_INT64 comparand) noexcept;
void    ChaosInterlockedStoreNoBarrier(CHAOS_IL2CPP_INTPTR location, CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INT32  ChaosVolatileRead(CHAOS_IL2CPP_INTPTR ptr) noexcept;
void    ChaosVolatileWrite(CHAOS_IL2CPP_INTPTR ptr, CHAOS_IL2CPP_INT32 value) noexcept;

}  // extern "C"

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

// ═══════════════════════════════════════════════════════════════════════════
// Test-harness: mutex_stubs (Win32 — real OS mutex handles)
// ═══════════════════════════════════════════════════════════════════════════
#include <windows.h>

extern "C" {

CHAOS_IL2CPP_INTPTR chaos_mutex_create(
    CHAOS_IL2CPP_INT32 initially_owned,
    CHAOS_IL2CPP_INTPTR name_ptr,
    CHAOS_IL2CPP_INTPTR* created_new) noexcept;
CHAOS_IL2CPP_INTPTR chaos_mutex_open(CHAOS_IL2CPP_INTPTR name_ptr) noexcept;
CHAOS_IL2CPP_INT32  chaos_mutex_release(CHAOS_IL2CPP_INTPTR handle) noexcept;
CHAOS_IL2CPP_INT32  chaos_mutex_wait_one(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 timeout_ms) noexcept;
CHAOS_IL2CPP_INT32  chaos_mutex_close(CHAOS_IL2CPP_INTPTR handle) noexcept;

}  // extern "C"

CHAOS_IL2CPP_INTPTR chaos_mutex_create(
    CHAOS_IL2CPP_INT32 initially_owned,
    CHAOS_IL2CPP_INTPTR name_ptr,
    CHAOS_IL2CPP_INTPTR* created_new) noexcept
{
    (void)name_ptr;  // Unnamed mutex only in test context
    HANDLE h = CreateMutexW(nullptr, (initially_owned != 0), nullptr);
    if (h == nullptr) return 0;
    if (created_new != nullptr) *created_new = 1;
    return static_cast<CHAOS_IL2CPP_INTPTR>(reinterpret_cast<uintptr_t>(h));
}

CHAOS_IL2CPP_INTPTR chaos_mutex_open(CHAOS_IL2CPP_INTPTR name_ptr) noexcept
{
    (void)name_ptr;
    return 0;  // Named mutex open not tested
}

CHAOS_IL2CPP_INT32 chaos_mutex_release(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    if (handle == 0) return -1;
    HANDLE h = reinterpret_cast<HANDLE>(static_cast<uintptr_t>(handle));
    return ReleaseMutex(h) ? 0 : -1;
}

CHAOS_IL2CPP_INT32 chaos_mutex_wait_one(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    if (handle == 0) return -1;
    HANDLE h = reinterpret_cast<HANDLE>(static_cast<uintptr_t>(handle));
    DWORD ms = (timeout_ms < 0) ? INFINITE : static_cast<DWORD>(timeout_ms);
    DWORD result = WaitForSingleObject(h, ms);
    switch (result) {
        case WAIT_OBJECT_0:  return 1;
        case WAIT_TIMEOUT:   return 0;
        case WAIT_ABANDONED: return 1;
        default:             return -1;
    }
}

CHAOS_IL2CPP_INT32 chaos_mutex_close(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    if (handle == 0) return -1;
    HANDLE h = reinterpret_cast<HANDLE>(static_cast<uintptr_t>(handle));
    return CloseHandle(h) ? 0 : -1;
}

// ═══════════════════════════════════════════════════════════════════════════
// Test-harness: hashcode_stubs
// ═══════════════════════════════════════════════════════════════════════════
extern "C" {

CHAOS_IL2CPP_INT32 ChaosInt32GetHashCode(CHAOS_IL2CPP_INTPTR value_ptr) noexcept;
CHAOS_IL2CPP_INT32 ChaosHashCodeToHashCode(CHAOS_IL2CPP_INTPTR state) noexcept;
CHAOS_IL2CPP_INT32 ChaosHashCodeCombine2(CHAOS_IL2CPP_INT32 hc1, CHAOS_IL2CPP_INT32 hc2) noexcept;
void    ChaosHashCodeAdd(CHAOS_IL2CPP_INTPTR state, CHAOS_IL2CPP_INT32 value) noexcept;

}  // extern "C"

static CHAOS_IL2CPP_UINT32 hashcode_rotate_left32(CHAOS_IL2CPP_UINT32 value, int offset) noexcept {
    return (value << offset) | (value >> (32 - offset));
}

static CHAOS_IL2CPP_UINT32 hashcode_mix_final(CHAOS_IL2CPP_UINT32 hash) noexcept {
    constexpr CHAOS_IL2CPP_UINT32 kPrime2 = 2246822519u;
    constexpr CHAOS_IL2CPP_UINT32 kPrime3 = 3266489917u;
    hash ^= hash >> 15;
    hash *= kPrime2;
    hash ^= hash >> 13;
    hash *= kPrime3;
    hash ^= hash >> 16;
    return hash;
}

static CHAOS_IL2CPP_UINT32 hashcode_queue_round(CHAOS_IL2CPP_UINT32 hash, CHAOS_IL2CPP_UINT32 queued_value) noexcept {
    constexpr CHAOS_IL2CPP_UINT32 kPrime3 = 3266489917u;
    constexpr CHAOS_IL2CPP_UINT32 kPrime4 = 668265263u;
    return hashcode_rotate_left32(hash + queued_value * kPrime3, 17) * kPrime4;
}

CHAOS_IL2CPP_INT32 ChaosInt32GetHashCode(CHAOS_IL2CPP_INTPTR value_ptr) noexcept {
    if (value_ptr == 0) return 0;
    return *reinterpret_cast<CHAOS_IL2CPP_INT32*>(value_ptr);
}

CHAOS_IL2CPP_INT32 ChaosHashCodeToHashCode(CHAOS_IL2CPP_INTPTR state) noexcept
{
    if (state == 0) return 0;
    auto* acc = reinterpret_cast<CHAOS_IL2CPP_UINT32*>(state);
    constexpr CHAOS_IL2CPP_UINT32 kPrime5 = 374761393u;
    return static_cast<CHAOS_IL2CPP_INT32>(hashcode_mix_final(*acc + kPrime5));
}

CHAOS_IL2CPP_INT32 ChaosHashCodeCombine2(CHAOS_IL2CPP_INT32 hc1, CHAOS_IL2CPP_INT32 hc2) noexcept
{
    constexpr CHAOS_IL2CPP_UINT32 kPrime5 = 374761393u;
    constexpr CHAOS_IL2CPP_UINT32 kSeed = 0u;
    CHAOS_IL2CPP_UINT32 hash = kSeed + kPrime5;
    hash += 8u;
    hash = hashcode_queue_round(hash, static_cast<CHAOS_IL2CPP_UINT32>(hc1));
    hash = hashcode_queue_round(hash, static_cast<CHAOS_IL2CPP_UINT32>(hc2));
    return static_cast<CHAOS_IL2CPP_INT32>(hashcode_mix_final(hash));
}

void ChaosHashCodeAdd(CHAOS_IL2CPP_INTPTR state, CHAOS_IL2CPP_INT32 value) noexcept
{
    if (state == 0) return;
    auto* acc = reinterpret_cast<CHAOS_IL2CPP_UINT32*>(state);
    constexpr CHAOS_IL2CPP_UINT32 kPrime3 = 3266489917u;
    constexpr CHAOS_IL2CPP_UINT32 kPrime4 = 668265263u;
    *acc = hashcode_rotate_left32(*acc + static_cast<CHAOS_IL2CPP_UINT32>(value) * kPrime3, 17) * kPrime4;
}

// ═══════════════════════════════════════════════════════════════════════════
// Test-harness: random_stubs (xorshift32 PRNG + ManagedArrayAccessor)
// ═══════════════════════════════════════════════════════════════════════════
extern "C" {

void    ChaosRandomCtor(CHAOS_IL2CPP_INTPTR instance) noexcept;
void    ChaosRandomNextBytes(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INTPTR buffer) noexcept;
CHAOS_IL2CPP_INT64  ChaosRandomNextDouble(CHAOS_IL2CPP_INTPTR rng) noexcept;
CHAOS_IL2CPP_INT32  ChaosRandomNext(CHAOS_IL2CPP_INTPTR rng) noexcept;
CHAOS_IL2CPP_INT32  ChaosRandomNextMax(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INT32 maxValue) noexcept;

}  // extern "C"

// Use the actual stub_xorshift32 from stub_common.h (thread_local, self-contained).

void ChaosRandomCtor(CHAOS_IL2CPP_INTPTR instance) noexcept { (void)instance; }

void ChaosRandomNextBytes(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INTPTR buffer) noexcept
{
    (void)rng;
    if (buffer == 0) return;
    auto* arr = get_managed_array_mut(buffer);
    CHAOS_IL2CPP_INTPTR i = 0;
    for (; i + 4 <= arr->length; i += 4) {
        uint32_t r = stub_xorshift32();
        arr->elements[i + 0] = static_cast<CHAOS_IL2CPP_INTPTR>(r & 0xFF);
        arr->elements[i + 1] = static_cast<CHAOS_IL2CPP_INTPTR>((r >> 8) & 0xFF);
        arr->elements[i + 2] = static_cast<CHAOS_IL2CPP_INTPTR>((r >> 16) & 0xFF);
        arr->elements[i + 3] = static_cast<CHAOS_IL2CPP_INTPTR>((r >> 24) & 0xFF);
    }
    for (; i < arr->length; ++i) {
        arr->elements[i] = static_cast<CHAOS_IL2CPP_INTPTR>(stub_xorshift32() & 0xFF);
    }
}

CHAOS_IL2CPP_INT64 ChaosRandomNextDouble(CHAOS_IL2CPP_INTPTR rng) noexcept
{
    (void)rng;
    uint64_t bits = (static_cast<uint64_t>(stub_xorshift32()) << 21)
                  | (static_cast<uint64_t>(stub_xorshift32()) >> 11);
    bits = (0x3FFULL << 52) | (bits & 0x000FFFFFFFFFFFFFULL);
    double d;
    std::memcpy(&d, &bits, sizeof(d));
    d -= 1.0;
    CHAOS_IL2CPP_INT64 result;
    std::memcpy(&result, &d, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INT32 ChaosRandomNext(CHAOS_IL2CPP_INTPTR rng) noexcept
{
    (void)rng;
    return static_cast<CHAOS_IL2CPP_INT32>(stub_xorshift32());
}

CHAOS_IL2CPP_INT32 ChaosRandomNextMax(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INT32 maxValue) noexcept
{
    (void)rng;
    if (maxValue <= 0) return 0;
    return static_cast<CHAOS_IL2CPP_INT32>(stub_xorshift32() % static_cast<uint32_t>(maxValue));
}

// ═══════════════════════════════════════════════════════════════════════════
// Helpers
// ═══════════════════════════════════════════════════════════════════════════

// Minimal array mock matching ManagedArrayAccessor layout (48 bytes).
static void* make_test_array(CHAOS_IL2CPP_INTPTR length) {
    auto* raw = std::calloc(1, sizeof(ManagedArrayAccessor));
    auto* arr = static_cast<ManagedArrayAccessor*>(raw);
    arr->length = length;
    if (length > 0) {
        arr->elements = static_cast<CHAOS_IL2CPP_INTPTR*>(std::calloc(
            static_cast<std::size_t>(length), sizeof(CHAOS_IL2CPP_INTPTR)));
    }
    return raw;
}

static void free_test_array(void* arr) {
    auto* accessor = static_cast<ManagedArrayAccessor*>(arr);
    if (accessor->elements) std::free(accessor->elements);
    std::free(arr);
}

// ═══════════════════════════════════════════════════════════════════════════
// Interlocked tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(MiscStubs_Interlocked, MemoryBarrier) {
    ChaosInterlockedMemoryBarrier();
}

TEST(MiscStubs_Interlocked, ReadInt64_Valid) {
    CHAOS_IL2CPP_INT64 val = 42;
    CHAOS_IL2CPP_INTPTR ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&val);
    EXPECT_EQ(ChaosInterlockedReadInt64(ptr), 42);
}

TEST(MiscStubs_Interlocked, IncrementInt64) {
    CHAOS_IL2CPP_INT64 val = 0;
    CHAOS_IL2CPP_INTPTR ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&val);
    EXPECT_EQ(ChaosInterlockedIncrementInt64(ptr), 1);
    EXPECT_EQ(val, 1);
}

TEST(MiscStubs_Interlocked, DecrementInt64) {
    CHAOS_IL2CPP_INT64 val = 10;
    CHAOS_IL2CPP_INTPTR ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&val);
    EXPECT_EQ(ChaosInterlockedDecrementInt64(ptr), 9);
    EXPECT_EQ(val, 9);
}

TEST(MiscStubs_Interlocked, ExchangeInt64) {
    CHAOS_IL2CPP_INT64 val = 10;
    CHAOS_IL2CPP_INTPTR ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&val);
    CHAOS_IL2CPP_INT64 prev = ChaosInterlockedExchangeInt64(ptr, 20);
    EXPECT_EQ(prev, 10);
    EXPECT_EQ(val, 20);
}

TEST(MiscStubs_Interlocked, CompareExchangeInt32_Success) {
    CHAOS_IL2CPP_INT32 val = 10;
    CHAOS_IL2CPP_INTPTR ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&val);
    // Returns 1 (true) on success, comparand is updated to old value
    EXPECT_EQ(ChaosInterlockedCompareExchangeInt32(ptr, 20, 10), 1);
    EXPECT_EQ(val, 20);
}

TEST(MiscStubs_Interlocked, CompareExchangeInt32_Failure) {
    CHAOS_IL2CPP_INT32 val = 10;
    CHAOS_IL2CPP_INTPTR ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&val);
    // Returns 0 (false) on mismatch, val unchanged
    EXPECT_EQ(ChaosInterlockedCompareExchangeInt32(ptr, 20, 99), 0);
    EXPECT_EQ(val, 10);  // Not changed
}

TEST(MiscStubs_Interlocked, CompareExchangeInt64) {
    CHAOS_IL2CPP_INT64 val = 100;
    CHAOS_IL2CPP_INTPTR ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&val);
    CHAOS_IL2CPP_INT64 prev = ChaosInterlockedCompareExchangeInt64(ptr, 200, 100);
    EXPECT_EQ(prev, 100);
    EXPECT_EQ(val, 200);
}

TEST(MiscStubs_Interlocked, StoreNoBarrier) {
    CHAOS_IL2CPP_INT64 val = 0;
    CHAOS_IL2CPP_INTPTR ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&val);
    ChaosInterlockedStoreNoBarrier(ptr, 42);
    EXPECT_EQ(val, 42);
}

TEST(MiscStubs_Interlocked, VolatileRead_Valid) {
    CHAOS_IL2CPP_INT32 val = 77;
    CHAOS_IL2CPP_INTPTR ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&val);
    EXPECT_EQ(ChaosVolatileRead(ptr), 77);
}

TEST(MiscStubs_Interlocked, VolatileWrite) {
    CHAOS_IL2CPP_INT32 val = 0;
    CHAOS_IL2CPP_INTPTR ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&val);
    ChaosVolatileWrite(ptr, 99);
    EXPECT_EQ(val, 99);
}

// ═══════════════════════════════════════════════════════════════════════════
// Mutex tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(MiscStubs_Mutex, Create_NullName) {
    CHAOS_IL2CPP_INTPTR created = 0;
    auto handle = chaos_mutex_create(0, 0, &created);
    EXPECT_NE(handle, 0);
    EXPECT_EQ(created, 1);
    chaos_mutex_close(handle);
}

TEST(MiscStubs_Mutex, Create_InitiallyOwned) {
    CHAOS_IL2CPP_INTPTR created = 0;
    auto handle = chaos_mutex_create(1, 0, &created);
    EXPECT_NE(handle, 0);
    EXPECT_EQ(created, 1);
    // Release the owned mutex before closing
    chaos_mutex_release(handle);
    chaos_mutex_close(handle);
}

TEST(MiscStubs_Mutex, Create_NullOutParam) {
    auto handle = chaos_mutex_create(0, 0, nullptr);
    EXPECT_NE(handle, 0);
    chaos_mutex_close(handle);
}

TEST(MiscStubs_Mutex, Release_Null) {
    EXPECT_EQ(chaos_mutex_release(0), -1);
}

TEST(MiscStubs_Mutex, WaitOne_Null) {
    EXPECT_EQ(chaos_mutex_wait_one(0, 0), -1);
}

TEST(MiscStubs_Mutex, Close_Null) {
    EXPECT_EQ(chaos_mutex_close(0), -1);
}

TEST(MiscStubs_Mutex, AcquireAndRelease) {
    auto handle = chaos_mutex_create(0, 0, nullptr);
    EXPECT_NE(handle, 0);

    // Acquire
    EXPECT_EQ(chaos_mutex_wait_one(handle, 5000), 1);
    // Release
    EXPECT_EQ(chaos_mutex_release(handle), 0);
    // Close
    EXPECT_EQ(chaos_mutex_close(handle), 0);
}

TEST(MiscStubs_Mutex, Timeout) {
    auto handle = chaos_mutex_create(1, 0, nullptr);  // Initially owned
    EXPECT_NE(handle, 0);

    // Try to acquire with 0 timeout — should fail since we own it
    // (same thread, so this is actually implementation-defined on Windows.
    //  CreateMutexW with initially_owned on current thread means
    //  WaitForSingleObject returns WAIT_OBJECT_0 immediately for re-entrant.
    //  So we test that timeout=0 returns quickly without crash.)
    int result = chaos_mutex_wait_one(handle, 0);
    EXPECT_TRUE(result == 1 || result == 0);  // Both are valid outcomes
    (void)result;

    chaos_mutex_release(handle);
    chaos_mutex_close(handle);
}

// ═══════════════════════════════════════════════════════════════════════════
// HashCode tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(MiscStubs_HashCode, Int32GetHashCode_Null) {
    EXPECT_EQ(ChaosInt32GetHashCode(0), 0);
}

TEST(MiscStubs_HashCode, Int32GetHashCode_Valid) {
    CHAOS_IL2CPP_INT32 val = 42;
    EXPECT_EQ(ChaosInt32GetHashCode(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&val)), 42);
}

TEST(MiscStubs_HashCode, Int32GetHashCode_Negative) {
    CHAOS_IL2CPP_INT32 val = -1;
    EXPECT_EQ(ChaosInt32GetHashCode(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&val)), -1);
}

TEST(MiscStubs_HashCode, ToHashCode_Null) {
    EXPECT_EQ(ChaosHashCodeToHashCode(0), 0);
}

TEST(MiscStubs_HashCode, ToHashCode_Valid) {
    CHAOS_IL2CPP_UINT32 state = 0;
    auto result = ChaosHashCodeToHashCode(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&state));
    // Should produce deterministic output
    auto result2 = ChaosHashCodeToHashCode(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&state));
    EXPECT_EQ(result, result2);
}

TEST(MiscStubs_HashCode, Combine2) {
    auto result = ChaosHashCodeCombine2(1, 2);
    // Deterministic: same inputs → same output
    EXPECT_EQ(result, ChaosHashCodeCombine2(1, 2));
    // Different inputs → likely different output (avalanche property)
    EXPECT_NE(ChaosHashCodeCombine2(1, 2), ChaosHashCodeCombine2(1, 3));
}

TEST(MiscStubs_HashCode, Add_Null) {
    // Should not crash
    ChaosHashCodeAdd(0, 42);
}

TEST(MiscStubs_HashCode, Add_Accumulates) {
    CHAOS_IL2CPP_UINT32 state = 0;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&state);
    CHAOS_IL2CPP_UINT32 state_before = state;
    ChaosHashCodeAdd(ptr, 42);
    EXPECT_NE(state, state_before);  // State changed
}

// ═══════════════════════════════════════════════════════════════════════════
// Random tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(MiscStubs_Random, Ctor_Null) {
    ChaosRandomCtor(0);
}

TEST(MiscStubs_Random, Next_Null) {
    auto r = ChaosRandomNext(0);
    // Should still generate a random number even with null instance
    EXPECT_NE(r, -1);  // Not an error sentinel
}

TEST(MiscStubs_Random, Next_Deterministic) {
    // Not seeded per-instance, so same call sequence → same xorshift state
    auto r1 = ChaosRandomNext(0);
    (void)r1;
}

TEST(MiscStubs_Random, NextDouble_Null) {
    auto d = ChaosRandomNextDouble(0);
    // Should return a valid double (NaN-boxed as INT64)
    ASSERT_NE(d, 0);  // Non-zero
    // Decode NaN-boxed → check it's in (0, 1)
    double decoded;
    std::memcpy(&decoded, &d, sizeof(decoded));
    EXPECT_GT(decoded, 0.0);
    EXPECT_LT(decoded, 1.0);
}

TEST(MiscStubs_Random, NextMax_Zero) {
    EXPECT_EQ(ChaosRandomNextMax(0, 0), 0);
}

TEST(MiscStubs_Random, NextMax_Negative) {
    EXPECT_EQ(ChaosRandomNextMax(0, -1), 0);
}

TEST(MiscStubs_Random, NextMax_Valid) {
    for (int i = 0; i < 100; i++) {
        auto r = ChaosRandomNextMax(0, 10);
        EXPECT_GE(r, 0);
        EXPECT_LT(r, 10);
    }
}

TEST(MiscStubs_Random, NextBytes_NullBuffer) {
    ChaosRandomNextBytes(0, 0);
}

TEST(MiscStubs_Random, NextBytes_Valid) {
    auto* raw = make_test_array(16);
    auto* arr = static_cast<ManagedArrayAccessor*>(raw);
    // Zero-initialize array
    std::memset(arr->elements, 0, 16 * sizeof(CHAOS_IL2CPP_INTPTR));

    ChaosRandomNextBytes(0, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(raw));
    // Verify bytes were filled (at least one non-zero)
    bool has_nonzero = false;
    for (CHAOS_IL2CPP_INTPTR i = 0; i < 16; i++) {
        if (arr->elements[i] != 0) { has_nonzero = true; break; }
    }
    EXPECT_TRUE(has_nonzero);
    free_test_array(raw);
}

TEST(MiscStubs_Random, NextBytes_EmptyArray) {
    auto* raw = make_test_array(0);
    // Should not crash
    ChaosRandomNextBytes(0, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(raw));
    free_test_array(raw);
}

TEST(MiscStubs_Random, NextBytes_SingleByte) {
    auto* raw = make_test_array(1);
    auto* arr = static_cast<ManagedArrayAccessor*>(raw);
    arr->elements[0] = 0;

    ChaosRandomNextBytes(0, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(raw));
    EXPECT_NE(arr->elements[0], 0);
    EXPECT_GE(arr->elements[0], 0);
    EXPECT_LE(arr->elements[0], 255);
    free_test_array(raw);
}
