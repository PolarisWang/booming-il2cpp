// patch_data.h — .patchdata binary format for hot-update method replacement
//
// This header defines the on-disk binary layout of .patchdata files produced
// by the codegen PatchDataExtractor (C#) and consumed by the runtime
// PatchLoader (C++).  The format is a self-contained metadata archive:
// FlatBuffer-inspired — compact, little-endian, zero-copy accessible from
// C++ by casting pointers against the header.
//
// File layout (all integers little-endian):
//   PatchDataHeader
//   StringHeap      (raw ECMA #Strings bytes — null-terminated C strings)
//   BlobHeap        (raw ECMA #Blobs  bytes — length-prefixed binary blobs)
//   UserStringHeap  (raw ECMA #US     bytes — length-prefixed UTF-16)
//   AssemblyRefEntry[]
//   TypeRefEntry[]
//   TypeDefEntry[]
//   FieldDefEntry[]
//   MethodDefEntry[]
//   MemberRefEntry[]
//   StandaloneSigEntry[]
//   BodyDataSection  (raw IL bytecode concatenated; MethodDefEntry.body_offset
//                     points here)
//   AotCoreIrSection  (see below: uint32_t index[count] + null-terminated JSON strings)

#ifndef CHAOS_IL2CPP_PATCH_DATA_H_
#define CHAOS_IL2CPP_PATCH_DATA_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ── Magic and version ────────────────────────────────────────────────
#define PATCH_DATA_MAGIC     0x50415854u   // "PADT" little-endian
#define PATCH_DATA_VERSION   4u            // v4: trailing min_host_revision + patch_revision

// ── Main header (fixed-size, all fields uint32_t, no padding) ────────
// v1:  28 fields = 112 bytes (through aot_core_ir_count)
// v2:  31 fields = 124 bytes (reg_ir_offset/reg_ir_size/reg_ir_count appended)
// v3:  33 fields = 132 bytes (dependency_offset/dependency_count appended)
// v4:  35 fields = 140 bytes (min_host_revision + patch_revision appended)
// The WRITER stores header_size = sizeof(PatchDataHeader) at write time;
// readers use header_size (not sizeof) to know how far the header extends.
// The trailing two v4 fields are OPTIONAL — a v3 reader that only knows
// sizeof(v3)=132 stops at dependency_count, never touches the two new
// fields.  A v4 reader MUST verify header_size >= offsetof(patch_revision)+4
// before reading them (a v3 blob has header_size==132 < 140).
typedef struct PatchDataHeader {    uint32_t magic;
    uint32_t version;
    uint32_t header_size;               // sizeof(PatchDataHeader)

    // Heaps — offsets from file start, sizes in bytes
    uint32_t string_heap_offset;
    uint32_t string_heap_size;
    uint32_t blob_heap_offset;
    uint32_t blob_heap_size;
    uint32_t user_string_heap_offset;
    uint32_t user_string_heap_size;

    // Table sections — offsets from file start, counts in entries
    uint32_t assembly_ref_offset;
    uint32_t assembly_ref_count;
    uint32_t type_ref_offset;
    uint32_t type_ref_count;
    uint32_t type_def_offset;
    uint32_t type_def_count;
    uint32_t field_def_offset;
    uint32_t field_def_count;
    uint32_t method_def_offset;
    uint32_t method_def_count;
    uint32_t member_ref_offset;
    uint32_t member_ref_count;
    uint32_t standalone_sig_offset;
    uint32_t standalone_sig_count;

    // Method body IL data — raw bytes, indexed by MethodDefEntry.body_offset
    uint32_t body_data_offset;
    uint32_t body_data_size;

    // AotCoreIr JSON section — per-method lowered IR strings.
    // Format: [index: uint32_t[count]] [null-terminated JSON strings]
    // index[i] = byte offset of i-th method's JSON from the start of strings.
    // GetAotCoreIr(i) = section + sizeof(uint32_t)*count + index[i]  (O(1)).
    uint32_t aot_core_ir_offset;
    uint32_t aot_core_ir_size;
    uint32_t aot_core_ir_count;

    // Register IR section — pre-allocated register-based IR (v2+).
    // Each method block (ordered by MethodDef index):
    //   uint32_t max_regs | uint32_t instr_count | uint32_t seh_count
    //   RegisterInstruction[instr_count]   (16 bytes each)
    //   SEHClauseCompact[seh_count]        (24 bytes each: 6 × uint32_t)
    //
    // Format: [offsets: uint32_t[count]] [method blocks]
    // offsets[i] = byte offset of i-th method's block from section start.
    // GetRegisterMethod(i) = section + offsets[i]  (O(1)).
    // When reg_ir_count == 0 (v1 fallback or empty), all reg_ir fields are 0.
    uint32_t reg_ir_offset;
    uint32_t reg_ir_size;
    uint32_t reg_ir_count;

    // Dependency section (v3+): list of required assembly dependencies.
    // Each entry identifies a dependency by name; PatchLoader verifies
    // the dependency is registered before applying this patch.
    uint32_t dependency_offset;
    uint32_t dependency_count;

    // Version-compatibility fields (v4+, trailing optional):
    //   min_host_revision — minimum host build revision this patch requires.
    //                       Hosts below this must reject the patch. 0 = any.
    //   patch_revision    — incrementing patch ID (conflict/staleness detect).
    uint32_t min_host_revision;
    uint32_t patch_revision;
    //
    // These are appended AFTER dependency_count so a v3 reader that only knows
    // sizeof(v3)==132 bytes stops exactly at dependency_count and never touches
    // the two new trailing fields.  A v4 reader MUST verify header_size is large
    // enough before reading them (an old v3 blob has header_size==132 < 140).
} PatchDataHeader;

// ── Table entry structs ──────────────────────────────────────────────
// All string/blob references are offsets into the corresponding heap
// section.  A zero offset represents null/empty.
//
// Token values use the same encoding as ECMA 335 metadata tokens:
//   high byte = table kind (0x23=AssemblyRef, 0x01=TypeRef,
//               0x02=TypeDef, 0x04=Field, 0x06=Method, 0x0A=MemberRef,
//               0x11=StandAloneSig)
//   low 24 bits = row index (1-based).

typedef struct PatchAssemblyRefEntry {
    uint32_t name_offset;               // assembly name in string heap
    uint32_t token;                     // 0x23000000 | row
} PatchAssemblyRefEntry;

typedef struct PatchTypeRefEntry {
    uint32_t type_name_offset;
    uint32_t namespace_offset;
    uint32_t resolution_scope_token;    // AssemblyRef or ModuleRef token
    uint32_t token;                     // 0x01000000 | row
} PatchTypeRefEntry;

typedef struct PatchTypeDefEntry {
    uint32_t type_name_offset;
    uint32_t namespace_offset;
    uint32_t enclosing_type_token;      // 0 for top-level
    uint32_t extends_token;             // base type, 0 if none
    uint32_t token;                     // 0x02000000 | row
    uint16_t flags;
} PatchTypeDefEntry;

typedef struct PatchFieldDefEntry {
    uint32_t name_offset;
    uint32_t signature_offset;          // into blob heap (field sig)
    uint32_t declaring_type_token;      // TypeDef token
    uint32_t token;                     // 0x04000000 | row
    uint16_t flags;
} PatchFieldDefEntry;

typedef struct PatchMethodDefEntry {
    uint32_t name_offset;               // method name in string heap
    uint32_t signature_offset;          // into blob heap (method sig)
    uint32_t body_offset;               // into body-data section
    uint32_t body_size;
    uint32_t declaring_type_token;      // TypeDef token
    uint32_t token;                     // 0x06000000 | row
    uint16_t impl_flags;
    uint16_t flags;
} PatchMethodDefEntry;

typedef struct PatchMemberRefEntry {
    uint32_t name_offset;
    uint32_t signature_offset;          // into blob heap
    uint32_t parent_token;              // TypeDef/TypeRef/ModuleRef token
    uint32_t token;                     // 0x0A000000 | row
} PatchMemberRefEntry;

typedef struct PatchStandaloneSigEntry {
    uint32_t signature_offset;          // into blob heap
    uint32_t token;                     // 0x11000000 | row
} PatchStandaloneSigEntry;

// ── Dependency section (v3+) ──────────────────────────────────────────
// Declares a dependency on another assembly. The PatchLoader verifies
// that a module matching assembly_name is already registered before
// applying this patch. Cross-module token resolution uses the
// HotpatchNameRegistry to find method dispatch entries in the
// dependency module.
typedef struct PatchDataDependency {
    uint32_t assembly_name_offset;      // assembly name in string heap
    uint32_t min_version;               // minimum version (0 = any)
} PatchDataDependency;

// ── Inline helpers ──────────────────────────────────────────────────
// These are safe to call on a mapped file after header validation.

static inline uint32_t PatchData_TotalSize(const PatchDataHeader* hdr) {
    uint32_t section_end = hdr->body_data_offset + hdr->body_data_size;
    uint32_t ir_end = hdr->aot_core_ir_offset + hdr->aot_core_ir_size;
    uint32_t reg_ir_end = hdr->reg_ir_offset + hdr->reg_ir_size;
    uint32_t dep_end = hdr->dependency_offset + hdr->dependency_count * sizeof(PatchDataDependency);
    uint32_t max_end = (ir_end > section_end) ? ir_end : section_end;
    max_end = (reg_ir_end > max_end) ? reg_ir_end : max_end;
    return (dep_end > max_end) ? dep_end : max_end;
}

static inline const char* PatchData_String(const PatchDataHeader* hdr, uint32_t offset) {
    if (offset == 0) return "";
    return (const char*)hdr + hdr->string_heap_offset + offset;
}

static inline const void* PatchData_Blob(const PatchDataHeader* hdr, uint32_t offset) {
    if (offset == 0) return NULL;
    return (const uint8_t*)hdr + hdr->blob_heap_offset + offset;
}

static inline const void* PatchData_Body(const PatchDataHeader* hdr, uint32_t offset) {
    if (offset == 0) return NULL;
    return (const uint8_t*)hdr + offset;
}

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CHAOS_IL2CPP_PATCH_DATA_H_
