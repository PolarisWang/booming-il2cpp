// managed_handle_stubs_test.cpp — T2.1 exit criterion.
//
// WHAT THIS PINS
// --------------
// T2.1's stated exit is "托管 `SemaphoreSlim` 实例可绑定 native 槽位" — a managed
// instance can be bound to a native slot.  The binding is a handle stored in a
// field ON the object at a fixed offset, so the properties worth testing are:
//
//   1. store -> load round-trips (the field is where we think it is)
//   2. the binding is per-instance (two instances do not share one slot)
//   3. GetOrCreate creates ONCE — a second call must not mint a second native
//      object, because the first one's state (a semaphore's count!) would then
//      be silently split across two slots
//   4. release CLEARS the field before destroying, so a concurrent reader sees
//      "unbound", not a dying id
//   5. release is idempotent (managed Dispose() can run twice: explicit
//      Dispose then finalizer)
//   6. real native objects survive the round trip — a handle stored and loaded
//      out of the field must still be the handle the native table knows
//
// (6) is the one that matters most: a bug that stored, say, an index into some
// other table would pass (1)-(5) and still be wrong.  It is checked by actually
// driving a semaphore through the stored handle.
//
// The test does NOT include synchronization.h / wait_handle.h — it goes through
// the ABI headers only, the same way a generated TU would.

#include "runtime_stubs/managed_handle_stubs.h"
#include "runtime_stubs/synchronization_stubs.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

namespace {

// A stand-in managed object: 8 bytes of header (ThinLockableHeader is
// TypeInfo* at [0], sync_state having moved to ThinLockTable) followed by the
// handle field.  This mirrors the layout contract managed_handle_stubs.cpp
// documents; if that offset were wrong, every test below would read the wrong
// word out of this buffer.
struct FakeManagedInstance {
    CHAOS_IL2CPP_INTPTR header[1] = {0xDEADBEEF};  // 8 bytes: stands in for TypeInfo*
    CHAOS_IL2CPP_INTPTR handle   = 0;              // the field under test
};

static_assert(sizeof(FakeManagedInstance) == 16,
              "the fake instance must be header + one pointer, so that a wrong "
              "handle offset lands on the header word and is caught");

CHAOS_IL2CPP_INTPTR AsIntPtr(FakeManagedInstance* inst) noexcept {
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(inst);
}

// ── Native create/destroy used as the GetOrCreate hooks ────────────────
//
// These exist because GetOrCreate takes function pointers.  They route to the
// T2.0 ABI exports so the handles produced are REAL — that is what makes test
// (6) meaningful rather than a restatement of (1).
CHAOS_IL2CPP_INTPTR CreateSemaphoreHook(CHAOS_IL2CPP_INTPTR initial,
                                        CHAOS_IL2CPP_INTPTR max_count) noexcept {
    return ChaosSemaphoreSlimCreate(static_cast<CHAOS_IL2CPP_INT32>(initial),
                                    static_cast<CHAOS_IL2CPP_INT32>(max_count));
}

void DestroySemaphoreHook(CHAOS_IL2CPP_INTPTR handle) noexcept {
    ChaosSemaphoreSlimDestroy(handle);
}

}  // namespace

TEST(ManagedHandleBinding, StoreThenLoadRoundTripsThroughTheInstance)
{
    FakeManagedInstance inst;
    ASSERT_EQ(ChaosManagedHandleLoad(AsIntPtr(&inst)), 0) << "starts unbound";
    EXPECT_EQ(inst.header[0], static_cast<CHAOS_IL2CPP_INTPTR>(0xDEADBEEF))
        << "the store must not trample the object header";

    EXPECT_EQ(ChaosManagedHandleStore(AsIntPtr(&inst), 0x1234), 0x1234);
    EXPECT_EQ(ChaosManagedHandleLoad(AsIntPtr(&inst)), 0x1234);
    EXPECT_EQ(inst.handle, static_cast<CHAOS_IL2CPP_INTPTR>(0x1234))
        << "the handle must land in the field, not somewhere else";
    EXPECT_EQ(inst.header[0], static_cast<CHAOS_IL2CPP_INTPTR>(0xDEADBEEF))
        << "header intact after the store";
}

TEST(ManagedHandleBinding, TwoInstancesDoNotShareASlot)
{
    FakeManagedInstance a;
    FakeManagedInstance b;

    ChaosManagedHandleStore(AsIntPtr(&a), 0xAAAA);
    ChaosManagedHandleStore(AsIntPtr(&b), 0xBBBB);

    EXPECT_EQ(ChaosManagedHandleLoad(AsIntPtr(&a)), 0xAAAA);
    EXPECT_EQ(ChaosManagedHandleLoad(AsIntPtr(&b)), 0xBBBB)
        << "binding must be per-instance; a shared slot would make two "
           "independent SemaphoreSlim objects alias one native semaphore";
}

TEST(ManagedHandleBinding, ANullInstanceIsAnsweredRatherThanDereferenced)
{
    // Managed code can reach here with null after a failed constructor or a
    // null field read.  Writing through the offset of a null pointer would be
    // a small-address store instead of a clean failure.
    EXPECT_EQ(ChaosManagedHandleLoad(0), 0);
    EXPECT_EQ(ChaosManagedHandleStore(0, 0x1234), 0);
    EXPECT_EQ(ChaosManagedHandleRelease(0, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&DestroySemaphoreHook)), 0);
}

TEST(ManagedHandleBinding, GetOrCreateCreatesExactlyOnce)
{
    FakeManagedInstance inst;

    const CHAOS_IL2CPP_INTPTR first = ChaosManagedHandleGetOrCreate(
        AsIntPtr(&inst), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&CreateSemaphoreHook), 1, 2);
    ASSERT_NE(first, 0) << "first call must create a real semaphore";

    // The second call MUST return the same handle.  Creating a second native
    // object would be silent and destructive: the semaphore's count would then
    // live in whichever slot the racing call happened to use, so a Wait() and a
    // Release() could target different objects and the mutual exclusion would
    // simply not happen.
    const CHAOS_IL2CPP_INTPTR second = ChaosManagedHandleGetOrCreate(
        AsIntPtr(&inst), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&CreateSemaphoreHook), 1, 2);
    EXPECT_EQ(second, first) << "GetOrCreate must be idempotent per instance";

    // And it must be the handle the native table knows: acquire the one count.
    EXPECT_EQ(ChaosSemaphoreSlimWait(first, 0), 1)
        << "the created handle must be a live semaphore with its count intact";

    ChaosSemaphoreSlimDestroy(first);
}

TEST(ManagedHandleBinding, ReleaseClearsTheFieldBeforeDestroying)
{
    FakeManagedInstance inst;
    const CHAOS_IL2CPP_INTPTR sem = ChaosManagedHandleGetOrCreate(
        AsIntPtr(&inst), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&CreateSemaphoreHook), 2, 5);
    ASSERT_NE(sem, 0);

    EXPECT_EQ(ChaosManagedHandleRelease(
                  AsIntPtr(&inst), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&DestroySemaphoreHook)),
              1) << "release reports that it released something";

    EXPECT_EQ(ChaosManagedHandleLoad(AsIntPtr(&inst)), 0)
        << "the field must be cleared, so a later operation reads 'unbound' "
           "rather than an id that has already been destroyed";
    // The native object really is gone — this is the observable consequence.
    EXPECT_EQ(ChaosSemaphoreSlimWait(sem, 0), -1)
        << "after release the underlying semaphore must be invalid";
}

TEST(ManagedHandleBinding, ReleaseIsIdempotent)
{
    FakeManagedInstance inst;
    const CHAOS_IL2CPP_INTPTR sem = ChaosManagedHandleGetOrCreate(
        AsIntPtr(&inst), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&CreateSemaphoreHook), 1, 1);
    ASSERT_NE(sem, 0);

    EXPECT_EQ(ChaosManagedHandleRelease(
                  AsIntPtr(&inst), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&DestroySemaphoreHook)),
              1);
    // Managed Dispose() + finalizer can both run.  The second must be a no-op,
    // NOT a second destroy of an already-freed id — with id reuse in the native
    // tables that would tear down a different live object.
    EXPECT_EQ(ChaosManagedHandleRelease(
                  AsIntPtr(&inst), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&DestroySemaphoreHook)),
              0) << "a second release has nothing to release";
}

TEST(ManagedHandleBinding, AFailedCreateLeavesTheInstanceUnbound)
{
    FakeManagedInstance inst;

    // max_count <= 0 is rejected by SemaphoreSlimCreate, so this create fails.
    const CHAOS_IL2CPP_INTPTR bad = ChaosManagedHandleGetOrCreate(
        AsIntPtr(&inst), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&CreateSemaphoreHook), 0, 0);
    EXPECT_EQ(bad, 0) << "a rejected create must be reported";

    // The field must not have been written with the failure, so a later call
    // with valid arguments can still succeed rather than being poisoned.
    EXPECT_EQ(ChaosManagedHandleLoad(AsIntPtr(&inst)), 0);

    const CHAOS_IL2CPP_INTPTR good = ChaosManagedHandleGetOrCreate(
        AsIntPtr(&inst), reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&CreateSemaphoreHook), 1, 1);
    EXPECT_NE(good, 0) << "a failed create must not prevent a later successful one";

    ChaosSemaphoreSlimDestroy(good);
}

TEST(ManagedHandleBinding, ReleaseWithNoDestroyHookStillClearsTheField)
{
    FakeManagedInstance inst;
    ChaosManagedHandleStore(AsIntPtr(&inst), 0x42);

    // A null destroy_fn means "unbind without destroying" — used where the
    // native lifetime is owned elsewhere.  It must still clear the field;
    // leaving a stale id bound would make the next operation act on an object
    // this instance no longer owns.
    EXPECT_EQ(ChaosManagedHandleRelease(AsIntPtr(&inst), 0), 1);
    EXPECT_EQ(ChaosManagedHandleLoad(AsIntPtr(&inst)), 0);
}
