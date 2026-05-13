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
using namespace chaos::il2cpp::marshal_abi;

// ── A5-Trinity Object Header Layouts ────────────────────────────
// Three header kinds discriminated by TypeInfoHot.flags[1:0]:
//   PureType (00):  8B  {TypeInfoHot* type_info}
//   ThinLockable (01): 16B {TypeInfoHot* type_info, uint64_t sync_state}
//   Fat (10):          24B {TypeInfoHot* type_info, void** vtable, uint64_t sync_state}

namespace {

constexpr CHAOS_IL2CPP_UINT64 kDateTimeTicksMask = 0x3FFFFFFFFFFFFFFFull;

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
    memory_domain::DomainId domain_id;
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

// Allocate memory that contains no pointers (e.g., string bytes, boxed value data).
static void* AllocateBytesAtomic(CHAOS_IL2CPP_SIZE size) {
    return NurseryAllocateAtomic(size);
}

}  // anonymous namespace

void* CHAOS_RUNTIME_ABI_CALL DefaultAllocate(CHAOS_IL2CPP_SIZE size, void* user_data) {
    (void)user_data;
    return NurseryAllocate(size);
}

void CHAOS_RUNTIME_ABI_CALL DefaultDeallocate(void* ptr, void* user_data) {
    (void)user_data;
    (void)ptr;
}

void* GcAllocate(CHAOS_IL2CPP_SIZE size) {
    CHAOS_IL2CPP_PROFILE_SCOPE("GcAllocate");
    return NurseryAllocate(size);
}

void* GcAllocateAtomic(CHAOS_IL2CPP_SIZE size) {
    CHAOS_IL2CPP_PROFILE_SCOPE("GcAllocateAtomic");
    return NurseryAllocateAtomic(size);
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

}  // namespace chaos::il2cpp::runtime_core