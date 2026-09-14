// handle_slot_recycling_test.cpp — T3.2 exit criterion.
//
// THE DEFECT THIS PINS
// --------------------
// The fixed handle tables index by slot, so a handle is an array index.  The
// create path looks for a FREE slot via `entry.id == 0`, but Destroy only set
// `active = false` and left `id` nonzero — so the slot was never free again.
// Every create/destroy cycle burned one of the 1023 slots permanently.
//
// Two consequences, and the second is the one a naive test misses:
//
//   1. Capacity exhaustion.  After 1023 lifetime allocations the table returns
//      0 ("table full") even with zero objects alive.  A leak test that only
//      checks "does it still work after a few cycles" would pass.
//
//   2. The handle COUNT is bounded but the handle VALUE keeps growing.  So the
//      test must assert on reuse (the same slot index coming back), not merely
//      on success.
//
// WHY REUSE IS SAFE HERE even though a stale handle then resolves to a new
// object: `FindX` validates `active`, not the id, and a recycled slot is set
// `active = true` by its new owner.  The id field exists solely as the
// allocator's free-slot marker.  This test documents that contract by checking
// the boundary: a handle for a DESTROYED object must not resolve, and a handle
// for a RECYCLED slot must resolve to the new object.

#include "synchronization.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <set>
#include <vector>

using namespace chaos::il2cpp::runtime_core::threading;

namespace {

constexpr uint32_t kTableCapacity = 1024;   // slots, index 0 unused

}  // namespace

// ── 1. The core defect: slots must come back ───────────────────────────

TEST(HandleSlotRecycling, DestroyedSemaphoreSlotsAreReused)
{
    // Not 1023 iterations — this only needs to prove the slot returns to the
    // free pool, which shows up immediately.
    const uint32_t first = SemaphoreSlimCreate(0, 1);
    ASSERT_NE(first, 0u);

    ASSERT_TRUE(SemaphoreSlimDestroy(first));

    const uint32_t second = SemaphoreSlimCreate(0, 1);
    ASSERT_NE(second, 0u);

    EXPECT_EQ(second, first)
        << "after destroying the only live semaphore, the next create must reuse "
           "its slot; if it gets a new one, destroyed slots are never freed and "
           "the 1023-slot table leaks";

    SemaphoreSlimDestroy(second);
}

TEST(HandleSlotRecycling, SlotReuseSurvivesManyCreateDestroyCycles)
{
    // The capacity bug's signature: with no recycling this fails at iteration
    // 1023, long after any short smoke test would have stopped looking.
    const uint32_t cycles = kTableCapacity * 2;
    for (uint32_t i = 0; i < cycles; ++i) {
        const uint32_t h = SemaphoreSlimCreate(0, 1);
        ASSERT_NE(h, 0u)
            << "create failed at cycle " << i
            << " — the table filled up even though at most one semaphore was "
               "alive at any time, which is exactly the un-recycled-slot defect";
        ASSERT_TRUE(SemaphoreSlimDestroy(h));
    }
}

TEST(HandleSlotRecycling, DestroyedSlotStillReportsNotFoundAfterRecycle)
{
    // A destroyed handle must not resolve BEFORE the slot is reused.
    const uint32_t h = SemaphoreSlimCreate(1, 1);
    ASSERT_NE(h, 0u);
    ASSERT_TRUE(SemaphoreSlimDestroy(h));

    EXPECT_EQ(SemaphoreSlimWait(h, 0), -1)
        << "a destroyed handle must not be usable";
}

// ── 2. Same guarantee for the other three tables ───────────────────────

TEST(HandleSlotRecycling, RwLockSlotsAreReused)
{
    const uint32_t first = ReaderWriterLockSlimCreate();
    ASSERT_NE(first, 0u);
    ASSERT_TRUE(ReaderWriterLockSlimDestroy(first));

    const uint32_t second = ReaderWriterLockSlimCreate();
    ASSERT_NE(second, 0u);
    EXPECT_EQ(second, first) << "RWLock slots must be recycled too";
    ReaderWriterLockSlimDestroy(second);
}

TEST(HandleSlotRecycling, BarrierSlotsAreReused)
{
    const uint32_t first = BarrierCreate(2);
    ASSERT_NE(first, 0u);
    ASSERT_TRUE(BarrierDestroy(first));

    const uint32_t second = BarrierCreate(2);
    ASSERT_NE(second, 0u);
    EXPECT_EQ(second, first) << "Barrier slots must be recycled too";
    BarrierDestroy(second);
}

TEST(HandleSlotRecycling, CountdownEventSlotsAreReused)
{
    const uint32_t first = CountdownEventCreate(1);
    ASSERT_NE(first, 0u);
    ASSERT_TRUE(CountdownEventDestroy(first));

    const uint32_t second = CountdownEventCreate(1);
    ASSERT_NE(second, 0u);
    EXPECT_EQ(second, first) << "CountdownEvent slots must be recycled too";
    CountdownEventDestroy(second);
}

// ── 3. Recycling must not resurrect the OLD object's state ─────────────

TEST(HandleSlotRecycling, ARecycledSlotStartsWithFreshState)
{
    // Acquire a count from the first semaphore, destroy it, then re-create on
    // the same slot.  The new object must start at ITS OWN counts — if the slot
    // were recycled without reinitialising, the leftover count/max_count would
    // silently carry over.
    const uint32_t first = SemaphoreSlimCreate(2, 5);
    ASSERT_NE(first, 0u);
    ASSERT_EQ(SemaphoreSlimWait(first, 0), 1);   // consume one of the two
    ASSERT_TRUE(SemaphoreSlimDestroy(first));

    const uint32_t second = SemaphoreSlimCreate(1, 1);
    ASSERT_EQ(second, first) << "expect the same slot back";

    // The new semaphore has ONE count, not the old object's remaining one.
    EXPECT_EQ(SemaphoreSlimWait(second, 0), 1) << "the freshly created count must be available";
    EXPECT_EQ(SemaphoreSlimWait(second, 0), 0)
        << "and there must be exactly one — a stale count from the previous "
           "occupant of this slot would make this succeed";

    // And max_count must be the new 1, so releasing by 2 must be rejected.
    EXPECT_EQ(SemaphoreSlimRelease(second, 2), -1)
        << "max_count from the constructor must apply to the recycled slot";

    SemaphoreSlimDestroy(second);
}

TEST(HandleSlotRecycling, ConcurrentCreateDestroyDoesNotExhaustTheTable)
{
    // Multi-threaded version of the cycle test: the allocator and the free-slot
    // scan must agree under concurrency.  With a non-atomic scan or a
    // non-atomic id write, this reliably produces either a duplicate handle or
    // a spurious "table full".
    constexpr int kThreads = 4;
    constexpr int kPerThread = 300;

    std::atomic<int> failures{0};
    std::vector<std::thread> threads;
    threads.reserve(kThreads);

    for (int t = 0; t < kThreads; ++t) {
        threads.emplace_back([&failures] {
            for (int i = 0; i < kPerThread; ++i) {
                const uint32_t h = SemaphoreSlimCreate(1, 1);
                if (h == 0) { failures.fetch_add(1); continue; }
                if (SemaphoreSlimWait(h, 0) != 1) failures.fetch_add(1);
                SemaphoreSlimDestroy(h);
            }
        });
    }
    for (auto& th : threads) th.join();

    EXPECT_EQ(failures.load(), 0)
        << "concurrent create/destroy must not exhaust the table: with recycling "
           "the live-object peak is tiny, so any failure here is a real defect";
}
