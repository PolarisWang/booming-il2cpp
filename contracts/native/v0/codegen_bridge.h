#ifndef CHAOS_CODEGEN_BRIDGE_H_
#define CHAOS_CODEGEN_BRIDGE_H_

#include "runtime_abi.h"

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#endif

#define CHAOS_CODEGEN_BRIDGE_V0 0u

/* High-level status codes returned by generated-code helpers. Deferred helpers may return NOT_SUPPORTED. */
typedef int32_t BridgeStatus;

enum {
    CHAOS_BRIDGE_STATUS_OK = 0,
    CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT = 1,
    CHAOS_BRIDGE_STATUS_NOT_READY = 2,
    CHAOS_BRIDGE_STATUS_NOT_FOUND = 3,
    CHAOS_BRIDGE_STATUS_NOT_SUPPORTED = 4,
    CHAOS_BRIDGE_STATUS_MANAGED_EXCEPTION = 5,
    CHAOS_BRIDGE_STATUS_INTERNAL_ERROR = 6,
    CHAOS_BRIDGE_STATUS_METADATA_RESOLUTION_FAILED = 7,
    CHAOS_BRIDGE_STATUS_CLASS_INIT_FAILED = 8,
    CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED = 9
};

/* ── VTable descriptor (codegen-emitted, registered during BootstrapRuntime) ──
 *
 * One per reference/interface type with virtual methods.  The slots array
 * contains only the method slots that THIS type declares or overrides;
 * the inheritance walk in ResolveVirtualMethodPointer handles the rest.
 * vtable_array points to the flat const void*[] emitted alongside.          */
typedef struct VTableDescriptorV0 {
    uint64_t stable_id;
    uint32_t type_token;
    uint32_t base_token;       /* 0 = no base (System.Object)    */
    uint32_t slot_count;
    const void*          slots;            /* -> const VTableSlot[]          */
    const void**         vtable_array;     /* -> const void*[] (flat ptrs)   */
    uint32_t vtable_length;
    uint8_t  type_shape;       /* 1=ref, 2=value, 3=interface    */
    uint8_t             _pad[3];
    const void*          iface_map;        /* -> const InterfaceMapEntry[]   */
    uint32_t iface_count;
} VTableDescriptorV0;

typedef struct CodeRegistrationV0 {
    uint32_t struct_size;
    const void* method_pointers;
    uint32_t method_pointer_count;
    const void* reverse_pinvoke_wrappers;
    uint32_t reverse_pinvoke_wrapper_count;
    const void* invoker_pointers;
    uint32_t invoker_pointer_count;
    const void* unresolved_virtual_calls;
    uint32_t unresolved_virtual_call_count;
    const RuntimeTypeCapabilityEntryV0* type_capabilities;
    uint32_t type_capability_count;
    const VTableDescriptorV0* vtable_descriptors;
    uint32_t       vtable_descriptor_count;

    /// Pointers to the .gc_slot_maps section for batch registration of GC slot maps.
    /// AOT codegen emits one GcSlotMapSectionEntryHdrV0 per method into this section.
    /// Null if the module has no AOT-compiled methods with GC refs.
    const void* slot_map_section_begin;
    const void* slot_map_section_end;
} CodeRegistrationV0;

/*
 * ── Generic instantiation registration entries ──
 *
 * Codegen emits flat arrays of these structs into the AOT data section.
 * Bootstrap iterates them, resolves tokens→handles, and populates the
 * GenericContextRegistry at startup.
 */
typedef struct GenericTypeRegistrationEntryV0 {
    uint32_t open_token;          /* e.g. 0x02000010 (List<T> definition)   */
    uint32_t closed_token;        /* e.g. 0x02000050 (List<int>)           */
    uint32_t arg_count;           /* number of type arguments              */
    uint32_t args_start_index;    /* offset into the parallel token pool   */
} GenericTypeRegistrationEntryV0;

/* Method generic context entries use the same layout as type entries. */
typedef GenericTypeRegistrationEntryV0 GenericMethodRegistrationEntryV0;

/*
 * ── Generic method AOT entries ──
 *
 * Codegen emits these for each closed generic method instantiation that was
 * pre-compiled to native AOT code.  The open_token identifies the generic
 * method definition; the closed_token identifies the AOT-compiled instance.
 * type_args are stored in a flat uint32_t[] token pool referenced by
 * args_start_index and arg_count.
 *
 * The array MUST be sorted by open_token (ascending) to enable per-module
 * binary search at runtime.
 */
typedef struct GenericMethodAotEntryV0 {
    uint32_t open_token;          /* open generic method token (e.g. 0x06000010 for M<>)   */
    uint32_t closed_token;        /* AOT-compiled closed method token (e.g. 0x06000050)    */
    uint32_t arg_count;           /* number of type arguments                              */
    uint32_t args_start_index;    /* offset into the flat type-arg token pool              */
} GenericMethodAotEntryV0;

/*
 * Aggregate registration bundle for one module (AOT or hot-update).
 * Codegen data + runtime-filled fields (source_image, bridge).
 */
typedef struct ModuleGenericRegistrationV0 {
    uint32_t struct_size;

    /* Module identity. */
    uint32_t     module_id;          /* 0 = AOT root, >0 = hot-update */
    const char*  module_name_utf8;   /* diagnostic only                */

    /* Type generic instantiations (GenericTypeRegistrationEntryV0[]). */
    const GenericTypeRegistrationEntryV0*  generic_types;
    uint32_t generic_type_count;
    const uint32_t* generic_type_args;      /* flat token pool */
    uint32_t generic_type_arg_count;

    /* Method generic contexts (GenericMethodRegistrationEntryV0[]). */
    const GenericMethodRegistrationEntryV0*  generic_methods;
    uint32_t generic_method_count;
    const uint32_t* generic_method_args;    /* flat token pool */
    uint32_t generic_method_arg_count;

    /* Method AOT entries (GenericMethodAotEntryV0[]). */
    const GenericMethodAotEntryV0*  method_aot_entries;
    uint32_t method_aot_entry_count;
    const uint32_t* method_aot_entry_args;      /* flat type-arg token pool */
    uint32_t method_aot_entry_arg_count;

    /* Runtime-filled: the image owning every token in this bundle. */
    ImageHandle source_image;
} ModuleGenericRegistrationV0;

typedef struct MetadataRegistrationV0 {
    uint32_t struct_size;

    /* Generic type instantiation entries (GenericTypeRegistrationEntryV0[]). */
    const GenericTypeRegistrationEntryV0*  generic_types;
    uint32_t generic_type_count;
    const uint32_t* generic_type_args;      /* flat token pool */
    uint32_t generic_type_arg_count;

    /* Generic method context entries (GenericMethodRegistrationEntryV0[]). */
    const GenericMethodRegistrationEntryV0*  generic_methods;
    uint32_t generic_method_count;
    const uint32_t* generic_method_args;    /* flat token pool */
    uint32_t generic_method_arg_count;

    /* Generic method AOT entries (GenericMethodAotEntryV0[]). */
    const GenericMethodAotEntryV0*  method_aot_entries;
    uint32_t method_aot_entry_count;
    const uint32_t* method_aot_entry_args;      /* flat type-arg token pool */
    uint32_t method_aot_entry_arg_count;

    const void* field_offsets;
    uint32_t field_offset_count;
    const void* metadata_usages;
    uint32_t metadata_usage_count;
} MetadataRegistrationV0;

/* ── GcSlotMap — GC root slot descriptor for managed stack frames ──
 *
 * Codegen emits one GcSlotMap per managed method, placed in the
 * .gc_slot_maps section.  The runtime root scanner reads these maps
 * during GC to precisely identify object references in each stack frame,
 * eliminating the need for conservative BDWGC scanning of managed frames.
 *
 * Each slot entry encodes the offset within the frame and the reference kind.
 *
 * Layout (32 bits per slot):
 *   bits [0:11] — offset_in_frame (0-4095 bytes from frame base)
 *   bits [12]   — kind (0 = object reference, 1 = interior/byref pointer)
 *   bits [13:31] — reserved (must be 0)
 */
#define CHAOS_GC_SLOT_OFFSET_MASK   0xFFFu
#define CHAOS_GC_SLOT_KIND_OFFSET   12u
#define CHAOS_GC_SLOT_KIND_MASK     (1u << 12)
#define CHAOS_GC_SLOT_KIND_OBJECT   0u          /* exact object reference */
#define CHAOS_GC_SLOT_KIND_INTERIOR (1u << 12)  /* interior/byref pointer */

#define CHAOS_GC_SLOT_ENCODE(offset, kind) \
    (((offset) & CHAOS_GC_SLOT_OFFSET_MASK) | ((kind) & CHAOS_GC_SLOT_KIND_MASK))

/// GcSlotMap header — followed by @a num_gc_slots 32-bit slot entries.
typedef struct GcSlotMapV0 {
    uint32_t frame_size;         /* stack frame size in bytes */
    uint32_t num_gc_slots;       /* number of GC root slots in this frame */
    uint32_t slots[];            /* variable-length array of slot encodings */
} GcSlotMapV0;

/// Per-entry header in the .gc_slot_maps section.
/// Each entry is: [GcSlotMapSectionEntryHdrV0][GcSlotMapV0 body][slots data].
/// The runtime iterates the section by advancing entry_total_size bytes per entry.
typedef struct GcSlotMapSectionEntryHdrV0 {
    uint32_t entry_total_size;   /* total bytes of this section entry (incl. this field) */
    const void* code_address;    /* code address this slot map belongs to */
} GcSlotMapSectionEntryHdrV0;

/// Section attribute for .gc_slot_maps — collected by the linker into
/// a contiguous range that the runtime can iterate at GC time.
#if defined(_MSC_VER)
#define CHAOS_GC_SLOT_MAP_SECTION __declspec(allocate(".gc_slot_maps"))
#elif defined(__GNUC__) || defined(__clang__)
#define CHAOS_GC_SLOT_MAP_SECTION __attribute__((section(".gc_slot_maps")))
#else
#define CHAOS_GC_SLOT_MAP_SECTION
#endif

/* ── Hotpatch Dispatch Table entry ─────────────────────────────────────
 *
 * Each AOT module emits a static dispatch table with one entry per public
 * method.  Codegen-generated call sites use pattern-aware branching:
 *   if (entry.flags & kHotpatchActive) call InterpreterEntryDirect
 *   else                               call direct_ptr directly.
 *
 * The dispatch table is an extern "C" symbol emitted by codegen and consumed
 * by both generated code and the runtime PatchLoader.                           */
#define kHotpatchActive      (1u << 0)
#define kHotpatchKeepNative  (1u << 1)

typedef struct HotpatchEntryV0 {
    void*       direct_ptr;        /* AOT function pointer (set by codegen)   */
    void*       interrupt_ptr;     /* = &InterpreterEntryDirect              */
    uintptr_t   method_key;        /* = PatchMethod* (0 when not patched)   */
    uint32_t    flags;             /* bit 0: kHotpatchActive                 */
} HotpatchEntryV0;

/* ── Hotpatch type name index entries ────────────────────────────
 *
 * Two-level name-based method index used by PatchLoader at runtime to map
 * (type_name, method_name) → AOT token.  Codegen emits flat sorted arrays
 * in .rodata; the HotpatchNameRegistry performs binary search on them.       */
typedef struct HotpatchTypeEntryV0 {
    const char* type_name;          /* short type name (UTF-8, e.g. "String") */
    const char* namespace_name;     /* namespace (UTF-8, e.g. "System") or ""  */
    uint32_t    first_method_index; /* index into the parallel method array */
    uint16_t    method_count;       /* number of methods in this type       */
} HotpatchTypeEntryV0;

typedef struct HotpatchMethodEntryV0 {
    const char* method_name;        /* method name (UTF-8)                  */
    uint32_t    method_token;       /* AOT metadata token (0x06xxxxxx)     */
    uint16_t    param_count;        /* number of parameters                 */
} HotpatchMethodEntryV0;

/* ── Token→Slot entry ───────────────────────────────────────────────
 *
 * Reverse mapping from metadata token to dispatch table slot index.
 * Sorted by token for binary search.                                     */
typedef struct HotpatchSlotEntryV0 {
    uint32_t    token;              /* AOT metadata token                   */
    uint32_t    slot;               /* dispatch table slot index            */
} HotpatchSlotEntryV0;

/* ── Per-module hotpatch registration bundle ────────────────────-─────
 *
 * Codegen emits one of these per AOT module.  Bootstrap passes it to
 * HotpatchNameRegistry::RegisterModule().  All arrays point into .rodata.     */
typedef struct HotpatchModuleV0 {
    const char*                 module_name;          /* diagnostic only   */
    const HotpatchTypeEntryV0*  type_entries;
    uint32_t                    type_entry_count;
    const HotpatchMethodEntryV0* method_entries;
    uint32_t                    method_entry_count;
    const HotpatchSlotEntryV0*  token_slot_entries;
    uint32_t                    token_slot_entry_count;
    HotpatchEntryV0*            entry_table;       /* pointer to table  */
    uint32_t                    entry_table_size;
} HotpatchModuleV0;

typedef struct CodegenRegistrationOptionsV0 {
    uint32_t struct_size;
    uint32_t registration_flags;
    const char* image_name_utf8;
} CodegenRegistrationOptionsV0;

/*
 * Assembly-bound native-reference dispatch request consumed by
 * RunNativeReferenceAssembly-style generated entry points.
 */
typedef struct NativeReferenceAssemblyDispatchRequestV0 {
    const char* subject_id_utf8;
    void* managed_args;
    uint32_t method_id;
} NativeReferenceAssemblyDispatchRequestV0;

/*
 * Process-wide helper surface consumed by generated C++.
 * Query helpers return null or status codes, while managed semantic failures
 * surface through controlled exception exits.
 */
typedef struct CodegenBridgeV0 {
    uint32_t abi_version;
    uint32_t struct_size;

    /* Registration and bootstrap. */
    BridgeStatus (CHAOS_RUNTIME_ABI_CALL* register_codegen)(
        const CodeRegistrationV0* code_registration,
        const MetadataRegistrationV0* metadata_registration,
        const CodegenRegistrationOptionsV0* options);
    BridgeStatus (CHAOS_RUNTIME_ABI_CALL* bootstrap_runtime)(void);

    /* Runtime metadata helpers. */
    TypeInfoHandle (CHAOS_RUNTIME_ABI_CALL* resolve_type_by_token)(
        ImageHandle image,
        uint32_t type_token);
    BridgeStatus (CHAOS_RUNTIME_ABI_CALL* query_type_capability)(
        TypeInfoHandle type,
        RuntimeTypeCapabilityInfoV0* out_capability_info);
    MethodInfoHandle (CHAOS_RUNTIME_ABI_CALL* resolve_method_by_token)(
        ImageHandle image,
        uint32_t method_token);
    FieldInfoHandle (CHAOS_RUNTIME_ABI_CALL* resolve_field_by_token)(
        ImageHandle image,
        uint32_t field_token);

    /* Boxing helpers. */
    void* (CHAOS_RUNTIME_ABI_CALL* box_value)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        TypeInfoHandle value_type,
        const void* value,
        size_t value_size);
    BridgeStatus (CHAOS_RUNTIME_ABI_CALL* unbox_value)(
        RuntimeState* runtime_state,
        void* boxed_object,
        void* out_value,
        size_t out_value_size);

    /* Virtual dispatch helpers. */
    MethodInfoHandle (CHAOS_RUNTIME_ABI_CALL* resolve_virtual_method)(
        TypeInfoHandle instance_type,
        MethodInfoHandle declared_method);
    BridgeStatus (CHAOS_RUNTIME_ABI_CALL* invoke_virtual)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        void* object_instance,
        MethodInfoHandle method,
        void* const* argv,
        uint32_t argc,
        void* out_return_value,
        size_t out_return_value_size,
        ExceptionHandle* out_exception);

    /* Virtual dispatch helpers (token-based, for AOT codegen). */
    MethodInfoHandle (CHAOS_RUNTIME_ABI_CALL* resolve_virtual_method_by_token)(
        uint32_t instance_type_token,
        uint32_t declared_method_token);

    /* Delegate helpers. */
    void* (CHAOS_RUNTIME_ABI_CALL* create_delegate)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        MethodInfoHandle method,
        void* target_instance);
    BridgeStatus (CHAOS_RUNTIME_ABI_CALL* delegate_invoke)(
        RuntimeState* runtime_state,
        ThreadState* thread_state,
        void* delegate_instance,
        void* const* argv,
        uint32_t argc,
        void* out_return_value,
        size_t out_return_value_size,
        ExceptionHandle* out_exception);

    /* Internal call resolution. */
    void* (CHAOS_RUNTIME_ABI_CALL* resolve_icall)(const char* icall_name_utf8);

    /* ── String token resolution ─────────────────────────────────────────────

       Resolve a metadata string token (0x70xxxxxx) from the #US heap of
       the given image.  Returns a null-terminated UTF-8 string owned by the
       bridge (caller must not free).  Returns nullptr on failure.

       Added late to V0 — callers MUST check for nullptr before calling. */
    const char* (CHAOS_RUNTIME_ABI_CALL* resolve_string_by_token)(
        ImageHandle image,
        uint32_t string_token);
} CodegenBridgeV0;

/* Returns the process-wide v0 bridge table or null when unavailable. */
CHAOS_RUNTIME_ABI_EXPORT const CodegenBridgeV0* CHAOS_RUNTIME_ABI_CALL chaos_codegen_get_bridge_v0(void);

/* ── StringId tag bit helpers ─────────────────────────────────────────
 *
 * StringId values on the native eval stack use bit 63 as a tag to
 * distinguish them from heap object pointers.  AMD64 and ARM64 user-space
 * addresses occupy at most 48 bits, so bit 63 is never set by legitimate
 * pointers.  The static_assert below confirms the pointer size invariant.
 *
 * StringId encoding:
 *   - uint64_t content hash (63 bits) | 1 (ensures non-zero)
 *   - Tagged value: CHAOS_STRING_ID_TAG | id
 */
#define CHAOS_STRING_ID_TAG (static_cast<intptr_t>(1) << (sizeof(intptr_t) * 8 - 1))

static_assert(sizeof(intptr_t) == sizeof(uint64_t),
    "CHAOS_STRING_ID_TAG requires 64-bit intptr_t so that bit 63 "
    "is never set by valid user-space addresses on AMD64 or ARM64.");

inline bool chaos_is_string_id(intptr_t v) noexcept
{
    return (v & CHAOS_STRING_ID_TAG) != 0;
}

inline uint64_t chaos_extract_string_id(intptr_t v) noexcept
{
    return static_cast<uint64_t>(v & ~CHAOS_STRING_ID_TAG);
}

inline intptr_t chaos_make_string_id_value(uint64_t id) noexcept
{
    return CHAOS_STRING_ID_TAG | static_cast<intptr_t>(id);
}

#ifdef __cplusplus
}

// ── Strong StringId type with compile-time FNV-1a ──────────────────

/// Strong type for a compile-time string identity value.
/// Use CHAOS_IL2CPP_STRING_ID("literal") to construct one.
/// Implicitly converts to tagged intptr_t for eval stack assignment.
struct chaos_string_id_t {
    uint64_t value;

    /// Tagged for eval stack push (sets bit 63).
    constexpr intptr_t tagged() const noexcept {
        return CHAOS_STRING_ID_TAG | static_cast<intptr_t>(value);
    }

    /// Implicit conversion -> tagged intptr_t for stack assignment.
    constexpr operator intptr_t() const noexcept {
        return tagged();
    }
};

/// Compile-time FNV-1a 64-bit hash.  Must stay in sync with
/// string_table.cpp:Register() so that runtime Intern/Resolve
/// produce / consume the same StringId values.
constexpr uint64_t chaos_constexpr_string_hash(
    const char* str, size_t len) noexcept
{
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<unsigned char>(str[i]);
        hash *= 1099511628211ULL;
    }
    return (hash & ~(1ULL << 63)) | 1ULL;
}

/// Compile-time StringId from a string literal.
constexpr chaos_string_id_t chaos_make_string_id(
    const char* str, size_t len) noexcept
{
    return chaos_string_id_t{ chaos_constexpr_string_hash(str, len) };
}

/// Compile-time StringId — replaces per-call heap allocation in ldstr.
/// The hash is computed at C++ compile time (zero runtime overhead).
/// Example:  chaos_eval_stack[top++] = CHAOS_IL2CPP_STRING_ID("hello");
#define CHAOS_IL2CPP_STRING_ID(literal) \
    chaos_make_string_id((literal), sizeof((literal)) - 1)

#endif  // __cplusplus

#endif  // CHAOS_CODEGEN_BRIDGE_H_

