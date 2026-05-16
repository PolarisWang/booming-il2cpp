#include "gc_stress.h"
#include "gc_old_gen.h"
#include "gc_card_table.h"

#include <chaos/log.h>

#include <cstdlib>
#include <cstring>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// GCStress Mode
// ======================================================================

std::atomic<int> g_gc_stress_mode{0};

void GcStressInit() noexcept {
#if defined(CHAOS_IL2CPP_DEBUG) || !defined(NDEBUG)
    const char* env = nullptr;
#if defined(_WIN32) || defined(_WIN64)
    char buf[32];
    auto ret = GetEnvironmentVariableA("CHAOS_GC_STRESS_MODE", buf, static_cast<DWORD>(sizeof(buf)));
    if (ret > 0 && ret < static_cast<DWORD>(sizeof(buf))) {
        env = buf;
    }
#else
    env = std::getenv("CHAOS_GC_STRESS_MODE");
#endif
    if (env != nullptr) {
        int val = static_cast<int>(std::atol(env));
        if (val >= 0 && val <= 3) {
            g_gc_stress_mode.store(val, std::memory_order_release);
            CHAOS_IL2CPP_LOG_INFO_M("GCStress", "mode={0}", val);
        }
    }
#else
    // SHIP builds: stress mode is always 0.
    g_gc_stress_mode.store(0, std::memory_order_release);
#endif
}

void GcStressCheckAlloc() noexcept {
    int mode = g_gc_stress_mode.load(std::memory_order_acquire);
    if (mode != 1 && mode != 3) return;

    // In stress mode 1 or 3, trigger a full GC after every allocation.
    // Use CollectFull instead of the full Collect to avoid reentrancy
    // issues (CollectFull just delegates to Collect(nullptr, nullptr)
    // which is re-entrant safe since it's STW).
    g_old_gen.CollectFull();
}

void GcStressCheckVerify() noexcept {
    int mode = g_gc_stress_mode.load(std::memory_order_acquire);
    if (mode != 2 && mode != 3) return;

    if (!GcVerifyHeap()) {
        CHAOS_IL2CPP_LOG_ERROR_M("GCStress", "heap_verification_failed");
#if defined(_MSC_VER)
        __debugbreak();
#else
        __builtin_trap();
#endif
    }
}

// ======================================================================
// Heap Verification
// ======================================================================

bool GcVerifyPage(const void* page) noexcept {
    if (page == nullptr) return true;

    // Reinterpret as OldGenPage to access metadata, but only pass
    // the pointer around as const — we're strictly reading.
    auto* p = static_cast<const OldGenPage*>(page);

    // Check page header consistency.
    if (p->page_size < sizeof(OldGenPage)) {
        CHAOS_IL2CPP_LOG_ERROR_M("GCStress", "verify_page_bad_size page={0} size={1}",
            static_cast<const void*>(p),
            static_cast<unsigned long long>(p->page_size));
        return false;
    }

    if (p->payload_size == 0 || p->payload_size > p->page_size) {
        CHAOS_IL2CPP_LOG_ERROR_M("GCStress", "verify_page_bad_payload page={0} payload={1}",
            static_cast<const void*>(p),
            static_cast<unsigned long long>(p->payload_size));
        return false;
    }

    // Skip oversized pages (they use the full payload for one object).
    if (p->is_oversized) return true;

    // Verify free lists: all blocks should be within the page payload.
    const char* payload_start = p->Payload();
    const char* payload_end = payload_start + p->payload_size;

    for (int sc = 0; sc < kOldGenNumSizeClasses; sc++) {
        const auto* fb = p->free_lists[sc];
        int block_count = 0;
        while (fb != nullptr) {
            // Check block is within page payload.
            auto* block_addr = reinterpret_cast<const char*>(fb);
            if (block_addr < payload_start || block_addr >= payload_end) {
                CHAOS_IL2CPP_LOG_ERROR_M("GCStress",
                    "verify_page_free_out_of_bounds page={0} sc={1} block={2}",
                    static_cast<const void*>(p), sc,
                    static_cast<const void*>(fb));
                return false;
            }
            // Check alignment.
            if (reinterpret_cast<uintptr_t>(fb) % sizeof(void*) != 0) {
                CHAOS_IL2CPP_LOG_ERROR_M("GCStress",
                    "verify_page_free_misaligned page={0} sc={1} block={2}",
                    static_cast<const void*>(p), sc,
                    static_cast<const void*>(fb));
                return false;
            }
            fb = fb->next;
            block_count++;
            // Sanity: prevent infinite loop from corrupted list.
            if (block_count > 100000) {
                CHAOS_IL2CPP_LOG_ERROR_M("GCStress",
                    "verify_page_free_infinite page={0} sc={1}",
                    static_cast<const void*>(p), sc);
                return false;
            }
        }
    }

    return true;
}

bool GcVerifyCardTable() noexcept {
    if (g_heap_base == 0) return true;  // No heap allocated yet — nothing to check.

    // Verify that all old-gen pages have valid card table entries.
    // This is a light check: ensures every old-gen page's payload range
    // is covered by the card table's L1/L2 mapping.
    auto* page_list = g_old_gen.PageList();
    while (page_list != nullptr) {
        if (!page_list->in_use.load(std::memory_order_acquire)) {
            page_list = page_list->next;
            continue;
        }

        // Check that the card table can map this page.
        uintptr_t page_start = reinterpret_cast<uintptr_t>(page_list->Payload());
        uintptr_t page_end = page_start + page_list->payload_size;

        // Verify that the card table can map this page's addresses.
        // Check that L1 entries for this range are non-null (registered).
        for (uintptr_t addr = page_start; addr < page_end; addr += kCardSize) {
            uintptr_t idx = (addr - g_heap_base) >> kCardShift;
            uintptr_t seg_idx = idx / kCardsPerSegment;
            if (seg_idx >= static_cast<uintptr_t>(kCardL1Entries)) {
                CHAOS_IL2CPP_LOG_ERROR_M("GCStress",
                    "verify_card_l1_oob addr=0x{0} seg_idx={1}",
                    static_cast<unsigned long long>(addr),
                    static_cast<unsigned long long>(seg_idx));
                return false;
            }
            auto* seg = g_card_l1[seg_idx].load(std::memory_order_acquire);
            if (seg == nullptr) {
                CHAOS_IL2CPP_LOG_ERROR_M("GCStress",
                    "verify_card_l1_null addr=0x{0} seg_idx={1}",
                    static_cast<unsigned long long>(addr),
                    static_cast<unsigned long long>(seg_idx));
                return false;
            }
        }

        page_list = page_list->next;
    }

    return true;
}

bool GcVerifyHeap() noexcept {
    CHAOS_IL2CPP_LOG_DEBUG_M("GCStress", "verify_heap_start");

    // Phase 1: Verify each page in the old-gen page list.
    {
        auto* p = g_old_gen.PageList();
        int page_idx = 0;
        while (p != nullptr) {
            if (p->in_use.load(std::memory_order_acquire)) {
                if (!GcVerifyPage(p)) {
                    CHAOS_IL2CPP_LOG_ERROR_M("GCStress",
                        "verify_page_failed idx={0}", page_idx);
                    return false;
                }
            }
            p = p->next;
            page_idx++;
        }
    }

    // Phase 2: Verify card table coverage.
    if (!GcVerifyCardTable()) {
        return false;
    }

    // Phase 3: Verify mark bitmap is clean (no stray mark bits after sweep).
    // This is valid only post-sweep (after SweepPage clears the bitmap).
    {
        auto* p = g_old_gen.PageList();
        while (p != nullptr) {
            if (p->in_use.load(std::memory_order_acquire) && !p->is_oversized) {
                auto* bitmap = p->MarkBitmap();
                for (CHAOS_IL2CPP_SIZE b = 0; b < p->bitmap_bytes; b++) {
                    if (bitmap[b] != 0) {
                        CHAOS_IL2CPP_LOG_ERROR_M("GCStress",
                            "verify_stale_mark_bit page={0} byte={1}",
                            static_cast<const void*>(p),
                            static_cast<unsigned long long>(b));
                        return false;
                    }
                }
            }
            p = p->next;
        }
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("GCStress", "verify_heap_ok");
    return true;
}

}  // namespace chaos::il2cpp::runtime_core
