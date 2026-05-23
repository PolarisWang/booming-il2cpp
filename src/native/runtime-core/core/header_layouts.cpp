// File-scope struct declarations — not inside chaos::il2cpp::runtime_core
// (These are referenced by TLS state variables at file scope.)

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
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_STRING, CHAOS_IL2CPP_INT32) thread_static_int32_slots;
    CHAOS_IL2CPP_VECTOR(ThreadRootRecord) reported_roots;
    CHAOS_IL2CPP_INT32 last_pinvoke_error = 0;
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
using namespace chaos::il2cpp::marshal_abi;

// ── Type System Phase 1 Object Header Layouts ────────────────────
// Two header kinds discriminated by TypeInfoHot.flags[1:0],
// both exactly 8 bytes:
//   PureType (00):  8B  {TypeInfoHot* type_info}
//   ThinLockable (01): 8B  {TypeInfoHot* type_info} — sync via ThinLockTable
//
// sync_state removed from ThinLockable — ThinLockTable handles lock state.
// GC reads TypeInfo* from [0] — unchanged, no branch.

namespace {

constexpr CHAOS_IL2CPP_UINT64 kDateTimeTicksMask = 0x3FFFFFFFFFFFFFFFull;


struct ObjectHeaderThin {  // 8B — no sync_state
    const TypeInfoHot* type_info   = nullptr;  // [0]
};

// FatHeader removed — all non-PureType use ThinLockable (16B).

// ── Header size helpers ──────────────────────────────────────────
// After Type System Phase 1, ALL headers are 8 bytes (Pure and ThinLockable).
inline CHAOS_IL2CPP_SIZE HeaderSizeFromFlags(CHAOS_IL2CPP_UINT8) noexcept {
    return 8;
}

// GetSyncStatePtr is no longer valid — sync state moved to ThinLockTable.
// Callers must use ThinLockTable::Instance() for sync operations.
// Returns nullptr unconditionally to catch stale callers at runtime.
inline uint64_t* GetSyncStatePtr(void*) noexcept {
    return nullptr;
}

struct StringObjectHeader {
    TypeInfoHandle type;
    CHAOS_IL2CPP_UINTPTR byte_count;
};

struct BoxedValueHeader {
    TypeInfoHandle type;
    CHAOS_IL2CPP_UINTPTR byte_count;
};

}  // anonymous namespace

struct ArrayHeader {
    TypeInfoHandle element_type;
    CHAOS_IL2CPP_UINTPTR length;
};

struct UInt128Layout {
    CHAOS_IL2CPP_UINT64 lower;
    CHAOS_IL2CPP_UINT64 upper;
};

struct Int128Layout {
    CHAOS_IL2CPP_UINT64 lower;
    CHAOS_IL2CPP_INT64 upper;
};

#ifndef CHAOS_IL2CPP_MARSHAL_ALLOC_HEADER_ALREADY_DEFINED
#define CHAOS_IL2CPP_MARSHAL_ALLOC_HEADER_ALREADY_DEFINED

struct MarshalAllocationHeader {
    CHAOS_IL2CPP_UINT64 magic;
    memory_domain::DomainId domain_id;
    MarshalAllocationKind kind;
    CHAOS_IL2CPP_SIZE requested_size;
};

constexpr CHAOS_IL2CPP_UINT64 kMarshalAllocationMagic = 0x43484D4152534841ull;
#endif

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

}  // namespace chaos::il2cpp::runtime_core