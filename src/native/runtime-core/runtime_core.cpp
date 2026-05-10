#include "runtime_core.h"

#include <chaos/trace.h>
#include <chaos/log.h>

#include "memory_domain.h"
#include "gc_bump_cache.h"
#include "reflection_query_model.h"
#include "generic_context.h"
#include "vtable_registry.h"
#include "runtime_vtable.h"
#include "../bootstrap/bootstrap.h"
#include "runtime_instantiation.h"
#include "reflection_query_model.h"
#include "module_registry.h"

#include <gc.h>

#include <cmath>
#include <atomic>
#include <limits>
#include <cstdlib>
#include <condition_variable>
#include <thread>

#include "gc_transition.h"

// Spin-loop hint (x86 PAUSE / ARM YIELD)
#if defined(_M_ARM64) || defined(__aarch64__)
    #define CHAOS_SPIN_HINT()  __yield()
#else
    #include <immintrin.h>
    #define CHAOS_SPIN_HINT()  _mm_pause()
#endif
#include <cstring>
#include <memory>
#include <mutex>
#include <new>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#if defined(_WIN32)
#include <Windows.h>
#endif

struct ThreadRootRecord {
    const void* address;
    CHAOS_IL2CPP_SIZE size;
};

struct FinalizerWorkItem {
    void* object_instance;
    chaos::il2cpp::runtime_core::FinalizerCallback finalizer;
};

struct RuntimeInternalState {
    CHAOS_IL2CPP_MUTEX finalizer_queue_mutex;
    CHAOS_IL2CPP_VECTOR(FinalizerWorkItem) finalizer_queue;
};

struct ThreadInternalState {
    CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_STRING, CHAOS_IL2CPP_INT32) thread_static_int32_slots;
    CHAOS_IL2CPP_VECTOR(ThreadRootRecord) reported_roots;
    bool at_gc_safepoint = false;
};

struct RuntimeState {
    RuntimeConfig config;
    RuntimeInternalState* internal_state;
};

struct ThreadState {
    RuntimeState* runtime_state;
    ThreadInternalState* internal_state;
};

namespace chaos::il2cpp::runtime_core {

using namespace chaos::il2cpp::runtime_capability;

namespace {

constexpr CHAOS_IL2CPP_UINT64 kDateTimeTicksMask = 0x3FFFFFFFFFFFFFFFull;
// struct ManagedExceptionCarrier is declared in runtime_core.h and used as the cold EH payload.

// ── A5-Trinity Object Header Layouts ────────────────────────────
// Three header kinds discriminated by TypeInfoHot.flags[1:0]:
//   PureType (00):  8B  {TypeInfoHot* type_info}
//   ThinLockable (01): 16B {TypeInfoHot* type_info, uint64_t sync_state}
//   Fat (10):          24B {TypeInfoHot* type_info, void** vtable, uint64_t sync_state}

struct ObjectHeaderFat {  // 24B — full dispatch + sync
    const TypeInfoHot* type_info   = nullptr;  // [0]
    const void**       vtable      = nullptr;  // [8]
    uint64_t           sync_state  = 0;        // [16]
};

struct ObjectHeaderThin {  // 16B — sync only, no vtable
    const TypeInfoHot* type_info   = nullptr;  // [0]
    uint64_t           sync_state  = 0;        // [8]
};

// Legacy alias for code that handles Fat objects specifically.
using ObjectHeader = ObjectHeaderFat;

// ── Header size helpers ──────────────────────────────────────────
inline CHAOS_IL2CPP_SIZE HeaderSizeFromFlags(CHAOS_IL2CPP_UINT8 flags) noexcept {
    switch (flags & kTypeInfoHeaderKindMask) {
        case kTypeInfoHeaderKindPure: return 8;  // PureTypeHeader
        case kTypeInfoHeaderKindThin: return 16; // ObjectHeaderThin
        default:                     return 24;  // ObjectHeaderFat
    }
}

inline uint64_t* GetSyncStatePtr(void* obj) noexcept {
    const auto* ti = *static_cast<const TypeInfoHot* const*>(obj);
    const auto kind = ti->flags & kTypeInfoHeaderKindMask;
    if (kind == kTypeInfoHeaderKindThin)
        return &static_cast<ObjectHeaderThin*>(obj)->sync_state;
    return &static_cast<ObjectHeaderFat*>(obj)->sync_state;
}

struct StringObjectHeader {
    TypeInfoHandle type;
    CHAOS_IL2CPP_UINTPTR byte_count;
};

struct ArrayHeader {
    TypeInfoHandle element_type;
    CHAOS_IL2CPP_UINTPTR length;
};

struct BoxedValueHeader {
    TypeInfoHandle type;
    CHAOS_IL2CPP_UINTPTR byte_count;
};

struct UInt128Layout {
    CHAOS_IL2CPP_UINT64 lower;
    CHAOS_IL2CPP_UINT64 upper;
};

struct Int128Layout {
    CHAOS_IL2CPP_UINT64 lower;
    CHAOS_IL2CPP_INT64 upper;
};

struct MarshalAllocationHeader {
    CHAOS_IL2CPP_UINT64 magic;
    memory_domain::DomainId domain_id;  ///< Originating domain. kDomainIdInvalid = untracked.
    MarshalAllocationKind kind;
    CHAOS_IL2CPP_SIZE requested_size;
};

constexpr CHAOS_IL2CPP_UINT64 kMarshalAllocationMagic = 0x43484D4152534841ull;

constexpr ValueTypeKernelBackendKind DetectCharClassificationKernelBackend() {
    return ValueTypeKernelBackendKind::Intrinsic;
}

constexpr ValueTypeKernelBackendKind DetectFloatingClassificationKernelBackend() {
    return ValueTypeKernelBackendKind::Intrinsic;
}

constexpr ValueTypeKernelBackendKind DetectBitArithmeticKernelBackend() {
    return ValueTypeKernelBackendKind::Intrinsic;
}

constexpr ValueTypeKernelBackendKind DetectTickArithmeticKernelBackend() {
    return ValueTypeKernelBackendKind::Generic;
}

constexpr VectorKernelBackendKind DetectVectorKernelBackend() {
#if defined(__AVX512F__) || defined(__AVX512BW__)
    return VectorKernelBackendKind::Intrinsic;
#elif defined(__AVX2__) || defined(__AVX__)
    return VectorKernelBackendKind::Intrinsic;
#else
    return VectorKernelBackendKind::Generic;
#endif
}

constexpr ValueTypeKernelBackendKind kCharClassificationKernelBackend = DetectCharClassificationKernelBackend();
constexpr ValueTypeKernelBackendKind kFloatingClassificationKernelBackend = DetectFloatingClassificationKernelBackend();
constexpr ValueTypeKernelBackendKind kBitArithmeticKernelBackend = DetectBitArithmeticKernelBackend();
constexpr ValueTypeKernelBackendKind kTickArithmeticKernelBackend = DetectTickArithmeticKernelBackend();
constexpr VectorKernelBackendKind kVectorKernelBackend = DetectVectorKernelBackend();

constexpr CHAOS_IL2CPP_UINT32 kMarshalPlatformAbiVersion = 1u;
constexpr CHAOS_IL2CPP_UINT32 kTaskRuntimeKernelAbiVersion = 1u;

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

struct EngineLifecycleRegistration {
    CHAOS_IL2CPP_STRING phase;
    EngineLifecycleCallback callback;
    void* user_data;
};

constexpr const char* kEngineObservePrefix = "CHAOS_ENGINE_OBSERVE ";

CHAOS_IL2CPP_MUTEX g_engine_binding_mutex;
CHAOS_IL2CPP_UINTPTR g_next_engine_handle = 1u;
CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINTPTR, void*) g_engine_handles = {};
CHAOS_IL2CPP_VECTOR(EngineLifecycleRegistration) g_engine_lifecycle_registrations = {};
const CHAOS_IL2CPP_THREAD::id g_main_thread_id = CHAOS_IL2CPP_THIS_THREAD_GET_ID();
CHAOS_IL2CPP_ATOMIC(RuntimeMode) g_runtime_mode = RuntimeMode::Aot;
CHAOS_IL2CPP_ATOMIC(CHAOS_IL2CPP_INT32) g_next_task_id{1};

// GC handle table: maps handle IDs to object instances.
// Pinned handles are registered as explicit GC roots so the collector never moves/collects them.
struct GcHandleEntry {
    void* object_instance;
    bool pinned;
};
static CHAOS_IL2CPP_MUTEX s_gc_handle_mutex;
static CHAOS_IL2CPP_ATOMIC(CHAOS_IL2CPP_UINT64) s_next_gc_handle{1};
static CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT64, GcHandleEntry) s_gc_handle_table;

void* CHAOS_RUNTIME_ABI_CALL DefaultAllocate(CHAOS_IL2CPP_SIZE size, void* user_data) {
    (void)user_data;
    return GC_MALLOC(size);
}

void CHAOS_RUNTIME_ABI_CALL DefaultDeallocate(void* ptr, void* user_data) {
    (void)user_data;
    (void)ptr;
    // GC manages deallocation automatically — no explicit free needed
}

// Allocate memory that contains no pointers (e.g., string bytes, boxed value data).
// GC_MALLOC_ATOMIC allows the GC to skip scanning this region for pointers,
// improving collection performance.
static void* AllocateBytesAtomic(CHAOS_IL2CPP_SIZE size) {
    return GC_MALLOC_ATOMIC(size);
}

}  // close anonymous — GcAllocate/GcAllocateAtomic need external linkage

// Thread-local bump cache for GC allocations (Phase B: Size-class cache).
// Collocated here so it lives in the same translation unit as
// GcAllocate/GcAllocateAtomic.
static thread_local chaos::il2cpp::runtime_core::GcBumpCache tls_gc_arena;

void* GcAllocate(CHAOS_IL2CPP_SIZE size) {
    return tls_gc_arena.Allocate(size);
}

void* GcAllocateAtomic(CHAOS_IL2CPP_SIZE size) {
    return tls_gc_arena.AllocateAtomic(size);
}

char* DomainStrDup(const char* src) {
    if (src == nullptr) return nullptr;
    if (chaos::il2cpp::memory_domain::CurrentDomain() == nullptr) {
        CHAOS_IL2CPP_LOG_TRACE("MemoryDomain", "DomainStrDup",
            "CurrentDomain()=null, falling back to malloc for src=%p", (const void*)src);
    }
    CHAOS_IL2CPP_SIZE len = std::strlen(src);
    auto* buf = static_cast<char*>(CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(len + 1));
    if (buf == nullptr) return nullptr;
    std::memcpy(buf, src, len + 1);
    return buf;
}

// ======================================================================
// Thread-local RuntimeState — must have EXTERNAL linkage (outside
// anonymous namespace) so reflection_api.obj and codegen-generated
// TUs can reference them.
// ======================================================================

thread_local RuntimeState* g_tls_runtime_state = nullptr;

void SetCurrentRuntimeState(RuntimeState* runtime_state) {
    g_tls_runtime_state = runtime_state;
}

RuntimeState* GetCurrentRuntimeState() {
    return g_tls_runtime_state;
}

thread_local ThreadState* g_tls_thread_state = nullptr;

void SetCurrentThreadState(ThreadState* thread_state) {
    g_tls_thread_state = thread_state;
}

ThreadState* GetCurrentThreadState() {
    return g_tls_thread_state;
}

namespace {  // re-open anonymous for internal helpers

bool TryNormalizeConfig(const RuntimeConfig* config, RuntimeConfig* out_config) {
    if (out_config == nullptr) {
        return false;
    }

    RuntimeConfig normalized = {};
    if (config != nullptr) {
        if (config->struct_size < sizeof(RuntimeConfig)) {
            return false;
        }

        normalized = *config;
    }

    if ((normalized.allocator == nullptr) != (normalized.deallocator == nullptr)) {
        return false;
    }

    if (normalized.allocator == nullptr) {
        normalized.allocator = &DefaultAllocate;
        normalized.deallocator = &DefaultDeallocate;
        normalized.allocator_user_data = nullptr;
    }

    normalized.struct_size = sizeof(RuntimeConfig);
    *out_config = normalized;
    return true;
}

void* AllocateBytes(const RuntimeConfig& config, CHAOS_IL2CPP_SIZE size) {
    if (config.allocator == nullptr) {
        return nullptr;
    }

    return config.allocator(size, config.allocator_user_data);
}

void FreeBytes(const RuntimeConfig& config, void* ptr) {
    if (ptr == nullptr || config.deallocator == nullptr) {
        return;
    }

    config.deallocator(ptr, config.allocator_user_data);
}

static void* GetMarshalAllocationStorage(CHAOS_IL2CPP_INTPTR memory) {
    return reinterpret_cast<void*>(memory);
}

static MarshalAllocationHeader* TryGetMarshalAllocationHeader(CHAOS_IL2CPP_INTPTR memory) {
    if (memory == 0) {
        return nullptr;
    }

    auto* user_storage = static_cast<unsigned char*>(GetMarshalAllocationStorage(memory));
    auto* header = reinterpret_cast<MarshalAllocationHeader*>(user_storage - sizeof(MarshalAllocationHeader));
    return header->magic == kMarshalAllocationMagic ? header : nullptr;
}

static CHAOS_IL2CPP_INTPTR AllocateMarshalBlock(RuntimeState* runtime_state, CHAOS_IL2CPP_SIZE requested_size, MarshalAllocationKind kind) {
    const CHAOS_IL2CPP_SIZE safe_size = requested_size == 0u ? 1u : requested_size;

    auto* domain = memory_domain::CurrentDomain();
    void* allocation = nullptr;

    if (domain != nullptr && domain->heap != nullptr) {
        allocation = domain->heap->Allocate(sizeof(MarshalAllocationHeader) + safe_size);
        if (allocation != nullptr) {
            domain->current_usage += safe_size;
            if (domain->current_usage > domain->peak_usage) {
                domain->peak_usage = domain->current_usage;
            }
        }
    } else {
        allocation = AllocateBytes(runtime_state->config, sizeof(MarshalAllocationHeader) + safe_size);
    }

    if (allocation == nullptr) {
        return 0;
    }

    auto* header = reinterpret_cast<MarshalAllocationHeader*>(allocation);
    header->magic = kMarshalAllocationMagic;
    header->domain_id = domain != nullptr ? domain->domain_id : memory_domain::kDomainIdInvalid;
    header->kind = kind;
    header->requested_size = safe_size;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(allocation) + sizeof(MarshalAllocationHeader);
}

static CHAOS_IL2CPP_INTPTR ReallocateMarshalBlock(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, CHAOS_IL2CPP_SIZE requested_size, MarshalAllocationKind kind) {
    if (memory == 0) {
        return AllocateMarshalBlock(runtime_state, requested_size, kind);
    }

    const CHAOS_IL2CPP_SIZE safe_size = requested_size == 0u ? 1u : requested_size;

    if (auto* old_header = TryGetMarshalAllocationHeader(memory)) {
        // Realloc within the same domain that originally allocated it.
        auto* domain = old_header->domain_id != memory_domain::kDomainIdInvalid
            ? memory_domain::FindDomainById(old_header->domain_id)
            : nullptr;

        if (domain != nullptr && domain->heap != nullptr) {
            void* resized = domain->heap->Reallocate(old_header, sizeof(MarshalAllocationHeader) + safe_size);
            if (resized == nullptr) {
                return 0;
            }
            domain->current_usage += static_cast<CHAOS_IL2CPP_INT64>(safe_size) -
                                     static_cast<CHAOS_IL2CPP_INT64>(old_header->requested_size);
            auto* new_header = reinterpret_cast<MarshalAllocationHeader*>(resized);
            new_header->kind = kind;
            new_header->requested_size = safe_size;
            new_header->magic = kMarshalAllocationMagic;
            return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new_header) + sizeof(MarshalAllocationHeader);
        }

        // Fallback: alloc-copy-free via config allocator.
        void* old_ptr = old_header;
        auto* resized = static_cast<unsigned char*>(AllocateBytes(runtime_state->config, sizeof(MarshalAllocationHeader) + safe_size));
        if (resized == nullptr) {
            return 0;
        }
        CHAOS_IL2CPP_MEMCPY(resized, old_ptr, sizeof(MarshalAllocationHeader) + CHAOS_IL2CPP_MIN(old_header->requested_size, safe_size));
        FreeBytes(runtime_state->config, old_ptr);
        auto* new_header = reinterpret_cast<MarshalAllocationHeader*>(resized);
        new_header->magic = kMarshalAllocationMagic;
        new_header->kind = kind;
        new_header->requested_size = safe_size;
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new_header) + sizeof(MarshalAllocationHeader);
    }

    // No valid header — raw realloc fallback.
    void* old_storage = GetMarshalAllocationStorage(memory);
    auto* resized = static_cast<unsigned char*>(AllocateBytes(runtime_state->config, safe_size));
    if (resized == nullptr) {
        return 0;
    }
    CHAOS_IL2CPP_MEMCPY(resized, old_storage, safe_size);
    FreeBytes(runtime_state->config, old_storage);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(resized);
}

static bool FreeMarshalBlock(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, MarshalAllocationKind kind, bool zero_before_free) {
    if (memory == 0) {
        return true;
    }

    if (auto* header = TryGetMarshalAllocationHeader(memory)) {
        if (zero_before_free) {
            CHAOS_IL2CPP_MEMSET(
                reinterpret_cast<unsigned char*>(header) + sizeof(MarshalAllocationHeader),
                0,
                header->requested_size);
        }

        if (header->kind != kind) {
            return false;
        }

        // Free back to the originating domain's heap, if still alive.
        auto* domain = header->domain_id != memory_domain::kDomainIdInvalid
            ? memory_domain::FindDomainById(header->domain_id)
            : nullptr;

        if (domain != nullptr && domain->heap != nullptr) {
            domain->current_usage -= static_cast<CHAOS_IL2CPP_INT64>(header->requested_size);
            domain->heap->Free(header);
        } else {
            FreeBytes(runtime_state->config, header);
        }
        return true;
    }

    FreeBytes(runtime_state->config, GetMarshalAllocationStorage(memory));
    return true;
}

template <typename TValue>
static TValue MarshalReadValue(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset) {
    TValue value = {};
    if (address == 0) {
        return value;
    }

    auto* source = reinterpret_cast<const unsigned char*>(GetMarshalAllocationStorage(address)) + offset;
    CHAOS_IL2CPP_MEMCPY(&value, source, sizeof(TValue));
    return value;
}

template <typename TValue>
static void MarshalWriteValue(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, TValue value) {
    if (address == 0) {
        return;
    }

    auto* destination = reinterpret_cast<unsigned char*>(GetMarshalAllocationStorage(address)) + offset;
    CHAOS_IL2CPP_MEMCPY(destination, &value, sizeof(TValue));
}

bool IsAttached(RuntimeState* runtime_state, ThreadState* thread_state) {
    return runtime_state != nullptr && thread_state != nullptr && thread_state->runtime_state == runtime_state;
}

RuntimeInternalState* GetRuntimeInternalState(RuntimeState* runtime_state) {
    return runtime_state != nullptr ? runtime_state->internal_state : nullptr;
}

ThreadInternalState* GetThreadInternalState(ThreadState* thread_state) {
    return thread_state != nullptr ? thread_state->internal_state : nullptr;
}

// ── Atomic helpers for uint64_t (sync_state) ──────────────────────────
// ObjectHeader keeps a plain uint64_t field (GC_MALLOC memory doesn't
// run constructors, so std::atomic<uint64_t> is unsafe).  These helpers
// use platform intrinsics directly.
//
// On x64, Interlocked intrinsics include full memory barriers (seq_cst),
// which is stronger than what we strictly need for thin-lock CAS, but
// the performance cost is negligible on x64 hardware.

#if defined(_MSC_VER)
    #include <intrin.h>
    #pragma intrinsic(_InterlockedCompareExchange64, _InterlockedExchange64)
    // MSVC doesn't define __ATOMIC_* constants (they are GCC built-in).
    // Our MSVC AtomicCAS() ignores memory-order parameters (the Interlocked
    // intrinsics are already seq_cst), so define them as harmless zeros.
    #define __ATOMIC_RELAXED 0
    #define __ATOMIC_ACQUIRE 0
    #define __ATOMIC_RELEASE 0
    inline uint64_t AtomicLoadRelaxed(const uint64_t* p) noexcept {
        return *const_cast<volatile uint64_t*>(p);
    }
    inline uint64_t AtomicLoadAcquire(const uint64_t* p) noexcept {
        uint64_t v = *const_cast<volatile uint64_t*>(p);
        _ReadWriteBarrier();
        return v;
    }
    inline void AtomicStoreRelease(uint64_t* p, uint64_t val) noexcept {
        _InterlockedExchange64(reinterpret_cast<volatile LONG64*>(p),
                               static_cast<LONG64>(val));
    }
    inline void AtomicStoreRelaxed(uint64_t* p, uint64_t val) noexcept {
        *const_cast<volatile uint64_t*>(p) = val;
    }
    inline bool AtomicCAS(uint64_t* p, uint64_t& expected, uint64_t desired,
                          int = 0, int = 0) noexcept {
        LONG64 prev = _InterlockedCompareExchange64(
            reinterpret_cast<volatile LONG64*>(p),
            static_cast<LONG64>(desired),
            static_cast<LONG64>(expected));
        if (prev == static_cast<LONG64>(expected)) return true;
        expected = static_cast<uint64_t>(prev);
        return false;
    }
#else
    inline uint64_t AtomicLoadRelaxed(const uint64_t* p) noexcept {
        return __atomic_load_n(p, __ATOMIC_RELAXED);
    }
    inline uint64_t AtomicLoadAcquire(const uint64_t* p) noexcept {
        return __atomic_load_n(p, __ATOMIC_ACQUIRE);
    }
    inline void AtomicStoreRelease(uint64_t* p, uint64_t val) noexcept {
        __atomic_store_n(p, val, __ATOMIC_RELEASE);
    }
    inline void AtomicStoreRelaxed(uint64_t* p, uint64_t val) noexcept {
        __atomic_store_n(p, val, __ATOMIC_RELAXED);
    }
    inline bool AtomicCAS(uint64_t* p, uint64_t& expected, uint64_t desired,
                          int success_order = __ATOMIC_ACQUIRE,
                          int failure_order = __ATOMIC_RELAXED) noexcept {
        return __atomic_compare_exchange_n(p, &expected, desired,
            false, success_order, failure_order);
    }
#endif

// Thin lock / SyncBlock constants
//
// ObjectHeader::sync_state encoding:
//   bits [1:0] = 00 : free (unlocked)
//   bits [1:0] = 01 : thin lock held (bits 2-31 = thread_id, bits 32-63 = recursion)
//   bits [1:0] = 10 : inflated to SyncBlock table (bits 2-63 = index)
//   bits [1:0] = 11 : reserved (hash code, not yet used)

constexpr uint64_t kSyncLockedBit     = 1ull << 0;
constexpr uint64_t kSyncInflatedBit   = 1ull << 1;
constexpr uint64_t kSyncThreadShift   = 2;
constexpr uint64_t kSyncRecursionShift = 32;

constexpr uint32_t kSyncBlockStripes  = 64;
constexpr uint32_t kSyncBlockSpinMax  = 1000;

struct SyncBlock {
    const TypeInfoHot*                     type_info = nullptr;  // saved type_info for ThinLockable inflation
    CHAOS_IL2CPP_RECURSIVE_LOCK_MUTEX    mutex;
    std::condition_variable_any          cond;  // for Monitor.Wait/Pulse
};

struct SyncBlockStripe {
    CHAOS_IL2CPP_MUTEX                              table_lock;
    CHAOS_IL2CPP_UNORDERED_MAP(void*, SyncBlock*)   entries;
};

SyncBlockStripe g_sync_block_stripes[kSyncBlockStripes];

/// Hash an object pointer to a stripe index.
inline uint32_t SyncBlockStripeIndex(void* obj) noexcept {
    return (reinterpret_cast<uintptr_t>(obj) >> 3) % kSyncBlockStripes;
}

/// Inflate a thin lock to a SyncBlock (contention path).
/// Must be called after CAS failure & spinning.
static bool InflateAndEnter(void* obj, uint64_t current_sync) noexcept {
    const uint32_t stripe_idx = SyncBlockStripeIndex(obj);
    auto& stripe = g_sync_block_stripes[stripe_idx];

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.table_lock);

    // Double-check: another thread may have inflated already.
    auto* sync_ptr = GetSyncStatePtr(obj);
    uint64_t sync = *sync_ptr;
    if ((sync & kSyncInflatedBit) != 0) {
        const auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
        if (sb != nullptr) {
            // Already inflated, find it in the stripe and lock.
            for (auto& [ptr, block] : stripe.entries) {
                if (ptr == obj && block != nullptr) {
                    block->mutex.lock();
                    return true;
                }
            }
        }
        return false;
    }

    // Allocate a new SyncBlock and inflate.
    auto* sb = new SyncBlock();
    stripe.entries[obj] = sb;

    // Mark inflated in header. Use SyncBlock* itself (unique, no lookup needed).
    const uint64_t inflated_val = kSyncInflatedBit | reinterpret_cast<uint64_t>(sb);
    AtomicStoreRelease(sync_ptr, inflated_val);

    sb->mutex.lock();
    return true;
}

bool IsLikelyMetadataTokenHandle(MethodInfoHandle method) {
    const CHAOS_IL2CPP_UINTPTR raw_method = static_cast<CHAOS_IL2CPP_UINTPTR>(method);
    if (raw_method == 0u) return false;
    // On 64-bit platforms, real pointers always have nonzero high 32 bits,
    // while metadata tokens are zero-extended 32-bit values (table code in
    // the low 8 bits, row index in bits 8-23).  Checking the high bits
    // eliminates all valid pointer aliases.
    if (sizeof(CHAOS_IL2CPP_UINTPTR) > sizeof(CHAOS_IL2CPP_UINT32) && (raw_method >> 32) != 0u) return false;
    // Tighten to the maximum valid IL metadata token range.
    // Token format: 0xTT###### where TT ∈ [0x00, 0x2B].
    return raw_method <= static_cast<CHAOS_IL2CPP_UINTPTR>(0x2BFFFFFFu);
}

static const ReflectionQueryTypeDescriptor* TryResolveRuntimeCoreTypeDescriptor(TypeInfoHandle type) {
    if (type == 0) {
        return nullptr;
    }

    return TryDecodeReflectionQueryTypeHandle(type);
}

static bool TryPopulateVectorCapabilityFromDisplayName(
    const CHAOS_IL2CPP_STRING& display_name,
    RuntimeTypeCapabilityInfoV0* out_capability_info) {
    constexpr CHAOS_IL2CPP_STRING_VIEW kNumericsVectorPrefix = "System.Numerics.Vector<";
    if (display_name.size() >= kNumericsVectorPrefix.size() &&
        memcmp(display_name.data(), kNumericsVectorPrefix.data(), kNumericsVectorPrefix.size()) == 0 &&
        display_name.size() > kNumericsVectorPrefix.size() &&
        display_name.back() == '>') {
        const auto scalar_display_name = display_name.substr(
            kNumericsVectorPrefix.size(),
            display_name.size() - kNumericsVectorPrefix.size() - 1u);
        return TryPopulateVectorCapabilityFromDisplayName(scalar_display_name, out_capability_info);
    }

    if (display_name == "System.Byte" || display_name == "System.SByte") {
        out_capability_info->value_size_bytes = 1u;
        out_capability_info->vector_lane_kind = TypeCapabilityVectorLaneKindInteger;
        out_capability_info->scalar_kind = display_name == "System.SByte"
            ? TypeCapabilityScalarKindSignedInteger
            : TypeCapabilityScalarKindUnsignedInteger;
        return true;
    }

    if (display_name == "System.Int16" || display_name == "System.UInt16") {
        out_capability_info->value_size_bytes = 2u;
        out_capability_info->vector_lane_kind = TypeCapabilityVectorLaneKindInteger;
        out_capability_info->scalar_kind = display_name == "System.Int16"
            ? TypeCapabilityScalarKindSignedInteger
            : TypeCapabilityScalarKindUnsignedInteger;
        return true;
    }

    if (display_name == "System.Int32" || display_name == "System.UInt32" || display_name == "System.Single") {
        out_capability_info->value_size_bytes = 4u;
        out_capability_info->vector_lane_kind = display_name == "System.Single"
            ? TypeCapabilityVectorLaneKindFloating
            : TypeCapabilityVectorLaneKindInteger;
        out_capability_info->scalar_kind = display_name == "System.Single"
            ? TypeCapabilityScalarKindFloating
            : (display_name == "System.Int32"
                ? TypeCapabilityScalarKindSignedInteger
                : TypeCapabilityScalarKindUnsignedInteger);
        return true;
    }

    if (display_name == "System.Int64" || display_name == "System.UInt64" || display_name == "System.Double") {
        out_capability_info->value_size_bytes = 8u;
        out_capability_info->vector_lane_kind = display_name == "System.Double"
            ? TypeCapabilityVectorLaneKindFloating
            : TypeCapabilityVectorLaneKindInteger;
        out_capability_info->scalar_kind = display_name == "System.Double"
            ? TypeCapabilityScalarKindFloating
            : (display_name == "System.Int64"
                ? TypeCapabilityScalarKindSignedInteger
                : TypeCapabilityScalarKindUnsignedInteger);
        return true;
    }

    if (display_name == "System.IntPtr" || display_name == "System.UIntPtr") {
#if INTPTR_MAX == INT64_MAX
        out_capability_info->value_size_bytes = 8u;
#else
        out_capability_info->value_size_bytes = 4u;
#endif
        out_capability_info->vector_lane_kind = TypeCapabilityVectorLaneKindInteger;
        out_capability_info->scalar_kind = display_name == "System.IntPtr"
            ? TypeCapabilityScalarKindNativeSignedInteger
            : TypeCapabilityScalarKindNativeUnsignedInteger;
        return true;
    }

    return false;
}

static bool TryPopulateRegisteredTypeCapability(
    TypeInfoHandle type,
    RuntimeTypeCapabilityInfoV0* out_capability_info) {
    if (type == 0) {
        return false;
    }

    const CHAOS_IL2CPP_UINTPTR raw_handle = static_cast<CHAOS_IL2CPP_UINTPTR>(type);
    if ((raw_handle & kReflectionQueryHandleTag) != 0u) {
        return false;
    }

    const CHAOS_IL2CPP_UINT32 type_token = static_cast<CHAOS_IL2CPP_UINT32>(raw_handle);
    if (const auto* entry = chaos::il2cpp::bootstrap::FindRegisteredTypeCapabilityEntry(type_token)) {
        *out_capability_info = entry->capability_info;
        return true;
    }

    return false;
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL RuntimeInit(
    const RuntimeInitParams* init_params,
    const RuntimeConfig* config,
    RuntimeState** out_runtime_state) {
    CHAOS_IL2CPP_LOG_TRACE_INIT();
    CHAOS_IL2CPP_LOG_TRACE("runtime", "RuntimeInit", "");
    if (init_params == nullptr || out_runtime_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    *out_runtime_state = nullptr;
    if (init_params->struct_size < sizeof(RuntimeInitParams)) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    RuntimeConfig normalized_config = {};
    if (!TryNormalizeConfig(config, &normalized_config)) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    // Initialize BDWGC exactly once per process.
    static CHAOS_IL2CPP_ONCE_FLAG s_gc_init_flag;
    CHAOS_IL2CPP_CALL_ONCE(s_gc_init_flag, []() { GC_INIT(); });

    RuntimeState* runtime_state = static_cast<RuntimeState*>(AllocateBytes(normalized_config, sizeof(RuntimeState)));
    if (runtime_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
    }

    runtime_state->config = normalized_config;

    void* internal_mem = AllocateBytes(normalized_config, sizeof(RuntimeInternalState));
    if (internal_mem == nullptr) {
        FreeBytes(normalized_config, runtime_state);
        return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
    }

    runtime_state->internal_state = ::new (internal_mem) RuntimeInternalState();

    SetRuntimeMode(RuntimeMode::Aot);
    *out_runtime_state = runtime_state;
    return CHAOS_RUNTIME_STATUS_OK;
}

void CHAOS_RUNTIME_ABI_CALL RuntimeShutdown(RuntimeState* runtime_state) {
    if (runtime_state == nullptr) {
        return;
    }

    SetRuntimeMode(RuntimeMode::Aot);
    if (runtime_state->internal_state != nullptr) {
        runtime_state->internal_state->~RuntimeInternalState();
        FreeBytes(runtime_state->config, runtime_state->internal_state);
        runtime_state->internal_state = nullptr;
    }
    // RuntimeState itself is GC-managed; no explicit free needed.
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL ThreadAttach(
    RuntimeState* runtime_state,
    ThreadState** out_thread_state) {
    if (runtime_state == nullptr || out_thread_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    *out_thread_state = nullptr;
    ThreadState* thread_state = static_cast<ThreadState*>(AllocateBytes(runtime_state->config, sizeof(ThreadState)));
    if (thread_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
    }

    thread_state->runtime_state = runtime_state;

    void* internal_mem = AllocateBytes(runtime_state->config, sizeof(ThreadInternalState));
    if (internal_mem == nullptr) {
        FreeBytes(runtime_state->config, thread_state);
        return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
    }

    thread_state->internal_state = ::new (internal_mem) ThreadInternalState();

    // Register this thread with BDWGC so it can scan the thread's stack for roots.
    // On Windows (GC_WIN32_THREADS), BDWGC auto-registers threads via DllMain / Win32 hooks;
    // explicit registration is neither needed nor allowed.
    // On pthreads platforms, we must register manually.
#if !defined(_WIN32) && !defined(_WIN64)
    struct GC_stack_base sb;
    if (GC_get_stack_base(&sb) == GC_SUCCESS) {
        const int gc_reg_result = GC_register_my_thread(&sb);
        (void)gc_reg_result;  // GC_DUPLICATE is fine for the main thread
    }
#endif

    *out_thread_state = thread_state;
    SetCurrentThreadState(thread_state);

    // Register the main thread in the managed thread registry.
    threading::RegisterThread(threading::kMainThreadId, nullptr);

    return CHAOS_RUNTIME_STATUS_OK;
}

void CHAOS_RUNTIME_ABI_CALL ThreadDetach(
    RuntimeState* runtime_state,
    ThreadState* thread_state) {
    if (runtime_state == nullptr || thread_state == nullptr) {
        return;
    }

    // Unregister this thread from BDWGC on pthreads platforms only.
    // On Windows, BDWGC auto-manages thread lifecycle via Win32 hooks.
#if !defined(_WIN32) && !defined(_WIN64)
    GC_unregister_my_thread();
#endif

    if (thread_state->internal_state != nullptr) {
        thread_state->internal_state->~ThreadInternalState();
        FreeBytes(runtime_state->config, thread_state->internal_state);
        thread_state->internal_state = nullptr;
    }
    SetCurrentThreadState(nullptr);
    threading::UnregisterThread();
    FreeBytes(runtime_state->config, thread_state);
}

namespace {

/// Resolve the TypeInfo* and vtable for a given TypeInfoHandle.
///
/// Handles three encoding schemes:
///   1. Tag-encoded (RuntimeInstantiatedType): compute stable_id from
///      subject_id_utf8, look up vtable.  type_info is not available.
///   2. HotUpdate registered type: look up by stable_id in the dynamic
///      type registry.
///   3. Module-registry handle: fallback — type_info / vtable = nullptr.
///
/// Returns true when at least the vtable was resolved.
static bool ResolveObjectTypeInfo(TypeInfoHandle type_handle,
                                   const TypeInfoHot*& out_type_info,
                                   const void**& out_vtable) noexcept
{
    out_type_info = nullptr;
    out_vtable    = nullptr;

    if (type_handle == 0u) return false;

    // ── Path 1: Tag-encoded RuntimeInstantiatedType handle ──
    const auto* desc = TryDecodeReflectionQueryTypeHandle(type_handle);
    if (desc != nullptr) {
        if (desc->subject_id_utf8 != nullptr) {
            const CHAOS_IL2CPP_UINT64 stable_id =
                chaos_compute_type_stable_id(desc->subject_id_utf8);
            out_vtable = runtime_vtable::FindVTable(stable_id);
            return (out_vtable != nullptr);
        }
        return false;
    }

    // ── Path 2: Module-registry / HotUpdate handle ──
    // Extract module_id and metadata token, then look up TypeInfo*
    // via the module's type_info_ptrs array (Phase 3+).
    uint32_t module_id = GetModuleId(type_handle);
    uint32_t token = GetTypeToken(type_handle);
    if (token != 0) {
        const auto* mod = LookupModule(module_id != 0u ? module_id : 0u);
        if (mod != nullptr && !mod->tombstone && mod->type_flags != nullptr) {
            uint32_t idx = TokenToIndex(token);
            if (idx < mod->type_count && mod->type_info_ptrs != nullptr) {
                out_type_info = mod->type_info_ptrs[idx];
                if (out_type_info != nullptr && out_type_info->stable_id != 0) {
                    out_vtable = runtime_vtable::FindVTable(out_type_info->stable_id);
                    return (out_vtable != nullptr);
                }
            }
        }
    }

    return false;
}

}  // anonymous namespace

void* CHAOS_RUNTIME_ABI_CALL ObjectNew(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle type) {
    if (!IsAttached(runtime_state, thread_state)) {
        return nullptr;
    }

    // Step 1: resolve TypeInfoHot* from handle.
    const TypeInfoHot* type_info = nullptr;
    const void** vtable = nullptr;
    ResolveObjectTypeInfo(type, type_info, vtable);
    if (type_info == nullptr) {
        return nullptr;
    }

    // Step 2: determine header kind and allocate.
    const CHAOS_IL2CPP_SIZE header_size = HeaderSizeFromFlags(type_info->flags);
    auto* object = static_cast<ObjectHeaderFat*>(AllocateBytes(runtime_state->config, header_size));
    if (object == nullptr) {
        return nullptr;
    }

    // Step 3: populate fields per kind.
    object->type_info = type_info;
    const auto kind = type_info->flags & kTypeInfoHeaderKindMask;
    if (kind == kTypeInfoHeaderKindFat) {
        object->vtable = vtable;
        object->sync_state = 0;
    } else if (kind == kTypeInfoHeaderKindThin) {
        auto* thin = static_cast<ObjectHeaderThin*>(static_cast<void*>(object));
        thin->sync_state = 0;
    }
    // PureType: type_info already set, nothing else needed.

    return object;
}

// ── ObjectNewDirect (Phase 3: skip TypeInfoHandle resolution) ─────
// For AOT static types where TypeInfoHot* and vtable are known at
// compile time.  Skips the ResolveObjectTypeInfo parse path entirely.
// Callers from codegen-generated code need RuntimeState/ThreadState
// available (passed through global or explicit parameter).
void* CHAOS_RUNTIME_ABI_CALL ObjectNewDirect(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const TypeInfoHot* type_info,
    const void** vtable) noexcept
{
    if (!IsAttached(runtime_state, thread_state)) {
        return nullptr;
    }
    if (type_info == nullptr) {
        return nullptr;
    }

    const CHAOS_IL2CPP_SIZE header_size = HeaderSizeFromFlags(type_info->flags);
    auto* object = static_cast<ObjectHeaderFat*>(AllocateBytes(runtime_state->config, header_size));
    if (object == nullptr) {
        return nullptr;
    }

    object->type_info = type_info;
    const auto kind = type_info->flags & kTypeInfoHeaderKindMask;
    if (kind == kTypeInfoHeaderKindFat) {
        object->vtable = vtable;
        object->sync_state = 0;
    } else if (kind == kTypeInfoHeaderKindThin) {
        auto* thin = static_cast<ObjectHeaderThin*>(static_cast<void*>(object));
        thin->sync_state = 0;
    }
    return object;
}

void* CHAOS_RUNTIME_ABI_CALL ArrayNew(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle element_type,
    CHAOS_IL2CPP_UINTPTR length) {
    if (!IsAttached(runtime_state, thread_state) || element_type == 0) {
        return nullptr;
    }

    const CHAOS_IL2CPP_SIZE allocation_size =
        sizeof(ArrayHeader) + (static_cast<CHAOS_IL2CPP_SIZE>(length) * sizeof(void*));
    unsigned char* storage = static_cast<unsigned char*>(AllocateBytes(runtime_state->config, allocation_size));
    if (storage == nullptr) {
        return nullptr;
    }

    auto* header = reinterpret_cast<ArrayHeader*>(storage);
    header->element_type = element_type;
    header->length = length;

    void** elements = reinterpret_cast<void**>(storage + sizeof(ArrayHeader));
    if (length != 0u) {
        CHAOS_IL2CPP_MEMSET(elements, 0, static_cast<CHAOS_IL2CPP_SIZE>(length) * sizeof(void*));
    }

    return header;
}

void* CHAOS_RUNTIME_ABI_CALL StringNewUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const char* utf8_bytes,
    CHAOS_IL2CPP_UINTPTR byte_count) {
    if (!IsAttached(runtime_state, thread_state)) {
        return nullptr;
    }

    if (utf8_bytes == nullptr && byte_count != 0u) {
        return nullptr;
    }

    const CHAOS_IL2CPP_SIZE allocation_size = sizeof(StringObjectHeader) + static_cast<CHAOS_IL2CPP_SIZE>(byte_count) + 1u;
    // Use atomic allocation: string bytes contain no pointers, so GC need not scan them.
    unsigned char* storage = static_cast<unsigned char*>(AllocateBytesAtomic(allocation_size));
    if (storage == nullptr) {
        return nullptr;
    }

    auto* header = reinterpret_cast<StringObjectHeader*>(storage);
    header->type = 0;
    header->byte_count = byte_count;

    char* text = reinterpret_cast<char*>(storage + sizeof(StringObjectHeader));
    if (byte_count != 0u) {
        CHAOS_IL2CPP_MEMCPY(text, utf8_bytes, static_cast<CHAOS_IL2CPP_SIZE>(byte_count));
    }

    text[byte_count] = '\0';
    return header;
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL ClassInit(
    RuntimeState* runtime_state,
    TypeInfoHandle type) {
    if (runtime_state == nullptr) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    return type != 0 ? CHAOS_RUNTIME_STATUS_OK : CHAOS_RUNTIME_STATUS_NOT_FOUND;
}

RuntimeStatus TypeQueryCapabilityImpl(
    TypeInfoHandle type,
    RuntimeTypeCapabilityInfoV0* out_capability_info) {
    if (out_capability_info == nullptr || out_capability_info->struct_size < sizeof(RuntimeTypeCapabilityInfoV0)) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    out_capability_info->capability_bits = 0u;
    out_capability_info->value_size_bytes = 0u;
    out_capability_info->vector_width_bytes = 0u;
    out_capability_info->vector_lane_count = 0u;
    out_capability_info->vector_lane_kind = TypeCapabilityVectorLaneKindNone;
    out_capability_info->scalar_kind = TypeCapabilityScalarKindNone;

    if (TryPopulateRegisteredTypeCapability(type, out_capability_info)) {
        return CHAOS_RUNTIME_STATUS_OK;
    }

    if (const auto* descriptor = TryResolveRuntimeCoreTypeDescriptor(type)) {
        CHAOS_IL2CPP_STRING display_name = descriptor->display_name_utf8 != nullptr
            ? descriptor->display_name_utf8
            : "";
        if (TryPopulateVectorCapabilityFromDisplayName(display_name, out_capability_info)) {
            // Resolve vector register width from the type name.
            // Order matters: longer prefixes first to avoid false prefix match.
            out_capability_info->vector_width_bytes = [&display_name]() -> CHAOS_IL2CPP_UINT32 {
                auto starts_with = [&](const char* prefix) -> bool {
                    const auto len = strlen(prefix);
                    return display_name.size() >= len && memcmp(display_name.data(), prefix, len) == 0;
                };
                if (starts_with("System.Numerics.Vector512<")) return 64u;
                if (starts_with("System.Numerics.Vector256<")) return 32u;
                if (starts_with("System.Numerics.Vector128<")) return 16u;
                if (starts_with("System.Numerics.Vector64<"))  return 8u;
                if (starts_with("System.Numerics.Vector<"))
                    return static_cast<CHAOS_IL2CPP_UINT32>(VectorPlatformByteWidth());
                return 16u;  // default: hardware-agnostic fixed SIMD
            }();
            out_capability_info->vector_lane_count = out_capability_info->value_size_bytes == 0u
                ? 0u
                : (out_capability_info->vector_width_bytes / out_capability_info->value_size_bytes);
            out_capability_info->capability_bits = 1u;
            return CHAOS_RUNTIME_STATUS_OK;
        }

        return CHAOS_RUNTIME_STATUS_NOT_SUPPORTED;
    }

    return CHAOS_RUNTIME_STATUS_NOT_SUPPORTED;
}

GCHandle CHAOS_RUNTIME_ABI_CALL GcHandleNew(
    RuntimeState* runtime_state,
    void* object_instance,
    bool pinned) {
    if (runtime_state == nullptr || object_instance == nullptr) {
        return CHAOS_GC_HANDLE_INVALID;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_gc_handle_mutex);
    CHAOS_IL2CPP_UINT64 handle = s_next_gc_handle++;
    s_gc_handle_table[handle] = GcHandleEntry{ object_instance, pinned };

    if (pinned) {
        // Tell GC this address is an explicit root so the object is never collected.
        GC_add_roots(object_instance,
            static_cast<char*>(object_instance) + sizeof(void*));
    }

    return static_cast<GCHandle>(handle);
}

void CHAOS_RUNTIME_ABI_CALL GcHandleFree(
    RuntimeState* runtime_state,
    GCHandle gc_handle) {
    if (runtime_state == nullptr || gc_handle == CHAOS_GC_HANDLE_INVALID) {
        return;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_gc_handle_mutex);
    auto it = s_gc_handle_table.find(static_cast<CHAOS_IL2CPP_UINT64>(gc_handle));
    if (it != s_gc_handle_table.end()) {
        // Note: for pinned handles we intentionally do not call GC_remove_roots here.
        // GC_add_roots registers a memory range (not the object itself) as a scan root.
        // Once the handle is removed from the table, no live code will use this object
        // through the handle, so the range is a harmless false positive until the next GC.
        // GC_remove_roots requires DYNAMIC_LOADING and is not available on all platforms.
        s_gc_handle_table.erase(it);
    }
}

void CHAOS_RUNTIME_ABI_CALL RaiseManagedException(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    ExceptionHandle exception) {
    if (!IsAttached(runtime_state, thread_state)) {
        return;
    }

    throw ManagedExceptionCarrier
    {
        exception,
    };
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL FieldGetValue(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    FieldInfoHandle field,
    void* object_instance,
    void* out_value,
    CHAOS_IL2CPP_SIZE out_value_size) {
    if (!IsAttached(runtime_state, thread_state)
        || object_instance == nullptr
        || out_value == nullptr
        || out_value_size == 0u) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    if (field == 0) {
        return CHAOS_RUNTIME_STATUS_NOT_FOUND;
    }

    // A5-Trinity: header size varies by kind (PureType 8B, ThinLockable 16B, Fat 24B).
    const auto* ti = *static_cast<const TypeInfoHot* const*>(object_instance);
    const auto header_size = HeaderSizeFromFlags(ti != nullptr ? ti->flags : 0);
    CHAOS_IL2CPP_MEMCPY(out_value,
        static_cast<const unsigned char*>(object_instance) + header_size,
        out_value_size);
    return CHAOS_RUNTIME_STATUS_OK;
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL FieldSetValue(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    FieldInfoHandle field,
    void* object_instance,
    const void* value,
    CHAOS_IL2CPP_SIZE value_size) {
    if (!IsAttached(runtime_state, thread_state)
        || object_instance == nullptr
        || value == nullptr
        || value_size == 0u) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    if (field == 0) {
        return CHAOS_RUNTIME_STATUS_NOT_FOUND;
    }

    // A5-Trinity: header size varies by kind (PureType 8B, ThinLockable 16B, Fat 24B).
    const auto* ti = *static_cast<const TypeInfoHot* const*>(object_instance);
    const auto header_size = HeaderSizeFromFlags(ti != nullptr ? ti->flags : 0);
    CHAOS_IL2CPP_MEMCPY(
        static_cast<unsigned char*>(object_instance) + header_size,
        value, value_size);
    return CHAOS_RUNTIME_STATUS_OK;
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL MethodInvoke(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    MethodInfoHandle method,
    void* object_instance,
    void* const* argv,
    CHAOS_IL2CPP_UINT32 argc,
    void* out_return_value,
    CHAOS_IL2CPP_SIZE out_return_value_size,
    ExceptionHandle* out_exception) {
    using RawMethodInvokerFn = void* (CHAOS_RUNTIME_ABI_CALL*)(
        RuntimeState* runtime,
        ThreadState* thread,
        void* __this,
        void* const* argv,
        CHAOS_IL2CPP_UINT32 argc);

    if (!IsAttached(runtime_state, thread_state) || method == 0) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    if (out_exception != nullptr) {
        *out_exception = nullptr;
    }

    // Decode the method handle to a token agnostically, then look up
    // the invoker from the table built at RegisterCodegen time.
    // This handles all three handle types (opaque token, reflection-query,
    // function pointer) through a single uniform path.
    void* invoker_ptr = nullptr;
    CHAOS_IL2CPP_UINT32 method_token = 0;
    bool is_token_based = false;

    if (IsLikelyMetadataTokenHandle(method)) {
        method_token = static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_UINTPTR>(method));
        is_token_based = true;
    } else if (const auto* desc = TryDecodeReflectionQueryMethodHandle(method)) {
        method_token = desc->metadata_token;
        is_token_based = true;
    } else {
        // Already a real function pointer — use directly.
        invoker_ptr = reinterpret_cast<void*>(method);
    }

    if (is_token_based) {
        invoker_ptr = chaos::il2cpp::bootstrap::FindInvokerPointer(method_token);
        if (invoker_ptr == nullptr) {
            // Runtime-allocated token (>= 0x80000000): fall back to interpreted
            // bridge for MakeGenericMethod-created methods.
            if (method_token >= 0x80000000u) {
                const auto* bridge = ChaosRuntimeInstantiationGetBridgeV0();
                if (bridge != nullptr && bridge->interpret_method_call != nullptr) {
                    return bridge->interpret_method_call(
                        runtime_state, thread_state, method,
                        object_instance, argv, argc,
                        out_return_value, out_return_value_size,
                        out_exception);
                }
            }
            return CHAOS_RUNTIME_STATUS_NOT_FOUND;
        }
    }

    try {
        auto* const invoker = reinterpret_cast<RawMethodInvokerFn>(invoker_ptr);
        if (invoker == nullptr) {
            return CHAOS_RUNTIME_STATUS_NOT_FOUND;
        }

        void* return_value = invoker(runtime_state, thread_state, object_instance, argv, argc);
        if (out_return_value != nullptr) {
            if (out_return_value_size == sizeof(void*)) {
                CHAOS_IL2CPP_MEMCPY(out_return_value, &return_value, sizeof(return_value));
            } else {
                auto* indirect_return_value = reinterpret_cast<void* const*>(out_return_value);
                if (indirect_return_value == nullptr || *indirect_return_value == nullptr) {
                    return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
                }

                if (return_value == nullptr) {
                    return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
                }

                CHAOS_IL2CPP_MEMCPY(*indirect_return_value, return_value, out_return_value_size);
            }
        }

        return CHAOS_RUNTIME_STATUS_OK;
    } catch (const ManagedExceptionCarrier& carrier) {
        if (out_exception != nullptr) {
            *out_exception = carrier.exception;
        }

        return CHAOS_RUNTIME_STATUS_MANAGED_EXCEPTION;
    }
}

ImageHandle CHAOS_RUNTIME_ABI_CALL AssemblyGetImage(AssemblyHandle assembly) {
    return reinterpret_cast<ImageHandle>(assembly);
}

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL ImageFindType(
    ImageHandle image,
    const char* namespace_utf8,
    const char* type_name_utf8) {
    if (image == 0 || namespace_utf8 == nullptr || type_name_utf8 == nullptr) {
        return 0;
    }

    if (const auto* reflection_image = TryDecodeReflectionQueryImageHandle(image)) {
        const auto* type = FindReflectionQueryTypeByName(reflection_image, namespace_utf8, type_name_utf8);
        return EncodeReflectionQueryTypeHandle(type);
    }

    return static_cast<TypeInfoHandle>(image);
}

MethodInfoHandle CHAOS_RUNTIME_ABI_CALL TypeFindMethod(
    TypeInfoHandle type,
    const char* method_name_utf8,
    CHAOS_IL2CPP_INT32 parameter_count) {
    if (type == 0 || method_name_utf8 == nullptr || parameter_count < 0) {
        return 0;
    }

    if (const auto* reflection_type = TryDecodeReflectionQueryTypeHandle(type)) {
        const auto* method = FindReflectionQueryMethod(reflection_type, method_name_utf8, parameter_count);
        return EncodeReflectionQueryMethodHandle(method);
    }

    return static_cast<MethodInfoHandle>(type);
}

FieldInfoHandle CHAOS_RUNTIME_ABI_CALL TypeFindField(
    TypeInfoHandle type,
    const char* field_name_utf8) {
    if (type == 0 || field_name_utf8 == nullptr) {
        return 0;
    }

    if (const auto* reflection_type = TryDecodeReflectionQueryTypeHandle(type)) {
        const auto* field = FindReflectionQueryField(reflection_type, field_name_utf8);
        return EncodeReflectionQueryFieldHandle(field);
    }

    return static_cast<FieldInfoHandle>(type);
}

PropertyInfoHandle CHAOS_RUNTIME_ABI_CALL TypeFindProperty(
    TypeInfoHandle type,
    const char* property_name_utf8) {
    if (type == 0 || property_name_utf8 == nullptr) {
        return 0;
    }

    if (const auto* reflection_type = TryDecodeReflectionQueryTypeHandle(type)) {
        const auto* property = FindReflectionQueryProperty(reflection_type, property_name_utf8);
        return EncodeReflectionQueryPropertyHandle(property);
    }

    return 0;
}

EventInfoHandle CHAOS_RUNTIME_ABI_CALL TypeFindEvent(
    TypeInfoHandle type,
    const char* event_name_utf8) {
    (void)type;
    (void)event_name_utf8;
    return 0;
}

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL TypeGetGenericTypeDefinition(TypeInfoHandle type) {
    if (type == 0) {
        return 0;
    }

    if (const auto* reflection_type = TryDecodeReflectionQueryTypeHandle(type)) {
        return EncodeReflectionQueryTypeHandle(reflection_type->generic_type_definition);
    }

    return 0;
}

ParameterInfoHandle CHAOS_RUNTIME_ABI_CALL MethodGetParameter(
    MethodInfoHandle method,
    CHAOS_IL2CPP_UINT32 parameter_index) {
    if (method == 0) {
        return 0;
    }

    if (const auto* reflection_method = TryDecodeReflectionQueryMethodHandle(method)) {
        const auto* parameter = FindReflectionQueryParameter(reflection_method, parameter_index);
        return EncodeReflectionQueryParameterHandle(parameter);
    }

    return 0;
}

GenericContextHandle CHAOS_RUNTIME_ABI_CALL MethodGetGenericContext(MethodInfoHandle method) {
    if (method == 0) {
        return nullptr;
    }
    const CHAOS_IL2CPP_UINT32 method_token = static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_UINTPTR>(method));
    return chaos::il2cpp::generic_context::GetGenericContextForMethod(method_token);
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL GenericContextGetClassArgCount(GenericContextHandle generic_context) {
    return chaos::il2cpp::generic_context::GetClassTypeArgCount(generic_context);
}

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL GenericContextGetClassArg(
    GenericContextHandle generic_context,
    CHAOS_IL2CPP_UINT32 index) {
    return chaos::il2cpp::generic_context::GetClassTypeArg(generic_context, index);
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL GenericContextGetMethodArgCount(GenericContextHandle generic_context) {
    return chaos::il2cpp::generic_context::GetMethodTypeArgCount(generic_context);
}

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL GenericContextGetMethodArg(
    GenericContextHandle generic_context,
    CHAOS_IL2CPP_UINT32 index) {
    return chaos::il2cpp::generic_context::GetMethodTypeArg(generic_context, index);
}

const RuntimeAbiV0 kRuntimeAbiV0 = {
    CHAOS_RUNTIME_ABI_V0,
    sizeof(RuntimeAbiV0),
    &RuntimeInit,
    &RuntimeShutdown,
    &ThreadAttach,
    &ThreadDetach,
    &ObjectNew,
    &ArrayNew,
    &StringNewUtf8,
    &ClassInit,
    &GcHandleNew,
    &GcHandleFree,
    &RaiseManagedException,
    &FieldGetValue,
    &FieldSetValue,
    &MethodInvoke,
    &AssemblyGetImage,
    &ImageFindType,
    &TypeFindMethod,
    &TypeFindField,
    &TypeFindProperty,
    &TypeFindEvent,
    &TypeGetGenericTypeDefinition,
    &TypeQueryCapabilityImpl,
    &MethodGetParameter,
    &MethodGetGenericContext,
    &GenericContextGetClassArgCount,
    &GenericContextGetClassArg,
    &GenericContextGetMethodArgCount,
    &GenericContextGetMethodArg,
};

}  // namespace

const RuntimeAbiV0* GetRuntimeAbiV0() {
    return &kRuntimeAbiV0;
}

const MarshalPlatformAbiRootV1* GetMarshalPlatformAbiRootV1() {
    return &kMarshalPlatformAbiRootV1;
}

const TaskRuntimeKernelV1* GetTaskRuntimeKernelV1() {
    return &kTaskRuntimeKernelV1;
}

RuntimeStatus TypeQueryCapability(
    TypeInfoHandle type,
    RuntimeTypeCapabilityInfoV0* out_capability_info) {
    return TypeQueryCapabilityImpl(type, out_capability_info);
}

RuntimeMode GetRuntimeMode() {
    return g_runtime_mode.load(CHAOS_IL2CPP_MEMORY_ORDER_ACQUIRE);
}

void SetRuntimeMode(RuntimeMode mode) {
    g_runtime_mode.store(mode, CHAOS_IL2CPP_MEMORY_ORDER_RELEASE);
}

bool IsMixedMode() {
    return GetRuntimeMode() == RuntimeMode::Mixed;
}

void* BoxValueObject(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle value_type,
    const void* value,
    CHAOS_IL2CPP_SIZE value_size) {
    if (!IsAttached(runtime_state, thread_state)
        || value_type == 0
        || value == nullptr
        || value_size == 0u) {
        return nullptr;
    }

    const CHAOS_IL2CPP_SIZE allocation_size = sizeof(BoxedValueHeader) + value_size;
    // Value data contains no pointers; use atomic allocation so GC skips scanning it.
    unsigned char* storage = static_cast<unsigned char*>(AllocateBytesAtomic(allocation_size));
    if (storage == nullptr) {
        return nullptr;
    }

    auto* header = reinterpret_cast<BoxedValueHeader*>(storage);
    header->type = value_type;
    header->byte_count = value_size;
    CHAOS_IL2CPP_MEMCPY(storage + sizeof(BoxedValueHeader), value, value_size);
    return header;
}

RuntimeStatus UnboxValueObject(
    RuntimeState* runtime_state,
    void* boxed_object,
    void* out_value,
    CHAOS_IL2CPP_SIZE out_value_size) {
    if (runtime_state == nullptr || boxed_object == nullptr || out_value == nullptr || out_value_size == 0u) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    const auto* header = reinterpret_cast<const BoxedValueHeader*>(boxed_object);
    if (header->byte_count < out_value_size) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    CHAOS_IL2CPP_MEMCPY(out_value, reinterpret_cast<const unsigned char*>(boxed_object) + sizeof(BoxedValueHeader), out_value_size);
    return CHAOS_RUNTIME_STATUS_OK;
}

bool ArrayStoreReference(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR index,
    void* value) {
    if (array_instance == nullptr) {
        return false;
    }

    auto* header = reinterpret_cast<ArrayHeader*>(array_instance);
    if (index >= header->length) {
        return false;
    }

    auto* elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(array_instance) + sizeof(ArrayHeader));
    elements[index] = value;
    return true;
}

void* ArrayLoadReference(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR index) {
    if (array_instance == nullptr) {
        return nullptr;
    }

    auto* header = reinterpret_cast<ArrayHeader*>(array_instance);
    if (index >= header->length) {
        return nullptr;
    }

    auto* elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(array_instance) + sizeof(ArrayHeader));
    return elements[index];
}

bool ArrayCopyReferenceRange(
    void* source_array_instance,
    CHAOS_IL2CPP_UINTPTR source_index,
    void* target_array_instance,
    CHAOS_IL2CPP_UINTPTR target_index,
    CHAOS_IL2CPP_UINTPTR length) {
    if (source_array_instance == nullptr || target_array_instance == nullptr) {
        return false;
    }

    auto* source_header = reinterpret_cast<ArrayHeader*>(source_array_instance);
    auto* target_header = reinterpret_cast<ArrayHeader*>(target_array_instance);
    if (source_header->element_type == 0 ||
        target_header->element_type == 0 ||
        source_header->element_type != target_header->element_type) {
        return false;
    }

    if (source_index > source_header->length ||
        target_index > target_header->length ||
        length > (source_header->length - source_index) ||
        length > (target_header->length - target_index)) {
        return false;
    }

    if (length == 0u) {
        return true;
    }

    auto* source_elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(source_array_instance) + sizeof(ArrayHeader));
    auto* target_elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(target_array_instance) + sizeof(ArrayHeader));
    CHAOS_IL2CPP_MEMMOVE(
        target_elements + target_index,
        source_elements + source_index,
        static_cast<CHAOS_IL2CPP_SIZE>(length) * sizeof(void*));
    return true;
}

bool ArrayClearReferenceRange(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR start_index,
    CHAOS_IL2CPP_UINTPTR length) {
    if (array_instance == nullptr) {
        return false;
    }

    auto* header = reinterpret_cast<ArrayHeader*>(array_instance);
    if (header->element_type == 0) {
        return false;
    }

    if (start_index > header->length || length > (header->length - start_index)) {
        return false;
    }

    auto* elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(array_instance) + sizeof(ArrayHeader));
    for (CHAOS_IL2CPP_UINTPTR index = 0; index < length; ++index) {
        elements[start_index + index] = nullptr;
    }

    return true;
}

bool ArrayReverseReferenceRange(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR start_index,
    CHAOS_IL2CPP_UINTPTR length) {
    if (array_instance == nullptr) {
        return false;
    }

    auto* header = reinterpret_cast<ArrayHeader*>(array_instance);
    if (header->element_type == 0) {
        return false;
    }

    if (start_index > header->length || length > (header->length - start_index)) {
        return false;
    }

    if (length <= 1u) {
        return true;
    }

    auto* elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(array_instance) + sizeof(ArrayHeader));
    CHAOS_IL2CPP_UINTPTR left = start_index;
    CHAOS_IL2CPP_UINTPTR right = start_index + length - 1u;
    while (left < right) {
        void* temporary = elements[left];
        elements[left] = elements[right];
        elements[right] = temporary;
        ++left;
        --right;
    }

    return true;
}

CHAOS_IL2CPP_INT32 EngineLogWrite(
    const char* category_utf8,
    const char* message_utf8) {
    (void)category_utf8;

    if (message_utf8 == nullptr) {
        return 1;
    }

    CHAOS_IL2CPP_LOG_WRITE_RAW_M("{0}{1}\n", kEngineObservePrefix, message_utf8);
    CHAOS_IL2CPP_LOG_FLUSH_STDOUT();
    return 0;
}

CHAOS_IL2CPP_UINTPTR CreateEngineObjectHandle(void* object_instance) {
    if (object_instance == nullptr) {
        return 0u;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_engine_binding_mutex);
    const CHAOS_IL2CPP_UINTPTR handle = g_next_engine_handle++;
    g_engine_handles[handle] = object_instance;
    return handle;
}

void* ResolveEngineObjectHandle(CHAOS_IL2CPP_UINTPTR handle) {
    if (handle == 0u) {
        return nullptr;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_engine_binding_mutex);
    const auto iterator = g_engine_handles.find(handle);
    return iterator != g_engine_handles.end() ? iterator->second : nullptr;
}

bool RegisterEngineLifecycleCallback(
    const char* phase_utf8,
    EngineLifecycleCallback callback,
    void* user_data) {
    if (phase_utf8 == nullptr || callback == nullptr) {
        return false;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_engine_binding_mutex);
    g_engine_lifecycle_registrations.push_back(
        EngineLifecycleRegistration
        {
            phase_utf8,
            callback,
            user_data,
        });
    return true;
}

bool DispatchEngineLifecycleCallbacks(const char* phase_utf8) {
    if (phase_utf8 == nullptr) {
        return false;
    }

    CHAOS_IL2CPP_VECTOR(EngineLifecycleRegistration) callbacks = {};
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_engine_binding_mutex);
        for (const auto& registration : g_engine_lifecycle_registrations) {
            if (registration.phase == phase_utf8) {
                callbacks.push_back(registration);
            }
        }
    }

    for (const auto& registration : callbacks) {
        registration.callback(phase_utf8, registration.user_data);
    }

    return !callbacks.empty();
}

bool IsMainThreadLane() {
    return CHAOS_IL2CPP_THIS_THREAD_GET_ID() == g_main_thread_id;
}

bool ThreadStaticInt32Add(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const char* slot_key_utf8,
    CHAOS_IL2CPP_INT32 delta,
    CHAOS_IL2CPP_INT32* out_value) {
    if (!IsAttached(runtime_state, thread_state) || slot_key_utf8 == nullptr || out_value == nullptr) {
        return false;
    }

    auto* thread_internal_state = GetThreadInternalState(thread_state);
    if (thread_internal_state == nullptr) {
        return false;
    }

    CHAOS_IL2CPP_INT32& value = thread_internal_state->thread_static_int32_slots[slot_key_utf8];
    value += delta;
    *out_value = value;
    return true;
}

bool MonitorEnter(void* monitor_target) {
    if (monitor_target == nullptr) return false;

    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    const int32_t tid = threading::GetCurrentThreadId();
    if (tid == 0) return false;  // not a managed thread

    const uint64_t tid_bits = static_cast<uint64_t>(tid) << kSyncThreadShift;

    // Fast path: thin lock, uncontended CAS.
    uint64_t sync = *sync_ptr;
    for (;;) {
        if ((sync & kSyncInflatedBit) != 0) {
            // Inflated — forward to SyncBlock.
            auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
            if (sb != nullptr) {
                sb->mutex.lock();
                return true;
            }
            return false;
        }

        if ((sync & kSyncLockedBit) == 0) {
            // Free — try to acquire as thin lock.
            const uint64_t desired = kSyncLockedBit | tid_bits;  // recursion = 0
            if (AtomicCAS(sync_ptr, sync, desired,
                    __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
                return true;
            }
            // CAS failed, reload sync and retry.
            continue;
        }

        // Already locked. Check if we already own it (recursive).
        const int32_t owner_tid = static_cast<int32_t>((sync & ~3ull) >> kSyncThreadShift);
        if (owner_tid == tid) {
            // Recursive enter: increment recursion count.
            const uint64_t recursion = (sync >> kSyncRecursionShift) + 1;
            const uint64_t desired = kSyncLockedBit | tid_bits |
                                     (recursion << kSyncRecursionShift);
            if (AtomicCAS(sync_ptr, sync, desired,
                    __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
                return true;
            }
            continue;
        }

        // Contention: spin, then inflate.
        for (uint32_t spin = 0; spin < kSyncBlockSpinMax; ++spin) {
            CHAOS_SPIN_HINT();
            sync = *sync_ptr;
            if ((sync & kSyncLockedBit) == 0) {
                // Lock was released — retry acquisition.
                const uint64_t desired = kSyncLockedBit | tid_bits;
                if (AtomicCAS(sync_ptr, sync, desired,
                        __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
                    return true;
                }
                break;  // reload sync outside spin loop
            }
            if ((sync & kSyncInflatedBit) != 0) {
                // Another thread inflated during our spin.
                auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
                if (sb != nullptr) {
                    sb->mutex.lock();
                    return true;
                }
                return false;
            }
        }

        // Spin failed — inflate.
        return InflateAndEnter(monitor_target, sync);
    }
}

bool MonitorExit(void* monitor_target) {
    if (monitor_target == nullptr) return false;

    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    const int32_t tid = threading::GetCurrentThreadId();

    uint64_t sync = *sync_ptr;

    if ((sync & kSyncInflatedBit) != 0) {
        // Inflated — unlock via SyncBlock.
        auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
        if (sb != nullptr) {
            sb->mutex.unlock();
            return true;
        }
        return false;
    }

    if ((sync & kSyncLockedBit) == 0) {
        return false;  // not locked
    }

    // Verify we own this thin lock.
    const int32_t owner_tid = static_cast<int32_t>((sync & ~3ull) >> kSyncThreadShift);
    if (owner_tid != tid) {
        return false;  // not the owner
    }

    const uint64_t recursion = sync >> kSyncRecursionShift;
    if (recursion > 0) {
        // Recursive release: decrement.
        const uint64_t desired = kSyncLockedBit |
            (static_cast<uint64_t>(tid) << kSyncThreadShift) |
            ((recursion - 1) << kSyncRecursionShift);
        AtomicStoreRelaxed(sync_ptr, desired);
    } else {
        // Final release.
        AtomicStoreRelease(sync_ptr, 0);
    }
    return true;
}

// --- Monitor condition variable primitives ---

bool MonitorTryEnter(void* monitor_target) {
    if (monitor_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    const int32_t tid = threading::GetCurrentThreadId();
    if (tid == 0) return false;
    uint64_t sync = *sync_ptr;
    for (;;) {
        if ((sync & kSyncInflatedBit) != 0) {
            auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
            return sb != nullptr && sb->mutex.try_lock();
        }
        if ((sync & kSyncLockedBit) == 0) {
            const uint64_t desired = kSyncLockedBit |
                (static_cast<uint64_t>(tid) << kSyncThreadShift);
            if (AtomicCAS(sync_ptr, sync, desired,
                    __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
                return true;
            }
            continue;
        }
        return false;
    }
}

bool MonitorIsEntered(void* monitor_target) {
    if (monitor_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    const int32_t tid = threading::GetCurrentThreadId();
    if (tid == 0) return false;
    uint64_t sync = *sync_ptr;
    if ((sync & kSyncInflatedBit) != 0) {
        return sync != 0;
    }
    if ((sync & kSyncLockedBit) != 0) {
        const uint64_t stored_tid = (sync >> kSyncThreadShift) & 0x3FFFFFFF;
        return stored_tid == static_cast<uint64_t>(tid);
    }
    return false;
}

bool MonitorWait(void* monitor_target, int32_t timeout_ms) {
    if (monitor_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    uint64_t sync = *sync_ptr;
    SyncBlock* sb = nullptr;
    if ((sync & kSyncInflatedBit) != 0) {
        sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
    } else {
        if ((sync & kSyncLockedBit) == 0) return false;
        sb = new SyncBlock();
        const uint32_t stripe_idx = SyncBlockStripeIndex(monitor_target);
        auto& stripe = g_sync_block_stripes[stripe_idx];
        {
            CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(stripe.table_lock);
            stripe.entries[monitor_target] = sb;
            const uint64_t inflated_val = kSyncInflatedBit | reinterpret_cast<uint64_t>(sb);
            AtomicStoreRelease(sync_ptr, inflated_val);
        }
    }
    if (sb == nullptr) return false;
    if (timeout_ms < 0) {
        sb->cond.wait(sb->mutex);
        return true;
    }
    return sb->cond.wait_for(sb->mutex,
        std::chrono::milliseconds(timeout_ms)) == std::cv_status::no_timeout;
}

bool MonitorPulse(void* monitor_target) {
    if (monitor_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    uint64_t sync = *sync_ptr;
    if ((sync & kSyncInflatedBit) != 0) {
        auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
        if (sb != nullptr) {
            sb->cond.notify_one();
            return true;
        }
    }
    return false;
}

bool MonitorPulseAll(void* monitor_target) {
    if (monitor_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(monitor_target);
    uint64_t sync = *sync_ptr;
    if ((sync & kSyncInflatedBit) != 0) {
        auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
        if (sb != nullptr) {
            sb->cond.notify_all();
            return true;
        }
    }
    return false;
}

bool ThreadSleep(int32_t timeout_ms) {
    if (timeout_ms < 0) return false;
    auto* thread = threading::tls_this_thread;
    if (thread != nullptr && thread->pending_abort.load(std::memory_order_acquire)) {
        return false;
    }
    GC_TRANSITION_TO_PREEMPTIVE();
    if (timeout_ms == 0) {
        std::this_thread::yield();
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
    }
    GC_TRANSITION_TO_COOPERATIVE();
    return true;
}

// --- Missing sync primitives (Scheme C) ---

bool SpinLockExit(void* spinlock_target) {
    if (spinlock_target == nullptr) return false;
    auto* sync_ptr = GetSyncStatePtr(spinlock_target);
    const int32_t tid = threading::GetCurrentThreadId();
    uint64_t sync = *sync_ptr;
    if ((sync & kSyncLockedBit) == 0) return false;
    const uint64_t stored_tid = (sync >> kSyncThreadShift) & 0x3FFFFFFF;
    if (stored_tid != static_cast<uint64_t>(tid)) return false;
    AtomicStoreRelease(sync_ptr, 0);
    return true;
}

bool SpinLockIsHeld(void* spinlock_target) {
    if (spinlock_target == nullptr) return false;
    return (*GetSyncStatePtr(spinlock_target) & kSyncLockedBit) != 0;
}

bool LockEnter(void* lock_target) {
    return MonitorEnter(lock_target);
}

bool LockExit(void* lock_target) {
    return MonitorExit(lock_target);
}

bool WaitHandleSet(void* /*wait_handle*/) {
    // V1: stub — WaitHandle integration not yet complete.
    return false;
}

bool WaitHandleReset(void* /*wait_handle*/) {
    // V1: stub — WaitHandle integration not yet complete.
    return false;
}

bool GcSafepoint(
    RuntimeState* runtime_state,
    ThreadState* thread_state) {
    if (!IsAttached(runtime_state, thread_state)) {
        return false;
    }

    auto* thread_internal_state = GetThreadInternalState(thread_state);
    if (thread_internal_state == nullptr) {
        return false;
    }

    thread_internal_state->at_gc_safepoint = true;
    // Perform a small slice of incremental GC work.
    // This distributes GC pauses across frames rather than causing a single large pause.
    GC_collect_a_little();
    thread_internal_state->at_gc_safepoint = false;
    return true;
}

CHAOS_IL2CPP_SIZE ReportThreadRoot(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* root_address,
    CHAOS_IL2CPP_SIZE root_size) {
    if (!IsAttached(runtime_state, thread_state) || root_address == nullptr || root_size == 0u) {
        return 0u;
    }

    auto* thread_internal_state = GetThreadInternalState(thread_state);
    if (thread_internal_state == nullptr) {
        return 0u;
    }

    thread_internal_state->reported_roots.push_back(
        ThreadRootRecord
        {
            root_address,
            root_size,
        });
    return thread_internal_state->reported_roots.size();
}

bool EnqueueFinalizer(
    RuntimeState* runtime_state,
    void* object_instance,
    FinalizerCallback finalizer) {
    auto* runtime_internal_state = GetRuntimeInternalState(runtime_state);
    if (runtime_internal_state == nullptr || object_instance == nullptr || finalizer == nullptr) {
        return false;
    }

    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(runtime_internal_state->finalizer_queue_mutex);
        runtime_internal_state->finalizer_queue.push_back(
            FinalizerWorkItem
            {
                object_instance,
                finalizer,
            });
    }

    // Also register with BDWGC so it can invoke the finalizer automatically
    // when the object becomes unreachable during a GC cycle.
    GC_register_finalizer_no_order(
        object_instance,
        [](void* obj, void* client_data) {
            auto* cb = reinterpret_cast<FinalizerCallback>(client_data);
            if (cb) { cb(obj); }
        },
        reinterpret_cast<void*>(finalizer),
        nullptr, nullptr);

    return true;
}

CHAOS_IL2CPP_SIZE DrainFinalizerQueue(RuntimeState* runtime_state) {
    auto* runtime_internal_state = GetRuntimeInternalState(runtime_state);
    if (runtime_internal_state == nullptr) {
        return 0u;
    }

    // Flush any GC-triggered finalizers first.
    GC_invoke_finalizers();

    CHAOS_IL2CPP_VECTOR(FinalizerWorkItem) pending_finalizers = {};
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(runtime_internal_state->finalizer_queue_mutex);
        pending_finalizers.swap(runtime_internal_state->finalizer_queue);
    }

    for (const auto& work_item : pending_finalizers) {
        work_item.finalizer(work_item.object_instance);
    }

    return pending_finalizers.size();
}

CHAOS_IL2CPP_INTPTR MarshalAllocHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR size) {
    return runtime_state == nullptr || size < 0 ? 0 : AllocateMarshalBlock(runtime_state, static_cast<CHAOS_IL2CPP_SIZE>(size), MarshalAllocationKind::HGlobal);
}

CHAOS_IL2CPP_INTPTR MarshalAllocCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INT32 size) {
    return runtime_state == nullptr || size < 0 ? 0 : AllocateMarshalBlock(runtime_state, static_cast<CHAOS_IL2CPP_SIZE>(size), MarshalAllocationKind::CoTaskMem);
}

CHAOS_IL2CPP_INTPTR MarshalReAllocHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, CHAOS_IL2CPP_INTPTR size) {
    return runtime_state == nullptr || size < 0 ? 0 : ReallocateMarshalBlock(runtime_state, memory, static_cast<CHAOS_IL2CPP_SIZE>(size), MarshalAllocationKind::HGlobal);
}

CHAOS_IL2CPP_INTPTR MarshalReAllocCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, CHAOS_IL2CPP_INT32 size) {
    return runtime_state == nullptr || size < 0 ? 0 : ReallocateMarshalBlock(runtime_state, memory, static_cast<CHAOS_IL2CPP_SIZE>(size), MarshalAllocationKind::CoTaskMem);
}

bool MarshalFreeHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory) {
    return runtime_state == nullptr ? false : FreeMarshalBlock(runtime_state, memory, MarshalAllocationKind::HGlobal, false);
}

bool MarshalFreeCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory) {
    return runtime_state == nullptr ? false : FreeMarshalBlock(runtime_state, memory, MarshalAllocationKind::CoTaskMem, false);
}

bool MarshalZeroFreeCoTaskMemUtf8(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory) {
    return runtime_state == nullptr ? false : FreeMarshalBlock(runtime_state, memory, MarshalAllocationKind::CoTaskMem, true);
}

void* MarshalPtrToStringUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR utf8_buffer,
    CHAOS_IL2CPP_INT32 length,
    bool has_explicit_length) {
    if (!IsAttached(runtime_state, thread_state) || utf8_buffer == 0) {
        return nullptr;
    }

    const char* utf8_bytes = reinterpret_cast<const char*>(GetMarshalAllocationStorage(utf8_buffer));
    CHAOS_IL2CPP_SIZE byte_count = 0u;
    if (has_explicit_length) {
        if (length < 0) {
            return nullptr;
        }

        byte_count = static_cast<CHAOS_IL2CPP_SIZE>(length);
    } else {
        byte_count = CHAOS_IL2CPP_STRLEN(utf8_bytes);
    }

    return StringNewUtf8(runtime_state, thread_state, utf8_bytes, static_cast<CHAOS_IL2CPP_UINTPTR>(byte_count));
}

CHAOS_IL2CPP_INTPTR MarshalStringToCoTaskMemUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* managed_string) {
    if (!IsAttached(runtime_state, thread_state) || managed_string == nullptr) {
        return 0;
    }

    auto* string_header = static_cast<StringObjectHeader*>(managed_string);
    const auto byte_count = static_cast<CHAOS_IL2CPP_SIZE>(string_header->byte_count);
    auto memory = AllocateMarshalBlock(runtime_state, byte_count + 1u, MarshalAllocationKind::CoTaskMem);
    if (memory == 0) {
        return 0;
    }

    auto* target = reinterpret_cast<char*>(GetMarshalAllocationStorage(memory));
    const char* source = reinterpret_cast<const char*>(string_header + 1);
    if (byte_count != 0u) {
        CHAOS_IL2CPP_MEMCPY(target, source, byte_count);
    }

    target[byte_count] = '\0';
    return memory;
}

CHAOS_IL2CPP_UINT8 MarshalReadByte(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset) {
    return MarshalReadValue<CHAOS_IL2CPP_UINT8>(address, offset);
}

CHAOS_IL2CPP_INT16 MarshalReadInt16(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset) {
    return MarshalReadValue<CHAOS_IL2CPP_INT16>(address, offset);
}

CHAOS_IL2CPP_INT32 MarshalReadInt32(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset) {
    return MarshalReadValue<CHAOS_IL2CPP_INT32>(address, offset);
}

CHAOS_IL2CPP_INT64 MarshalReadInt64(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset) {
    return MarshalReadValue<CHAOS_IL2CPP_INT64>(address, offset);
}

CHAOS_IL2CPP_INTPTR MarshalReadIntPtr(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset) {
    return MarshalReadValue<CHAOS_IL2CPP_INTPTR>(address, offset);
}

void MarshalWriteByte(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_UINT8 value) {
    MarshalWriteValue(address, offset, value);
}

void MarshalWriteInt16(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT16 value) {
    MarshalWriteValue(address, offset, value);
}

void MarshalWriteInt32(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 value) {
    MarshalWriteValue(address, offset, value);
}

void MarshalWriteInt64(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT64 value) {
    MarshalWriteValue(address, offset, value);
}

void MarshalWriteIntPtr(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INTPTR value) {
    MarshalWriteValue(address, offset, value);
}

CHAOS_IL2CPP_INT32 TaskKernelNewId() {
    return g_next_task_id.fetch_add(1, CHAOS_IL2CPP_MEMORY_ORDER_ACQ_REL);
}

CHAOS_IL2CPP_INT32 InteropKernel32GetLastError() {
#if defined(_WIN32)
    return static_cast<CHAOS_IL2CPP_INT32>(::GetLastError());
#else
    return 0;
#endif
}

CHAOS_IL2CPP_INT32 InteropKernel32GetCurrentProcessId() {
#if defined(_WIN32)
    return static_cast<CHAOS_IL2CPP_INT32>(::GetCurrentProcessId());
#else
    return 0;
#endif
}

CHAOS_IL2CPP_INT32 InteropKernel32GetCurrentThreadId() {
#if defined(_WIN32)
    return static_cast<CHAOS_IL2CPP_INT32>(::GetCurrentThreadId());
#else
    return 0;
#endif
}

CHAOS_IL2CPP_INTPTR InteropKernel32GetCurrentProcess() {
#if defined(_WIN32)
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(::GetCurrentProcess());
#else
    return 0;
#endif
}

CHAOS_IL2CPP_INTPTR InteropKernel32GetCurrentThread() {
#if defined(_WIN32)
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(::GetCurrentThread());
#else
    return 0;
#endif
}

bool InteropKernel32CloseHandle(CHAOS_IL2CPP_INTPTR handle) {
#if defined(_WIN32)
    return handle == 0 || ::CloseHandle(reinterpret_cast<HANDLE>(handle)) != 0;
#else
    (void)handle;
    return false;
#endif
}

bool InteropKernel32FreeLibrary(CHAOS_IL2CPP_INTPTR handle) {
#if defined(_WIN32)
    return handle == 0 || ::FreeLibrary(reinterpret_cast<HMODULE>(handle)) != 0;
#else
    (void)handle;
    return false;
#endif
}

// ── String marshalling helpers (P/Invoke) ────────────────────────────
// Managed strings are stored as UTF-8 (StringObjectHeader + data + NUL).
// For CharSet.Unicode P/Invoke we must convert to/from UTF-16.
// For CharSet.Ansi the string is already UTF-8 — just copy.

// stub_string_data is provided by runtime_stubs/stub_common.h (included via generated_code_compat.h).

CHAOS_IL2CPP_INT32 MarshalStringToWide(void* managed_string, CHAOS_IL2CPP_UINT16* wide_buf, CHAOS_IL2CPP_INT32 buf_capacity) {
    if (managed_string == nullptr) return 0;
    auto* hdr = static_cast<StringObjectHeader*>(managed_string);
    const char* utf8 = stub_string_data(hdr);
    const auto byte_len = static_cast<int>(hdr->byte_count);

#if defined(_WIN32)
    int needed = ::MultiByteToWideChar(CP_UTF8, 0, utf8, byte_len, nullptr, 0);
    if (needed <= 0) return 0;
    if (wide_buf == nullptr || buf_capacity <= 0) return needed;
    int written = ::MultiByteToWideChar(CP_UTF8, 0, utf8, byte_len, reinterpret_cast<wchar_t*>(wide_buf), buf_capacity);
    return written > 0 ? written : 0;
#else
    // Manual UTF-8 → UTF-16 conversion.
    int needed = 0;
    const auto* in = reinterpret_cast<const unsigned char*>(utf8);
    const auto* end = in + byte_len;
    while (in < end) {
        uint32_t cp;
        if ((*in & 0x80) == 0) { cp = *in++; }
        else if ((*in & 0xE0) == 0xC0 && in + 1 < end) { cp = (*in & 0x1F) << 6 | (in[1] & 0x3F); in += 2; }
        else if ((*in & 0xF0) == 0xE0 && in + 2 < end) { cp = (*in & 0x0F) << 12 | (in[1] & 0x3F) << 6 | (in[2] & 0x3F); in += 3; }
        else if ((*in & 0xF8) == 0xF0 && in + 3 < end) { cp = (*in & 0x07) << 18 | (in[1] & 0x3F) << 12 | (in[2] & 0x3F) << 6 | (in[3] & 0x3F); in += 4; }
        else { cp = 0xFFFD; ++in; }
        if (cp < 0x10000) {
            ++needed;
        } else {
            needed += 2;  // surrogate pair
        }
    }
    if (wide_buf == nullptr || buf_capacity <= 0) return needed;

    int written = 0;
    in = reinterpret_cast<const unsigned char*>(utf8);
    while (in < end && written < buf_capacity) {
        uint32_t cp;
        if ((*in & 0x80) == 0) { cp = *in++; }
        else if ((*in & 0xE0) == 0xC0 && in + 1 < end) { cp = (*in & 0x1F) << 6 | (in[1] & 0x3F); in += 2; }
        else if ((*in & 0xF0) == 0xE0 && in + 2 < end) { cp = (*in & 0x0F) << 12 | (in[1] & 0x3F) << 6 | (in[2] & 0x3F); in += 3; }
        else if ((*in & 0xF8) == 0xF0 && in + 3 < end) { cp = (*in & 0x07) << 18 | (in[1] & 0x3F) << 12 | (in[2] & 0x3F) << 6 | (in[3] & 0x3F); in += 4; }
        else { cp = 0xFFFD; ++in; }
        if (cp < 0x10000) {
            wide_buf[written++] = static_cast<CHAOS_IL2CPP_UINT16>(cp);
        } else if (written + 1 < buf_capacity) {
            cp -= 0x10000;
            wide_buf[written++] = static_cast<CHAOS_IL2CPP_UINT16>(0xD800 | (cp >> 10));
            wide_buf[written++] = static_cast<CHAOS_IL2CPP_UINT16>(0xDC00 | (cp & 0x3FF));
        }
    }
    return written;
#endif
}

void* MarshalWideToString(RuntimeState* runtime_state, ThreadState* thread_state, const CHAOS_IL2CPP_UINT16* wide_buf, CHAOS_IL2CPP_INT32 wide_len) {
    if (!IsAttached(runtime_state, thread_state) || wide_buf == nullptr || wide_len <= 0) {
        return nullptr;
    }

#if defined(_WIN32)
    int needed = ::WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t*>(wide_buf), wide_len, nullptr, 0, nullptr, nullptr);
    if (needed <= 0) return nullptr;

    auto* temp = static_cast<char*>(CHAOS_IL2CPP_MALLOC(static_cast<CHAOS_IL2CPP_SIZE>(needed) + 1u));
    if (temp == nullptr) return nullptr;
    ::WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t*>(wide_buf), wide_len, temp, needed, nullptr, nullptr);
    temp[needed] = '\0';
#else
    // Manual UTF-16 → UTF-8
    CHAOS_IL2CPP_SIZE needed = 0;
    for (int i = 0; i < wide_len; ++i) {
        uint32_t cp = wide_buf[i];
        if (cp >= 0xD800 && cp <= 0xDBFF && i + 1 < wide_len) {
            cp = 0x10000 + ((cp - 0xD800) << 10) + (wide_buf[++i] - 0xDC00);
        }
        needed += (cp < 0x80) ? 1 : (cp < 0x800) ? 2 : (cp < 0x10000) ? 3 : 4;
    }
    auto* temp = static_cast<char*>(CHAOS_IL2CPP_MALLOC(needed + 1u));
    if (temp == nullptr) return nullptr;
    CHAOS_IL2CPP_SIZE pos = 0;
    for (int i = 0; i < wide_len; ++i) {
        uint32_t cp = wide_buf[i];
        if (cp >= 0xD800 && cp <= 0xDBFF && i + 1 < wide_len) {
            cp = 0x10000 + ((cp - 0xD800) << 10) + (wide_buf[++i] - 0xDC00);
        }
        if (cp < 0x80) { temp[pos++] = static_cast<char>(cp); }
        else if (cp < 0x800) { temp[pos++] = static_cast<char>(0xC0 | (cp >> 6)); temp[pos++] = static_cast<char>(0x80 | (cp & 0x3F)); }
        else if (cp < 0x10000) { temp[pos++] = static_cast<char>(0xE0 | (cp >> 12)); temp[pos++] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F)); temp[pos++] = static_cast<char>(0x80 | (cp & 0x3F)); }
        else { temp[pos++] = static_cast<char>(0xF0 | (cp >> 18)); temp[pos++] = static_cast<char>(0x80 | ((cp >> 12) & 0x3F)); temp[pos++] = static_cast<char>(0x80 | ((cp >> 6) & 0x3F)); temp[pos++] = static_cast<char>(0x80 | (cp & 0x3F)); }
    }
    temp[pos] = '\0';
#endif

    void* result = StringNewUtf8(runtime_state, thread_state, temp, static_cast<CHAOS_IL2CPP_UINTPTR>(needed));
    CHAOS_IL2CPP_FREE(temp);
    return result;
}

void* MarshalAnsiToString(RuntimeState* runtime_state, ThreadState* thread_state, const char* ansi_buf, CHAOS_IL2CPP_INT32 byte_len) {
    // Managed strings are UTF-8. On Windows, "ANSI" means the system
    // active code page (typically Windows-1252). On POSIX, assume
    // the buffer is already UTF-8.
    if (!IsAttached(runtime_state, thread_state) || ansi_buf == nullptr || byte_len <= 0) {
        return nullptr;
    }

#if defined(_WIN32)
    // Convert from system code page to UTF-16 first, then UTF-16 to UTF-8.
    int wide_needed = ::MultiByteToWideChar(CP_ACP, 0, ansi_buf, byte_len, nullptr, 0);
    if (wide_needed <= 0) return nullptr;
    auto* wide = static_cast<wchar_t*>(CHAOS_IL2CPP_MALLOC(static_cast<CHAOS_IL2CPP_SIZE>(wide_needed) * sizeof(wchar_t)));
    if (wide == nullptr) return nullptr;
    ::MultiByteToWideChar(CP_ACP, 0, ansi_buf, byte_len, wide, wide_needed);

    int utf8_needed = ::WideCharToMultiByte(CP_UTF8, 0, wide, wide_needed, nullptr, 0, nullptr, nullptr);
    if (utf8_needed <= 0) { CHAOS_IL2CPP_FREE(wide); return nullptr; }
    auto* utf8 = static_cast<char*>(CHAOS_IL2CPP_MALLOC(static_cast<CHAOS_IL2CPP_SIZE>(utf8_needed) + 1u));
    if (utf8 == nullptr) { CHAOS_IL2CPP_FREE(wide); return nullptr; }
    ::WideCharToMultiByte(CP_UTF8, 0, wide, wide_needed, utf8, utf8_needed, nullptr, nullptr);
    utf8[utf8_needed] = '\0';
    CHAOS_IL2CPP_FREE(wide);

    void* result = StringNewUtf8(runtime_state, thread_state, utf8, static_cast<CHAOS_IL2CPP_UINTPTR>(utf8_needed));
    CHAOS_IL2CPP_FREE(utf8);
    return result;
#else
    // Assume already UTF-8.
    return StringNewUtf8(runtime_state, thread_state, ansi_buf, static_cast<CHAOS_IL2CPP_UINTPTR>(byte_len));
#endif
}

ValueTypeKernelBackendKind GetCharClassificationKernelBackend() {
    return kCharClassificationKernelBackend;
}

ValueTypeKernelBackendKind GetFloatingClassificationKernelBackend() {
    return kFloatingClassificationKernelBackend;
}

ValueTypeKernelBackendKind GetBitArithmeticKernelBackend() {
    return kBitArithmeticKernelBackend;
}

ValueTypeKernelBackendKind GetTickArithmeticKernelBackend() {
    return kTickArithmeticKernelBackend;
}

VectorKernelBackendKind GetVectorKernelBackend() {
    return kVectorKernelBackend;
}

static bool CharIsAsciiGeneric(CHAOS_IL2CPP_UINT16 value) {
    return value <= 0x7Fu;
}

static bool CharIsAsciiIntrinsicImpl(CHAOS_IL2CPP_UINT16 value) {
    return (value & 0xFF80u) == 0u;
}

bool CharIsAscii(CHAOS_IL2CPP_UINT16 value) {
    return kCharClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? CharIsAsciiIntrinsicImpl(value)
        : CharIsAsciiGeneric(value);
}

bool CharIsAsciiDigit(CHAOS_IL2CPP_UINT16 value) {
    return static_cast<CHAOS_IL2CPP_UINT16>(value - static_cast<CHAOS_IL2CPP_UINT16>('0')) <= 9u;
}

bool CharIsAsciiHexDigitLower(CHAOS_IL2CPP_UINT16 value) {
    return static_cast<CHAOS_IL2CPP_UINT16>(value - static_cast<CHAOS_IL2CPP_UINT16>('a')) <= 5u;
}

bool CharIsAsciiHexDigitUpper(CHAOS_IL2CPP_UINT16 value) {
    return static_cast<CHAOS_IL2CPP_UINT16>(value - static_cast<CHAOS_IL2CPP_UINT16>('A')) <= 5u;
}

bool CharIsAsciiHexDigit(CHAOS_IL2CPP_UINT16 value) {
    return CharIsAsciiDigit(value) || CharIsAsciiHexDigitLower(value) || CharIsAsciiHexDigitUpper(value);
}

bool CharIsAsciiLetterLower(CHAOS_IL2CPP_UINT16 value) {
    return static_cast<CHAOS_IL2CPP_UINT16>(value - static_cast<CHAOS_IL2CPP_UINT16>('a')) <= 25u;
}

bool CharIsAsciiLetterUpper(CHAOS_IL2CPP_UINT16 value) {
    return static_cast<CHAOS_IL2CPP_UINT16>(value - static_cast<CHAOS_IL2CPP_UINT16>('A')) <= 25u;
}

bool CharIsAsciiLetter(CHAOS_IL2CPP_UINT16 value) {
    return CharIsAsciiLetterLower(value) || CharIsAsciiLetterUpper(value);
}

bool CharIsAsciiLetterOrDigit(CHAOS_IL2CPP_UINT16 value) {
    return CharIsAsciiLetter(value) || CharIsAsciiDigit(value);
}

bool CharIsBetween(CHAOS_IL2CPP_UINT16 value, CHAOS_IL2CPP_UINT16 lower_bound, CHAOS_IL2CPP_UINT16 upper_bound) {
    return value >= lower_bound && value <= upper_bound;
}

static bool CharIsLatin1Generic(CHAOS_IL2CPP_UINT16 value) {
    return value <= 0xFFu;
}

static bool CharIsLatin1IntrinsicImpl(CHAOS_IL2CPP_UINT16 value) {
    return (value & 0xFF00u) == 0u;
}

bool CharIsLatin1(CHAOS_IL2CPP_UINT16 value) {
    return kCharClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? CharIsLatin1IntrinsicImpl(value)
        : CharIsLatin1Generic(value);
}

bool CharIsHighSurrogate(CHAOS_IL2CPP_UINT16 value) {
    return CharIsBetween(value, 0xD800u, 0xDBFFu);
}

bool CharIsLowSurrogate(CHAOS_IL2CPP_UINT16 value) {
    return CharIsBetween(value, 0xDC00u, 0xDFFFu);
}

bool CharIsSeparatorLatin1(CHAOS_IL2CPP_UINT16 value) {
    return value == 0x20u || value == 0xA0u;
}

bool CharIsSurrogate(CHAOS_IL2CPP_UINT16 value) {
    return CharIsBetween(value, 0xD800u, 0xDFFFu);
}

bool CharIsSurrogatePair(CHAOS_IL2CPP_UINT16 high_surrogate, CHAOS_IL2CPP_UINT16 low_surrogate) {
    return CharIsHighSurrogate(high_surrogate) && CharIsLowSurrogate(low_surrogate);
}

bool CharIsWhiteSpaceLatin1(CHAOS_IL2CPP_UINT16 value) {
    return value == 0x20u ||
           static_cast<CHAOS_IL2CPP_UINT16>(value - 0x09u) <= static_cast<CHAOS_IL2CPP_UINT16>(0x0Du - 0x09u) ||
           value == 0x85u ||
           value == 0xA0u;
}

CHAOS_IL2CPP_INT32 CharCompare(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    if (left_value < right_value) {
        return -1;
    }

    if (left_value > right_value) {
        return 1;
    }

    return 0;
}

bool CharEquals(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    return left_value == right_value;
}

static CHAOS_IL2CPP_UINT32 BitCastSingleToUInt32(float value) {
    CHAOS_IL2CPP_UINT32 bits = 0u;
    CHAOS_IL2CPP_MEMCPY(&bits, &value, sizeof(bits));
    return bits;
}

static CHAOS_IL2CPP_UINT64 BitCastDoubleToUInt64(double value) {
    CHAOS_IL2CPP_UINT64 bits = 0u;
    CHAOS_IL2CPP_MEMCPY(&bits, &value, sizeof(bits));
    return bits;
}

constexpr CHAOS_IL2CPP_UINT32 kHashCodePrime1 = 2654435761u;
constexpr CHAOS_IL2CPP_UINT32 kHashCodePrime2 = 2246822519u;
constexpr CHAOS_IL2CPP_UINT32 kHashCodePrime3 = 3266489917u;
constexpr CHAOS_IL2CPP_UINT32 kHashCodePrime4 = 668265263u;
constexpr CHAOS_IL2CPP_UINT32 kHashCodePrime5 = 374761393u;
constexpr CHAOS_IL2CPP_UINT32 kHashCodeSeed = 0u;

static CHAOS_IL2CPP_UINT32 RotateLeft32(CHAOS_IL2CPP_UINT32 value, int offset) {
    return (value << offset) | (value >> (32 - offset));
}

static CHAOS_IL2CPP_UINT32 HashCodeRound(CHAOS_IL2CPP_UINT32 hash, CHAOS_IL2CPP_UINT32 input) {
    return RotateLeft32(hash + input * kHashCodePrime2, 13) * kHashCodePrime1;
}

static CHAOS_IL2CPP_UINT32 HashCodeQueueRound(CHAOS_IL2CPP_UINT32 hash, CHAOS_IL2CPP_UINT32 queued_value) {
    return RotateLeft32(hash + queued_value * kHashCodePrime3, 17) * kHashCodePrime4;
}

static CHAOS_IL2CPP_UINT32 HashCodeMixState(CHAOS_IL2CPP_UINT32 v1, CHAOS_IL2CPP_UINT32 v2, CHAOS_IL2CPP_UINT32 v3, CHAOS_IL2CPP_UINT32 v4) {
    return RotateLeft32(v1, 1) +
           RotateLeft32(v2, 7) +
           RotateLeft32(v3, 12) +
           RotateLeft32(v4, 18);
}

static CHAOS_IL2CPP_UINT32 HashCodeMixEmptyState() {
    return kHashCodeSeed + kHashCodePrime5;
}

static CHAOS_IL2CPP_UINT32 HashCodeMixFinal(CHAOS_IL2CPP_UINT32 hash) {
    hash ^= hash >> 15;
    hash *= kHashCodePrime2;
    hash ^= hash >> 13;
    hash *= kHashCodePrime3;
    hash ^= hash >> 16;
    return hash;
}

static CHAOS_IL2CPP_UINT32 SingleGetHashCodeValue(float value) {
    CHAOS_IL2CPP_UINT32 bits = BitCastSingleToUInt32(value);
    if (SingleIsNaNOrZero(value)) {
        bits &= 0x7F800000u;
    }

    return bits;
}

static CHAOS_IL2CPP_INT32 HashCodeCombine2(CHAOS_IL2CPP_UINT32 hc1, CHAOS_IL2CPP_UINT32 hc2) {
    CHAOS_IL2CPP_UINT32 hash = HashCodeMixEmptyState();
    hash += 8u;
    hash = HashCodeQueueRound(hash, hc1);
    hash = HashCodeQueueRound(hash, hc2);
    return static_cast<CHAOS_IL2CPP_INT32>(HashCodeMixFinal(hash));
}

static CHAOS_IL2CPP_INT32 HashCodeCombine3(CHAOS_IL2CPP_UINT32 hc1, CHAOS_IL2CPP_UINT32 hc2, CHAOS_IL2CPP_UINT32 hc3) {
    CHAOS_IL2CPP_UINT32 hash = HashCodeMixEmptyState();
    hash += 12u;
    hash = HashCodeQueueRound(hash, hc1);
    hash = HashCodeQueueRound(hash, hc2);
    hash = HashCodeQueueRound(hash, hc3);
    return static_cast<CHAOS_IL2CPP_INT32>(HashCodeMixFinal(hash));
}

static CHAOS_IL2CPP_INT32 HashCodeCombine4(CHAOS_IL2CPP_UINT32 hc1, CHAOS_IL2CPP_UINT32 hc2, CHAOS_IL2CPP_UINT32 hc3, CHAOS_IL2CPP_UINT32 hc4) {
    CHAOS_IL2CPP_UINT32 v1 = kHashCodeSeed + kHashCodePrime1 + kHashCodePrime2;
    CHAOS_IL2CPP_UINT32 v2 = kHashCodeSeed + kHashCodePrime2;
    CHAOS_IL2CPP_UINT32 v3 = kHashCodeSeed;
    CHAOS_IL2CPP_UINT32 v4 = kHashCodeSeed - kHashCodePrime1;

    v1 = HashCodeRound(v1, hc1);
    v2 = HashCodeRound(v2, hc2);
    v3 = HashCodeRound(v3, hc3);
    v4 = HashCodeRound(v4, hc4);

    CHAOS_IL2CPP_UINT32 hash = HashCodeMixState(v1, v2, v3, v4);
    hash += 16u;
    return static_cast<CHAOS_IL2CPP_INT32>(HashCodeMixFinal(hash));
}

static float VectorMinElement(float left_value, float right_value) {
    return CHAOS_IL2CPP_FMIN(left_value, right_value);
}

static float VectorMaxElement(float left_value, float right_value) {
    return CHAOS_IL2CPP_FMAX(left_value, right_value);
}

static bool HalfIsFiniteGeneric(CHAOS_IL2CPP_UINT16 value) {
    return (value & 0x7C00u) != 0x7C00u;
}

static bool HalfIsNaNGeneric(CHAOS_IL2CPP_UINT16 value) {
    return (value & 0x7C00u) == 0x7C00u && (value & 0x03FFu) != 0u;
}

static bool HalfIsInfinityGeneric(CHAOS_IL2CPP_UINT16 value) {
    return (value & 0x7FFFu) == 0x7C00u;
}

static float HalfToFloatValue(CHAOS_IL2CPP_UINT16 value);
bool HalfIsInteger(CHAOS_IL2CPP_UINT16 value);

bool HalfIsFinite(CHAOS_IL2CPP_UINT16 value) {
    return HalfIsFiniteGeneric(value);
}

bool HalfIsEvenInteger(CHAOS_IL2CPP_UINT16 value) {
    return HalfIsInteger(value) &&
           CHAOS_IL2CPP_FABS(CHAOS_IL2CPP_FMOD(HalfToFloatValue(value), 2.0f)) == 0.0f;
}

bool HalfIsNaN(CHAOS_IL2CPP_UINT16 value) {
    return HalfIsNaNGeneric(value);
}

bool HalfIsNaNOrZero(CHAOS_IL2CPP_UINT16 value) {
    return (value & 0x7FFFu) == 0u || HalfIsNaN(value);
}

bool HalfIsNegative(CHAOS_IL2CPP_UINT16 value) {
    return (value & 0x8000u) == 0x8000u;
}

bool HalfIsNegativeInfinity(CHAOS_IL2CPP_UINT16 value) {
    return value == 0xFC00u;
}

bool HalfIsNormal(CHAOS_IL2CPP_UINT16 value) {
    const CHAOS_IL2CPP_UINT16 abs_value = static_cast<CHAOS_IL2CPP_UINT16>(value & 0x7FFFu);
    return abs_value < 0x7C00u && abs_value != 0u && (abs_value & 0x7C00u) != 0u;
}

bool HalfIsOddInteger(CHAOS_IL2CPP_UINT16 value) {
    return HalfIsInteger(value) &&
           CHAOS_IL2CPP_FABS(CHAOS_IL2CPP_FMOD(HalfToFloatValue(value), 2.0f)) == 1.0f;
}

bool HalfIsPositive(CHAOS_IL2CPP_UINT16 value) {
    return !HalfIsNegative(value);
}

bool HalfIsPositiveInfinity(CHAOS_IL2CPP_UINT16 value) {
    return value == 0x7C00u;
}

bool HalfIsPow2(CHAOS_IL2CPP_UINT16 value) {
    if (!HalfIsFinite(value)) {
        return false;
    }

    const float float_value = HalfToFloatValue(value);
    if (!(float_value > 0.0f)) {
        return false;
    }

    int exponent = 0;
    return CHAOS_IL2CPP_FREXP(float_value, &exponent) == 0.5f;
}

bool HalfIsRealNumber(CHAOS_IL2CPP_UINT16 value) {
    return !HalfIsNaN(value);
}

bool HalfIsSubnormal(CHAOS_IL2CPP_UINT16 value) {
    const CHAOS_IL2CPP_UINT16 abs_value = static_cast<CHAOS_IL2CPP_UINT16>(value & 0x7FFFu);
    return abs_value != 0u && (abs_value & 0x7C00u) == 0u;
}

bool HalfIsZero(CHAOS_IL2CPP_UINT16 value) {
    return (value & 0x7FFFu) == 0u;
}

bool HalfIsInteger(CHAOS_IL2CPP_UINT16 value) {
    if (!HalfIsFinite(value)) {
        return false;
    }

    const float float_value = HalfToFloatValue(value);
    return CHAOS_IL2CPP_TRUNC(float_value) == float_value;
}

bool HalfIsInfinity(CHAOS_IL2CPP_UINT16 value) {
    return HalfIsInfinityGeneric(value);
}

static float HalfToFloatValue(CHAOS_IL2CPP_UINT16 value) {
    const CHAOS_IL2CPP_UINT16 exponent = static_cast<CHAOS_IL2CPP_UINT16>((value >> 10) & 0x1Fu);
    const CHAOS_IL2CPP_UINT16 mantissa = static_cast<CHAOS_IL2CPP_UINT16>(value & 0x03FFu);
    const bool negative = (value & 0x8000u) != 0u;

    float result = 0.0f;
    if (exponent == 0u) {
        if (mantissa == 0u) {
            result = 0.0f;
        } else {
            result = CHAOS_IL2CPP_LDEXP(static_cast<float>(mantissa), -24);
        }
    } else if (exponent == 0x1Fu) {
        result = mantissa == 0u
            ? CHAOS_IL2CPP_NUMERIC_LIMITS_INFINITY(float)
            : CHAOS_IL2CPP_NUMERIC_LIMITS_QUIET_NAN(float);
    } else {
        result = CHAOS_IL2CPP_LDEXP(1.0f + (static_cast<float>(mantissa) / 1024.0f), static_cast<int>(exponent) - 15);
    }

    return negative ? -result : result;
}

CHAOS_IL2CPP_INT32 HalfCompare(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    const bool left_is_nan = HalfIsNaN(left_value);
    const bool right_is_nan = HalfIsNaN(right_value);
    if (left_is_nan && right_is_nan) {
        return 0;
    }

    if (left_is_nan) {
        return -1;
    }

    if (right_is_nan) {
        return 1;
    }

    const float left = HalfToFloatValue(left_value);
    const float right = HalfToFloatValue(right_value);
    if (left < right) {
        return -1;
    }

    if (left > right) {
        return 1;
    }

    return 0;
}

bool HalfEquals(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    return left_value == right_value ||
           (HalfIsZero(left_value) && HalfIsZero(right_value)) ||
           (HalfIsNaN(left_value) && HalfIsNaN(right_value));
}

static bool HalfAreZero(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    return ((left_value | right_value) & 0x7FFFu) == 0u;
}

bool HalfOperatorEquals(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    return !HalfIsNaN(left_value) &&
           !HalfIsNaN(right_value) &&
           (left_value == right_value || HalfAreZero(left_value, right_value));
}

bool HalfOperatorLessThan(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    if (HalfIsNaN(left_value) || HalfIsNaN(right_value)) {
        return false;
    }

    const bool left_is_negative = HalfIsNegative(left_value);
    if (left_is_negative != HalfIsNegative(right_value)) {
        return left_is_negative && !HalfAreZero(left_value, right_value);
    }

    return left_value != right_value && ((left_value < right_value) ^ left_is_negative);
}

bool HalfOperatorLessThanOrEqual(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    if (HalfIsNaN(left_value) || HalfIsNaN(right_value)) {
        return false;
    }

    const bool left_is_negative = HalfIsNegative(left_value);
    if (left_is_negative != HalfIsNegative(right_value)) {
        return left_is_negative || HalfAreZero(left_value, right_value);
    }

    return left_value == right_value || ((left_value < right_value) ^ left_is_negative);
}

bool HalfOperatorGreaterThan(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    return HalfOperatorLessThan(right_value, left_value);
}

bool HalfOperatorGreaterThanOrEqual(CHAOS_IL2CPP_UINT16 left_value, CHAOS_IL2CPP_UINT16 right_value) {
    return HalfOperatorLessThanOrEqual(right_value, left_value);
}

template <typename T>
static CHAOS_IL2CPP_INT32 FloatingCompareGeneric(T left_value, T right_value) {
    const bool left_is_nan = CHAOS_IL2CPP_ISNAN(left_value);
    const bool right_is_nan = CHAOS_IL2CPP_ISNAN(right_value);
    if (left_is_nan && right_is_nan) {
        return 0;
    }

    if (left_is_nan) {
        return -1;
    }

    if (right_is_nan) {
        return 1;
    }

    if (left_value < right_value) {
        return -1;
    }

    if (left_value > right_value) {
        return 1;
    }

    return 0;
}

template <typename T>
static bool FloatingEqualsGeneric(T left_value, T right_value) {
    return left_value == right_value || (CHAOS_IL2CPP_ISNAN(left_value) && CHAOS_IL2CPP_ISNAN(right_value));
}

template <typename T>
static bool FloatingIsIntegerGeneric(T value) {
    return CHAOS_IL2CPP_ISFINITE(value) && CHAOS_IL2CPP_TRUNC(value) == value;
}

template <typename T>
static bool FloatingIsEvenIntegerGeneric(T value) {
    return FloatingIsIntegerGeneric(value) &&
           CHAOS_IL2CPP_FABS(CHAOS_IL2CPP_FMOD(value, static_cast<T>(2))) == static_cast<T>(0);
}

template <typename T>
static bool FloatingIsOddIntegerGeneric(T value) {
    return FloatingIsIntegerGeneric(value) &&
           CHAOS_IL2CPP_FABS(CHAOS_IL2CPP_FMOD(value, static_cast<T>(2))) == static_cast<T>(1);
}

template <typename T>
static bool FloatingIsPow2Generic(T value) {
    if (!CHAOS_IL2CPP_ISFINITE(value) || !(value > static_cast<T>(0))) {
        return false;
    }

    int exponent = 0;
    return CHAOS_IL2CPP_FREXP(value, &exponent) == static_cast<T>(0.5);
}

static bool SingleIsFiniteGeneric(float value) {
    return CHAOS_IL2CPP_ISFINITE(value);
}

static bool SingleIsFiniteIntrinsicImpl(float value) {
    return (BitCastSingleToUInt32(value) & 0x7F800000u) != 0x7F800000u;
}

bool SingleIsFinite(float value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? SingleIsFiniteIntrinsicImpl(value)
        : SingleIsFiniteGeneric(value);
}

static bool SingleIsNaNGeneric(float value) {
    return CHAOS_IL2CPP_ISNAN(value);
}

static bool SingleIsNaNIntrinsicImpl(float value) {
    const CHAOS_IL2CPP_UINT32 bits = BitCastSingleToUInt32(value);
    return (bits & 0x7F800000u) == 0x7F800000u && (bits & 0x007FFFFFu) != 0u;
}

bool SingleIsNaN(float value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? SingleIsNaNIntrinsicImpl(value)
        : SingleIsNaNGeneric(value);
}

static bool SingleIsInfinityGeneric(float value) {
    return CHAOS_IL2CPP_ISINF(value);
}

static bool SingleIsInfinityIntrinsicImpl(float value) {
    return (BitCastSingleToUInt32(value) & 0x7FFFFFFFu) == 0x7F800000u;
}

bool SingleIsInfinity(float value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? SingleIsInfinityIntrinsicImpl(value)
        : SingleIsInfinityGeneric(value);
}

CHAOS_IL2CPP_INT32 SingleCompare(float left_value, float right_value) {
    return FloatingCompareGeneric(left_value, right_value);
}

bool SingleEquals(float left_value, float right_value) {
    return FloatingEqualsGeneric(left_value, right_value);
}

bool SingleIsEvenInteger(float value) {
    return FloatingIsEvenIntegerGeneric(value);
}

bool SingleIsNaNOrZero(float value) {
    return (BitCastSingleToUInt32(value) & 0x7FFFFFFFu) == 0u || SingleIsNaN(value);
}

bool SingleIsNegative(float value) {
    return (BitCastSingleToUInt32(value) & 0x80000000u) != 0u;
}

bool SingleIsNegativeInfinity(float value) {
    return BitCastSingleToUInt32(value) == 0xFF800000u;
}

bool SingleIsNormal(float value) {
    const CHAOS_IL2CPP_UINT32 bits = BitCastSingleToUInt32(value);
    return (bits & 0x7FFFFFFFu) < 0x7F800000u &&
           (bits & 0x7FFFFFFFu) != 0u &&
           (bits & 0x7F800000u) != 0u;
}

bool SingleIsOddInteger(float value) {
    return FloatingIsOddIntegerGeneric(value);
}

bool SingleIsPositive(float value) {
    return (BitCastSingleToUInt32(value) & 0x80000000u) == 0u;
}

bool SingleIsPositiveInfinity(float value) {
    return BitCastSingleToUInt32(value) == 0x7F800000u;
}

bool SingleIsPow2(float value) {
    return FloatingIsPow2Generic(value);
}

bool SingleIsRealNumber(float value) {
    return !SingleIsNaN(value);
}

bool SingleIsSubnormal(float value) {
    const CHAOS_IL2CPP_UINT32 bits = BitCastSingleToUInt32(value);
    return (bits & 0x7FFFFFFFu) != 0u && (bits & 0x7F800000u) == 0u;
}

bool SingleIsZero(float value) {
    return value == 0.0f;
}

bool SingleIsInteger(float value) {
    return FloatingIsIntegerGeneric(value);
}

static bool DoubleIsFiniteGeneric(double value) {
    return CHAOS_IL2CPP_ISFINITE(value);
}

static bool DoubleIsFiniteIntrinsicImpl(double value) {
    return (BitCastDoubleToUInt64(value) & 0x7FF0000000000000ull) != 0x7FF0000000000000ull;
}

bool DoubleIsFinite(double value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? DoubleIsFiniteIntrinsicImpl(value)
        : DoubleIsFiniteGeneric(value);
}

static bool DoubleIsNaNGeneric(double value) {
    return CHAOS_IL2CPP_ISNAN(value);
}

static bool DoubleIsNaNIntrinsicImpl(double value) {
    const CHAOS_IL2CPP_UINT64 bits = BitCastDoubleToUInt64(value);
    return (bits & 0x7FF0000000000000ull) == 0x7FF0000000000000ull && (bits & 0x000FFFFFFFFFFFFFull) != 0u;
}

static bool DoubleIsInfinityGeneric(double value) {
    return CHAOS_IL2CPP_ISINF(value);
}

static bool DoubleIsInfinityIntrinsicImpl(double value) {
    return (BitCastDoubleToUInt64(value) & 0x7FFFFFFFFFFFFFFFull) == 0x7FF0000000000000ull;
}

bool DoubleIsNaN(double value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? DoubleIsNaNIntrinsicImpl(value)
        : DoubleIsNaNGeneric(value);
}

bool DoubleIsInfinity(double value) {
    return kFloatingClassificationKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? DoubleIsInfinityIntrinsicImpl(value)
        : DoubleIsInfinityGeneric(value);
}

CHAOS_IL2CPP_INT32 DoubleCompare(double left_value, double right_value) {
    return FloatingCompareGeneric(left_value, right_value);
}

bool DoubleEquals(double left_value, double right_value) {
    return FloatingEqualsGeneric(left_value, right_value);
}

bool DoubleIsEvenInteger(double value) {
    return FloatingIsEvenIntegerGeneric(value);
}

bool DoubleIsNaNOrZero(double value) {
    return (BitCastDoubleToUInt64(value) & 0x7FFFFFFFFFFFFFFFull) == 0u || DoubleIsNaN(value);
}

bool DoubleIsNegative(double value) {
    return (BitCastDoubleToUInt64(value) & 0x8000000000000000ull) != 0u;
}

bool DoubleIsNegativeInfinity(double value) {
    return BitCastDoubleToUInt64(value) == 0xFFF0000000000000ull;
}

bool DoubleIsNormal(double value) {
    const CHAOS_IL2CPP_UINT64 bits = BitCastDoubleToUInt64(value);
    return (bits & 0x7FFFFFFFFFFFFFFFull) < 0x7FF0000000000000ull &&
           (bits & 0x7FFFFFFFFFFFFFFFull) != 0u &&
           (bits & 0x7FF0000000000000ull) != 0u;
}

bool DoubleIsOddInteger(double value) {
    return FloatingIsOddIntegerGeneric(value);
}

bool DoubleIsPositive(double value) {
    return (BitCastDoubleToUInt64(value) & 0x8000000000000000ull) == 0u;
}

bool DoubleIsPositiveInfinity(double value) {
    return BitCastDoubleToUInt64(value) == 0x7FF0000000000000ull;
}

bool DoubleIsPow2(double value) {
    return FloatingIsPow2Generic(value);
}

bool DoubleIsRealNumber(double value) {
    return !DoubleIsNaN(value);
}

bool DoubleIsSubnormal(double value) {
    const CHAOS_IL2CPP_UINT64 bits = BitCastDoubleToUInt64(value);
    return (bits & 0x7FFFFFFFFFFFFFFFull) != 0u && (bits & 0x7FF0000000000000ull) == 0u;
}

bool DoubleIsZero(double value) {
    return value == 0.0;
}

bool DoubleIsInteger(double value) {
    return FloatingIsIntegerGeneric(value);
}

// NFloat delegates — each NFloat* function is a 1:1 forward to the
// corresponding Double* function.  Macros avoid 20 nearly-identical
// definitions while keeping the symbol names in the ABI surface.
#define DEFINE_NFLOAT_BOOL_DELEGATE(name) \
    bool NFloat##name(double value) { return Double##name(value); }
#define DEFINE_NFLOAT_CMP_DELEGATE(name) \
    CHAOS_IL2CPP_INT32 NFloat##name(double left, double right) { return Double##name(left, right); }

DEFINE_NFLOAT_BOOL_DELEGATE(IsFinite);
DEFINE_NFLOAT_BOOL_DELEGATE(IsNaN);
DEFINE_NFLOAT_BOOL_DELEGATE(IsInfinity);
DEFINE_NFLOAT_BOOL_DELEGATE(IsEvenInteger);
DEFINE_NFLOAT_BOOL_DELEGATE(IsNegative);
DEFINE_NFLOAT_BOOL_DELEGATE(IsNegativeInfinity);
DEFINE_NFLOAT_BOOL_DELEGATE(IsNormal);
DEFINE_NFLOAT_BOOL_DELEGATE(IsOddInteger);
DEFINE_NFLOAT_BOOL_DELEGATE(IsPositive);
DEFINE_NFLOAT_BOOL_DELEGATE(IsPositiveInfinity);
DEFINE_NFLOAT_BOOL_DELEGATE(IsPow2);
DEFINE_NFLOAT_BOOL_DELEGATE(IsRealNumber);
DEFINE_NFLOAT_BOOL_DELEGATE(IsSubnormal);
DEFINE_NFLOAT_BOOL_DELEGATE(IsInteger);

DEFINE_NFLOAT_CMP_DELEGATE(Compare);

// Equals takes two doubles and returns bool (not CHAOS_IL2CPP_INT32), so it needs a
// dedicated definition rather than the CMP or BOOL macro.
bool NFloatEquals(double left, double right) {
    return DoubleEquals(left, right);
}

#undef DEFINE_NFLOAT_BOOL_DELEGATE
#undef DEFINE_NFLOAT_CMP_DELEGATE

static CHAOS_IL2CPP_INT32 Int128CompareGeneric(const void* left_value, const void* right_value) {
    if (left_value == nullptr || right_value == nullptr) {
        return 0;
    }

    Int128Layout left = {};
    Int128Layout right = {};
    CHAOS_IL2CPP_MEMCPY(&left, left_value, sizeof(left));
    CHAOS_IL2CPP_MEMCPY(&right, right_value, sizeof(right));
    if (left.upper < right.upper) {
        return -1;
    }

    if (left.upper > right.upper) {
        return 1;
    }

    if (left.lower < right.lower) {
        return -1;
    }

    if (left.lower > right.lower) {
        return 1;
    }

    return 0;
}

static CHAOS_IL2CPP_INT32 Int128CompareIntrinsicImpl(const void* left_value, const void* right_value) {
    return Int128CompareGeneric(left_value, right_value);
}

CHAOS_IL2CPP_INT32 Int128Compare(const void* left_value, const void* right_value) {
    return kBitArithmeticKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? Int128CompareIntrinsicImpl(left_value, right_value)
        : Int128CompareGeneric(left_value, right_value);
}

bool Int128Equals(const void* left_value, const void* right_value) {
    return Int128Compare(left_value, right_value) == 0;
}

static CHAOS_IL2CPP_INT32 UInt128CompareGeneric(const void* left_value, const void* right_value) {
    if (left_value == nullptr || right_value == nullptr) {
        return 0;
    }

    UInt128Layout left = {};
    UInt128Layout right = {};
    CHAOS_IL2CPP_MEMCPY(&left, left_value, sizeof(left));
    CHAOS_IL2CPP_MEMCPY(&right, right_value, sizeof(right));
    if (left.upper < right.upper) {
        return -1;
    }

    if (left.upper > right.upper) {
        return 1;
    }

    if (left.lower < right.lower) {
        return -1;
    }

    if (left.lower > right.lower) {
        return 1;
    }

    return 0;
}

static CHAOS_IL2CPP_INT32 UInt128CompareIntrinsicImpl(const void* left_value, const void* right_value) {
    return UInt128CompareGeneric(left_value, right_value);
}

CHAOS_IL2CPP_INT32 UInt128Compare(const void* left_value, const void* right_value) {
    return kBitArithmeticKernelBackend == ValueTypeKernelBackendKind::Intrinsic
        ? UInt128CompareIntrinsicImpl(left_value, right_value)
        : UInt128CompareGeneric(left_value, right_value);
}

bool UInt128Equals(const void* left_value, const void* right_value) {
    return UInt128Compare(left_value, right_value) == 0;
}

CHAOS_IL2CPP_INT32 IntPtrCompare(CHAOS_IL2CPP_INTPTR left_value, CHAOS_IL2CPP_INTPTR right_value) {
    if (left_value < right_value) {
        return -1;
    }

    if (left_value > right_value) {
        return 1;
    }

    return 0;
}

bool IntPtrEquals(CHAOS_IL2CPP_INTPTR left_value, CHAOS_IL2CPP_INTPTR right_value) {
    return left_value == right_value;
}

CHAOS_IL2CPP_INT32 UIntPtrCompare(CHAOS_IL2CPP_UINTPTR left_value, CHAOS_IL2CPP_UINTPTR right_value) {
    if (left_value < right_value) {
        return -1;
    }

    if (left_value > right_value) {
        return 1;
    }

    return 0;
}

bool UIntPtrEquals(CHAOS_IL2CPP_UINTPTR left_value, CHAOS_IL2CPP_UINTPTR right_value) {
    return left_value == right_value;
}

CHAOS_IL2CPP_INT32 DateTimeCompareTicks(const void* left_value, const void* right_value) {
    if (left_value == nullptr || right_value == nullptr) {
        return 0;
    }

    CHAOS_IL2CPP_UINT64 left = 0u;
    CHAOS_IL2CPP_UINT64 right = 0u;
    CHAOS_IL2CPP_MEMCPY(&left, left_value, sizeof(left));
    CHAOS_IL2CPP_MEMCPY(&right, right_value, sizeof(right));
    left &= kDateTimeTicksMask;
    right &= kDateTimeTicksMask;
    if (left < right) {
        return -1;
    }

    if (left > right) {
        return 1;
    }

    return 0;
}

bool DateTimeEqualsTicks(const void* left_value, const void* right_value) {
    return DateTimeCompareTicks(left_value, right_value) == 0;
}

CHAOS_IL2CPP_INT32 TimeSpanCompareTicks(const void* left_value, const void* right_value) {
    if (left_value == nullptr || right_value == nullptr) {
        return 0;
    }

    CHAOS_IL2CPP_INT64 left = 0;
    CHAOS_IL2CPP_INT64 right = 0;
    CHAOS_IL2CPP_MEMCPY(&left, left_value, sizeof(left));
    CHAOS_IL2CPP_MEMCPY(&right, right_value, sizeof(right));
    if (left < right) {
        return -1;
    }

    if (left > right) {
        return 1;
    }

    return 0;
}

bool TimeSpanEqualsTicks(const void* left_value, const void* right_value) {
    return TimeSpanCompareTicks(left_value, right_value) == 0;
}

CHAOS_IL2CPP_INT32 DateOnlyCompareDayNumber(CHAOS_IL2CPP_INT32 left_value, CHAOS_IL2CPP_INT32 right_value) {
    if (left_value < right_value) {
        return -1;
    }

    if (left_value > right_value) {
        return 1;
    }

    return 0;
}

bool DateOnlyEqualsDayNumber(CHAOS_IL2CPP_INT32 left_value, CHAOS_IL2CPP_INT32 right_value) {
    return left_value == right_value;
}

CHAOS_IL2CPP_INT32 TimeOnlyCompareTicksValue(CHAOS_IL2CPP_INT64 left_value, CHAOS_IL2CPP_INT64 right_value) {
    if (left_value < right_value) {
        return -1;
    }

    if (left_value > right_value) {
        return 1;
    }

    return 0;
}

bool TimeOnlyEqualsTicksValue(CHAOS_IL2CPP_INT64 left_value, CHAOS_IL2CPP_INT64 right_value) {
    return left_value == right_value;
}

static RuntimeNumericsVector2Carrier MakeVector2(float x, float y) {
    return RuntimeNumericsVector2Carrier{ x, y };
}

static RuntimeNumericsVector3Carrier MakeVector3(float x, float y, float z) {
    return RuntimeNumericsVector3Carrier{ x, y, z };
}

static RuntimeNumericsVector4Carrier MakeVector4(float x, float y, float z, float w) {
    return RuntimeNumericsVector4Carrier{ x, y, z, w };
}

template <typename TCarrier>
static bool VectorTryCopyToRaw(TCarrier value, float* destination, CHAOS_IL2CPP_SIZE destination_length, CHAOS_IL2CPP_SIZE start_index) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(float);

    if (start_index > destination_length) {
        return false;
    }

    if (destination == nullptr) {
        return false;
    }

    const CHAOS_IL2CPP_SIZE remaining_length = destination_length - start_index;
    if (remaining_length < lane_count) {
        return false;
    }

    CHAOS_IL2CPP_MEMCPY(destination + start_index, &value, sizeof(TCarrier));
    return true;
}

static RuntimeNumericsVector2Carrier TransformVector2ByMatrix3x2(
    RuntimeNumericsVector2Carrier value,
    RuntimeNumericsMatrix3x2Carrier matrix,
    bool include_translation) {
    const float translation_x = include_translation ? matrix.m31 : 0.0f;
    const float translation_y = include_translation ? matrix.m32 : 0.0f;
    return MakeVector2(
        (value.x * matrix.m11) + (value.y * matrix.m21) + translation_x,
        (value.x * matrix.m12) + (value.y * matrix.m22) + translation_y);
}

static RuntimeNumericsVector4Carrier TransformVector2ByMatrix4x4(
    RuntimeNumericsVector2Carrier value,
    RuntimeNumericsMatrix4x4Carrier matrix) {
    return MakeVector4(
        (value.x * matrix.m11) + (value.y * matrix.m21) + matrix.m41,
        (value.x * matrix.m12) + (value.y * matrix.m22) + matrix.m42,
        (value.x * matrix.m13) + (value.y * matrix.m23) + matrix.m43,
        (value.x * matrix.m14) + (value.y * matrix.m24) + matrix.m44);
}

static RuntimeNumericsVector3Carrier TransformVector3ByMatrix4x4(
    RuntimeNumericsVector3Carrier value,
    RuntimeNumericsMatrix4x4Carrier matrix,
    bool include_translation) {
    const float translation_x = include_translation ? matrix.m41 : 0.0f;
    const float translation_y = include_translation ? matrix.m42 : 0.0f;
    const float translation_z = include_translation ? matrix.m43 : 0.0f;
    return MakeVector3(
        (value.x * matrix.m11) + (value.y * matrix.m21) + (value.z * matrix.m31) + translation_x,
        (value.x * matrix.m12) + (value.y * matrix.m22) + (value.z * matrix.m32) + translation_y,
        (value.x * matrix.m13) + (value.y * matrix.m23) + (value.z * matrix.m33) + translation_z);
}

static RuntimeNumericsVector3Carrier RotateVector3ByQuaternion(
    RuntimeNumericsVector3Carrier value,
    RuntimeNumericsQuaternionCarrier rotation) {
    const float x2 = rotation.x + rotation.x;
    const float y2 = rotation.y + rotation.y;
    const float z2 = rotation.z + rotation.z;
    const float wx2 = rotation.w * x2;
    const float wy2 = rotation.w * y2;
    const float wz2 = rotation.w * z2;
    const float xx2 = rotation.x * x2;
    const float xy2 = rotation.x * y2;
    const float xz2 = rotation.x * z2;
    const float yy2 = rotation.y * y2;
    const float yz2 = rotation.y * z2;
    const float zz2 = rotation.z * z2;

    return MakeVector3(
        (value.x * (1.0f - yy2 - zz2)) + (value.y * (xy2 - wz2)) + (value.z * (xz2 + wy2)),
        (value.x * (xy2 + wz2)) + (value.y * (1.0f - xx2 - zz2)) + (value.z * (yz2 - wx2)),
        (value.x * (xz2 - wy2)) + (value.y * (yz2 + wx2)) + (value.z * (1.0f - xx2 - yy2)));
}

RuntimeNumericsVector2Carrier Vector2Zero() {
    return MakeVector2(0.0f, 0.0f);
}

RuntimeNumericsVector2Carrier Vector2One() {
    return MakeVector2(1.0f, 1.0f);
}

RuntimeNumericsVector2Carrier Vector2UnitX() {
    return MakeVector2(1.0f, 0.0f);
}

RuntimeNumericsVector2Carrier Vector2UnitY() {
    return MakeVector2(0.0f, 1.0f);
}

RuntimeNumericsVector2Carrier Vector2Add(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value) {
    return MakeVector2(
        left_value.x + right_value.x,
        left_value.y + right_value.y);
}

RuntimeNumericsVector2Carrier Vector2Subtract(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value) {
    return MakeVector2(
        left_value.x - right_value.x,
        left_value.y - right_value.y);
}

RuntimeNumericsVector2Carrier Vector2Multiply(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value) {
    return MakeVector2(
        left_value.x * right_value.x,
        left_value.y * right_value.y);
}

RuntimeNumericsVector2Carrier Vector2MultiplyScalar(RuntimeNumericsVector2Carrier value, float scalar) {
    return MakeVector2(
        value.x * scalar,
        value.y * scalar);
}

RuntimeNumericsVector2Carrier Vector2Divide(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value) {
    return MakeVector2(
        left_value.x / right_value.x,
        left_value.y / right_value.y);
}

RuntimeNumericsVector2Carrier Vector2DivideScalar(RuntimeNumericsVector2Carrier value, float scalar) {
    return MakeVector2(
        value.x / scalar,
        value.y / scalar);
}

RuntimeNumericsVector2Carrier Vector2Abs(RuntimeNumericsVector2Carrier value) {
    return MakeVector2(
        CHAOS_IL2CPP_FABS(value.x),
        CHAOS_IL2CPP_FABS(value.y));
}

RuntimeNumericsVector2Carrier Vector2Min(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value) {
    return MakeVector2(
        VectorMinElement(left_value.x, right_value.x),
        VectorMinElement(left_value.y, right_value.y));
}

RuntimeNumericsVector2Carrier Vector2Max(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value) {
    return MakeVector2(
        VectorMaxElement(left_value.x, right_value.x),
        VectorMaxElement(left_value.y, right_value.y));
}

RuntimeNumericsVector2Carrier Vector2Clamp(RuntimeNumericsVector2Carrier value, RuntimeNumericsVector2Carrier min_value, RuntimeNumericsVector2Carrier max_value) {
    return Vector2Min(Vector2Max(value, min_value), max_value);
}

RuntimeNumericsVector2Carrier Vector2SquareRoot(RuntimeNumericsVector2Carrier value) {
    return MakeVector2(
        CHAOS_IL2CPP_SQRT(value.x),
        CHAOS_IL2CPP_SQRT(value.y));
}

RuntimeNumericsVector2Carrier Vector2Normalize(RuntimeNumericsVector2Carrier value) {
    return Vector2DivideScalar(value, Vector2Length(value));
}

RuntimeNumericsVector2Carrier Vector2Lerp(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value, float amount) {
    return Vector2Add(left_value, Vector2MultiplyScalar(Vector2Subtract(right_value, left_value), amount));
}

RuntimeNumericsVector2Carrier Vector2Reflect(RuntimeNumericsVector2Carrier vector, RuntimeNumericsVector2Carrier normal) {
    return Vector2Subtract(vector, Vector2MultiplyScalar(normal, 2.0f * Vector2Dot(vector, normal)));
}

bool Vector2Equals(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value) {
    return left_value.x == right_value.x &&
           left_value.y == right_value.y;
}

CHAOS_IL2CPP_INT32 Vector2GetHashCode(RuntimeNumericsVector2Carrier value) {
    return HashCodeCombine2(
        SingleGetHashCodeValue(value.x),
        SingleGetHashCodeValue(value.y));
}

float Vector2Length(RuntimeNumericsVector2Carrier value) {
    return CHAOS_IL2CPP_SQRT(Vector2LengthSquared(value));
}

float Vector2GetElement(RuntimeNumericsVector2Carrier value, CHAOS_IL2CPP_INT32 index) {
    switch (index) {
        case 0:
            return value.x;
        case 1:
            return value.y;
        default:
            return 0.0f;
    }
}

RuntimeNumericsVector2Carrier Vector2Negate(RuntimeNumericsVector2Carrier value) {
    return MakeVector2(-value.x, -value.y);
}

float Vector2Dot(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value) {
    return (left_value.x * right_value.x) + (left_value.y * right_value.y);
}

float Vector2LengthSquared(RuntimeNumericsVector2Carrier value) {
    return Vector2Dot(value, value);
}

float Vector2DistanceSquared(RuntimeNumericsVector2Carrier left_value, RuntimeNumericsVector2Carrier right_value) {
    return Vector2LengthSquared(Vector2Subtract(left_value, right_value));
}

RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix3x2Carrier matrix) {
    return TransformVector2ByMatrix3x2(value, matrix, true);
}

RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix4x4Carrier matrix) {
    RuntimeNumericsVector4Carrier transformed = TransformVector2ByMatrix4x4(value, matrix);
    return MakeVector2(transformed.x, transformed.y);
}

RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsQuaternionCarrier rotation) {
    RuntimeNumericsVector3Carrier transformed = RotateVector3ByQuaternion(MakeVector3(value.x, value.y, 0.0f), rotation);
    return MakeVector2(transformed.x, transformed.y);
}

RuntimeNumericsVector2Carrier Vector2TransformNormal(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix3x2Carrier matrix) {
    return TransformVector2ByMatrix3x2(value, matrix, false);
}

RuntimeNumericsVector2Carrier Vector2TransformNormal(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix4x4Carrier matrix) {
    return MakeVector2(
        (value.x * matrix.m11) + (value.y * matrix.m21),
        (value.x * matrix.m12) + (value.y * matrix.m22));
}

bool Vector2TryCopyTo(RuntimeNumericsVector2Carrier value, float* destination, CHAOS_IL2CPP_SIZE destination_length, CHAOS_IL2CPP_SIZE start_index) {
    return VectorTryCopyToRaw(value, destination, destination_length, start_index);
}

RuntimeNumericsVector3Carrier Vector3Zero() {
    return MakeVector3(0.0f, 0.0f, 0.0f);
}

RuntimeNumericsVector3Carrier Vector3One() {
    return MakeVector3(1.0f, 1.0f, 1.0f);
}

RuntimeNumericsVector3Carrier Vector3UnitX() {
    return MakeVector3(1.0f, 0.0f, 0.0f);
}

RuntimeNumericsVector3Carrier Vector3UnitY() {
    return MakeVector3(0.0f, 1.0f, 0.0f);
}

RuntimeNumericsVector3Carrier Vector3UnitZ() {
    return MakeVector3(0.0f, 0.0f, 1.0f);
}

RuntimeNumericsVector3Carrier Vector3Add(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value) {
    return MakeVector3(
        left_value.x + right_value.x,
        left_value.y + right_value.y,
        left_value.z + right_value.z);
}

RuntimeNumericsVector3Carrier Vector3Subtract(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value) {
    return MakeVector3(
        left_value.x - right_value.x,
        left_value.y - right_value.y,
        left_value.z - right_value.z);
}

RuntimeNumericsVector3Carrier Vector3Multiply(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value) {
    return MakeVector3(
        left_value.x * right_value.x,
        left_value.y * right_value.y,
        left_value.z * right_value.z);
}

RuntimeNumericsVector3Carrier Vector3MultiplyScalar(RuntimeNumericsVector3Carrier value, float scalar) {
    return MakeVector3(
        value.x * scalar,
        value.y * scalar,
        value.z * scalar);
}

RuntimeNumericsVector3Carrier Vector3Divide(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value) {
    return MakeVector3(
        left_value.x / right_value.x,
        left_value.y / right_value.y,
        left_value.z / right_value.z);
}

RuntimeNumericsVector3Carrier Vector3DivideScalar(RuntimeNumericsVector3Carrier value, float scalar) {
    return MakeVector3(
        value.x / scalar,
        value.y / scalar,
        value.z / scalar);
}

RuntimeNumericsVector3Carrier Vector3Abs(RuntimeNumericsVector3Carrier value) {
    return MakeVector3(
        CHAOS_IL2CPP_FABS(value.x),
        CHAOS_IL2CPP_FABS(value.y),
        CHAOS_IL2CPP_FABS(value.z));
}

RuntimeNumericsVector3Carrier Vector3Min(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value) {
    return MakeVector3(
        VectorMinElement(left_value.x, right_value.x),
        VectorMinElement(left_value.y, right_value.y),
        VectorMinElement(left_value.z, right_value.z));
}

RuntimeNumericsVector3Carrier Vector3Max(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value) {
    return MakeVector3(
        VectorMaxElement(left_value.x, right_value.x),
        VectorMaxElement(left_value.y, right_value.y),
        VectorMaxElement(left_value.z, right_value.z));
}

RuntimeNumericsVector3Carrier Vector3Clamp(RuntimeNumericsVector3Carrier value, RuntimeNumericsVector3Carrier min_value, RuntimeNumericsVector3Carrier max_value) {
    return Vector3Min(Vector3Max(value, min_value), max_value);
}

RuntimeNumericsVector3Carrier Vector3SquareRoot(RuntimeNumericsVector3Carrier value) {
    return MakeVector3(
        CHAOS_IL2CPP_SQRT(value.x),
        CHAOS_IL2CPP_SQRT(value.y),
        CHAOS_IL2CPP_SQRT(value.z));
}

RuntimeNumericsVector3Carrier Vector3Normalize(RuntimeNumericsVector3Carrier value) {
    return Vector3DivideScalar(value, Vector3Length(value));
}

RuntimeNumericsVector3Carrier Vector3Lerp(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value, float amount) {
    return Vector3Add(left_value, Vector3MultiplyScalar(Vector3Subtract(right_value, left_value), amount));
}

RuntimeNumericsVector3Carrier Vector3Reflect(RuntimeNumericsVector3Carrier vector, RuntimeNumericsVector3Carrier normal) {
    return Vector3Subtract(vector, Vector3MultiplyScalar(normal, 2.0f * Vector3Dot(vector, normal)));
}

RuntimeNumericsVector3Carrier Vector3Cross(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value) {
    return MakeVector3(
        (left_value.y * right_value.z) - (left_value.z * right_value.y),
        (left_value.z * right_value.x) - (left_value.x * right_value.z),
        (left_value.x * right_value.y) - (left_value.y * right_value.x));
}

bool Vector3Equals(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value) {
    return left_value.x == right_value.x &&
           left_value.y == right_value.y &&
           left_value.z == right_value.z;
}

CHAOS_IL2CPP_INT32 Vector3GetHashCode(RuntimeNumericsVector3Carrier value) {
    return HashCodeCombine3(
        SingleGetHashCodeValue(value.x),
        SingleGetHashCodeValue(value.y),
        SingleGetHashCodeValue(value.z));
}

float Vector3Length(RuntimeNumericsVector3Carrier value) {
    return CHAOS_IL2CPP_SQRT(Vector3LengthSquared(value));
}

float Vector3GetElement(RuntimeNumericsVector3Carrier value, CHAOS_IL2CPP_INT32 index) {
    switch (index) {
        case 0:
            return value.x;
        case 1:
            return value.y;
        case 2:
            return value.z;
        default:
            return 0.0f;
    }
}

RuntimeNumericsVector3Carrier Vector3Negate(RuntimeNumericsVector3Carrier value) {
    return MakeVector3(-value.x, -value.y, -value.z);
}

float Vector3Dot(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value) {
    return (left_value.x * right_value.x) +
           (left_value.y * right_value.y) +
           (left_value.z * right_value.z);
}

float Vector3LengthSquared(RuntimeNumericsVector3Carrier value) {
    return Vector3Dot(value, value);
}

float Vector3DistanceSquared(RuntimeNumericsVector3Carrier left_value, RuntimeNumericsVector3Carrier right_value) {
    return Vector3LengthSquared(Vector3Subtract(left_value, right_value));
}

RuntimeNumericsVector3Carrier Vector3Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsMatrix4x4Carrier matrix) {
    return TransformVector3ByMatrix4x4(value, matrix, true);
}

RuntimeNumericsVector3Carrier Vector3Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsQuaternionCarrier rotation) {
    return RotateVector3ByQuaternion(value, rotation);
}

RuntimeNumericsVector3Carrier Vector3TransformNormal(RuntimeNumericsVector3Carrier value, RuntimeNumericsMatrix4x4Carrier matrix) {
    return TransformVector3ByMatrix4x4(value, matrix, false);
}

bool Vector3TryCopyTo(RuntimeNumericsVector3Carrier value, float* destination, CHAOS_IL2CPP_SIZE destination_length, CHAOS_IL2CPP_SIZE start_index) {
    return VectorTryCopyToRaw(value, destination, destination_length, start_index);
}

RuntimeNumericsVector4Carrier Vector4Zero() {
    return MakeVector4(0.0f, 0.0f, 0.0f, 0.0f);
}

RuntimeNumericsVector4Carrier Vector4One() {
    return MakeVector4(1.0f, 1.0f, 1.0f, 1.0f);
}

RuntimeNumericsVector4Carrier Vector4UnitX() {
    return MakeVector4(1.0f, 0.0f, 0.0f, 0.0f);
}

RuntimeNumericsVector4Carrier Vector4UnitY() {
    return MakeVector4(0.0f, 1.0f, 0.0f, 0.0f);
}

RuntimeNumericsVector4Carrier Vector4UnitZ() {
    return MakeVector4(0.0f, 0.0f, 1.0f, 0.0f);
}

RuntimeNumericsVector4Carrier Vector4UnitW() {
    return MakeVector4(0.0f, 0.0f, 0.0f, 1.0f);
}

RuntimeNumericsVector4Carrier Vector4Add(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value) {
    return MakeVector4(
        left_value.x + right_value.x,
        left_value.y + right_value.y,
        left_value.z + right_value.z,
        left_value.w + right_value.w);
}

RuntimeNumericsVector4Carrier Vector4Subtract(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value) {
    return MakeVector4(
        left_value.x - right_value.x,
        left_value.y - right_value.y,
        left_value.z - right_value.z,
        left_value.w - right_value.w);
}

RuntimeNumericsVector4Carrier Vector4Multiply(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value) {
    return MakeVector4(
        left_value.x * right_value.x,
        left_value.y * right_value.y,
        left_value.z * right_value.z,
        left_value.w * right_value.w);
}

RuntimeNumericsVector4Carrier Vector4MultiplyScalar(RuntimeNumericsVector4Carrier value, float scalar) {
    return MakeVector4(
        value.x * scalar,
        value.y * scalar,
        value.z * scalar,
        value.w * scalar);
}

RuntimeNumericsVector4Carrier Vector4Divide(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value) {
    return MakeVector4(
        left_value.x / right_value.x,
        left_value.y / right_value.y,
        left_value.z / right_value.z,
        left_value.w / right_value.w);
}

RuntimeNumericsVector4Carrier Vector4DivideScalar(RuntimeNumericsVector4Carrier value, float scalar) {
    return MakeVector4(
        value.x / scalar,
        value.y / scalar,
        value.z / scalar,
        value.w / scalar);
}

RuntimeNumericsVector4Carrier Vector4Abs(RuntimeNumericsVector4Carrier value) {
    return MakeVector4(
        CHAOS_IL2CPP_FABS(value.x),
        CHAOS_IL2CPP_FABS(value.y),
        CHAOS_IL2CPP_FABS(value.z),
        CHAOS_IL2CPP_FABS(value.w));
}

RuntimeNumericsVector4Carrier Vector4Min(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value) {
    return MakeVector4(
        VectorMinElement(left_value.x, right_value.x),
        VectorMinElement(left_value.y, right_value.y),
        VectorMinElement(left_value.z, right_value.z),
        VectorMinElement(left_value.w, right_value.w));
}

RuntimeNumericsVector4Carrier Vector4Max(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value) {
    return MakeVector4(
        VectorMaxElement(left_value.x, right_value.x),
        VectorMaxElement(left_value.y, right_value.y),
        VectorMaxElement(left_value.z, right_value.z),
        VectorMaxElement(left_value.w, right_value.w));
}

RuntimeNumericsVector4Carrier Vector4Clamp(RuntimeNumericsVector4Carrier value, RuntimeNumericsVector4Carrier min_value, RuntimeNumericsVector4Carrier max_value) {
    return Vector4Min(Vector4Max(value, min_value), max_value);
}

RuntimeNumericsVector4Carrier Vector4SquareRoot(RuntimeNumericsVector4Carrier value) {
    return MakeVector4(
        CHAOS_IL2CPP_SQRT(value.x),
        CHAOS_IL2CPP_SQRT(value.y),
        CHAOS_IL2CPP_SQRT(value.z),
        CHAOS_IL2CPP_SQRT(value.w));
}

RuntimeNumericsVector4Carrier Vector4Normalize(RuntimeNumericsVector4Carrier value) {
    return Vector4DivideScalar(value, Vector4Length(value));
}

RuntimeNumericsVector4Carrier Vector4Lerp(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value, float amount) {
    return Vector4Add(left_value, Vector4MultiplyScalar(Vector4Subtract(right_value, left_value), amount));
}

bool Vector4Equals(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value) {
    return left_value.x == right_value.x &&
           left_value.y == right_value.y &&
           left_value.z == right_value.z &&
           left_value.w == right_value.w;
}

CHAOS_IL2CPP_INT32 Vector4GetHashCode(RuntimeNumericsVector4Carrier value) {
    return HashCodeCombine4(
        SingleGetHashCodeValue(value.x),
        SingleGetHashCodeValue(value.y),
        SingleGetHashCodeValue(value.z),
        SingleGetHashCodeValue(value.w));
}

float Vector4Length(RuntimeNumericsVector4Carrier value) {
    return CHAOS_IL2CPP_SQRT(Vector4LengthSquared(value));
}

float Vector4GetElement(RuntimeNumericsVector4Carrier value, CHAOS_IL2CPP_INT32 index) {
    switch (index) {
        case 0:
            return value.x;
        case 1:
            return value.y;
        case 2:
            return value.z;
        case 3:
            return value.w;
        default:
            return 0.0f;
    }
}

RuntimeNumericsVector4Carrier Vector4Negate(RuntimeNumericsVector4Carrier value) {
    return MakeVector4(-value.x, -value.y, -value.z, -value.w);
}

float Vector4Dot(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value) {
    return (left_value.x * right_value.x) +
           (left_value.y * right_value.y) +
           (left_value.z * right_value.z) +
           (left_value.w * right_value.w);
}

float Vector4LengthSquared(RuntimeNumericsVector4Carrier value) {
    return Vector4Dot(value, value);
}

float Vector4DistanceSquared(RuntimeNumericsVector4Carrier left_value, RuntimeNumericsVector4Carrier right_value) {
    return Vector4LengthSquared(Vector4Subtract(left_value, right_value));
}

RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix4x4Carrier matrix) {
    return TransformVector2ByMatrix4x4(value, matrix);
}

RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsQuaternionCarrier rotation) {
    RuntimeNumericsVector3Carrier transformed = RotateVector3ByQuaternion(MakeVector3(value.x, value.y, 0.0f), rotation);
    return MakeVector4(transformed.x, transformed.y, transformed.z, 1.0f);
}

RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsMatrix4x4Carrier matrix) {
    return MakeVector4(
        (value.x * matrix.m11) + (value.y * matrix.m21) + (value.z * matrix.m31) + matrix.m41,
        (value.x * matrix.m12) + (value.y * matrix.m22) + (value.z * matrix.m32) + matrix.m42,
        (value.x * matrix.m13) + (value.y * matrix.m23) + (value.z * matrix.m33) + matrix.m43,
        (value.x * matrix.m14) + (value.y * matrix.m24) + (value.z * matrix.m34) + matrix.m44);
}

RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsQuaternionCarrier rotation) {
    RuntimeNumericsVector3Carrier transformed = RotateVector3ByQuaternion(value, rotation);
    return MakeVector4(transformed.x, transformed.y, transformed.z, 1.0f);
}

RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector4Carrier value, RuntimeNumericsMatrix4x4Carrier matrix) {
    return MakeVector4(
        (value.x * matrix.m11) + (value.y * matrix.m21) + (value.z * matrix.m31) + (value.w * matrix.m41),
        (value.x * matrix.m12) + (value.y * matrix.m22) + (value.z * matrix.m32) + (value.w * matrix.m42),
        (value.x * matrix.m13) + (value.y * matrix.m23) + (value.z * matrix.m33) + (value.w * matrix.m43),
        (value.x * matrix.m14) + (value.y * matrix.m24) + (value.z * matrix.m34) + (value.w * matrix.m44));
}

RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector4Carrier value, RuntimeNumericsQuaternionCarrier rotation) {
    RuntimeNumericsVector3Carrier transformed = RotateVector3ByQuaternion(MakeVector3(value.x, value.y, value.z), rotation);
    return MakeVector4(transformed.x, transformed.y, transformed.z, value.w);
}

bool Vector4TryCopyTo(RuntimeNumericsVector4Carrier value, float* destination, CHAOS_IL2CPP_SIZE destination_length, CHAOS_IL2CPP_SIZE start_index) {
    return VectorTryCopyToRaw(value, destination, destination_length, start_index);
}

RuntimeIntrinsicVector64Carrier Vector64Reinterpret(RuntimeIntrinsicVector64Carrier value) {
    return value;
}

RuntimeIntrinsicVector128Carrier Vector128Reinterpret(RuntimeIntrinsicVector128Carrier value) {
    return value;
}

RuntimeIntrinsicVector256Carrier Vector256Reinterpret(RuntimeIntrinsicVector256Carrier value) {
    return value;
}

RuntimeIntrinsicVector512Carrier Vector512Reinterpret(RuntimeIntrinsicVector512Carrier value) {
    return value;
}

CHAOS_IL2CPP_INT32 Vector64Count() {
    return 8 / 4;
}

CHAOS_IL2CPP_INT32 Vector128Count() {
    return 16 / 4;
}

CHAOS_IL2CPP_INT32 Vector256Count() {
    return 32 / 4;
}

CHAOS_IL2CPP_INT32 Vector512Count() {
    return 64 / 4;
}

CHAOS_IL2CPP_INT32 VectorPlatformByteWidth() {
    return 32;
}

bool Vector64IsSupported() {
    return true;
}

bool Vector128IsSupported() {
    return true;
}

bool Vector256IsSupported() {
    return true;
}

bool Vector512IsSupported() {
    return true;
}

// ── Delegate thunk registry (P/Invoke delegate marshalling) ──

struct DelegateThunkEntry {
    const char* type_id;            // Delegate type subject ID
    void* thunk_fn;                 // Native-callable thunk function pointer
    CHAOS_IL2CPP_INTPTR* target_slot; // Module-level static holding current delegate
};

static constexpr CHAOS_IL2CPP_INT32 kMaxDelegateThunks = 64;
static DelegateThunkEntry g_delegate_thunks[kMaxDelegateThunks];
static CHAOS_IL2CPP_INT32 g_delegate_thunk_count = 0;

void RegisterDelegateThunk(const char* type_id, void* thunk_fn, CHAOS_IL2CPP_INTPTR* target_slot) {
    if (type_id == nullptr || thunk_fn == nullptr || target_slot == nullptr) {
        return;
    }

    if (g_delegate_thunk_count >= kMaxDelegateThunks) {
        return; // V1: simple fixed-size table, no expansion
    }

    // Check for duplicate registration.
    for (CHAOS_IL2CPP_INT32 i = 0; i < g_delegate_thunk_count; i++) {
        if (std::strcmp(g_delegate_thunks[i].type_id, type_id) == 0) {
            g_delegate_thunks[i].thunk_fn = thunk_fn;
            g_delegate_thunks[i].target_slot = target_slot;
            return;
        }
    }

    g_delegate_thunks[g_delegate_thunk_count] = { type_id, thunk_fn, target_slot };
    g_delegate_thunk_count++;
}

void* FindDelegateThunk(const char* type_id) {
    if (type_id == nullptr) return nullptr;

    for (CHAOS_IL2CPP_INT32 i = 0; i < g_delegate_thunk_count; i++) {
        if (std::strcmp(g_delegate_thunks[i].type_id, type_id) == 0) {
            return g_delegate_thunks[i].thunk_fn;
        }
    }
    return nullptr;
}

void* MarshalGetFunctionPointerForDelegateImpl(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR delegate_obj,
    const char* delegate_type_id)
{
    (void)thread_state;

    if (runtime_state == nullptr || delegate_obj == static_cast<CHAOS_IL2CPP_INTPTR>(0) || delegate_type_id == nullptr) {
        return nullptr;
    }

    // V1: simple linear scan for the matching thunk entry.
    for (CHAOS_IL2CPP_INT32 i = 0; i < g_delegate_thunk_count; i++) {
        if (std::strcmp(g_delegate_thunks[i].type_id, delegate_type_id) == 0) {
            // Store the delegate object in the target slot so the thunk can find it.
            if (g_delegate_thunks[i].target_slot != nullptr) {
                *g_delegate_thunks[i].target_slot = delegate_obj;
            }
            return g_delegate_thunks[i].thunk_fn;
        }
    }

    return nullptr;
}

void* MarshalGetDelegateForFunctionPointerImpl(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR native_fn_ptr,
    const char* delegate_type_id)
{
    (void)runtime_state;
    (void)thread_state;
    (void)native_fn_ptr;
    (void)delegate_type_id;

    // V1: not yet implemented — delegate->native is the common path.
    // GetDelegateForFunctionPointer (native→managed) requires creating a managed
    // delegate that wraps a raw C function pointer, which needs a reverse thunk.
    // Return nullptr to signal "not supported", causing managed code to throw.
    return nullptr;
}

// ── Struct marshalling descriptor registry ─────────────────────────────────

namespace {

struct MarshallingDescriptorEntry {
    CHAOS_IL2CPP_UINT64 stable_id;
    const StructMarshallingDescriptorV1* desc;
};

// Fixed-size registry for codegen-emitted static descriptors.
// 256 entries covers the common case; if exceeded, runtime fallback works.
static constexpr CHAOS_IL2CPP_SIZE kMaxStaticDescriptors = 256;
static MarshallingDescriptorEntry g_static_descriptors[kMaxStaticDescriptors];
static CHAOS_IL2CPP_INT32 g_static_descriptor_count = 0;

// Cache for runtime-reflection-built descriptors (LRU-like, never evicted).
static std::unordered_map<CHAOS_IL2CPP_UINT64,
    std::unique_ptr<StructMarshallingDescriptorV1>> g_runtime_descriptor_cache;
static std::mutex g_runtime_descriptor_mutex;

}  // anonymous namespace

void RegisterStaticMarshallingDescriptor(
    CHAOS_IL2CPP_UINT64 stable_id,
    const StructMarshallingDescriptorV1* desc) noexcept
{
    if (desc == nullptr || stable_id == 0) return;
    if (g_static_descriptor_count >= kMaxStaticDescriptors) return;

    // Check for duplicate registration (same stable_id).
    for (CHAOS_IL2CPP_INT32 i = 0; i < g_static_descriptor_count; ++i) {
        if (g_static_descriptors[i].stable_id == stable_id) return;  // already registered
    }

    g_static_descriptors[g_static_descriptor_count].stable_id = stable_id;
    g_static_descriptors[g_static_descriptor_count].desc = desc;
    g_static_descriptor_count++;
}

static const StructMarshallingDescriptorV1* FindStaticDescriptor(
    CHAOS_IL2CPP_UINT64 stable_id) noexcept
{
    for (CHAOS_IL2CPP_INT32 i = 0; i < g_static_descriptor_count; ++i) {
        if (g_static_descriptors[i].stable_id == stable_id)
            return g_static_descriptors[i].desc;
    }
    return nullptr;
}

const StructMarshallingDescriptorV1*
ResolveStructMarshallingDescriptor(const TypeInfoHot* type) noexcept
{
    if (type == nullptr || type->stable_id == 0) return nullptr;

    // Priority 1: Static descriptor (codegen-emitted)
    auto* static_desc = FindStaticDescriptor(type->stable_id);
    if (static_desc != nullptr) return static_desc;

    // Priority 2: Runtime reflection-built (cached)
    {
        std::lock_guard<std::mutex> lock(g_runtime_descriptor_mutex);
        auto it = g_runtime_descriptor_cache.find(type->stable_id);
        if (it != g_runtime_descriptor_cache.end())
            return it->second.get();
    }

    // V1: Runtime reflection build not yet implemented — returns nullptr.
    // When implemented, this path will:
    //   1. Walk type->fields metadata via reflection_query_model
    //   2. For each field, determine StructFieldKind from field type
    //   3. Recursively build descriptors for nested structs
    //   4. Cache result in g_runtime_descriptor_cache
    // For now, static codegen descriptors cover all known P/Invoke types.
    return nullptr;
}

}  // namespace chaos::il2cpp::runtime_core

extern "C" CHAOS_RUNTIME_ABI_EXPORT const RuntimeAbiV0* CHAOS_RUNTIME_ABI_CALL chaos_runtime_get_abi_v0(void) {
    return chaos::il2cpp::runtime_core::GetRuntimeAbiV0();
}

