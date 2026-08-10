#ifndef CHAOS_IL2CPP_GC_CARD_TABLE_H_
#define CHAOS_IL2CPP_GC_CARD_TABLE_H_

#include <chaos/native_types.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>

#include "gc_bit_utils.h"
#include "gc_region.h"

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// CRAG Card Table — two-level cross-generation + cross-domain write barrier
//
// == Motivation ==
// The previous flat card table (2M entries, 1GB heap cap) used an overflow
// flag when the heap exceeded 1GB, forcing conservative scanning of the
// entire heap — a correctness concern for large heaps.
//
// == Two-level design (CoreCLR-aligned) ==
// Level 1 (L1): 64K entries of CardSegment* (512KB total)
// Level 2 (L2): CardSegment with 128 card bytes, each covering 512B
//
// Coverage: 64K × 128 × 512 = 4 GB heap
// Barrier cost: sub + shr + load + and + conditional branch + store
// No mutex, no resize, no overflow fallback for heaps ≤ 4 GB.
//
// == Constants ==
// - kCardSize:        512 bytes per card (matches CoreCLR convention)
// - kCardsPerSegment: 128 cards per L2 segment
// - kSegmentCoverage: 64 KB per segment
// - kCardL1Entries:   64K L1 entries → 4 GB max heap
//
// == Segment lifecycle ==
// Segments are allocated on demand via GcRegisterHeapRange() when a new
// old-gen page is committed.  The write barrier (DirtyCard) reads the L1
// entry with a relaxed load — the segment must already exist because page
// registration precedes object allocation within the page.
//
// == Barrier cost ==
// ~6 native instructions (sub + shr + div + and + load + store), no mutex,
// one predictable branch (segment-null check, always taken on hot path).
// ======================================================================

static constexpr CHAOS_IL2CPP_SIZE kCardSize = 512;            // bytes per card
static constexpr CHAOS_IL2CPP_SIZE kCardShift = 9;             // log2(kCardSize)

// Two-level parameters
static constexpr int kCardsPerSegment = 128;                     // cards per L2 segment
static constexpr CHAOS_IL2CPP_SIZE kSegmentCoverage =
    static_cast<CHAOS_IL2CPP_SIZE>(kCardsPerSegment) * kCardSize;  // 64 KB
static constexpr int kCardL1Entries = 64 * 1024;                // 64K → 4 GB coverage

/// One L2 segment: 128 card bytes covering 64 KB of heap.
struct CardSegment {
    uint8_t cards[kCardsPerSegment];
};

/// L1 segment pointer table (dynamically growing).
/// Each entry is atomic for lock-free concurrent read in DirtyCard and
/// single-writer CAS in GcRegisterHeapRange.
/// Initial size: 64K entries (512 KB), auto-grows for heaps > 4 GB.
/// Uses unique_ptr<T[]> because std::atomic is not copyable (MSVC).
/// Size tracked separately in g_card_l1_size (atomic for lock-free read).
extern std::unique_ptr<std::atomic<CardSegment*>[]> g_card_l1;
extern std::atomic<size_t> g_card_l1_size;

/// Card bundle (GC-K2d, align CoreCLR card_bundle): a sparse 1-bit-per-2MB
/// upper index over the card table.  Set alongside a card write; ScanDirtyCards
/// checks it first to skip entire clean 2MB chunks without touching L2 cards.
/// size = (max_covered_bytes >> kCardBundleShift + 7) / 8 bytes.
/// kCardBundleShift = 21 → 2 MB per bundle bit.  Non-atomic: grown under the
/// card-table mutex alongside g_card_l1; bits written via relaxed stores.
static constexpr CHAOS_IL2CPP_SIZE kCardBundleShift = 21;
extern uint8_t* g_card_bundle;
extern std::atomic<size_t> g_card_bundle_size;

/// Get the bundle bit for @a heap_offset_idx (card-index space → 2MB units).
inline uintptr_t CardBundleBit(uintptr_t card_global_idx) noexcept {
    return (card_global_idx >> (kCardBundleShift - kCardShift));
}
/// Test whether a bundle bit is dirty.
inline bool CardBundleTest(uintptr_t bundle_bit) noexcept {
    if (g_card_bundle == nullptr || bundle_bit >= g_card_bundle_size.load(std::memory_order_relaxed) * 8)
        return true;  // out of coverage → conservatively "possibly dirty"
    return (g_card_bundle[bundle_bit >> 3] & (uint8_t)(1u << (bundle_bit & 7))) != 0;
}
/// Set a bundle bit (relaxed; called alongside DirtyCard).
inline void CardBundleSet(uintptr_t bundle_bit) noexcept {
    if (g_card_bundle == nullptr || bundle_bit >= g_card_bundle_size.load(std::memory_order_relaxed) * 8)
        return;
    g_card_bundle[bundle_bit >> 3] |= (uint8_t)(1u << (bundle_bit & 7));
}
/// Clear all bundle bits (called alongside ClearAllCards).
inline void CardBundleClearAll() noexcept {
    if (g_card_bundle == nullptr) return;
    std::memset(g_card_bundle, 0, g_card_bundle_size.load(std::memory_order_relaxed));
}

/// Base address of the managed heap.  Set once at startup via GcSetHeapBase().
extern uintptr_t g_heap_base;

/// Nursery address range for DirtyCard fast skip.
/// When the written-to address is within the nursery, the card table entry
/// is unnecessary — young GC Phase 2 scans the entire nursery precisely
/// (object-by-object).  DirtyCard can skip the card table entirely for
/// nursery-internal writes, saving an L2 segment access (~cache miss).
/// Updated by GcSetCardTableNurseryRange() when the nursery is resized.
extern uintptr_t g_nursery_range_begin;
extern uintptr_t g_nursery_range_end;

// ── Inline barrier helpers ─────────────────────────────────────

/// Mark the card covering @a obj as dirty.
/// Called from the post-write barrier stub inserted by codegen.
/// Two-level access: L1[idx / 128] → L2[idx % 128].
inline void DirtyCard(const void* obj) noexcept {
    uintptr_t addr = reinterpret_cast<uintptr_t>(obj);
    if (addr < g_heap_base) [[unlikely]] {
        return;  // below heap base — not managed memory
    }
    // Nursery fast skip: young GC Phase 2 scans the entire nursery
    // precisely (object-by-object with TypeInfo layouts).  The card
    // table is only needed for old-gen/LOH objects that young GC
    // would not otherwise scan.  Skipping nursery writes avoids an
    // expensive L2 segment access (cache miss) on the hot path.
    // The globals use relaxed ordering — this is an optimization hint;
    // false-negative (miss nursery) adds a harmless card write, and
    // false-positive (skip needed card) cannot happen because nursery
    // writes never need card tracking.
    if (addr >= g_nursery_range_begin && addr < g_nursery_range_end) [[likely]] {
        return;
    }
    uintptr_t idx = (addr - g_heap_base) >> kCardShift;
    uintptr_t seg_idx = idx / kCardsPerSegment;
    if (seg_idx >= g_card_l1_size.load(std::memory_order_relaxed)) [[unlikely]] {
        return;  // beyond card table coverage — not managed
    }
    uintptr_t card_idx = idx % kCardsPerSegment;
    auto* seg = g_card_l1[seg_idx].load(std::memory_order_relaxed);
    if (seg != nullptr) [[likely]] {
        // DC optimization: skip the store if the card is already dirty.
        // On multiprocessor systems this avoids cache-line invalidation
        // traffic for repeated writes to the same 512-byte card.
        if (seg->cards[card_idx] != 0xFF) {
            seg->cards[card_idx] = 0xFF;
            // GC-K2d: also set the 2MB bundle bit so ScanDirtyCards can fast-skip
            // clean chunks (align CoreCLR card_bundle_set).
            CardBundleSet(CardBundleBit(idx));
        }
    }
}

/// Inline wrapper for codegen-emitted barrier calls.  Defined in the header
/// so the compiler can inline it into generated C++ code (avoiding the
/// function-call overhead of the out-of-line definition in gc_api.cpp).
/// The codegen includes this header via <gc/gc_card_table.h>.
extern "C" void chaos_gc_dirty_card(const void* obj) noexcept;

/// GC-K2b generation-aware write barrier (dst + stored ref) — codegen emits
/// this at stfld / stelem.ref / stobj where the stored reference is available
/// to skip gen0→gen0 / same-mature card marking (faithful to CoreCLR region
/// write barrier).  Fallback to chaos_gc_dirty_card(dst) when ref is unknown.
extern "C" void chaos_gc_dirty_card_dst_ref(const void* dst, const void* ref) noexcept;

/// Check whether the card covering @a obj is dirty.
inline bool IsDirty(const void* obj) noexcept {
    uintptr_t addr = reinterpret_cast<uintptr_t>(obj);
    if (addr < g_heap_base) return false;
    uintptr_t idx = (addr - g_heap_base) >> kCardShift;
    uintptr_t seg_idx = idx / kCardsPerSegment;
    if (seg_idx >= g_card_l1_size.load(std::memory_order_acquire)) return false;
    uintptr_t card_idx = idx % kCardsPerSegment;
    auto* seg = g_card_l1[seg_idx].load(std::memory_order_acquire);
    if (seg == nullptr) return false;
    return seg->cards[card_idx] == 0xFF;
}

/// Clear the card covering @a obj.
inline void ClearCard(const void* obj) noexcept {
    uintptr_t addr = reinterpret_cast<uintptr_t>(obj);
    if (addr < g_heap_base) return;
    uintptr_t idx = (addr - g_heap_base) >> kCardShift;
    uintptr_t seg_idx = idx / kCardsPerSegment;
    if (seg_idx >= g_card_l1_size.load(std::memory_order_acquire)) return;
    uintptr_t card_idx = idx % kCardsPerSegment;
    auto* seg = g_card_l1[seg_idx].load(std::memory_order_relaxed);
    if (seg != nullptr) {
        seg->cards[card_idx] = 0;
    }
}

// ── Runtime API ────────────────────────────────────────────────

/// Set the managed heap base address.  Must be called once at startup
/// before any write barrier can function.
inline void GcSetHeapBase(void* heap_base) noexcept {
    g_heap_base = reinterpret_cast<uintptr_t>(heap_base);
}

/// Set the nursery address range for DirtyCard fast skip.
/// Thread-safe for concurrent DirtyCard readers: uses volatile stores
/// so the write propogates, but callers must ensure the nursery is fully
/// initialized before threads can write to managed objects in it.
void GcSetCardTableNurseryRange(uintptr_t begin, uintptr_t end) noexcept;

/// Register a heap range [start, end) with the card table.
/// Allocates L2 segments for any L1 entries that are still null.
/// Thread-safe: uses CAS to avoid double allocation.
/// Called from old-gen page allocation (under mutex, but may race with
/// concurrent DirtyCard reads on other threads).
void GcRegisterHeapRange(uintptr_t start, uintptr_t end);

/// Unregister a heap range [start, end) from the card table.
/// Sets L1 entries to null and frees the corresponding L2 segments.
/// Thread-safe: uses CAS and is safe against concurrent DirtyCard reads
/// (a null L1 entry is treated as "no segment" by DirtyCard).
/// Called from LOH sweep when releasing excess segments to the OS.
void GcUnregisterHeapRange(uintptr_t start, uintptr_t end) noexcept;

/// Clear the entire card table (e.g., after young GC).
/// Uses a tracked segment list to avoid walking all 64K L1 entries.
void ClearAllCards() noexcept;

/// Clear card table entries within address range [start, end).
/// More precise than ClearAllCards: only clears card bytes for segments
/// that overlap the given range.  Used by young GC to clear nursery
/// (and Gen1) cards without destroying old-gen card data that concurrent
/// BGC mark may still depend on for STW re-mark.
void ClearCardRange(uintptr_t start, uintptr_t end) noexcept;

/// Scan the card table for dirty cards within the range [@a start, @a end).
/// Calls @a callback(card_index, card_start, card_end) for each dirty card.
/// Two-level iteration: walks L1 segments, then dirty card bytes within.
template <typename Fn>
inline void ScanDirtyCards(uintptr_t start, uintptr_t end, Fn&& callback) noexcept {
    if (start < g_heap_base) start = g_heap_base;
    if (end <= g_heap_base) return;

    uintptr_t first = (start - g_heap_base) >> kCardShift;
    uintptr_t last  = (end - 1 - g_heap_base) >> kCardShift;

    uintptr_t first_seg = first / kCardsPerSegment;
    uintptr_t last_seg  = last / kCardsPerSegment;

    for (uintptr_t si = first_seg; si <= last_seg && si < g_card_l1_size.load(std::memory_order_acquire); si++) {
        auto* seg = g_card_l1[si].load(std::memory_order_acquire);
        if (seg == nullptr) continue;

        // GC-K2d: card-bundle fast skip (align CoreCLR find_card_dword).  If
        // this segment's 2MB bundle bit is CLEAR, no card in the entire chunk
        // was dirtied — skip the whole segment without touching L2 cards.
        uintptr_t global_card_for_seg = si * kCardsPerSegment;
        if (!CardBundleTest(CardBundleBit(global_card_for_seg))) {
            continue;
        }

        uintptr_t seg_first_card = (si == first_seg) ? (first % kCardsPerSegment) : 0;
        uintptr_t seg_last_card  = (si == last_seg)  ? (last  % kCardsPerSegment) : (kCardsPerSegment - 1);

        // SIMD fast-skip for full-segment scan: check all 128 bytes at once.
        if (seg_first_card == 0 && seg_last_card == (kCardsPerSegment - 1)) {
            if (!GcSegmentHasDirtyCards(seg->cards)) {
                continue;  // entire segment clean — skip
            }
        }

        for (uintptr_t ci = seg_first_card; ci <= seg_last_card; ci++) {
            if (seg->cards[ci] != 0) {
                uintptr_t global_card_idx = si * kCardsPerSegment + ci;
                uintptr_t card_start = g_heap_base + (global_card_idx << kCardShift);
                uintptr_t card_end   = card_start + kCardSize;
                callback(global_card_idx, card_start, card_end);
            }
        }
    }
}

/// Scan the card table for dirty cards within [@a start, @a end), batching
/// consecutive dirty cards into a single callback(range_start, range_end) call.
/// This reduces per-card callback overhead for young GC card table scans where
/// adjacent cards are often all dirty (e.g., after a large object array write).
/// @param dirty_card_count  Optional accumulator for total dirty cards found.
/// @param callback  Called as callback(range_start, range_end) for each batch.
template <typename Fn>
inline void ScanDirtyCardsBatched(uintptr_t start, uintptr_t end,
                                   CHAOS_IL2CPP_SIZE* dirty_card_count,
                                   Fn&& callback) noexcept {
    if (start < g_heap_base) start = g_heap_base;
    if (end <= g_heap_base) return;

    uintptr_t first = (start - g_heap_base) >> kCardShift;
    uintptr_t last  = (end - 1 - g_heap_base) >> kCardShift;

    uintptr_t first_seg = first / kCardsPerSegment;
    uintptr_t last_seg  = last / kCardsPerSegment;

    bool in_run = false;
    uintptr_t run_start_addr = 0;

    for (uintptr_t si = first_seg; si <= last_seg && si < g_card_l1_size.load(std::memory_order_acquire); si++) {
        auto* seg = g_card_l1[si].load(std::memory_order_acquire);
        if (seg == nullptr) {
            in_run = false;
            continue;
        }

        uintptr_t seg_first_card = (si == first_seg) ? (first % kCardsPerSegment) : 0;
        uintptr_t seg_last_card  = (si == last_seg)  ? (last  % kCardsPerSegment) : (kCardsPerSegment - 1);

        // SIMD fast-skip for full-segment scan: check all 128 bytes at once.
        if (seg_first_card == 0 && seg_last_card == (kCardsPerSegment - 1)) {
            if (!GcSegmentHasDirtyCards(seg->cards)) {
                in_run = false;
                continue;  // entire segment clean — skip
            }
        }

        for (uintptr_t ci = seg_first_card; ci <= seg_last_card; ci++) {
            if (seg->cards[ci] != 0) {
                if (dirty_card_count) (*dirty_card_count)++;
                if (!in_run) {
                    run_start_addr = g_heap_base + ((si * kCardsPerSegment + ci) << kCardShift);
                    in_run = true;
                }
            } else {
                if (in_run) {
                    callback(run_start_addr,
                             g_heap_base + ((si * kCardsPerSegment + ci) << kCardShift));
                    in_run = false;
                }
            }
        }
    }

    // Flush final dirty run — extend to the scan range end
    if (in_run) {
        callback(run_start_addr, end);
    }
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_CARD_TABLE_H_