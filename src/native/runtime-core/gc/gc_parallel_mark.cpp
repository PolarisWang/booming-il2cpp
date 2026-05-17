#include "gc_parallel_mark.h"

#include "gc_bit_utils.h"
#include "gc_layout.h"
#include "thread_pool.h"
#include "thread_state.h"

#include <chaos/log.h>
#include <chaos/profile.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <thread>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// Atomic mark-bit helpers
// ======================================================================

/// Atomically set a bit in the mark bitmap.
/// Returns true if the bit was newly set (was 0, now 1).
/// Returns false if the bit was already set.
static inline bool AtomicMarkBit(unsigned char* bitmap, CHAOS_IL2CPP_SIZE byte_idx,
                                  unsigned char mask) noexcept {
#if defined(_MSC_VER) && !defined(__clang__)
    auto prev = _InterlockedOr8(reinterpret_cast<volatile char*>(&bitmap[byte_idx]),
                                 static_cast<char>(mask));
    return (static_cast<unsigned char>(prev) & mask) == 0;
#else
    auto prev = __atomic_fetch_or(&bitmap[byte_idx], mask, __ATOMIC_RELAXED);
    return (prev & mask) == 0;
#endif
}

// ======================================================================
// Context lifecycle
// ======================================================================

ParallelMarkContext* InitParallelMarkContext(OldGenPage** pages, int page_count,
                                              int hw_concurrency) {
    // Worker count: min(pages/32 + 1, hw_concurrency, kMaxParallelMarkWorkers)
    int desired = (page_count / 32) + 1;
    desired = (std::min)(desired, hw_concurrency);
    desired = (std::min)(desired, kMaxParallelMarkWorkers);
    if (desired < 1) desired = 1;

    auto* ctx = static_cast<ParallelMarkContext*>(
        CHAOS_IL2CPP_MALLOC(sizeof(ParallelMarkContext)));
    if (ctx == nullptr) return nullptr;

    ctx->pages = pages;
    ctx->page_count = page_count;
    ctx->worker_count = desired;
    ctx->active_workers.store(0, std::memory_order_relaxed);
    ctx->total_marked.store(0, std::memory_order_relaxed);
    ctx->drain_started.store(false, std::memory_order_relaxed);
    ctx->parallel_done.store(false, std::memory_order_relaxed);

    // Build page_starts array for O(log n) page lookup.
    ctx->page_starts = static_cast<uintptr_t*>(
        CHAOS_IL2CPP_MALLOC(static_cast<size_t>(page_count) * sizeof(uintptr_t)));
    if (ctx->page_starts == nullptr) {
        CHAOS_IL2CPP_FREE(ctx);
        return nullptr;
    }
    for (int i = 0; i < page_count; i++) {
        if (pages[i] != nullptr) {
            ctx->page_starts[i] = reinterpret_cast<uintptr_t>(pages[i]->Payload());
        } else {
            ctx->page_starts[i] = 0;
        }
    }

    ctx->workers = static_cast<MarkWorkerState*>(
        CHAOS_IL2CPP_MALLOC(static_cast<size_t>(desired) * sizeof(MarkWorkerState)));
    if (ctx->workers == nullptr) {
        CHAOS_IL2CPP_FREE(ctx->page_starts);
        CHAOS_IL2CPP_FREE(ctx);
        return nullptr;
    }

    for (int i = 0; i < desired; i++) {
        new (&ctx->workers[i]) MarkWorkerState();
        ctx->workers[i].worker_idx = i;
    }

    return ctx;
}

int FindPageIndexByAddr(ParallelMarkContext* ctx, uintptr_t addr) {
    if (ctx == nullptr || ctx->page_starts == nullptr || ctx->page_count <= 0) {
        return -1;
    }

    // Binary search on payload start addresses.
    int lo = 0;
    int hi = ctx->page_count - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        uintptr_t start = ctx->page_starts[mid];
        uintptr_t end = start + ctx->pages[mid]->payload_size;
        if (addr < start) {
            hi = mid - 1;
        } else if (addr >= end) {
            lo = mid + 1;
        } else {
            return mid;
        }
    }
    return -1;
}

void DestroyParallelMarkContext(ParallelMarkContext* ctx) {
    if (ctx == nullptr) return;
    if (ctx->workers) {
        for (int i = 0; i < ctx->worker_count; i++) {
            ctx->workers[i].deque.~vector();
        }
        CHAOS_IL2CPP_FREE(ctx->workers);
    }
    if (ctx->page_starts) {
        CHAOS_IL2CPP_FREE(ctx->page_starts);
    }
    CHAOS_IL2CPP_FREE(ctx);
}

// ======================================================================
// Deque operations
// ======================================================================

void PushChunk(MarkWorkerState* worker, const MarkChunk& chunk) {
    // Local push: must guard against concurrent steal.
    std::lock_guard<std::mutex> lock(worker->steal_mutex);
    worker->deque.push_back(chunk);
}

bool PopChunk(MarkWorkerState* worker, MarkChunk* out) {
    // Local pop: must guard against concurrent steal.
    std::lock_guard<std::mutex> lock(worker->steal_mutex);
    if (worker->deque.empty()) return false;
    *out = worker->deque.back();
    worker->deque.pop_back();
    return true;
}

bool StealChunk(ParallelMarkContext* ctx, int thief_idx, MarkChunk* out) {
    // Try up to 3 random victims using a thread-local XorShift32 PRNG.
    // rand() is not thread-safe (global lock on glibc/musl).
    thread_local uint32_t prng_state = static_cast<uint32_t>(
        reinterpret_cast<uintptr_t>(&thief_idx) ^ 0xDEADBEEFu);
    auto prng_next = [&]() -> uint32_t {
        uint32_t x = prng_state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        prng_state = x;
        return x;
    };

    int n = ctx->worker_count;
    for (int attempt = 0; attempt < 3; attempt++) {
        int victim = static_cast<int>(prng_next() % n);
        if (victim == thief_idx) continue;

        auto& vw = ctx->workers[victim];
        std::lock_guard<std::mutex> lock(vw.steal_mutex);
        if (!vw.deque.empty()) {
            // Steal from the front (LIFO for the thief = oldest work).
            *out = vw.deque.front();
            vw.deque.erase(vw.deque.begin());
            return true;
        }
    }
    return false;
}

void FlushPending(MarkWorkerState* worker) {
    if (!worker->has_pending) return;
    if (worker->pending.bitmap != 0) {
        PushChunk(worker, worker->pending);
    }
    worker->has_pending = false;
    worker->pending.bitmap = 0;
    worker->pending.page_idx = 0;
    worker->pending.word_index = 0;
}

// ======================================================================
// Chunk processing
// ======================================================================

void ProcessChunk(ParallelMarkContext* ctx, MarkWorkerState* worker,
                  const MarkChunk& chunk) {
    CHAOS_IL2CPP_PROFILE_SCOPE("ParallelMark::ProcessChunk");

    if (chunk.page_idx < 0 || chunk.page_idx >= ctx->page_count) return;
    auto* page = ctx->pages[chunk.page_idx];
    if (page == nullptr) return;

    auto& layout_registry = GcLayoutRegistry::Instance();
    char* payload = page->Payload();
    auto* bitmap = page->MarkBitmap();
    uint64_t word = chunk.bitmap;

    // Iterate each set bit in the chunk bitmap.
    while (word != 0) {
        int bit = GcCtz64(word);
        word &= word - 1;  // clear lowest set bit

        CHAOS_IL2CPP_SIZE slot_idx = static_cast<CHAOS_IL2CPP_SIZE>(
            static_cast<CHAOS_IL2CPP_SIZE>(chunk.word_index) * 64 + bit);
        if (slot_idx * sizeof(void*) >= page->payload_size) {
            continue;
        }
        void* obj = payload + slot_idx * sizeof(void*);

        // Read TypeInfo* from first word.
        const void* type_info_ptr = *static_cast<const void* const*>(obj);
        if (type_info_ptr == nullptr) continue;

        // Determine object size and pointer fields.
        CHAOS_IL2CPP_SIZE obj_size = sizeof(void*);
        const GcTypeLayout* layout = nullptr;

        if (layout_registry.IsValidTypeInfoPointer(type_info_ptr)) {
            auto* hot = static_cast<const TypeInfoHot*>(type_info_ptr);
            uint64_t stable_id = hot->stable_id;
            layout = layout_registry.Lookup(stable_id);
            if (layout != nullptr) {
                obj_size = layout->instance_size;
            }
        }

        if (layout == nullptr || layout->pointer_count == 0) {
            // Pointer-free or unknown — skip child scanning.
            continue;
        }

        // Precise scan: iterate declared pointer offsets.
        uintptr_t obj_base = reinterpret_cast<uintptr_t>(obj);
        for (uint16_t i = 0; i < layout->pointer_count; i++) {
            uint16_t offset = layout->pointer_offsets[i].offset;
            auto* slot = reinterpret_cast<void**>(obj_base + offset);
            void* ref = *slot;
            if (ref == nullptr) continue;

            // Find the page containing ref via binary search.
            int ref_page_idx = FindPageIndexByAddr(ctx, reinterpret_cast<uintptr_t>(ref));
            if (ref_page_idx < 0) continue;

            auto* rp = ctx->pages[ref_page_idx];
            uintptr_t rp_payload = reinterpret_cast<uintptr_t>(rp->Payload());
            CHAOS_IL2CPP_SIZE ref_offset = reinterpret_cast<uintptr_t>(ref) - rp_payload;
            CHAOS_IL2CPP_SIZE ref_slot = ref_offset / sizeof(void*);
            CHAOS_IL2CPP_SIZE ref_byte = ref_slot / 8;
            int ref_bit = static_cast<int>(ref_slot % 8);
            unsigned char ref_mask = static_cast<unsigned char>(1u << ref_bit);

            // Atomic mark.
            auto* ref_bitmap = rp->MarkBitmap();
            if (!AtomicMarkBit(ref_bitmap, ref_byte, ref_mask)) {
                continue;  // already marked
            }
            ctx->total_marked.fetch_add(1, std::memory_order_relaxed);

            // Accumulate into pending chunk.
            uint64_t ref_bitword = static_cast<uint64_t>(1) << ref_bit;
            CHAOS_IL2CPP_SIZE ref_word = ref_slot / 64;
            if (worker->has_pending &&
                worker->pending.page_idx == ref_page_idx &&
                worker->pending.word_index == ref_word) {
                // Same page + same 64-slot group — OR into existing bitmap word.
                worker->pending.bitmap |= ref_bitword;
            } else {
                // Different page or word group — flush pending, start new chunk.
                FlushPending(worker);
                worker->pending.page_idx = ref_page_idx;
                worker->pending.word_index = static_cast<uint16_t>(ref_word);
                worker->pending.bitmap = ref_bitword;
                worker->has_pending = true;
            }
        }
    }
}

// ======================================================================
// Worker entry point
// ======================================================================

void ParallelMarkWorkerLoop(ParallelMarkContext* ctx, int worker_idx) {
    CHAOS_IL2CPP_PROFILE_SCOPE("ParallelMark::WorkerLoop");

    // GcWorkerPool always wakes ALL pool threads, even when the caller
    // requested fewer participants.  Workers with idx >= ctx->worker_count
    // must return immediately — their state slot doesn't exist.
    if (worker_idx >= ctx->worker_count) {
        return;
    }

    auto* worker = &ctx->workers[worker_idx];

    // Register with the thread system for safepoint compatibility.
    // Thread-pool workers are already registered — skip if idx > 0.
    // Worker 0 is the GC thread (already registered).

    ctx->active_workers.fetch_add(1, std::memory_order_relaxed);

    // Wait for drain_started signal from the GC thread.
    while (!ctx->drain_started.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }

    // ====================================================================
    // Termination protocol:
    //
    //   active_workers counts how many workers are currently in the
    //   "processing work" phase (outer loop).  Each worker increments
    //   on entry and decrements when it runs out of work.  The LAST
    //   worker to decrement (prev <= 1) sets parallel_done = true.
    //
    //   Workers that are NOT the last enter an inner wait loop: they
    //   try pop/steal again periodically, and if they find work they
    //   re-increment active_workers and return to the outer loop.
    //
    //   This ensures EVERY worker eventually decrements (even those
    //   that never found work), so active_workers can reach 0.
    // ====================================================================

    bool work_found = false;
    while (!ctx->parallel_done.load(std::memory_order_acquire)) {
        // 1. Flush any pending accumulator.
        FlushPending(worker);

        // 2. Try local pop.
        MarkChunk chunk;
        if (PopChunk(worker, &chunk)) {
            ProcessChunk(ctx, worker, chunk);
            work_found = true;
            continue;
        }

        // 3. Try steal.
        if (StealChunk(ctx, worker_idx, &chunk)) {
            ProcessChunk(ctx, worker, chunk);
            work_found = true;
            continue;
        }

        // 4. No work found — decrement active count.
        work_found = false;
        int prev = ctx->active_workers.fetch_sub(1, std::memory_order_acq_rel);
        if (prev <= 1) {
            // We are the LAST worker to go idle. Signal done.
            ctx->parallel_done.store(true, std::memory_order_release);
            break;
        }

        // 5. Not the last — wait for work or done signal.
        while (!ctx->parallel_done.load(std::memory_order_acquire)) {
            // Try to find new work periodically.
            MarkChunk new_chunk;
            if (PopChunk(worker, &new_chunk)) {
                ctx->active_workers.fetch_add(1, std::memory_order_relaxed);
                ProcessChunk(ctx, worker, new_chunk);
                work_found = true;
                break;
            }
            if (StealChunk(ctx, worker_idx, &new_chunk)) {
                ctx->active_workers.fetch_add(1, std::memory_order_relaxed);
                ProcessChunk(ctx, worker, new_chunk);
                work_found = true;
                break;
            }
            std::this_thread::yield();
        }
    }
}

}  // namespace chaos::il2cpp::runtime_core