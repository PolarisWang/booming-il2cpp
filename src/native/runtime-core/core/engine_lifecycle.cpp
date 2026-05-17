namespace chaos::il2cpp::runtime_core {

// MSVC 14.44 cannot see anonymous-namespace symbols from other unity files.
// Close the anonymous namespace early — all shared symbols below are at
// runtime_core namespace scope for visibility across the unity build TU.

// ── ABI capability descriptors and engine-global state ──
// (runtime_core namespace scope for visibility across unity build TU)

using namespace chaos::il2cpp::runtime_capability;
using namespace chaos::il2cpp::marshal_abi;

static constexpr CHAOS_IL2CPP_UINT32 kMarshalPlatformAbiVersion = 1u;
static constexpr CHAOS_IL2CPP_UINT32 kTaskRuntimeKernelAbiVersion = 1u;

const MarshalMemoryBlockAbiV1 kMarshalMemoryBlockAbiV1 = {
    sizeof(MarshalMemoryBlockAbiV1),
};

const MarshalStringMarshalingAbiV1 kMarshalStringMarshalingAbiV1 = {
    sizeof(MarshalStringMarshalingAbiV1),
};

const MarshalStructureLayoutAbiV1 kMarshalStructureLayoutAbiV1 = {
    sizeof(MarshalStructureLayoutAbiV1),
};

const MarshalDelegateBridgeAbiV1 kMarshalDelegateBridgeAbiV1 = {
    sizeof(MarshalDelegateBridgeAbiV1),
};

const ComInteropAbiV1 kComInteropAbiV1 = {
    sizeof(ComInteropAbiV1),
    MarshalPlatformCapabilityComCore | MarshalPlatformCapabilityComActivation,
};

const VariantInteropAbiV1 kVariantInteropAbiV1 = {
    sizeof(VariantInteropAbiV1),
    MarshalPlatformCapabilityVariantScalar |
    MarshalPlatformCapabilityVariantBulk |
    MarshalPlatformCapabilityVariantExtended,
};

const MarshalPlatformAbiRootV1 kMarshalPlatformAbiRootV1 = {
    kMarshalPlatformAbiVersion,
    sizeof(MarshalPlatformAbiRootV1),
    MarshalPlatformCapabilityMemoryBlock |
    MarshalPlatformCapabilityStringMarshaling |
    MarshalPlatformCapabilityStructureLayout |
    MarshalPlatformCapabilityDelegateBridge |
    MarshalPlatformCapabilityComCore |
    MarshalPlatformCapabilityComActivation |
    MarshalPlatformCapabilityVariantScalar |
    MarshalPlatformCapabilityVariantBulk |
    MarshalPlatformCapabilityVariantExtended,
    &kMarshalMemoryBlockAbiV1,
    &kMarshalStringMarshalingAbiV1,
    &kMarshalStructureLayoutAbiV1,
    &kMarshalDelegateBridgeAbiV1,
    &kComInteropAbiV1,
    &kVariantInteropAbiV1,
};

const TaskRuntimeKernelV1 kTaskRuntimeKernelV1 = {
    kTaskRuntimeKernelAbiVersion,
    sizeof(TaskRuntimeKernelV1),
    TaskRuntimeKernelCapabilityStateCore |
    TaskRuntimeKernelCapabilityContinuation |
    TaskRuntimeKernelCapabilityFactory |
    TaskRuntimeKernelCapabilityIntrospection,
};

const char* kEngineObservePrefix = "CHAOS_ENGINE_OBSERVE ";

CHAOS_IL2CPP_MUTEX g_engine_binding_mutex;
CHAOS_IL2CPP_UINTPTR g_next_engine_handle = 1u;
CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINTPTR, void*) g_engine_handles = {};
CHAOS_IL2CPP_VECTOR(EngineLifecycleRegistration) g_engine_lifecycle_registrations = {};
const CHAOS_IL2CPP_THREAD::id g_main_thread_id = CHAOS_IL2CPP_THIS_THREAD_GET_ID();
CHAOS_IL2CPP_ATOMIC(RuntimeMode) g_runtime_mode = RuntimeMode::Aot;
CHAOS_IL2CPP_ATOMIC(CHAOS_IL2CPP_INT32) g_next_task_id{1};

// GC handle table: maps handle IDs to object instances.
// GcHandleEntry struct is defined in engine_lifecycle.h.
CHAOS_IL2CPP_MUTEX s_gc_handle_mutex;
CHAOS_IL2CPP_ATOMIC(CHAOS_IL2CPP_UINT64) s_next_gc_handle{1};
CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINT64, GcHandleEntry) s_gc_handle_table;

// Dependent handle table: (primary, secondary) pairs for ConditionalWeakTable.
// Semantics: if primary is alive during GC, secondary is kept alive.
// CoreCLR-aligned fixed-point iteration during mark phase.
struct DependentHandleNode {
    void* primary;      // key: if alive → secondary is kept alive
    void* secondary;    // value: kept alive if primary is alive
};
static CHAOS_IL2CPP_MUTEX s_dep_handle_mutex;
static CHAOS_IL2CPP_ATOMIC(CHAOS_IL2CPP_UINT64) s_next_dep_handle{1};
static CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINT64, DependentHandleNode) s_dep_handle_table;

// Pinned object set: objects that must not be moved by young GC.
// Mutex-protected for concurrent GcHandleNew/GcHandleFree.
// GcIsPinnedObject is called under STW safepoint during scavenge (single-threaded
// in young GC) so the mutex is not strictly required for correctness, but we
// take it anyway for safety.
static CHAOS_IL2CPP_MUTEX s_pin_set_mutex;
static CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(void*, bool) s_pin_set;

// These functions are defined here for external linkage (called from
// gc_old_gen, gc_young_collector that link against chaos_runtime_core.lib).

void GcIterateHandleTable(void (*callback)(void* object, void* user_data),
                           void* user_data) noexcept {
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    for (auto& kv : s_gc_handle_table) {
        if (kv.second.object_instance != nullptr) {
            callback(kv.second.object_instance, user_data);
        }
    }
}

// ── Gen-aware handle iteration ─────────────────────────────────────

/// Iterate only handles pointing to tenured (old-gen / LOH) objects.
/// Skips nursery pointers — callers doing full GC mark can use this to
/// avoid scanning objects that will be handled by the next young GC.
void GcIterateTenuredHandles(void (*callback)(void* object, void* user_data),
                              void* user_data) noexcept {
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    for (auto& kv : s_gc_handle_table) {
        void* obj = kv.second.object_instance;
        if (obj == nullptr) continue;
        if (!RegionManager::Instance().IsNurseryPointer(obj)) {
            callback(obj, user_data);
        }
    }
}

/// Iterate only handles pointing to nursery objects.
void GcIterateNurseryHandles(void (*callback)(void* object, void* user_data),
                              void* user_data) noexcept {
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    for (auto& kv : s_gc_handle_table) {
        void* obj = kv.second.object_instance;
        if (obj == nullptr) continue;
        if (RegionManager::Instance().IsNurseryPointer(obj)) {
            callback(obj, user_data);
        }
    }
}

// ── Type-aware handle iteration ────────────────────────────────────

/// Iterate only strong (non-weak, non-pinned) handles.
void GcIterateStrongHandles(void (*callback)(void* object, void* user_data),
                             void* user_data) noexcept {
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    for (auto& kv : s_gc_handle_table) {
        if (!kv.second.weak && !kv.second.pinned &&
            kv.second.object_instance != nullptr) {
            callback(kv.second.object_instance, user_data);
        }
    }
}

/// Iterate only weak handles.
void GcIterateWeakHandles(void (*callback)(void* object, void* user_data),
                           void* user_data) noexcept {
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    for (auto& kv : s_gc_handle_table) {
        if (kv.second.weak && kv.second.object_instance != nullptr) {
            callback(kv.second.object_instance, user_data);
        }
    }
}

/// Iterate only pinned handles.
void GcIteratePinnedHandles(void (*callback)(void* object, void* user_data),
                             void* user_data) noexcept {
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    for (auto& kv : s_gc_handle_table) {
        if (kv.second.pinned && kv.second.object_instance != nullptr) {
            callback(kv.second.object_instance, user_data);
        }
    }
}

void GcProcessWeakHandlesAfterYoungGC() noexcept {
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    for (auto& kv : s_gc_handle_table) {
        if (!kv.second.weak) continue;
        void* obj = kv.second.object_instance;
        if (obj == nullptr) continue;

        // Only process objects that were in the nursery.
        if (!RegionManager::Instance().IsNurseryPointer(obj)) continue;

        // If the nursery object was forwarded (promoted), update the handle
        // to point to the tenured copy.  Otherwise, null the handle (the
        // object was not reachable and has been collected).
        //
        // Forwarding pointer check: the first word has the lowest bit set.
        auto first_word = *static_cast<const uintptr_t*>(obj);
        if ((first_word & 1u) != 0) {
            void* tenured = reinterpret_cast<void*>(first_word & ~1ull);
            kv.second.object_instance = tenured;
        } else {
            kv.second.object_instance = nullptr;
        }
    }
}

// ======================================================================
// Internal handle API (no RuntimeState required)
// ======================================================================

CHAOS_IL2CPP_UINT64 GcCreateStrongHandle(void* object_instance) noexcept {
    if (object_instance == nullptr) return 0;
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    CHAOS_IL2CPP_UINT64 handle = s_next_gc_handle.fetch_add(1, std::memory_order_relaxed);
    s_gc_handle_table[handle] = GcHandleEntry{ object_instance, false, false };
    return handle;
}

CHAOS_IL2CPP_UINT64 GcCreateWeakHandle(void* object_instance) noexcept {
    if (object_instance == nullptr) return 0;
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    CHAOS_IL2CPP_UINT64 handle = s_next_gc_handle.fetch_add(1, std::memory_order_relaxed);
    s_gc_handle_table[handle] = GcHandleEntry{ object_instance, false, true };
    return handle;
}

CHAOS_IL2CPP_UINT64 GcCreatePinnedHandle(void* object_instance) noexcept {
    if (object_instance == nullptr) return 0;
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    CHAOS_IL2CPP_UINT64 handle = s_next_gc_handle.fetch_add(1, std::memory_order_relaxed);
    s_gc_handle_table[handle] = GcHandleEntry{ object_instance, true, false };
    GcAddPinnedObject(object_instance);
    return handle;
}

void GcFreeHandle(CHAOS_IL2CPP_UINT64 handle_id) noexcept {
    if (handle_id == 0) return;
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    auto it = s_gc_handle_table.find(handle_id);
    if (it != s_gc_handle_table.end()) {
        if (it->second.pinned) {
            GcRemovePinnedObject(it->second.object_instance);
        }
        s_gc_handle_table.erase(it);
    }
}

// ======================================================================
// DependentHandle API — for ConditionalWeakTable / Ephemeron semantics
// ======================================================================

/// Allocate a new dependent handle (primary → secondary).
/// Returns a nonzero handle ID on success, 0 on OOM.
CHAOS_IL2CPP_UINT64 GcCreateDependentHandle(void* primary, void* secondary) noexcept {
    std::lock_guard<std::mutex> lock(s_dep_handle_mutex);
    CHAOS_IL2CPP_UINT64 id = s_next_dep_handle.fetch_add(1, std::memory_order_relaxed);
    DependentHandleNode node;
    node.primary = primary;
    node.secondary = secondary;
    s_dep_handle_table[id] = node;
    return id;
}

/// Get the primary object of a dependent handle.
/// Returns nullptr if the handle ID is invalid.
void* GcGetDependentHandlePrimary(CHAOS_IL2CPP_UINT64 handle_id) noexcept {
    std::lock_guard<std::mutex> lock(s_dep_handle_mutex);
    auto it = s_dep_handle_table.find(handle_id);
    if (it == s_dep_handle_table.end()) return nullptr;
    return it->second.primary;
}

/// Get the secondary object of a dependent handle.
/// Returns nullptr if the handle ID is invalid or the secondary was collected.
void* GcGetDependentHandleSecondary(CHAOS_IL2CPP_UINT64 handle_id) noexcept {
    std::lock_guard<std::mutex> lock(s_dep_handle_mutex);
    auto it = s_dep_handle_table.find(handle_id);
    if (it == s_dep_handle_table.end()) return nullptr;
    return it->second.secondary;
}

/// Set the secondary object of a dependent handle.
void GcSetDependentHandleSecondary(CHAOS_IL2CPP_UINT64 handle_id, void* secondary) noexcept {
    std::lock_guard<std::mutex> lock(s_dep_handle_mutex);
    auto it = s_dep_handle_table.find(handle_id);
    if (it != s_dep_handle_table.end()) {
        it->second.secondary = secondary;
    }
}

/// Free a dependent handle and clear its references.
void GcFreeDependentHandle(CHAOS_IL2CPP_UINT64 handle_id) noexcept {
    std::lock_guard<std::mutex> lock(s_dep_handle_mutex);
    s_dep_handle_table.erase(handle_id);
}

/// Process dependent handles after a young GC.
/// For each (primary, secondary) pair:
///   - If primary was forwarded (promoted), update primary and also promote
///     secondary by recording it in the forwarding worklist.
///   - If primary was NOT promoted (collected), clear the pair (both null).
void GcProcessDependentHandlesAfterYoungGC() noexcept {
    std::lock_guard<std::mutex> lock(s_dep_handle_mutex);
    for (auto& kv : s_dep_handle_table) {
        void* primary = kv.second.primary;
        if (primary == nullptr) continue;

        // Only process nursery objects.
        if (!RegionManager::Instance().IsNurseryPointer(primary)) continue;

        // Check forwarding pointer.
        auto first_word = *static_cast<const uintptr_t*>(primary);
        if ((first_word & 1u) != 0) {
            // Primary was promoted — update to tenured copy.
            void* tenured_primary = reinterpret_cast<void*>(first_word & ~1ull);
            kv.second.primary = tenured_primary;

            // Secondary must also be promoted if it's in the nursery.
            void* secondary = kv.second.secondary;
            if (secondary != nullptr &&
                RegionManager::Instance().IsNurseryPointer(secondary)) {
                // Promote secondary: scavenge it to old gen.
                GcScavengeObjectKnownNursery(secondary, nullptr);
            }
        } else {
            // Primary was not promoted — clear both.
            kv.second.primary = nullptr;
            kv.second.secondary = nullptr;
        }
    }
}

/// Process dependent handles after a full GC (fixed-point iteration).
/// For each (primary, secondary) pair:
///   - If primary is marked alive in old-gen, secondary is also kept alive.
///   - Fixed-point: repeat up to 3 rounds until the reachable set stabilizes.
/// Returns the number of secondary objects kept alive by this pass.
int GcProcessDependentHandlesAfterFullGC() noexcept {
    std::lock_guard<std::mutex> lock(s_dep_handle_mutex);
    int total_kept = 0;

    // Fixed-point iteration: up to 3 rounds (CoreCLR-aligned).
    constexpr int kMaxFixedPointRounds = 3;
    for (int round = 0; round < kMaxFixedPointRounds; round++) {
        int kept_this_round = 0;

        for (auto& kv : s_dep_handle_table) {
            void* primary = kv.second.primary;
            void* secondary = kv.second.secondary;

            if (primary == nullptr || secondary == nullptr) continue;

            // Check if primary is in old-gen and marked alive.
            bool primary_alive = g_old_gen.IsInOldGen(primary) &&
                                 g_old_gen.IsMarked(primary);

            // Also check LOH.
            if (!primary_alive && g_loh.IsInLOH(primary)) {
                primary_alive = g_loh.IsMarked(primary);
            }

            if (primary_alive) {
                // Mark secondary as a GC root to keep it alive.
                if (g_old_gen.IsInOldGen(secondary)) {
                    if (g_old_gen.MarkObject(secondary)) {
                        g_old_gen.AddToMarkStack(secondary);
                        kept_this_round++;
                    }
                } else if (g_loh.IsInLOH(secondary)) {
                    if (g_loh.MarkObject(secondary)) {
                        kept_this_round++;
                    }
                }
            }
        }

        total_kept += kept_this_round;
        if (kept_this_round == 0) break;  // Stabilized — no more iterations needed.
    }

    return total_kept;
}

// ── BGC weak handle processing ───────────────────────────────────
// These functions are called from BgcThreadMain.  The mark bitmap is
// only valid DURING BgcSweep (before StwCompact clears it), so we
// collect dead weak-handle entries at that point and null them later,
// after finalization (for WeakTrackResurrection semantics).

void GcCollectDeadWeakHandles(
    std::vector<std::pair<uint64_t, void*>>& out_dead) noexcept {
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    for (auto& kv : s_gc_handle_table) {
        if (!kv.second.weak) continue;
        void* obj = kv.second.object_instance;
        if (obj == nullptr) continue;

        if (!g_old_gen.IsInOldGen(obj) && !g_loh.IsInLOH(obj)) continue;

        bool is_marked = false;
        if (g_old_gen.IsInOldGen(obj)) {
            is_marked = g_old_gen.IsMarked(obj);
        } else if (g_loh.IsInLOH(obj)) {
            is_marked = g_loh.IsMarked(obj);
        }

        if (!is_marked) {
            out_dead.emplace_back(kv.first, obj);
        }
    }
}

void GcProcessCollectedWeakHandles(
    const std::vector<std::pair<uint64_t, void*>>& dead_handles) noexcept {
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    for (auto& entry : dead_handles) {
        auto it = s_gc_handle_table.find(entry.first);
        if (it == s_gc_handle_table.end()) continue;

        if (it->second.object_instance == entry.second) {
            CHAOS_IL2CPP_LOG_DEBUG_M("BGC", "null_weak_handle id={0} obj={1}",
                static_cast<unsigned long long>(entry.first), entry.second);
            it->second.object_instance = nullptr;
        }
    }
}

int GcProcessDependentHandlesAfterBgc() noexcept {
    std::lock_guard<std::mutex> lock(s_dep_handle_mutex);
    int total_kept = 0;

    constexpr int kMaxFixedPointRounds = 3;
    for (int round = 0; round < kMaxFixedPointRounds; round++) {
        int kept_this_round = 0;

        for (auto& kv : s_dep_handle_table) {
            void* primary = kv.second.primary;
            void* secondary = kv.second.secondary;
            if (primary == nullptr || secondary == nullptr) continue;

            bool primary_alive = false;
            if (g_old_gen.IsInOldGen(primary) && g_old_gen.IsMarked(primary)) {
                primary_alive = true;
            } else if (g_loh.IsInLOH(primary) && g_loh.IsMarked(primary)) {
                primary_alive = true;
            }

            if (primary_alive) {
                if (g_old_gen.IsInOldGen(secondary)) {
                    if (g_old_gen.MarkObject(secondary)) {
                        g_old_gen.AddToMarkStack(secondary);
                        kept_this_round++;
                    }
                } else if (g_loh.IsInLOH(secondary)) {
                    if (g_loh.MarkObject(secondary)) {
                        kept_this_round++;
                    }
                }
            }
        }

        total_kept += kept_this_round;
        if (kept_this_round == 0) break;
    }

    return total_kept;
}

void GcRelocateHandles(
    const std::vector<std::pair<void*, void*>>& relocations) noexcept {
    std::lock_guard<std::mutex> lock(s_gc_handle_mutex);
    for (auto& kv : s_gc_handle_table) {
        void* obj = kv.second.object_instance;
        if (obj == nullptr) continue;
        for (auto& r : relocations) {
            if (r.first == obj) {
                kv.second.object_instance = r.second;
                break;
            }
        }
    }
}

// ── Pinned object set ─────────────────────────────────────────────

void GcAddPinnedObject(void* obj) noexcept {
    if (obj == nullptr) return;
    std::lock_guard<std::mutex> lock(s_pin_set_mutex);
    s_pin_set[obj] = true;
}

void GcRemovePinnedObject(void* obj) noexcept {
    if (obj == nullptr) return;
    std::lock_guard<std::mutex> lock(s_pin_set_mutex);
    s_pin_set.erase(obj);
}

bool GcIsPinnedObject(void* obj) noexcept {
    if (obj == nullptr) return false;
    std::lock_guard<std::mutex> lock(s_pin_set_mutex);
    return s_pin_set.contains(obj);
}

// ── POH (Pinned Object Heap) API ───────────────────────────────

void* GcAllocatePinned(CHAOS_IL2CPP_SIZE size) noexcept {
    if (size == 0) return nullptr;
    void* ptr = PohAllocate(size);
    if (ptr != nullptr) {
        // Automatically pin the allocated object.
        GcAddPinnedObject(ptr);
    }
    return ptr;
}

bool GcIsPohPointer(const void* ptr) noexcept {
    return IsPohPointer(ptr);
}

// ── COM apartment management ──────────────────────────────────────────
// CoInitializeEx/CoUninitialize are declared in combaseapi.h, included
// via Windows.h (already in scope from runtime_core.cpp unity build).

CHAOS_IL2CPP_INT32 CoInitializeApartment(CHAOS_IL2CPP_INT32 apartment_type) noexcept {
#if defined(_WIN32)
    HRESULT hr = ::CoInitializeEx(nullptr, static_cast<DWORD>(apartment_type));
    return static_cast<CHAOS_IL2CPP_INT32>(hr);
#else
    (void)apartment_type;
    return 0;  // S_OK on non-Windows (no-op)
#endif
}

void CoUninitializeApartment() noexcept {
#if defined(_WIN32)
    ::CoUninitialize();
#endif
}

}  // namespace chaos::il2cpp::runtime_core