#include "gc_card_table.h"

#include <chaos/log.h>

#include <cstdlib>
#include <memory>
#include <mutex>
#include <new>
#include <vector>

namespace chaos::il2cpp::runtime_core {

// ── L1 segment pointer table (dynamically growing) ──────────────
// Initial size: kCardL1Entries (64K entries = 512 KB for 4 GB coverage).
// Grows via GcRegisterHeapRange when the heap exceeds 4 GB.
// unique_ptr<T[]> used because std::atomic is not copyable (MSVC STL).
std::unique_ptr<std::atomic<CardSegment*>[]> g_card_l1(
    new std::atomic<CardSegment*>[kCardL1Entries]());
std::atomic<size_t> g_card_l1_size{kCardL1Entries};

// ── Card bundle : 1 bit per 2MB heap chunk, upper sparse index ──
// Sized to cover g_card_l1_size segments (each segment = 64KB).  One bundle
// bit covers 2MB = 32 card segments.  Grown alongside the L1 table.
uint8_t* g_card_bundle = nullptr;
std::atomic<size_t> g_card_bundle_size{0};

/// Ensure the card bundle bitset covers @a seg_count card-table segments.
/// Bytes needed = (seg_count * kBytesPerSegment) >> kCardBundleShift, /8.
/// Called under the card-table lock from GcRegisterHeapRange.
static void EnsureCardBundleCoverage(size_t seg_count) noexcept {
    // Pre-allocate the bundle ONCE to a fixed, generous capacity and never
    // reallocate afterwards.  1 bit covers 2 MB, so covering a large heap is
    // tiny (e.g. 64 GB -> 64 GB / 2 MB / 8 = 4 KB).  Keeping g_card_bundle a
    // stable pointer after init is critical: CardBundleSet runs on the hot
    // DirtyCard write path and may be concurrent with GC/setup — a realloc
    // under that traffic would be a use-after-free.
    if (g_card_bundle != nullptr) return;   // already allocated once; never realloc
    size_t covered_bytes = seg_count * kSegmentCoverage;    // kSegmentCoverage = 64KB
    size_t bundle_bits  = (covered_bytes >> kCardBundleShift) + 1;
    size_t need_bytes   = (bundle_bits + 7) >> 3;
    // Round up to a comfortable, fixed floor (covers growth without realloc).
    size_t cap = need_bytes;
    if (cap < 65536) cap = 65536;   // ≥ 64 KB => covers ≥ 1 TB of heap at 2 MB/bit
    g_card_bundle = static_cast<uint8_t*>(std::calloc(1, cap));
    if (g_card_bundle == nullptr) return;
    g_card_bundle_size.store(cap, std::memory_order_release);
}

// ── Tracked segment list for O(allocated) ClearAllCards ───────
struct CardSegmentNode {
    CardSegment* segment;
    uintptr_t    seg_idx;   // L1 index → covered address range = base + seg_idx*64KB
    CardSegmentNode* next;
};

static CardSegmentNode* g_card_segment_list = nullptr;
static std::mutex g_card_segment_list_mutex;

// ── Heap base (set once at startup) ────────────────────────────
uintptr_t g_heap_base = 0;

// ── Nursery range (for DirtyCard fast skip) ──────────────────
uintptr_t g_nursery_range_begin = 0;
uintptr_t g_nursery_range_end = 0;

void GcSetCardTableNurseryRange(uintptr_t begin, uintptr_t end) noexcept {
    // Volatile stores ensure visibility across threads without requiring
    // an atomic RMW.  The DirtyCard fast path reads these with relaxed
    // ordering; a stale read (old range) at worst causes an unnecessary
    // card table access — correctness is preserved.
    g_nursery_range_begin = begin;
    g_nursery_range_end = end;
}

// Out-of-line definition for chaos_gc_dirty_card (ABI export).
extern "C" void chaos_gc_dirty_card(const void* obj) noexcept {
    DirtyCard(obj);
}

// generation-aware write barrier.  Faithfully replicates CoreCLR's
// region write-barrier short-circuits (JitHelpers_FastWriteBarriers.asm
// Region64): only set a card when dst is non-gen0 AND the stored ref is in a
// strictly-younger (older-generation writing younger, or gen0 ref into old)
// region than dst.  Reading both operands' region generation lets gen0->gen0
// and same/mature-generation writes skip the card entirely — the core saving
// of the region-based barrier.  Cross-platform: pure C++ (addr>>shift + bit
// ops), no asm, no OS calls.  The legacy single-arg chaos_gc_dirty_card(dst)
// remains for callers without a ref operand (conservative: non-gen0 always
// mark).  ref==nullptr is a degenerate store and never needs a card.
//
// The region→gen table (GetRegionGen) is keyed by addr>>22 (4MB).  It must be
// correctly populated for every old-gen/LOH page: see GcMarkRangeOld, which
// old-gen/LOH page allocation now calls so their 4MB bytes are OLD (gen 2) and
// a co-located nursery/Gen1 SetRegionGen(.., young) cannot mislabel an old page
// as gen0 (which would skip carding → dropped old→nursery edges).
extern "C" void chaos_gc_dirty_card_dst_ref(const void* dst, const void* ref) noexcept {
    if (dst == nullptr) return;
    uintptr_t dst_addr = reinterpret_cast<uintptr_t>(dst);
    uint8_t dst_gen = GetRegionGen(dst_addr);
    // 1. dst in gen0 (nursery): young-generation contents are scanned wholesale
    //    — no card needed regardless of what is written into them.
    //
    //    R3/CoreCLR-aligned: we skip carding ONLY for true gen0 (nursery) here,
    //    NOT gen1.  CoreCLR's region barrier (JIT_WriteBarrier_Byte_Region64)
    //    skips only dst.gen==0 and still cards old→gen1 stores of younger refs.
    //    Chaos previously skipped gen1 too (dst_gen <= kRegionGenGen1), so a
    //    gen1→nursery store was never carded, yet young-GC Phase-2b scans gen1
    //    dirty cards for exactly those gen1→nursery edges — a self-contradiction
    //    that silently dropped the cross-gen edge.
    if (dst_gen == kRegionGenYoung) return;
    // 2. ref outside managed/NULL: not a managed pointer store — no cross-gen
    //    reference to record.  (GetRegionGen of an unmapped addr returns the
    //    conservative default kRegionGenOld, so an out-of-heap ref is treated
    //    as mature → the ref_gen>=dst_gen test below skips — matching CoreCLR's
    //    out-of-range early-out.)
    if (ref == nullptr) return;
    uint8_t ref_gen = GetRegionGen(reinterpret_cast<uintptr_t>(ref));
    // 3. Faithful CoreCLR condition: ref.gen < dst.gen (an older region wrote a
    //    young-object reference into dst).  Same-mature (ref.gen >= dst.gen)
    //    writes need no card.
    if (ref_gen >= dst_gen) return;
    DirtyCard(dst);
}

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
        EnsureCardBundleCoverage(new_size);
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
        EnsureCardBundleCoverage(new_size);
    }
    // Ensure the bundle covers even the initial L1 size on first registration.
    EnsureCardBundleCoverage(g_card_l1_size.load(std::memory_order_acquire));

    // ── Allocate L2 segments for any null entries ──────────────────
    for (uintptr_t si = first_seg; si <= last_seg; si++) {
        CardSegment* existing = g_card_l1[si].load(std::memory_order_acquire);
        if (existing != nullptr) continue;

        auto* seg = static_cast<CardSegment*>(CHAOS_IL2CPP_MALLOC(sizeof(CardSegment)));
        if (seg == nullptr) {
            CHAOS_IL2CPP_LOG_ERROR_M("CardTable", "OOM allocating segment {0}", si);
            return;
        }
        std::memset(seg->words, 0, sizeof(seg->words));

        if (!g_card_l1[si].compare_exchange_strong(existing, seg,
                std::memory_order_release, std::memory_order_acquire)) {
            CHAOS_IL2CPP_FREE(seg);
        } else {
            std::lock_guard<std::mutex> lock(g_card_segment_list_mutex);
            auto* node = static_cast<CardSegmentNode*>(CHAOS_IL2CPP_MALLOC(sizeof(CardSegmentNode)));
            if (node != nullptr) {
                node->segment = seg;
                node->seg_idx = si;
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
        std::memset(node->segment->words, 0, sizeof(node->segment->words));
        node = node->next;
    }
    // keep the bundle in sync (clear it; cards are now all clean).
    CardBundleClearAll();
}

// Scan every REGISTERED L2 card segment for dirty cards (CoreCLR-aligned).
// The write barrier (DirtyCard) only ever writes into a registered L2 segment
// (it no-ops when seg==nullptr).  Iterating the registered-segment set is a
// SUPERSET of any card the barrier wrote — the authoritative scan source.
// Prior page/segment-chain-driven scans could miss a barrier-written card if
// the page's L2 segment wasn't registered or the page range didn't cover the
// written card index (the cross-gen-edge-dropping root cause).
void ScanDirtyCardsInRegisteredSegments(CHAOS_IL2CPP_SIZE* dirty_card_count,
                                         void (*range_cb)(uintptr_t, uintptr_t, void*),
                                         void* user_data) noexcept {
    if (range_cb == nullptr || g_heap_base == 0) return;
    std::lock_guard<std::mutex> lock(g_card_segment_list_mutex);
    for (auto* node = g_card_segment_list; node != nullptr; node = node->next) {
        uintptr_t seg_start = g_heap_base + (node->seg_idx * kSegmentCoverage);
        uintptr_t seg_end   = seg_start + kSegmentCoverage;
        ScanDirtyCardsBatched(
            seg_start, seg_end, dirty_card_count,
            [range_cb, user_data](uintptr_t rs, uintptr_t re) {
                range_cb(rs, re, user_data);
            });
    }
}

void ClearCardRange(uintptr_t start, uintptr_t end) noexcept {
    if (start >= end || g_heap_base == 0) return;

    uintptr_t first_idx = (start - g_heap_base) >> kCardShift;
    uintptr_t last_idx  = (end - 1 - g_heap_base) >> kCardShift;

    uintptr_t first_seg = first_idx / kCardsPerSegment;
    uintptr_t last_seg  = last_idx / kCardsPerSegment;

    size_t l1_size = g_card_l1_size.load(std::memory_order_acquire);

    for (uintptr_t si = first_seg; si <= last_seg && si < l1_size; si++) {
        auto* seg = g_card_l1[si].load(std::memory_order_acquire);
        if (seg == nullptr) continue;

        // Determine which cards within this segment to clear.
        uintptr_t seg_first_card = (si == first_seg) ? (first_idx % kCardsPerSegment) : 0;
        uintptr_t seg_last_card  = (si == last_seg)  ? (last_idx  % kCardsPerSegment) : (kCardsPerSegment - 1);

        // Clear the overlapping card bits (bit-per-word).
        for (uintptr_t ci = seg_first_card; ci <= seg_last_card; ci++) {
            uint32_t bit_mask = 1u << (ci % kCardsPerWord);
            seg->words[ci / kCardsPerWord] &= ~bit_mask;
        }
    }
}

void GcUnregisterHeapRange(uintptr_t start, uintptr_t end) noexcept {
    if (start >= end) return;
    if (g_heap_base == 0) return;

    uintptr_t first_idx = (start - g_heap_base) >> kCardShift;
    uintptr_t last_idx  = (end - 1 - g_heap_base) >> kCardShift;

    uintptr_t first_seg = first_idx / kCardsPerSegment;
    uintptr_t last_seg  = last_idx / kCardsPerSegment;

    size_t l1_size = g_card_l1_size.load(std::memory_order_acquire);

    // Phase 1: CAS each L1 entry to null and collect segments to free.
    // Using CAS instead of store to safely claim ownership — the thread
    // that successfully CASes the entry from non-null to null owns it.
    std::vector<CardSegment*> to_free;
    for (uintptr_t si = first_seg; si <= last_seg && si < l1_size; si++) {
        CardSegment* expected = g_card_l1[si].load(std::memory_order_acquire);
        if (expected == nullptr) continue;
        if (g_card_l1[si].compare_exchange_strong(expected, nullptr,
                std::memory_order_release, std::memory_order_acquire)) {
            to_free.push_back(expected);
        }
    }

    if (to_free.empty()) return;

    // Phase 2: Remove freed segments from the tracked list and free them.
    {
        std::lock_guard<std::mutex> lock(g_card_segment_list_mutex);
        CardSegmentNode** pp = &g_card_segment_list;
        while (*pp != nullptr) {
            CardSegmentNode* node = *pp;
            bool found = false;
            for (auto* seg : to_free) {
                if (node->segment == seg) {
                    found = true;
                    break;
                }
            }
            if (found) {
                *pp = node->next;
                CHAOS_IL2CPP_FREE(node->segment);
                CHAOS_IL2CPP_FREE(node);
            } else {
                pp = &node->next;
            }
        }
    }
}

}  // namespace chaos::il2cpp::runtime_core