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

// ── Struct marshalling descriptor (for complex/nested structs) ──────────

/// Field classification for descriptor-driven marshalling.
enum class StructFieldKind : uint8_t {
    Blittable,       ///< Fixed-size blittable field — direct memcpy.
    BoolField,       ///< 1-byte C# bool ↔ 4-byte Win32 BOOL.
    StringField,     ///< System.String field — UTF-8 CoTaskMem marshalling.
    NestedStruct,    ///< Nested value type — recursive descriptor.
    ByValArray,      ///< Inline fixed-size array (e.g. [MarshalAs(UnmanagedType.ByValArray, SizeConst=N)]).
    LPArray,         ///< Pointer to array (LPArray style — element pointer + optional length).
    DecimalField,    ///< 16-byte System.Decimal ↔ COM DECIMAL.
    DateTimeField,   ///< 8-byte System.DateTime ↔ FILETIME / DATE.
    ObjectField,     ///< object marshalled as IUnknown* (IntPtr on non-COM platforms).
    GuidField,       ///< 16-byte System.Guid — effectively blittable.
};

/// Native type classification for array element marshalling.
enum class NativeElementType : uint8_t {
    None,            ///< Not an array element context.
    U1,              ///< unsigned char / byte
    I1,              ///< signed char / sbyte
    U2,              ///< unsigned short / char16_t
    I2,              ///< signed short
    U4,              ///< unsigned int
    I4,              ///< signed int
    U8,              ///< unsigned long long
    I8,              ///< signed long long
    R4,              ///< float
    R8,              ///< double
    Struct,          ///< struct element — use nested descriptor
};

/// Descriptor for a single struct field.
struct StructFieldDescriptorV1 {
    StructFieldKind  kind;
    uint16_t         offset;          ///< Byte offset within the struct.
    uint16_t         size;            ///< Field size in bytes (0 for StringField).
    uint16_t         array_count;     ///< For ByValArray: element count. 0 for non-array fields.
    NativeElementType element_type;   ///< Native element type for arrays; otherwise None.
    uint8_t          reserved;        ///< Reserved for future use (padding to 12 bytes).
    const struct StructMarshallingDescriptorV1* nested;  ///< For NestedStruct kind; nullptr otherwise.
};

/// Descriptor-driven struct marshalling plan.
/// Emitted by codegen into .rodata; consumed by runtime marshallers.
struct StructMarshallingDescriptorV1 {
    uint16_t total_size;              ///< Total struct size in bytes.
    uint16_t field_count;             ///< Number of fields in the descriptor.
    StructFieldDescriptorV1 fields[]; ///< Variable-length field array.
};

struct TaskRuntimeKernelV1 {
    CHAOS_IL2CPP_UINT32 abi_version;
    CHAOS_IL2CPP_SIZE struct_size;
    CHAOS_IL2CPP_UINT64 capability_bits;
};

}  // namespace chaos::il2cpp::marshal_abi

#endif  // CHAOS_IL2CPP_MARSHAL_ABI_H_
