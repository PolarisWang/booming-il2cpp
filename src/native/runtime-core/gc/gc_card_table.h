#ifndef CHAOS_IL2CPP_GC_CARD_TABLE_H_
#define CHAOS_IL2CPP_GC_CARD_TABLE_H_

#include <chaos/native_types.h>

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "gc_region.h"

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// CRAG Card Table — cross-generation + cross-domain write barrier
//
// A single card table serves two purposes:
//   1. Cross-gen: track old→young references (generational GC invariant)
//   2. Cross-domain: track core→hotupdate references (domain unload safety)
//
// == Constants ==
// - kCardSize:   512 bytes per card (matches CoreCLR convention)
// - kTableSize:  2 MB (covers 1 GB heap, tunable at runtime)
//
// == Barrier cost ==
// ~5 native instructions (sub + shr + mov), no branches, no cache-miss risk.
// ======================================================================

static constexpr CHAOS_IL2CPP_SIZE kCardSize = 512;          // bytes per card
static constexpr CHAOS_IL2CPP_SIZE kCardShift = 9;           // log2(kCardSize)
static constexpr CHAOS_IL2CPP_SIZE kCardTableEntries = 2 * 1024 * 1024;  // 2M cards → 1GB heap

/// Global card table.  Each byte corresponds to one 512-byte card.
/// 0xFF = dirty (has a potentially interesting cross-region pointer).
/// 0x00 = clean (no cross-region pointers in this card).
extern uint8_t g_card_table[kCardTableEntries];

/// Base address of the managed heap.  Set once at startup via GcSetHeapBase().
/// All card-table index calculations use: idx = (ptr - s_heap_base) >> kCardShift
extern uintptr_t g_heap_base;

// ── Inline barrier helpers ─────────────────────────────────────

/// Mark the card covering @a obj as dirty.
/// Called from the post-write barrier stub inserted by codegen.
inline void DirtyCard(const void* obj) noexcept {
    uintptr_t addr = reinterpret_cast<uintptr_t>(obj);
    uintptr_t idx = (addr - g_heap_base) >> kCardShift;
    if (idx < kCardTableEntries) [[likely]] {
        g_card_table[idx] = 0xFF;
    }
}

/// Check whether the card covering @a obj is dirty.
inline bool IsDirty(const void* obj) noexcept {
    uintptr_t addr = reinterpret_cast<uintptr_t>(obj);
    uintptr_t idx = (addr - g_heap_base) >> kCardShift;
    if (idx >= kCardTableEntries) return false;
    return g_card_table[idx] == 0xFF;
}

/// Clear the card covering @a obj.
inline void ClearCard(const void* obj) noexcept {
    uintptr_t addr = reinterpret_cast<uintptr_t>(obj);
    uintptr_t idx = (addr - g_heap_base) >> kCardShift;
    if (idx < kCardTableEntries) [[likely]] {
        g_card_table[idx] = 0;
    }
}

// ── Runtime API ────────────────────────────────────────────────

/// Set the managed heap base address.  Must be called once at startup
/// before any write barrier can function.
inline void GcSetHeapBase(void* heap_base) noexcept {
    g_heap_base = reinterpret_cast<uintptr_t>(heap_base);
}

/// Clear the entire card table (e.g., after a full GC).
inline void ClearAllCards() noexcept {
    std::memset(g_card_table, 0, sizeof(g_card_table));
}

/// Scan the card table for dirty cards within the range [@a start, @a end).
/// Calls @a callback(card_index, card_start, card_end) for each dirty card.
/// The callback receives pointers to the start and end of the 512-byte card.
template <typename Fn>
inline void ScanDirtyCards(uintptr_t start, uintptr_t end, Fn&& callback) noexcept {
    uintptr_t first = (start - g_heap_base) >> kCardShift;
    uintptr_t last  = (end - 1 - g_heap_base) >> kCardShift;

    for (uintptr_t i = first; i <= last && i < kCardTableEntries; i++) {
        if (g_card_table[i] != 0) {
            uintptr_t card_start = g_heap_base + (i << kCardShift);
            uintptr_t card_end   = card_start + kCardSize;
            callback(i, card_start, card_end);
        }
    }
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_CARD_TABLE_H_