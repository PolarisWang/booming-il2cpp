// managed_handle_stubs.cpp — T2.1: managed-object ↔ native-handle binding.
//
// See managed_handle_stubs.h for the design rationale and the precedents this
// follows.  The short version: the handle is a field ON the managed object read
// at a fixed offset, not an entry in a table keyed by object identity.
//
// Every function here is deliberately total: a null instance, an unbound field,
// and a zero handle are all *answered* rather than being treated as programmer
// errors to assert on.  Generated C++ calls these on paths it cannot fully
// reason about (a Dispose() racing a Wait()), and a hard failure there would
// turn a recoverable "not initialized yet" into a crash.

#include "managed_handle_stubs.h"

#include "generated_code_compat.h"   // ThinLockableHeader

#include <cstddef>
#include <cstdint>

namespace {

// ── The offset ─────────────────────────────────────────────────────────
//
// The handle lives immediately after the object header.  `ThinLockableHeader`
// is 8 bytes on this runtime (TypeInfo* at [0]; sync_state was moved out to
// ThinLockTable — see generated_code_compat.h), and the same "field right
// after the header" convention is what `chaos_list_fields` documents for the
// inline List<T> view.
//
// This is a LAYOUT CONTRACT with codegen's field emission order, not a local
// choice: if the object-model emission ever inserts a field before the handle,
// this offset must move with it.  The static_assert keeps the 8-byte assumption
// from silently rotting if the header changes again.
constexpr std::size_t kHandleOffset = sizeof(ThinLockableHeader);
static_assert(kHandleOffset == 8,
              "T2.1 assumes the handle sits at offset 8, immediately after an "
              "8-byte ThinLockableHeader.  If the header grew, the managed "
              "object-model emission order must be re-checked before this "
              "offset is changed — the two are a matched pair.");

// ── Handle field access ────────────────────────────────────────────────

/// The handle slot on a managed instance, or nullptr for a null instance.
CHAOS_IL2CPP_INTPTR* HandleSlot(CHAOS_IL2CPP_INTPTR instance) noexcept {
    if (instance == 0) return nullptr;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(
        reinterpret_cast<CHAOS_IL2CPP_UINT8*>(instance) + kHandleOffset);
}

}  // anonymous namespace

extern "C" {

CHAOS_IL2CPP_INTPTR ChaosManagedHandleStore(CHAOS_IL2CPP_INTPTR instance,
                                            CHAOS_IL2CPP_INTPTR handle) noexcept
{
    auto* slot = HandleSlot(instance);
    if (slot == nullptr) return 0;   // null instance: nothing to bind to
    *slot = handle;
    return handle;
}

CHAOS_IL2CPP_INTPTR ChaosManagedHandleLoad(CHAOS_IL2CPP_INTPTR instance) noexcept
{
    auto* slot = HandleSlot(instance);
    if (slot == nullptr) return 0;
    return *slot;
}

CHAOS_IL2CPP_INT32 ChaosManagedHandleRelease(CHAOS_IL2CPP_INTPTR instance,
                                             CHAOS_IL2CPP_INTPTR destroy_fn) noexcept
{
    auto* slot = HandleSlot(instance);
    if (slot == nullptr) return 0;

    const CHAOS_IL2CPP_INTPTR handle = *slot;
    if (handle == 0) return 0;   // already released (or never bound): idempotent

    // Clear first, destroy second.  A concurrent caller that reads between the
    // two finds 0 and fails cleanly; the reverse order would let it observe an
    // id that is about to be destroyed.
    *slot = 0;

    if (destroy_fn != 0) {
        // The destroy signature is whatever the native family uses; the only
        // universal part is that it takes the handle and returns a status we do
        // not model here (the caller's managed Dispose() ignores it too).
        using DestroyFn = void (*)(CHAOS_IL2CPP_INTPTR);
        reinterpret_cast<DestroyFn>(destroy_fn)(handle);
    }
    return 1;
}

CHAOS_IL2CPP_INTPTR ChaosManagedHandleGetOrCreate(CHAOS_IL2CPP_INTPTR instance,
                                                  CHAOS_IL2CPP_INTPTR create_fn,
                                                  CHAOS_IL2CPP_INTPTR arg0,
                                                  CHAOS_IL2CPP_INTPTR arg1) noexcept
{
    auto* slot = HandleSlot(instance);
    if (slot == nullptr) return 0;

    // Already bound: return it.  This is the common path — every operation after
    // the first does one load.
    if (*slot != 0) return *slot;

    // Unbound: create.  A null create_fn is a caller error, but answered rather
    // than asserted, for the same reason every other guard here is.
    if (create_fn == 0) return 0;

    using CreateFn = CHAOS_IL2CPP_INTPTR (*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR);
    const CHAOS_IL2CPP_INTPTR created =
        reinterpret_cast<CreateFn>(create_fn)(arg0, arg1);
    if (created == 0) return 0;

    // Store only on success.  A failed create must leave the field as it was —
    // writing 0 back would be harmless for the load path (0 already means
    // unbound) but would erase the distinction for any future caller that wants
    // to know whether a create was ever attempted.
    *slot = created;
    return created;
}

}  // extern "C"
