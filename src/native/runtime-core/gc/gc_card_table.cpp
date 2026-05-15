#include "gc_card_table.h"

#include <chaos/log.h>

#include <cstdlib>
#include <mutex>
#include <new>

namespace chaos::il2cpp::runtime_core {

// ── L1 segment pointer table (512 KB, lazily populated) ───────
std::atomic<CardSegment*> g_card_l1[kCardL1Entries] = {};

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
    if (start < g_heap_base) start = g_heap_base;

    uintptr_t first_idx = (start - g_heap_base) >> kCardShift;
    uintptr_t last_idx  = (end - 1 - g_heap_base) >> kCardShift;

    uintptr_t first_seg = first_idx / kCardsPerSegment;
    uintptr_t last_seg  = last_idx / kCardsPerSegment;

    for (uintptr_t si = first_seg; si <= last_seg && si < static_cast<uintptr_t>(kCardL1Entries); si++) {
        CardSegment* existing = g_card_l1[si].load(std::memory_order_acquire);
        if (existing != nullptr) continue;  // already allocated

        auto* seg = static_cast<CardSegment*>(CHAOS_IL2CPP_MALLOC(sizeof(CardSegment)));
        if (seg == nullptr) {
            CHAOS_IL2CPP_LOG_ERROR_M("CardTable", "OOM allocating segment {0}", si);
            return;
        }
        std::memset(seg->cards, 0, sizeof(seg->cards));

        // CAS to publish: if another thread raced and won, free ours.
        if (!g_card_l1[si].compare_exchange_strong(existing, seg,
                std::memory_order_release, std::memory_order_acquire)) {
            CHAOS_IL2CPP_FREE(seg);
        } else {
            // Track the newly allocated segment for fast ClearAllCards.
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