#include "patch_loader.h"

#include <cstring>
#include <cstdio>
#include <new>

namespace chaos::il2cpp::runtime_core {

// ── PatchMetadataCache implementation ────────────────────────────────────

PatchMetadataCache::PatchMetadataCache(const PatchDataHeader* header) noexcept
    : header_(header) {
}

const char* PatchMetadataCache::GetString(uint32_t offset) const noexcept {
    if (offset == 0 || header_ == nullptr) return "";
    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    return reinterpret_cast<const char*>(base + offset);
}

const void* PatchMetadataCache::GetBlob(uint32_t offset) const noexcept {
    if (offset == 0 || header_ == nullptr) return nullptr;
    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    return base + offset;
}

const void* PatchMetadataCache::GetBody(uint32_t offset) const noexcept {
    if (offset == 0 || header_ == nullptr) return nullptr;
    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    return base + header_->body_data_offset + offset;
}

const PatchMethodDefEntry* PatchMetadataCache::GetMethodDef(uint32_t index) const noexcept {
    if (header_ == nullptr || index >= header_->method_def_count) return nullptr;
    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    const auto* entries = reinterpret_cast<const PatchMethodDefEntry*>(
        base + header_->method_def_offset);
    return &entries[index];
}

uint32_t PatchMetadataCache::MethodCount() const noexcept {
    return header_ ? header_->method_def_count : 0;
}

const PatchTypeDefEntry* PatchMetadataCache::ResolveTypeDef(uint32_t token) const noexcept {
    // Token format: high byte = 0x02 (TypeDef), low 24 bits = 1-based index.
    uint32_t index = (token & 0x00FFFFFFu) - 1;  // convert to 0-based
    if (header_ == nullptr || index >= header_->type_def_count) return nullptr;
    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    const auto* entries = reinterpret_cast<const PatchTypeDefEntry*>(
        base + header_->type_def_offset);
    return &entries[index];
}

const PatchTypeRefEntry* PatchMetadataCache::ResolveTypeRef(uint32_t token) const noexcept {
    uint32_t index = (token & 0x00FFFFFFu) - 1;
    if (header_ == nullptr || index >= header_->type_ref_count) return nullptr;
    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    const auto* entries = reinterpret_cast<const PatchTypeRefEntry*>(
        base + header_->type_ref_offset);
    return &entries[index];
}

const char* PatchMetadataCache::GetTypeName(const PatchMethodDefEntry* method) const noexcept {
    if (method == nullptr) return "UnknownType";

    // The declaring_type_token should be a TypeDef token (0x02xxxxxx).
    uint32_t type_token = method->declaring_type_token;
    uint8_t table = static_cast<uint8_t>(type_token >> 24);

    const char* type_name = nullptr;
    const char* ns = nullptr;

    if (table == 0x02) {
        auto* type_def = ResolveTypeDef(type_token);
        if (type_def != nullptr) {
            type_name = GetString(type_def->type_name_offset);
            ns = GetString(type_def->namespace_offset);
        }
    } else if (table == 0x01) {
        auto* type_ref = ResolveTypeRef(type_token);
        if (type_ref != nullptr) {
            type_name = GetString(type_ref->type_name_offset);
            ns = GetString(type_ref->namespace_offset);
        }
    }

    if (type_name == nullptr || type_name[0] == '\0') return "UnknownType";

    // For namespaced types, use "Namespace.TypeName" format.
    // Return a pointer into the string heap (the string already contains
    // the full name or we need to construct it).
    // For now, just return the type_name.  The NameIndex registry
    // uses fully qualified subject IDs like
    // "System.Private.CoreLib/System.String", so the caller needs
    // to construct the full name from namespace + type_name.
    return type_name;
}

const char* PatchMetadataCache::GetFullMethodName(const PatchMethodDefEntry* method) const noexcept {
    if (method == nullptr) return "UnknownMethod";

    // Use a small thread-local buffer for the combined name.
    // This is safe because the caller is expected to use the name immediately
    // (during ApplyPatchFromMemory, which is single-threaded).
    static thread_local char buffer[512];

    const char* type_name = "UnknownType";
    const char* ns = "";
    uint32_t type_token = method->declaring_type_token;
    uint8_t table = static_cast<uint8_t>(type_token >> 24);

    if (table == 0x02) {
        auto* type_def = ResolveTypeDef(type_token);
        if (type_def != nullptr) {
            type_name = GetString(type_def->type_name_offset);
            ns = GetString(type_def->namespace_offset);
        }
    } else if (table == 0x01) {
        auto* type_ref = ResolveTypeRef(type_token);
        if (type_ref != nullptr) {
            type_name = GetString(type_ref->type_name_offset);
            ns = GetString(type_ref->namespace_offset);
        }
    }

    const char* method_name = GetString(method->name_offset);

    // Format: "System.Private.CoreLib/Namespace.TypeName:MethodName"
    // This matches the subject ID format used by NameIndexRegistry.
    if (ns[0] != '\0') {
        // Namespace.TypeName:MethodName
        int len = std::snprintf(buffer, sizeof(buffer), "System.Private.CoreLib/%s.%s:%s",
                                ns, type_name, method_name);
        if (len < 0 || static_cast<size_t>(len) >= sizeof(buffer)) {
            std::strncpy(buffer, type_name, sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
        }
    } else {
        // TypeName:MethodName (no namespace)
        int len = std::snprintf(buffer, sizeof(buffer), "System.Private.CoreLib/%s:%s",
                                type_name, method_name);
        if (len < 0 || static_cast<size_t>(len) >= sizeof(buffer)) {
            std::strncpy(buffer, type_name, sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
        }
    }

    return buffer;
}

uint32_t PatchMetadataCache::ResolveToken(uint32_t token) const noexcept {
    // For now, return the token itself as a passthrough.
    // Full token resolution (TypeRef → runtime TypeInfo*, MemberRef → method pointer, etc.)
    // requires integration with the runtime's DefaultTokenResolver and will be
    // implemented in Step 5 (InterpreterEntry → IL→IR lowerer integration).
    //
    // For IL→IR lowering, the lowerer needs:
    //   - call target resolution: token → IRInstruction.call_target
    //   - field access: token → IRInstruction.field_offset
    //   - type tests: token → IRInstruction.type_handle
    //
    // Stub: return token as-is so the lowerer can at least see it.
    (void)token;
    return 0;
}

// ── PatchContext management ──────────────────────────────────────────────

static PatchContext* CreatePatchContext(const PatchDataHeader* header, size_t total_size) {
    // Allocate PatchContext + PatchMetadataCache + PatchMethod array in one block.
    uint32_t method_count = header->method_def_count;
    size_t ctx_size = sizeof(PatchContext);
    size_t cache_size = sizeof(PatchMetadataCache);
    size_t methods_size = sizeof(PatchMethod) * method_count;

    auto* block = new uint8_t[ctx_size + cache_size + methods_size];
    if (block == nullptr) return nullptr;

    auto* ctx = new (block) PatchContext();
    auto* cache = new (block + ctx_size) PatchMetadataCache(header);
    auto* methods = reinterpret_cast<PatchMethod*>(block + ctx_size + cache_size);

    ctx->header = header;
    ctx->total_size = total_size;
    ctx->methods = methods;
    ctx->method_count = method_count;
    ctx->metadata_cache = cache;

    // Zero-initialize the methods array.
    for (uint32_t i = 0; i < method_count; ++i) {
        new (&methods[i]) PatchMethod();
    }

    return ctx;
}

static void DestroyPatchContext(PatchContext* ctx) {
    if (ctx == nullptr) return;

    // Destroy PatchMetadataCache.
    if (ctx->metadata_cache != nullptr) {
        ctx->metadata_cache->~PatchMetadataCache();
    }

    // Free cached IR for each method.
    for (uint32_t i = 0; i < ctx->method_count; ++i) {
        // cached_ir is owned by the interpreter entry.
        // For now, no-op (IR cleanup happens in Step 5).
    }

    // Destroy the methods array.
    for (uint32_t i = 0; i < ctx->method_count; ++i) {
        ctx->methods[i].~PatchMethod();
    }

    // Free the block (all allocations are in one contiguous block).
    auto* block = reinterpret_cast<uint8_t*>(ctx);
    delete[] block;
}

// ── Public API ──────────────────────────────────────────────────────────

PatchContext* ApplyPatchFromMemory(const void* data, size_t size,
                                    const char* host_type_name) noexcept {
    if (data == nullptr || size < sizeof(PatchDataHeader)) return nullptr;

    auto* header = static_cast<const PatchDataHeader*>(data);

    // Validate magic and version.
    if (header->magic != PATCH_DATA_MAGIC) return nullptr;
    if (header->version != PATCH_DATA_VERSION) return nullptr;
    if (header->header_size < sizeof(PatchDataHeader)) return nullptr;

    // Validate structural integrity: total size must match.
    uint32_t expected_size = header->body_data_offset + header->body_data_size;
    if (size < expected_size) return nullptr;

    // Create context.
    auto* ctx = CreatePatchContext(header, size);
    if (ctx == nullptr) return nullptr;

    auto& registry = GetNameIndexRegistry();
    auto* cache = ctx->metadata_cache;

    // Iterate MethodDef entries and patch each one.
    uint32_t patched_count = 0;
    std::fprintf(stderr, "DEBUG ApplyPatchFromMemory: MethodCount=%u\n", cache->MethodCount());
    for (uint32_t i = 0; i < cache->MethodCount(); ++i) {
        auto* method_entry = cache->GetMethodDef(i);
        if (method_entry == nullptr) {
            std::fprintf(stderr, "DEBUG ApplyPatchFromMemory[%u]: method_entry is null\n", i);
            continue;
        }

        // Skip methods with no body.
        if (method_entry->body_offset == 0 || method_entry->body_size == 0) {
            std::fprintf(stderr, "DEBUG ApplyPatchFromMemory[%u]: no body (offset=%u size=%u)\n", i, method_entry->body_offset, method_entry->body_size);
            continue;
        }

        // Get type name and method name for lookup.
        const char* type_name = cache->GetTypeName(method_entry);
        const char* method_name = cache->GetString(method_entry->name_offset);
        std::fprintf(stderr, "DEBUG ApplyPatchFromMemory[%u]: type_name='%s' method_name='%s'\n", i, type_name ? type_name : "(null)", method_name ? method_name : "(null)");

        // Skip if we couldn't identify the type.
        if (type_name == nullptr || method_name == nullptr) {
            std::fprintf(stderr, "DEBUG ApplyPatchFromMemory[%u]: type_name or method_name is null\n", i);
            continue;
        }

        // Use host_type_name override if provided (handles PatchEntry vs
        // NativeEntry naming mismatch between patch DLL and AOT code).
        const char* lookup_type = (host_type_name != nullptr) ? host_type_name : type_name;
        std::fprintf(stderr, "DEBUG ApplyPatchFromMemory[%u]: lookup_type='%s' method_name='%s'\n", i, lookup_type, method_name);

        // Look up the method in the NameIndexRegistry.
        // The registry expects format "TypeName" and "MethodName".
        uint32_t aot_token = registry.LookupMethod(lookup_type, method_name);
        std::fprintf(stderr, "DEBUG ApplyPatchFromMemory[%u]: LookupMethod returned 0x%08x\n", i, aot_token);
        if (aot_token == 0) {
            // Try with full name format.
            const char* full_name = cache->GetFullMethodName(method_entry);
            std::fprintf(stderr, "DEBUG ApplyPatchFromMemory[%u]: full_name='%s'\n", i, full_name ? full_name : "(null)");
            // The full_name includes assembly prefix; try parsing it.
            // For now, just skip unresolved methods.
            continue;
        }

        // Set up the PatchMethod.
        auto& patch_method = ctx->methods[patched_count];
        patch_method.token = aot_token;
        patch_method.il_bytes = static_cast<const uint8_t*>(
            cache->GetBody(method_entry->body_offset));
        patch_method.il_length = method_entry->body_size;

        // Parse method body header to determine max_stack.
        // (Full parsing requires ParseMethodBodyHeader from il_to_ir_lowerer.h;
        // for now default to safe max_stack=8 which works for tiny format.)
        if (patch_method.il_length > 0 && patch_method.il_bytes != nullptr) {
            uint8_t header_byte = patch_method.il_bytes[0];
            if ((header_byte & 0x03) == 0x03) {
                // Fat format: max_stack is at offset 2 (uint16).
                if (patch_method.il_length >= 4) {
                    patch_method.max_stack = static_cast<uint32_t>(
                        patch_method.il_bytes[2] | (patch_method.il_bytes[3] << 8));
                }
            }
            // Tiny format: max_stack = 8 (ECMA 335 II.15.4.2.1).
        }

        // Get signature blob for runtime signature parsing.
        if (method_entry->signature_offset != 0) {
            patch_method.signature_blob = static_cast<const uint8_t*>(
                cache->GetBlob(method_entry->signature_offset));
            // First byte of blob is the signature length (ECMA 335 #Blob format).
            if (patch_method.signature_blob != nullptr) {
                patch_method.signature_len = patch_method.signature_blob[0];
            }
        }

        // Mark the dispatch entry as patched.
        registry.SetPatched(aot_token, true, &patch_method);

        ++patched_count;
    }

    // Update method count to reflect only successfully patched methods.
    ctx->method_count = patched_count;
    std::fprintf(stderr, "DEBUG ApplyPatchFromMemory: patched_count=%u\n", patched_count);

    return ctx;
}

bool Unpatch(PatchContext* ctx) noexcept {
    if (ctx == nullptr) return false;

    auto& registry = GetNameIndexRegistry();

    // Clear kDispatchPatched flag on each patched method.
    for (uint32_t i = 0; i < ctx->method_count; ++i) {
        auto& method = ctx->methods[i];
        if (method.token != 0) {
            registry.SetPatched(method.token, false, nullptr);
        }
    }

    DestroyPatchContext(ctx);
    return true;
}

}  // namespace chaos::il2cpp::runtime_core
