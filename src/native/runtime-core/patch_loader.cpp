#include "patch_loader.h"

#include "interpreter_entry.h"
#include <interpreter_vm.h>       // interpreter::IRMethod (for delete in DestroyPatchContext)
#include "memory_domain.h"        // DomainCurrentAllocateTagged / DomainFreeTagged
#include "module_registry.h"
#include "reflection_query_model.h"
#include "instantiation_engine.h"  // CachedCallInfo (for delete[] in DestroyPatchContext)

#include "bootstrap/bootstrap.h"

#include <atomic>

#include <cstdio>
#include <cstring>
#include <fmt/format.h>
#include <new>

#ifdef CHAOS_HOTPATCH_DEBUG
#define HOTPATCH_DIAG(fmt, ...) std::fprintf(stderr, fmt, ##__VA_ARGS__); std::fflush(stderr)
#else
#define HOTPATCH_DIAG(fmt, ...) ((void)0)
#endif

namespace chaos::il2cpp::runtime_core {

// Global patch generation counter. Incremented on each ApplyPatchFromMemory.
// Used by CallVirt MIC to invalidate stale cache entries. Relaxed ordering
// since the counter is compared but not used for synchronizing other state.
std::atomic<uint64_t> g_patch_generation{0};

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

const char* PatchMetadataCache::GetAotCoreIr(uint32_t method_index) const noexcept {
    if (header_ == nullptr || method_index >= header_->aot_core_ir_count) return nullptr;
    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    const auto* section = base + header_->aot_core_ir_offset;

    // Format: [index: uint32_t[count]] [json strings (null-terminated)]
    // index[i] = byte offset of i-th method's JSON from the start of strings.
    // GetAotCoreIr(i) = section_start + sizeof(uint32_t)*count + index[i]  (O(1))
    const auto* index = reinterpret_cast<const uint32_t*>(section);
    uint32_t offset = index[method_index];
    const char* str_start = reinterpret_cast<const char*>(
        section + sizeof(uint32_t) * header_->aot_core_ir_count);
    return str_start + offset;
}

PatchMetadataCache::RegisterIrBlock PatchMetadataCache::GetRegisterIr(uint32_t method_index) const noexcept {
    RegisterIrBlock result = {};
    if (header_ == nullptr || header_->version < 2) return result;
    if (header_->reg_ir_offset == 0 || header_->reg_ir_count == 0) return result;
    if (method_index >= header_->reg_ir_count) return result;

    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    const auto* section = base + header_->reg_ir_offset;

    // Format: [offsets: uint32_t[count]] [method blocks]
    const auto* offsets = reinterpret_cast<const uint32_t*>(section);
    uint32_t block_off = offsets[method_index];
    const auto* block = section + sizeof(uint32_t) * header_->reg_ir_count + block_off;

    // Block header: max_regs | instr_count | seh_count (3 × uint32_t)
    result.max_regs    = *reinterpret_cast<const uint32_t*>(block);      block += 4;
    result.instr_count = *reinterpret_cast<const uint32_t*>(block);      block += 4;
    result.seh_count   = *reinterpret_cast<const uint32_t*>(block);      block += 4;
    result.data        = reinterpret_cast<const void*>(block);

    return result;
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

uint32_t PatchMetadataCache::FieldCount() const noexcept {
    return header_ ? 0 : 0;
}

// GetFieldDef and GetTypeDefByIndex are declared in the header but
// their implementations depend on PatchDataHeader fields not yet added.
// Stubbed to return nullptr until the header format is extended.
#if 0
const PatchFieldDefEntry* PatchMetadataCache::GetFieldDef(uint32_t index) const noexcept {
    if (header_ == nullptr || index >= header_->field_def_count) return nullptr;
    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    const auto* entries = reinterpret_cast<const PatchFieldDefEntry*>(
        base + header_->field_def_offset);
    return &entries[index];
}

const PatchTypeDefEntry* PatchMetadataCache::GetTypeDefByIndex(uint32_t index) const noexcept {
    if (header_ == nullptr || index >= header_->type_def_count) return nullptr;
    const auto* base = reinterpret_cast<const uint8_t*>(header_);
    const auto* entries = reinterpret_cast<const PatchTypeDefEntry*>(
        base + header_->type_def_offset);
    return &entries[index];
}
#endif

const PatchTypeDefEntry* PatchMetadataCache::GetTypeDefByIndex(uint32_t /*index*/) const noexcept {
    return nullptr;
}

const PatchFieldDefEntry* PatchMetadataCache::GetFieldDef(uint32_t /*index*/) const noexcept {
    return nullptr;
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

const char* PatchMetadataCache::GetTypeNamespace(const PatchMethodDefEntry* method) const noexcept {
    if (method == nullptr) return "";

    uint32_t type_token = method->declaring_type_token;
    uint8_t table = static_cast<uint8_t>(type_token >> 24);

    if (table == 0x02) {
        auto* type_def = ResolveTypeDef(type_token);
        if (type_def != nullptr) {
            const char* ns = GetString(type_def->namespace_offset);
            return (ns != nullptr) ? ns : "";
        }
    } else if (table == 0x01) {
        auto* type_ref = ResolveTypeRef(type_token);
        if (type_ref != nullptr) {
            const char* ns = GetString(type_ref->namespace_offset);
            return (ns != nullptr) ? ns : "";
        }
    }

    return "";
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
    // This matches the subject ID format used by HotpatchNameRegistry.
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

    // Check cache first (O(1) via unordered_map).
    auto it = user_string_cache_.find(offset);
    if (it != user_string_cache_.end()) {
        return it->second.c_str();
    }

    // Not cached — convert UTF-16 to UTF-8 and emplace.
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

    auto emplaced = user_string_cache_.emplace(offset, std::move(utf8));
    return emplaced.first->second.c_str();
}

uint32_t PatchMetadataCache::ResolveToken(uint32_t token) const noexcept {
    // Not needed for the JSON-driven interpreter path.
    // AotCoreIr JSON already carries resolved references via targetReference.subjectId,
    // so DeserializeAotCoreIrMethod uses ResolveSubjectId rather than raw token resolution.
    // This stub exists for API completeness; it is never called during normal execution.
    (void)token;
    return 0;
}

// ── PatchContext management ──────────────────────────────────────────────

static PatchContext* CreatePatchContext(const PatchDataHeader* header, size_t total_size) {
    // Allocate PatchContext + PatchMetadataCache + PatchMethod array in one block.
    // Use domain-tagged allocation so the block is bulk-freed when the
    // assembly domain is destroyed (R10: MD/IL metadata → MemoryDomain).
    uint32_t method_count = header->method_def_count;
    size_t ctx_size = sizeof(PatchContext);
    size_t cache_size = sizeof(PatchMetadataCache);
    size_t methods_size = sizeof(PatchMethod) * method_count;

    auto* block = static_cast<uint8_t*>(
        memory_domain::DomainCurrentAllocateTagged(ctx_size + cache_size + methods_size));
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

    // Free cached IR and call_cache for each method.
    for (uint32_t i = 0; i < ctx->method_count; ++i) {
        auto& m = ctx->methods[i];

        // cached_ir is heap-allocated by PatchMethodLowerIR (new IRMethod).
        if (m.cached_ir != nullptr) {
            delete static_cast<interpreter::IRMethod*>(m.cached_ir);
            m.cached_ir = nullptr;
        }

        // call_cache is allocated by RebuildCallCacheForT3 / InlineLeafCallees
        // via CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE (domain-tagged).
        // NOTE: if ReapplyInlining replaced the array, the old allocation is leaked
        // (currently acceptable since InlineLeafCallees is only called once per method).
        if (m.call_cache != nullptr) {
            CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(m.call_cache);
            m.call_cache = nullptr;
        }

        // Free heap-allocated arg type cache if small-buffer was exceeded.
        if (m.cached_arg_types != nullptr &&
            m.cached_arg_types != m.cached_arg_types_small) {
            delete[] m.cached_arg_types;
            // No need to restore small-buffer pointer — struct is being destroyed.
        }
    }

    // Destroy the methods array.
    for (uint32_t i = 0; i < ctx->method_count; ++i) {
        ctx->methods[i].~PatchMethod();
    }

    // Free the block (domain-tagged allocation — DomainFreeTagged routes
    // to the originating heap or std::free for fallback allocations).
    memory_domain::DomainFreeTagged(ctx);
}

// ── Public API ──────────────────────────────────────────────────────────

PatchContext* ApplyPatchFromMemory(const void* data, size_t size,
                                    const char* host_type_name,
                                    const char* const* host_method_names) noexcept {
    if (data == nullptr || size < sizeof(PatchDataHeader)) return nullptr;

    auto* header = static_cast<const PatchDataHeader*>(data);
    HOTPATCH_DIAG("DIAG[APFM]: magic=%x ver=%u\n", header->magic, header->version);

    // Validate magic and version (accept v1 or v2).
    if (header->magic != PATCH_DATA_MAGIC) return nullptr;
    if (header->version != 1 && header->version != 2) return nullptr;
    // v1 header: 112 bytes, v2 header: 124 bytes
    uint32_t min_header = (header->version == 1) ? 112 : sizeof(PatchDataHeader);
    if (header->header_size < min_header) return nullptr;
    HOTPATCH_DIAG("DIAG[APFM]: validation OK (v%u header_size=%u)\n", header->version, header->header_size);

    // Validate structural integrity: total size must include AotCoreIr section.
    uint32_t expected_size = header->body_data_offset + header->body_data_size;
    uint32_t ir_section_end = header->aot_core_ir_offset + header->aot_core_ir_size;
    if (ir_section_end > expected_size)
        expected_size = ir_section_end;
    if (size < expected_size) return nullptr;
    HOTPATCH_DIAG("DIAG[APFM]: size OK\n");

    // Create context.
    auto* ctx = CreatePatchContext(header, size);
    if (ctx == nullptr) return nullptr;
    HOTPATCH_DIAG("DIAG[APFM]: ctx created\n");

    auto& registry = GetHotpatchNameRegistry();
    auto* cache = ctx->metadata_cache;
    HOTPATCH_DIAG("DIAG[APFM]: registry ready\n");

    // Iterate MethodDef entries and patch each one.
    uint32_t patched_count = 0;
    HOTPATCH_DIAG("DIAG[APFM]: iterating %u methods\n",
        static_cast<unsigned>(cache->MethodCount()));
    for (uint32_t i = 0; i < cache->MethodCount(); ++i) {
        auto* method_entry = cache->GetMethodDef(i);
        if (method_entry == nullptr) continue;

        // Skip methods with no body.
        if (method_entry->body_size == 0) continue;

        // Get type name, namespace, and method name for lookup.
        const char* type_name = cache->GetTypeName(method_entry);
        const char* type_ns = cache->GetTypeNamespace(method_entry);
        const char* method_name = cache->GetString(method_entry->name_offset);
        if (type_name == nullptr || method_name == nullptr) continue;

        // Use host_type_name override if provided (handles PatchEntry vs
        // NativeEntry naming mismatch between patch DLL and AOT code).
        const char* lookup_type = (host_type_name != nullptr) ? host_type_name : type_name;

        // Use host_method_names override if provided (handles MethodDef
        // name vs AOT registry method name mismatch).
        if (host_method_names != nullptr && host_method_names[i] != nullptr) {
            method_name = host_method_names[i];
        }

        // Look up the method in the HotpatchNameRegistry (namespace+typename composite key).
        uint64_t lookup = registry.LookupMethod(type_ns, lookup_type, method_name);
        if (lookup == 0) {
            // Try with full name format (includes assembly prefix).
            // For now, just skip unresolved methods.
            continue;
        }

        // Extract module_id and token from composite key.
        uint32_t module_id = ExtractModuleId(lookup);
        uint32_t aot_token = ExtractToken(lookup);

        // Get the dispatch slot for this method within its module.
        uint32_t slot = registry.TokenToSlot(module_id, aot_token);
        if (slot == ~0u) {
            continue;
        }

        // Set up the PatchMethod.
        auto& patch_method = ctx->methods[patched_count];
        HOTPATCH_DIAG("DIAG[APFM]: setting up method module=%u token=%u slot=%u\n",
            static_cast<unsigned>(module_id),
            static_cast<unsigned>(aot_token),
            static_cast<unsigned>(slot));
        patch_method.token = aot_token;
        patch_method.module_id = module_id;
        patch_method.aot_core_ir_json = cache->GetAotCoreIr(i);
        patch_method.aot_core_ir_json_length = 0;  // null-terminated, length determined by strlen

        // v2+: populate pre-allocated register IR data on PatchMethod.
        // PatchMethodLowerIR will skip JSON deserialization when this is present.
        if (header->version >= 2) {
            auto reg_ir = cache->GetRegisterIr(i);
            if (reg_ir.data != nullptr) {
                patch_method.reg_ir_data         = reg_ir.data;
                patch_method.reg_ir_instr_count  = reg_ir.instr_count;
                patch_method.reg_ir_seh_count    = reg_ir.seh_count;
                patch_method.reg_ir_max_regs     = reg_ir.max_regs;
            }
        }

        // Store reference to the metadata cache for call_target resolution during IR lowering.
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

        // Mark the dispatch entry as patched — module-scoped, no token collision.
        HOTPATCH_DIAG("DIAG[APFM]: calling SetPatchedBySlot module=%u slot=%u\n",
            static_cast<unsigned>(module_id), static_cast<unsigned>(slot));
        registry.SetPatchedBySlot(module_id, slot, true, &patch_method);
        HOTPATCH_DIAG("DIAG[APFM]: SetPatchedBySlot OK\n");

        ++patched_count;
    }

    // Update method count to reflect only successfully patched methods.
    ctx->method_count = patched_count;
    // Step 5: Set AOT bridge + image on the metadata cache so that
    // ResolveSubjectId in InterpreterEntryDirect can resolve call_target
    // via the reflection query model during IR lowering.
    {
        const auto* bs = chaos::il2cpp::bootstrap::PeekBootstrapState();
        if (bs != nullptr && bs->is_bootstrapped) {
            const auto* bridge = chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
            if (bridge != nullptr && bs->aot_image_handle != 0) {
                ctx->metadata_cache->SetAotBridge(bridge, bs->aot_image_handle);
            }
        }
    }
    HOTPATCH_DIAG("DIAG[APFM]: returning ctx method_count=%u\n", static_cast<unsigned>(ctx->method_count));

    // Bump patch generation for CallVirt MIC cache invalidation.
    g_patch_generation.fetch_add(1, std::memory_order_relaxed);

    return ctx;
}

bool Unpatch(PatchContext* ctx) noexcept {
    if (ctx == nullptr) return false;

    auto& registry = GetHotpatchNameRegistry();

    // Clear kHotpatchActive flag on each patched method.
    for (uint32_t i = 0; i < ctx->method_count; ++i) {
        auto& method = ctx->methods[i];
        if (method.token != 0) {
            uint32_t slot = registry.TokenToSlot(method.module_id, method.token);
            if (slot != ~0u) {
                registry.SetPatchedBySlot(method.module_id, slot, false, nullptr);
            }
        }
    }

    DestroyPatchContext(ctx);
    return true;
}

}  // namespace chaos::il2cpp::runtime_core
