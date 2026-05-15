#ifndef CHAOS_IL2CPP_GC_CARD_TABLE_H_
#define CHAOS_IL2CPP_GC_CARD_TABLE_H_

#include <chaos/native_types.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>

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

/// L1 segment pointer table (512 KB).
/// Each entry is atomic for lock-free concurrent read in DirtyCard and
/// single-writer CAS in GcRegisterHeapRange.
extern std::atomic<CardSegment*> g_card_l1[kCardL1Entries];

/// Base address of the managed heap.  Set once at startup via GcSetHeapBase().
extern uintptr_t g_heap_base;

// ── Inline barrier helpers ─────────────────────────────────────

/// Mark the card covering @a obj as dirty.
/// Called from the post-write barrier stub inserted by codegen.
/// Two-level access: L1[idx / 128] → L2[idx % 128].
inline void DirtyCard(const void* obj) noexcept {
    uintptr_t addr = reinterpret_cast<uintptr_t>(obj);
    if (addr < g_heap_base) [[unlikely]] {
        return;  // below heap base — not managed memory
    }
    uintptr_t idx = (addr - g_heap_base) >> kCardShift;
    uintptr_t seg_idx = idx / kCardsPerSegment;
    if (seg_idx >= static_cast<uintptr_t>(kCardL1Entries)) [[unlikely]] {
        return;  // beyond card table coverage — not managed
    }
    uintptr_t card_idx = idx % kCardsPerSegment;
    auto* seg = g_card_l1[seg_idx].load(std::memory_order_relaxed);
    if (seg != nullptr) [[likely]] {
        seg->cards[card_idx] = 0xFF;
    }
}

/// Check whether the card covering @a obj is dirty.
inline bool IsDirty(const void* obj) noexcept {
    uintptr_t addr = reinterpret_cast<uintptr_t>(obj);
    if (addr < g_heap_base) return false;
    uintptr_t idx = (addr - g_heap_base) >> kCardShift;
    uintptr_t seg_idx = idx / kCardsPerSegment;
    if (seg_idx >= static_cast<uintptr_t>(kCardL1Entries)) return false;
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
    if (seg_idx >= static_cast<uintptr_t>(kCardL1Entries)) return;
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

/// Register a heap range [start, end) with the card table.
/// Allocates L2 segments for any L1 entries that are still null.
/// Thread-safe: uses CAS to avoid double allocation.
/// Called from old-gen page allocation (under mutex, but may race with
/// concurrent DirtyCard reads on other threads).
void GcRegisterHeapRange(uintptr_t start, uintptr_t end);

/// Clear the entire card table (e.g., after young GC).
/// Uses a tracked segment list to avoid walking all 64K L1 entries.
void ClearAllCards() noexcept;

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

    for (uintptr_t si = first_seg; si <= last_seg && si < static_cast<uintptr_t>(kCardL1Entries); si++) {
        auto* seg = g_card_l1[si].load(std::memory_order_acquire);
        if (seg == nullptr) continue;

        uintptr_t seg_first_card = (si == first_seg) ? (first % kCardsPerSegment) : 0;
        uintptr_t seg_last_card  = (si == last_seg)  ? (last  % kCardsPerSegment) : (kCardsPerSegment - 1);

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

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_CARD_TABLE_H_