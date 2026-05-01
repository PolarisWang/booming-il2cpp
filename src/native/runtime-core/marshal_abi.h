#ifndef CHAOS_IL2CPP_MARSHAL_ABI_H_
#define CHAOS_IL2CPP_MARSHAL_ABI_H_

#include <chaos/native_types.h>

namespace chaos::il2cpp::marshal_abi {

enum class MarshalStructureMarshallingClass : CHAOS_IL2CPP_UINT32 {
    FullyBlittableFastPath = 0,
    LayoutFixedMarshallerPath = 1,
    DescriptorDrivenMarshallerPath = 2,
};

enum class MarshalAllocationKind : CHAOS_IL2CPP_UINT32 {
    HGlobal = 0,
    CoTaskMem = 1,
    BStr = 2,
};

enum TaskRuntimeKernelCapabilityBits : CHAOS_IL2CPP_UINT64 {
    TaskRuntimeKernelCapabilityNone = 0ull,
    TaskRuntimeKernelCapabilityStateCore = 1ull << 0,
    TaskRuntimeKernelCapabilityContinuation = 1ull << 1,
    TaskRuntimeKernelCapabilityWait = 1ull << 2,
    TaskRuntimeKernelCapabilityFactory = 1ull << 3,
    TaskRuntimeKernelCapabilityIntrospection = 1ull << 4,
};

enum MarshalPlatformCapabilityBits : CHAOS_IL2CPP_UINT64 {
    MarshalPlatformCapabilityNone = 0ull,
    MarshalPlatformCapabilityMemoryBlock = 1ull << 0,
    MarshalPlatformCapabilityStringMarshaling = 1ull << 1,
    MarshalPlatformCapabilityStructureLayout = 1ull << 2,
    MarshalPlatformCapabilityDelegateBridge = 1ull << 3,
    MarshalPlatformCapabilityComCore = 1ull << 4,
    MarshalPlatformCapabilityComActivation = 1ull << 5,
    MarshalPlatformCapabilityVariantScalar = 1ull << 6,
    MarshalPlatformCapabilityVariantBulk = 1ull << 7,
    MarshalPlatformCapabilityVariantExtended = 1ull << 8,
};

struct MarshalMemoryBlockAbiV1 {
    CHAOS_IL2CPP_SIZE struct_size;
};

struct MarshalStringMarshalingAbiV1 {
    CHAOS_IL2CPP_SIZE struct_size;
};

struct MarshalStructureLayoutAbiV1 {
    CHAOS_IL2CPP_SIZE struct_size;
};

struct MarshalDelegateBridgeAbiV1 {
    CHAOS_IL2CPP_SIZE struct_size;
};

struct ComInteropAbiV1 {
    CHAOS_IL2CPP_SIZE struct_size;
    CHAOS_IL2CPP_UINT64 capability_bits;
};

struct VariantInteropAbiV1 {
    CHAOS_IL2CPP_SIZE struct_size;
    CHAOS_IL2CPP_UINT64 capability_bits;
};

struct MarshalPlatformAbiRootV1 {
    CHAOS_IL2CPP_UINT32 abi_version;
    CHAOS_IL2CPP_SIZE struct_size;
    CHAOS_IL2CPP_UINT64 capability_bits;
    const MarshalMemoryBlockAbiV1* memory_block;
    const MarshalStringMarshalingAbiV1* string_marshaling;
    const MarshalStructureLayoutAbiV1* structure_layout;
    const MarshalDelegateBridgeAbiV1* delegate_bridge;
    const ComInteropAbiV1* com_interop;
    const VariantInteropAbiV1* variant_interop;
};

struct TaskRuntimeKernelV1 {
    CHAOS_IL2CPP_UINT32 abi_version;
    CHAOS_IL2CPP_SIZE struct_size;
    CHAOS_IL2CPP_UINT64 capability_bits;
};

}  // namespace chaos::il2cpp::marshal_abi

#endif  // CHAOS_IL2CPP_MARSHAL_ABI_H_
