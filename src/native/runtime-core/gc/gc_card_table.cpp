#include "gc_card_table.h"

#include <chaos/log.h>

#include <cstdlib>
#include <memory>
#include <mutex>
#include <new>

namespace chaos::il2cpp::runtime_core {

// ── L1 segment pointer table (dynamically growing) ──────────────
// Initial size: kCardL1Entries (64K entries = 512 KB for 4 GB coverage).
// Grows via GcRegisterHeapRange when the heap exceeds 4 GB.
// unique_ptr<T[]> used because std::atomic is not copyable (MSVC STL).
std::unique_ptr<std::atomic<CardSegment*>[]> g_card_l1(
    new std::atomic<CardSegment*>[kCardL1Entries]());
std::atomic<size_t> g_card_l1_size{kCardL1Entries};

// ── Tracked segment list for O(allocated) ClearAllCards ───────
struct CardSegmentNode {
    CardSegment* segment;
    CardSegmentNode* next;
};

static CardSegmentNode* g_card_segment_list = nullptr;
static std::mutex g_card_segment_list_mutex;

// ── Heap base (set once at startup) ────────────────────────────
uintptr_t g_heap_base = 0;

void GcRegisterHeapRange(uintptr_t start, uintptr_t end) {
    if (start >= end) return;

    // First-time initialization.
    uintptr_t old_base = g_heap_base;
    if (old_base == 0) {
        g_heap_base = start;
        old_base = start;
    }

    // ── Handle range below current base ────────────────────────────
    // When a new page is allocated at an address below g_heap_base (can
    // happen with VirtualAlloc without a hint address), expand the L1
    // table downward and adjust g_heap_base so the write barrier's
    // (addr - g_heap_base) computation remains correct for all pages.
    if (start < old_base) {
        uintptr_t base_diff = old_base - start;
        size_t extra_segs = (base_diff + kSegmentCoverage - 1) / kSegmentCoverage;

        size_t current_size = g_card_l1_size.load(std::memory_order_acquire);
        size_t new_size = current_size + extra_segs;

        // Check if the end of this range also requires upward growth.
        uintptr_t rel_last_idx = (end - 1 - start) >> kCardShift;
        uintptr_t rel_last_seg = rel_last_idx / kCardsPerSegment;
        while (rel_last_seg >= new_size) {
            new_size *= 2;
        }

        auto new_table = std::make_unique<std::atomic<CardSegment*>[]>(new_size);
        // New lower entries: zero-initialized (nullptr) for future segment allocation.
        for (size_t i = 0; i < extra_segs; i++) {
            new_table[i].store(nullptr, std::memory_order_relaxed);
        }
        // Existing entries shifted up by extra_segs.
        for (size_t i = 0; i < current_size; i++) {
            new_table[i + extra_segs].store(
                g_card_l1[i].load(std::memory_order_relaxed),
                std::memory_order_relaxed);
        }
        // Remaining upper entries (if new_size > current_size + extra_segs):
        // already zero-initialized by the unique_ptr allocator.

        g_card_l1.swap(new_table);
        g_card_l1_size.store(new_size, std::memory_order_release);
        g_heap_base = start;
    }

    // ── Compute segment range relative to (possibly updated) base ──
    uintptr_t first_idx = (start - g_heap_base) >> kCardShift;
    uintptr_t last_idx  = (end - 1 - g_heap_base) >> kCardShift;

    uintptr_t first_seg = first_idx / kCardsPerSegment;
    uintptr_t last_seg  = last_idx / kCardsPerSegment;

    // ── Grow L1 table upward if this range exceeds coverage ────────
    size_t current_size = g_card_l1_size.load(std::memory_order_acquire);
    if (last_seg >= current_size) {
        size_t new_size = current_size;
        while (last_seg >= new_size) {
            new_size *= 2;
        }
        auto new_table = std::make_unique<std::atomic<CardSegment*>[]>(new_size);
        for (size_t i = 0; i < current_size; i++) {
            new_table[i].store(g_card_l1[i].load(std::memory_order_relaxed),
                               std::memory_order_relaxed);
        }
        g_card_l1.swap(new_table);
        g_card_l1_size.store(new_size, std::memory_order_release);
        current_size = new_size;
    }

    // ── Allocate L2 segments for any null entries ──────────────────
    for (uintptr_t si = first_seg; si <= last_seg; si++) {
        CardSegment* existing = g_card_l1[si].load(std::memory_order_acquire);
        if (existing != nullptr) continue;

        auto* seg = static_cast<CardSegment*>(CHAOS_IL2CPP_MALLOC(sizeof(CardSegment)));
        if (seg == nullptr) {
            CHAOS_IL2CPP_LOG_ERROR_M("CardTable", "OOM allocating segment {0}", si);
            return;
        }
        std::memset(seg->cards, 0, sizeof(seg->cards));

        if (!g_card_l1[si].compare_exchange_strong(existing, seg,
                std::memory_order_release, std::memory_order_acquire)) {
            CHAOS_IL2CPP_FREE(seg);
        } else {
            std::lock_guard<std::mutex> lock(g_card_segment_list_mutex);
            auto* node = static_cast<CardSegmentNode*>(CHAOS_IL2CPP_MALLOC(sizeof(CardSegmentNode)));
            if (node != nullptr) {
                node->segment = seg;
                node->next = g_card_segment_list;
                g_card_segment_list = node;
            }
        }
    }
}

void ClearAllCards() noexcept {
    // Walk only tracked segments (not all 64K L1 entries).
    // This is O(allocated_segments) instead of O(64K).
    std::lock_guard<std::mutex> lock(g_card_segment_list_mutex);
    auto* node = g_card_segment_list;
    while (node != nullptr) {
        std::memset(node->segment->cards, 0, sizeof(node->segment->cards));
        node = node->next;
    }
}

}  // namespace chaos::il2cpp::runtime_core