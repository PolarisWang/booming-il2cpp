namespace chaos::il2cpp::runtime_core {
namespace {

using namespace chaos::il2cpp::runtime_capability;
using namespace chaos::il2cpp::marshal_abi;

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
struct GcHandleEntry {
    void* object_instance;
    bool pinned;
};
static CHAOS_IL2CPP_MUTEX s_gc_handle_mutex;
static CHAOS_IL2CPP_ATOMIC(CHAOS_IL2CPP_UINT64) s_next_gc_handle{1};
static CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT64, GcHandleEntry) s_gc_handle_table;

}  // anonymous namespace
}  // namespace chaos::il2cpp::runtime_core