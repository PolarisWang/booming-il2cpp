#ifndef CHAOS_IL2CPP_PATCH_LOADER_H_
#define CHAOS_IL2CPP_PATCH_LOADER_H_

// ── PatchLoader: runtime loading of .patchdata binaries ─────────────────
//
// The PatchLoader is responsible for:
//   1. Validating a .patchdata binary (magic, version, structural integrity)
//   2. Iterating its MethodDef entries
//   3. Resolving each method's (type_name, method_name) via NameIndexRegistry
//   4. Building PatchMethod objects containing raw IL + signature
//   5. Marking dispatch table entries as patched (kDispatchPatched)
//   6. Providing PatchMetadataCache for token resolution during IL→IR lowering
//
// PatchMetadataCache provides a self-contained metadata resolver that only
// looks at the .patchdata's own metadata tables. It does NOT register types
// into the global type system (iOS-compatible — no global registration).

#include "dispatch_table.h"

#include "patch_data.h"

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::runtime_core {

// ── PatchMethod ─────────────────────────────────────────────────────────
// Represents a single patched method with its raw IL and cached lowered IR.
struct PatchMethod {
    const uint8_t* il_bytes        = nullptr;   // raw IL bytecode (body data)
    uint32_t       il_length       = 0;          // IL code length
    const uint8_t* signature_blob  = nullptr;   // method signature blob
    uint32_t       signature_len   = 0;          // signature blob length
    uint32_t       max_stack       = 8;          // max stack depth (tiny default)
    void*          cached_ir       = nullptr;   // cached IRMethod (lazy, null = not lowered)
    uint32_t       token;                        // AOT metadata token for this method
    class PatchMetadataCache* metadata_cache = nullptr;  // token resolution cache
};

// ── PatchMetadataCache ───────────────────────────────────────────────────
// Self-contained metadata resolver backed by a .patchdata binary.
// Provides token resolution for IL→IR lowering without registering types
// into the global type system.
class PatchMetadataCache {
public:
    // Construct from a validated .patchdata header.
    explicit PatchMetadataCache(const PatchDataHeader* header) noexcept;

    // Token resolution: given a metadata token, resolve it using the cache's
    // local tables. Returns 0 if the token cannot be resolved.
    //
    // For IL→IR lowering: this is called via ILTokenResolver callback.
    uint32_t ResolveToken(uint32_t token) const noexcept;

    // Lookup helpers for each token type.
    const char* GetString(uint32_t offset) const noexcept;
    const void* GetBlob(uint32_t offset) const noexcept;
    const void* GetBody(uint32_t offset) const noexcept;

    // Iterate methods: return the i-th MethodDef entry.
    const PatchMethodDefEntry* GetMethodDef(uint32_t index) const noexcept;
    uint32_t MethodCount() const noexcept;

    // Get declaring type name for a method entry.
    // Returns "UnknownType" if the type token cannot be resolved.
    const char* GetTypeName(const PatchMethodDefEntry* method) const noexcept;

    // Get full method name (type.method) for diagnostic/lookup purposes.
    // Returns pointer to a thread-local or static buffer — use or copy immediately.
    const char* GetFullMethodName(const PatchMethodDefEntry* method) const noexcept;

private:
    const PatchDataHeader* header_;

    // Resolve a TypeDef token (0x02xxxxxx) → TypeDefEntry.
    const PatchTypeDefEntry* ResolveTypeDef(uint32_t token) const noexcept;

    // Resolve a TypeRef token (0x01xxxxxx) → TypeRefEntry.
    const PatchTypeRefEntry* ResolveTypeRef(uint32_t token) const noexcept;
};

// ── PatchContext ─────────────────────────────────────────────────────────
// Opaque handle returned by ApplyPatchFromMemory. Passed to Unpatch().
struct PatchContext {
    const PatchDataHeader* header          = nullptr;  // mmap'd .patchdata header
    size_t                 total_size      = 0;         // total .patchdata size
    PatchMethod*           methods         = nullptr;   // array of patched methods
    uint32_t               method_count    = 0;         // number of methods
    PatchMetadataCache*    metadata_cache  = nullptr;   // token resolution cache
};

// ── Public API ──────────────────────────────────────────────────────────

// Apply a .patchdata binary blob.
// Validates the format, resolves each method's AOT token, builds PatchMethod
// objects, and marks dispatch table entries as patched.
// Returns a PatchContext (must be freed via Unpatch or delete).
// If host_type_name is provided (non-null), it is used instead of the
// patch DLL's type name when looking up methods in NameIndexRegistry.
// This handles the case where the patch DLL has different type names
// than the AOT host code (e.g. "PatchEntry" vs "NativeEntry").
PatchContext* ApplyPatchFromMemory(const void* data, size_t size,
                                    const char* host_type_name = nullptr) noexcept;

// Revert all patched methods in the given context.
// Clears kDispatchPatched flags on all affected dispatch table entries,
// frees PatchMethod cached IR, and deletes the PatchContext.
bool Unpatch(PatchContext* ctx) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_PATCH_LOADER_H_
