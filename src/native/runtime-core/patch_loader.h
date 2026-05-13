#ifndef CHAOS_IL2CPP_PATCH_LOADER_H_
#define CHAOS_IL2CPP_PATCH_LOADER_H_

// ── PatchLoader: runtime loading of .patchdata binaries ─────────────────
//
// The PatchLoader is responsible for:
//   1. Validating a .patchdata binary (magic, version, structural integrity)
//   2. Iterating its MethodDef entries
//   3. Resolving each method's (type_name, method_name) via HotpatchNameRegistry
//   4. Building PatchMethod objects containing raw IL + signature
//   5. Marking dispatch table entries as patched (kHotpatchActive)
//   6. Providing PatchMetadataCache for token resolution during IL→IR lowering
//
// PatchMetadataCache provides a self-contained metadata resolver that only
// looks at the .patchdata's own metadata tables. It does NOT register types
// into the global type system (iOS-compatible — no global registration).

#include "hotpatch_table.h"

#include "patch_data.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::runtime_core {

// ── PatchMethod ─────────────────────────────────────────────────────────
// Represents a single patched method with its AotCoreIr JSON and cached lowered IR.
struct PatchMethod {
    const char*     aot_core_ir_json        = nullptr;   // serialized AotCoreIr method JSON (v1)
    uint32_t        aot_core_ir_json_length = 0;          // JSON string length
    const uint8_t*  signature_blob  = nullptr;   // method signature blob
    uint32_t        signature_len   = 0;          // signature blob length
    void*           cached_ir       = nullptr;   // cached IRMethod (lazy, null = not lowered)
    void*           cached_reg_method = nullptr; // cached RegisterMethod (register-allocated, lazy)

    // ── v2 .patchdata: pre-allocated register IR binary data ─────────
    // When non-null, PatchMethodLowerIR skips JSON deserialization + register
    // allocation pass and directly builds RegisterMethod from binary data.
    const void*     reg_ir_data         = nullptr;  // raw RegisterInstruction[] block
    uint32_t        reg_ir_instr_count  = 0;         // number of instructions
    uint32_t        reg_ir_seh_count    = 0;         // number of SEH clauses
    uint32_t        reg_ir_max_regs     = 0;         // highest register used
    uint32_t        token;                        // AOT metadata token for this method
    uint32_t        module_id;                    // module index for module-scoped dispatch
    class PatchMetadataCache* metadata_cache = nullptr;  // token resolution cache

    // ── Cached signature parse results (populated lazily on first invocation) ──
    uint32_t        cached_arg_count        = 0;       // total arg count (including 'this')
    uint8_t         cached_ret_tag          = 0;       // ValueTag for return type
    uint8_t         cached_arg_types_small[8] = {};     // small-buffer for ≤8 args
    uint8_t*        cached_arg_types = cached_arg_types_small; // points to small buf or heap
    uint32_t        cached_arg_capacity = 8;            // allocated capacity
    bool            cached_sig_valid        = false;   // true when cache is populated

    // ── Per-instruction call-site metadata cache ──────────────────────────
    // Populated in PatchMethodLowerIR after IR deserialization.
    // Points to heap-allocated CachedCallInfo[instr_count], or nullptr.
    void*           call_cache          = nullptr;   // CachedCallInfo[]

    // ── Call count for hot path detection (A2.3) ────────────────────────
    // Atomically incremented on each call to InterpreterEntryDirect.
    // When call_count exceeds kHotCallThreshold, method is promoted to
    // hot path with priority inlining.
    mutable std::atomic<uint32_t> call_count{0};
    static constexpr uint32_t kHotCallThreshold = 100;

    // ── Lazy IR lowering state ───────────────────────────────────────────
    // 0=uninitialized, 1=lowering-in-progress, 2=done.
    // CAS-based to avoid global mutex contention across threads.
    mutable std::atomic<uint32_t> ir_state{0};
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

    // Get the AotCoreIr JSON for a method by its index in the MethodDef table.
    // Returns nullptr if the index is out of range or the section is empty.
    const char* GetAotCoreIr(uint32_t method_index) const noexcept;

    // Get the pre-allocated register IR binary block for a method (v2+).
    // Returns {nullptr, 0, 0, 0} if the section is empty or index out of range.
    struct RegisterIrBlock {
        const void*    data;         // pointer to RegisterInstruction[]
        uint32_t       instr_count;  // number of instructions
        uint32_t       seh_count;    // number of SEH clauses
        uint32_t       max_regs;     // highest register used
    };
    RegisterIrBlock GetRegisterIr(uint32_t method_index) const noexcept;

    // Iterate methods: return the i-th MethodDef entry.
    const PatchMethodDefEntry* GetMethodDef(uint32_t index) const noexcept;
    uint32_t MethodCount() const noexcept;

    // Get declaring type name for a method entry.
    // Returns "UnknownType" if the type token cannot be resolved.
    const char* GetTypeName(const PatchMethodDefEntry* method) const noexcept;

    // Get declaring type namespace for a method entry.
    // Returns "" (empty string) if the type has no namespace or cannot be resolved.
    const char* GetTypeNamespace(const PatchMethodDefEntry* method) const noexcept;

    // Get full method name (type.method) for diagnostic/lookup purposes.
    // Returns pointer to a thread-local or static buffer — use or copy immediately.
    const char* GetFullMethodName(const PatchMethodDefEntry* method) const noexcept;

    // ── AOT bridge for cross-module token resolution ──
    // Set by InterpreterEntryDirect before IL→IR lowering to enable
    // DefaultTokenResolver fallback for tokens outside the patch scope.
    void SetAotBridge(const CodegenBridgeV0* bridge, ImageHandle image) noexcept {
        bridge_ = bridge;
        aot_image_ = image;
    }

    const CodegenBridgeV0* GetBridge() const noexcept { return bridge_; }
    ImageHandle GetAotImage() const noexcept { return aot_image_; }

    // Resolve a UserString token (0x70xxxxxx) → UTF-8 string.
    // Reads from the patch data's #US heap and caches the converted result.
    const char* GetUserString(uint32_t token) const noexcept;

    // ── Inlining map accessors ──────────────────────────────────────────
    // Maps a (module_id << 32 | token) key to the PatchMethod that provides
    // replacement IR.  Populated during ApplyPatchFromMemory so that
    // InlineLeafCallees can find callee IR within the same patch context.
    void AddInliningTarget(uint32_t module_id, uint32_t token, PatchMethod* method) noexcept {
        uint64_t key = (static_cast<uint64_t>(module_id) << 32) | token;
        inlining_map_[key] = method;
    }

    PatchMethod* FindInliningTarget(uint32_t module_id, uint32_t token) const noexcept {
        uint64_t key = (static_cast<uint64_t>(module_id) << 32) | token;
        auto it = inlining_map_.find(key);
        return (it != inlining_map_.end()) ? it->second : nullptr;
    }
    const PatchTypeDefEntry* ResolveTypeDef(uint32_t token) const noexcept;
    const PatchTypeRefEntry* ResolveTypeRef(uint32_t token) const noexcept;
    const PatchMemberRefEntry* ResolveMemberRef(uint32_t token) const noexcept;

private:
    const PatchDataHeader* header_;
    const CodegenBridgeV0* bridge_ = nullptr;
    ImageHandle            aot_image_ = 0;

    // ── Inlining map: AOT token → PatchMethod* ─────────────────────────
    // Maps (module_id << 32 | token) to the PatchMethod that overrides it.
    // Built during ApplyPatchFromMemory after all methods are pre-lowered.
    // Used by InlineLeafCallees to find callee IR for same-patch-context inlining.
    std::unordered_map<uint64_t, PatchMethod*> inlining_map_;

    // Cached UTF-8 strings decoded from the #US heap.
    // Populated lazily by GetUserString(). Keyed by offset into the #US heap
    // for O(1) lookup and pointer stability (unordered_map does not invalidate
    // references on insertion).
    mutable std::unordered_map<uint32_t, std::string> user_string_cache_;
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
// patch DLL's type name when looking up methods in HotpatchNameRegistry.
// This handles the case where the patch DLL has different type names
// than the AOT host code (e.g. "PatchEntry" vs "NativeEntry").
PatchContext* ApplyPatchFromMemory(const void* data, size_t size,
                                    const char* host_type_name = nullptr) noexcept;

// Revert all patched methods in the given context.
// Clears kHotpatchActive flags on all affected dispatch table entries,
// frees PatchMethod cached IR, and deletes the PatchContext.
bool Unpatch(PatchContext* ctx) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_PATCH_LOADER_H_
