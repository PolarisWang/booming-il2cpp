#include "patch_loader.h"

#include <cstdio>
#include <cstring>
#include <fmt/format.h>
#include <new>

namespace chaos::il2cpp::runtime_core {

// ── PatchMetadataCache implementation ────────────────────────────────────

PatchMetadataCache::PatchMetadataCache(const PatchDataHeader* header) noexcept
    : header_(header) {
}

const char* PatchMetadataCache::GetString(uint32_t offset) const noexcept {
    if (offset == 0 || header_ == nullptr) return "";
    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    return reinterpret_cast<const char*>(base + header_->string_heap_offset + offset);
}

const void* PatchMetadataCache::GetBlob(uint32_t offset) const noexcept {
    if (offset == 0 || header_ == nullptr) return nullptr;
    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    return base + header_->blob_heap_offset + offset;
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

const PatchMemberRefEntry* PatchMetadataCache::ResolveMemberRef(uint32_t token) const noexcept {
    uint32_t index = (token & 0x00FFFFFFu) - 1;
    if (header_ == nullptr || index >= header_->member_ref_count) return nullptr;
    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    const auto* entries = reinterpret_cast<const PatchMemberRefEntry*>(
        base + header_->member_ref_offset);
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
        auto result = fmt::format_to_n(buffer, sizeof(buffer) - 1, "System.Private.CoreLib/{}.{}:{}",
                                       ns, type_name, method_name);
        if (result.size >= sizeof(buffer)) {
            std::strncpy(buffer, type_name, sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
        }
    } else {
        // TypeName:MethodName (no namespace)
        auto result = fmt::format_to_n(buffer, sizeof(buffer) - 1, "System.Private.CoreLib/{}:{}",
                                       type_name, method_name);
        if (result.size >= sizeof(buffer)) {
            std::strncpy(buffer, type_name, sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
        }
    }

    return buffer;
}

const char* PatchMetadataCache::GetUserString(uint32_t token) const noexcept {
    // Token format: high byte = 0x70 (UserString), low 24 bits = offset into #US heap.
    uint32_t offset = token & 0x00FFFFFFu;
    if (header_ == nullptr || offset >= header_->user_string_heap_size) return nullptr;

    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    const auto* us_start = base + header_->user_string_heap_offset;

    // Read compressed unsigned int for blob length (includes 1 byte padding).
    uint32_t blob_len = 0;
    uint32_t bytes_read = 0;
    const uint8_t* len_ptr = us_start + offset;
    if ((*len_ptr & 0x80) == 0) {
        blob_len = *len_ptr;
        bytes_read = 1;
    } else if ((*len_ptr & 0xC0) == 0x80) {
        blob_len = static_cast<uint32_t>((*len_ptr & 0x3F) << 8) | (*(len_ptr + 1));
        bytes_read = 2;
    } else {
        return nullptr; // 4-byte encoding not expected for user strings
    }

    if (blob_len < 2) return nullptr; // minimum: 1 char + padding

    // String data is UTF-16LE, blob_len includes the 1-byte padding.
    uint32_t utf16_byte_count = blob_len - 1;
    uint32_t char_count = utf16_byte_count / 2;

    if (char_count == 0) return nullptr;

    const uint16_t* utf16_data = reinterpret_cast<const uint16_t*>(len_ptr + bytes_read);

    // Convert UTF-16 to UTF-8 and cache.
    // We cache lazily to avoid converting all strings upfront.
    // Use a simple scheme: the cache index = offset into #US heap.
    // Since offsets into the US heap are sparse and the cache is a vector,
    // we store a parallel offset-to-index mapping.
    // For simplicity, check if we already have this offset cached in the vector.
    // Each cached entry is stored as "offset\0utf8_data" in a single string.
    // Instead, use a simple approach: store in a map-like structure.

    // Simple linear scan of cache (small number of ldstr per method).
    // Cache entries are stored as: [4-byte offset][utf8_string\0]
    // We need to find or create an entry for this offset.

    // For now, allocate and convert on each call, relying on the fact that
    // LowerILToIR calls the resolver once per token during lowering.
    // The string is stored in IRInstruction.string_operand which points to
    // the const char* from our cache, so we need it to live long enough.

    // Check cache first.
    for (const auto& entry : user_string_cache_) {
        // Each entry starts with the offset (4 bytes LE) followed by the string.
        if (entry.size() >= 4) {
            uint32_t cached_offset =
                static_cast<uint32_t>(static_cast<uint8_t>(entry[0])) |
                (static_cast<uint32_t>(static_cast<uint8_t>(entry[1])) << 8) |
                (static_cast<uint32_t>(static_cast<uint8_t>(entry[2])) << 16) |
                (static_cast<uint32_t>(static_cast<uint8_t>(entry[3])) << 24);
            if (cached_offset == offset) {
                return entry.data() + 4;
            }
        }
    }

    // Not cached — convert UTF-16 to UTF-8.
    // Allocate worst-case UTF-8 buffer (each UTF-16 code unit → up to 3 bytes).
    std::string utf8;
    utf8.reserve(char_count * 3);

    for (uint32_t i = 0; i < char_count; ++i) {
        uint16_t uc = utf16_data[i];

        if (uc < 0x80) {
            utf8 += static_cast<char>(uc);
        } else if (uc < 0x800) {
            utf8 += static_cast<char>(0xC0 | (uc >> 6));
            utf8 += static_cast<char>(0x80 | (uc & 0x3F));
        } else {
            utf8 += static_cast<char>(0xE0 | (uc >> 12));
            utf8 += static_cast<char>(0x80 | ((uc >> 6) & 0x3F));
            utf8 += static_cast<char>(0x80 | (uc & 0x3F));
        }
    }

    // Store with offset prefix: [4-byte LE offset][utf8\0]
    // std::string can contain embedded nulls, but we'll use a vector of strings
    // where each entry is "offset_prefix + utf8_data + null".
    std::string cache_entry;
    cache_entry.reserve(4 + utf8.size() + 1);
    cache_entry.push_back(static_cast<char>(offset & 0xFF));
    cache_entry.push_back(static_cast<char>((offset >> 8) & 0xFF));
    cache_entry.push_back(static_cast<char>((offset >> 16) & 0xFF));
    cache_entry.push_back(static_cast<char>((offset >> 24) & 0xFF));
    cache_entry += utf8;
    cache_entry.push_back('\0');

    const char* result = cache_entry.data() + 4;
    user_string_cache_.push_back(std::move(cache_entry));
    return result;
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
    std::fprintf(stderr, "DIAG[APFM]: magic=%x ver=%u\n", header->magic, header->version); std::fflush(stderr);

    // Validate magic and version.
    if (header->magic != PATCH_DATA_MAGIC) return nullptr;
    if (header->version != PATCH_DATA_VERSION) return nullptr;
    if (header->header_size < sizeof(PatchDataHeader)) return nullptr;
    std::fprintf(stderr, "DIAG[APFM]: validation OK\n"); std::fflush(stderr);

    // Validate structural integrity: total size must match.
    uint32_t expected_size = header->body_data_offset + header->body_data_size;
    if (size < expected_size) return nullptr;
    std::fprintf(stderr, "DIAG[APFM]: size OK\n"); std::fflush(stderr);

    // Create context.
    auto* ctx = CreatePatchContext(header, size);
    if (ctx == nullptr) return nullptr;
    std::fprintf(stderr, "DIAG[APFM]: ctx created\n"); std::fflush(stderr);

    auto& registry = GetNameIndexRegistry();
    auto* cache = ctx->metadata_cache;
    std::fprintf(stderr, "DIAG[APFM]: registry ready\n"); std::fflush(stderr);

    // Iterate MethodDef entries and patch each one.
    uint32_t patched_count = 0;
    std::fprintf(stderr, "DIAG[APFM]: iterating %u methods\n",
        static_cast<unsigned>(cache->MethodCount())); std::fflush(stderr);
    for (uint32_t i = 0; i < cache->MethodCount(); ++i) {
        auto* method_entry = cache->GetMethodDef(i);
        if (method_entry == nullptr) continue;

        // Skip methods with no body.
        if (method_entry->body_size == 0) continue;

        // Get type name and method name for lookup.
        const char* type_name = cache->GetTypeName(method_entry);
        const char* method_name = cache->GetString(method_entry->name_offset);
        if (type_name == nullptr || method_name == nullptr) continue;

        // Use host_type_name override if provided (handles PatchEntry vs
        // NativeEntry naming mismatch between patch DLL and AOT code).
        const char* lookup_type = (host_type_name != nullptr) ? host_type_name : type_name;

        // Look up the method in the NameIndexRegistry.
        uint32_t aot_token = registry.LookupMethod(lookup_type, method_name);
        if (aot_token == 0) {
            // Try with full name format (includes assembly prefix).
            // For now, just skip unresolved methods.
            continue;
        }

        // Set up the PatchMethod.
        auto& patch_method = ctx->methods[patched_count];
        std::fprintf(stderr, "DIAG[APFM]: setting up method token=%u\n",
            static_cast<unsigned>(aot_token)); std::fflush(stderr);
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

        // Store reference to the metadata cache for token resolution during IR lowering.
        patch_method.metadata_cache = cache;

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
        std::fprintf(stderr, "DIAG[APFM]: calling SetPatched token=%u\n", static_cast<unsigned>(aot_token)); std::fflush(stderr);
        registry.SetPatched(aot_token, true, &patch_method);
        std::fprintf(stderr, "DIAG[APFM]: SetPatched OK\n"); std::fflush(stderr);

        ++patched_count;
    }

    // Update method count to reflect only successfully patched methods.
    ctx->method_count = patched_count;
    std::fprintf(stderr, "DIAG[APFM]: returning ctx method_count=%u\n", static_cast<unsigned>(ctx->method_count)); std::fflush(stderr);

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
