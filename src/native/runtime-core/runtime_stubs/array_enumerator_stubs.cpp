// ── Array enumerator implementations (T6) ─────────────────────────────────
// See array_enumerator_stubs.h for why these exist.

#include "array_enumerator_stubs.h"

#include <cstddef>
#include <cstring>

#include "runtime_stubs/stub_common.h"
#include "gc_helpers.h"

// GcAllocateAtomic lives in chaos::il2cpp::runtime_core (declared in
// gc_helpers.h); this file's definitions sit at global scope inside the
// extern "C" block below, so the name must be pulled in explicitly.  Same
// idiom as array_stubs.cpp.
using chaos::il2cpp::runtime_core::GcAllocateAtomic;

extern "C" {

namespace {

/// Native enumerator state.  `cursor` is the index of the element MoveNext
/// most recently advanced onto (-1 before the first MoveNext, matching the
/// IEnumerator contract where get_Current is undefined until MoveNext).
struct ChaosArrayEnumerator {
    CHAOS_IL2CPP_INTPTR array;   // source managed array (0 when exhausted)
    CHAOS_IL2CPP_INTPTR cursor;  // -1 before first MoveNext, else current index
};

/// Recover the enumerator from an opaque handle.  Returns nullptr for 0.
ChaosArrayEnumerator* AsEnumerator(CHAOS_IL2CPP_INTPTR handle) noexcept {
    return reinterpret_cast<ChaosArrayEnumerator*>(handle);
}

}  // namespace

CHAOS_IL2CPP_INTPTR ChaosArrayGetEnumerator(CHAOS_IL2CPP_INTPTR array) noexcept {
    // A null source enumerates as empty rather than throwing: the call sites
    // in Assert.AreEqual<T> have already null-checked their operands, and a
    // hard failure here would turn a comparison outcome into a crash.
    if (array == 0) return 0;

    auto* storage = static_cast<CHAOS_IL2CPP_UINT8*>(
        GcAllocateAtomic(sizeof(ChaosArrayEnumerator)));
    if (storage == nullptr) return 0;

    auto* enumerator = reinterpret_cast<ChaosArrayEnumerator*>(storage);
    enumerator->array = array;
    enumerator->cursor = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(enumerator);
}

CHAOS_IL2CPP_INTPTR ChaosArrayEnumeratorMoveNext(CHAOS_IL2CPP_INTPTR handle) noexcept {
    auto* enumerator = AsEnumerator(handle);
    if (enumerator == nullptr) return 0;

    const auto array = enumerator->array;
    if (array == 0) return 0;

    const auto* accessor = get_managed_array(array);
    if (accessor == nullptr) return 0;

    const auto next = enumerator->cursor + 1;
    if (next >= accessor->length ||
        next < 0)  // guard against overflow past INTPTR_MAX
    {
        // Exhausted: drop the array reference so the enumerator no longer
        // pins the source (it stays allocated until the GC sweeps it).
        enumerator->array = 0;
        return 0;
    }

    enumerator->cursor = next;
    return 1;
}

CHAOS_IL2CPP_INTPTR ChaosArrayEnumeratorGetCurrent(CHAOS_IL2CPP_INTPTR handle) noexcept {
    auto* enumerator = AsEnumerator(handle);
    if (enumerator == nullptr) return 0;
    if (enumerator->cursor < 0) return 0;   // before first MoveNext

    const auto array = enumerator->array;
    if (array == 0) return 0;

    const auto* accessor = get_managed_array(array);
    if (accessor == nullptr) return 0;
    if (enumerator->cursor >= accessor->length) return 0;

    return accessor_get_elements(accessor)[enumerator->cursor];
}

}  // extern "C"
