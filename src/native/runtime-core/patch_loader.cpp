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
    return header_ ? header_->field_def_count : 0;
}

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

    // Overflow protection for the methods allocation.
    size_t methods_size = 0;
    if (method_count > 0) {
        if (method_count > SIZE_MAX / sizeof(PatchMethod)) {
            return nullptr;  // multiplication overflow
        }
        methods_size = sizeof(PatchMethod) * method_count;
    }

    // Overflow protection for the total block allocation.
    size_t total_alloc = ctx_size + cache_size;
    if (methods_size > SIZE_MAX - total_alloc) {
        return nullptr;  // addition overflow
    }
    total_alloc += methods_size;

    auto* block = static_cast<uint8_t*>(
        memory_domain::DomainCurrentAllocateTagged(total_alloc));
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
        // ReapplyInlining and InlineLeafCallees free the old cache before
        // replacement, so only the current call_cache reaches this point.
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

// ── Global deferred patch queue ──────────────────────────────────────────
// Patches with unsatisfied dependencies (multi-module scenario) are stored
// here and retried when a new module registers in HotpatchNameRegistry.

namespace {

struct DeferredPatchEntry {
    const void*     data;               // copy of patch data
    size_t          size;               // data size
    const char*     host_type_name;     // optional host type name override
    const char* const* host_method_names; // optional host method name overrides
    uint32_t        retry_count;        // number of retry attempts
    bool            applied;            // true when successfully applied
};

static CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(uintptr_t, DeferredPatchEntry) g_deferred_patches;

// Address of RegisterHotpatchModule used as a callback trigger.
// Set in TryApplyDeferredPatches to detect new module registrations.
static uint32_t g_last_module_count = 0;

} // anonymous namespace

// Retry all deferred patches. Called after each RegisterHotpatchModule.
// Returns the number of patches successfully applied.
uint32_t TryApplyDeferredPatches() noexcept
{
    auto& registry = GetHotpatchNameRegistry();
    uint32_t current_count = static_cast<uint32_t>(registry.ModuleCount());
    if (current_count == g_last_module_count)
        return 0; // no new modules registered

    g_last_module_count = current_count;
    uint32_t applied_count = 0;

    for (auto& [key, entry] : g_deferred_patches)
    {
        if (entry.applied)
            continue;

        // Re-validate dependencies: check if all dependency modules are now registered.
        const auto* header = static_cast<const PatchDataHeader*>(entry.data);
        bool all_deps_satisfied = true;

        if (header->version >= 3 && header->dependency_count > 0)
        {
            const auto* dep_entries = reinterpret_cast<const PatchDataDependency*>(
                reinterpret_cast<const uint8_t*>(header) + header->dependency_offset);

            for (uint32_t di = 0; di < header->dependency_count; ++di)
            {
                const char* dep_name = PatchData_String(header, dep_entries[di].assembly_name_offset);
                if (dep_name == nullptr || dep_name[0] == '\0')
                    continue;

                bool found = false;
                for (size_t mi = 0; mi < registry.ModuleCount(); ++mi)
                {
                    const auto* mod = registry.GetModuleByIndex(mi);
                    if (mod != nullptr && mod->module_name != nullptr &&
                        std::strcmp(mod->module_name, dep_name) == 0)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    // Optional dependency: skip without blocking.
                    if (dep_entries[di].min_version == 0)
                        continue;

                    all_deps_satisfied = false;
                    break;
                }
            }
        }

        if (!all_deps_satisfied)
        {
            entry.retry_count++;
            continue;
        }

        // Dependencies satisfied — apply the patch now.
        // We can't recursively call ApplyPatchFromMemory since that would
        // create a new PatchContext. Instead, inline the patching logic
        // for the deferred entry.
        auto* ctx = ApplyPatchFromMemory(entry.data, entry.size,
                                          entry.host_type_name,
                                          entry.host_method_names);
        if (ctx != nullptr)
        {
            entry.applied = true;
            applied_count++;
            HOTPATCH_DIAG("DIAG[DEFERRED]: applied deferred patch (retries=%u)\n",
                static_cast<unsigned>(entry.retry_count));
        }
        else
        {
            entry.retry_count++;
        }
    }

    return applied_count;
}

void InitializeMultiModulePatchSupport() noexcept
{
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    SetModuleRegisteredCallback([]() noexcept {
        TryApplyDeferredPatches();
    });
}

PatchContext* ApplyPatchFromMemory(const void* data, size_t size,
                                    const char* host_type_name,
                                    const char* const* host_method_names) noexcept {
    if (data == nullptr || size < sizeof(PatchDataHeader)) return nullptr;

    auto* header = static_cast<const PatchDataHeader*>(data);
    HOTPATCH_DIAG("DIAG[APFM]: magic=%x ver=%u\n", header->magic, header->version);

    // Validate magic and version (accept v1, v2, or v3).
    if (header->magic != PATCH_DATA_MAGIC) return nullptr;
    if (header->version != 1 && header->version != 2 && header->version != 3) return nullptr;
    // v1 header: 112 bytes, v2 header: 124 bytes, v3 header: 132 bytes
    uint32_t min_header = (header->version == 1) ? 112 :
                          (header->version == 2) ? 124 : sizeof(PatchDataHeader);
    if (header->header_size < min_header) return nullptr;
    HOTPATCH_DIAG("DIAG[APFM]: validation OK (v%u header_size=%u)\n", header->version, header->header_size);

    // Validate structural integrity: total size must include AotCoreIr section.
    uint32_t expected_size = header->body_data_offset + header->body_data_size;
    uint32_t ir_section_end = header->aot_core_ir_offset + header->aot_core_ir_size;
    if (ir_section_end > expected_size)
        expected_size = ir_section_end;
    if (size < expected_size) return nullptr;
    HOTPATCH_DIAG("DIAG[APFM]: size OK\n");

    // ── Multi-module dependency validation (v3+) ───────────────────────
    // Check that all declared dependencies have a registered module in the
    // HotpatchNameRegistry. If a required dependency (min_version > 0) is
    // missing, queue this patch for deferred application.
    // Optional dependencies (min_version == 0) are informational — the
    // PatchDataExtractor emits all AssemblyRef entries as dependencies,
    // and system/framework references won't have a matching module in the
    // registry. Only hotpatch module dependencies need to be verified.
    if (header->version >= 3 && header->dependency_count > 0)
    {
        auto& registry = GetHotpatchNameRegistry();
        const auto* dep_entries = reinterpret_cast<const PatchDataDependency*>(
            reinterpret_cast<const uint8_t*>(header) + header->dependency_offset);
        bool all_deps_satisfied = true;

        for (uint32_t di = 0; di < header->dependency_count; ++di)
        {
            const char* dep_name = PatchData_String(header, dep_entries[di].assembly_name_offset);
            if (dep_name == nullptr || dep_name[0] == '\0')
            {
                HOTPATCH_DIAG("DIAG[APFM]: dependency %u has empty name, skipping\n",
                    static_cast<unsigned>(di));
                continue;
            }

            // Verify that a registered module matches this dependency.
            // We check by scanning registered module assembly names (module_name field).
            bool found = false;
            for (size_t mi = 0; mi < registry.ModuleCount(); ++mi)
            {
                const auto* mod = registry.GetModuleByIndex(mi);
                if (mod != nullptr && mod->module_name != nullptr &&
                    std::strcmp(mod->module_name, dep_name) == 0)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                // Optional dependency (min_version == 0): not found in registry,
                // likely a system/framework reference — skip without blocking.
                if (dep_entries[di].min_version == 0)
                {
                    HOTPATCH_DIAG("DIAG[APFM]: optional dependency '%s' not in registry, skipping\n",
                        dep_name);
                    continue;
                }

                // Required dependency — must be satisfied or we defer.
                HOTPATCH_DIAG("DIAG[APFM]: missing required dependency '%s' for multi-module patch\n",
                    dep_name);
                all_deps_satisfied = false;
                break;
            }
        }

        if (!all_deps_satisfied)
        {
            // Queue this patch for deferred application.
            // Store a copy of the patch data so it survives if the original
            // mapping is unmapped before the dependency arrives.
            HOTPATCH_DIAG("DIAG[APFM]: deferred patch queued (dependencies not yet loaded)\n");

            auto* data_copy = static_cast<uint8_t*>(
                memory_domain::DomainCurrentAllocateTagged(size));
            if (data_copy == nullptr)
                return nullptr;

            std::memcpy(data_copy, data, size);

            uintptr_t key = reinterpret_cast<uintptr_t>(data_copy);
            g_deferred_patches[key] = DeferredPatchEntry{
                data_copy, size, host_type_name, host_method_names, 0, false
            };
            return nullptr;
        }
    }

    // Create context.
    auto* ctx = CreatePatchContext(header, size);
    if (ctx == nullptr) return nullptr;
    HOTPATCH_DIAG("DIAG[APFM]: ctx created\n");

    auto& registry = GetHotpatchNameRegistry();
    auto* cache = ctx->metadata_cache;
    HOTPATCH_DIAG("DIAG[APFM]: registry ready\n");

    // Capture the current domain for dispatch-entry tracking.
    // When this patch context belongs to a hot-update domain, the domain_id
    // is recorded so ClearDomainDispatchEntries can find and clear these
    // entries during domain unload.
    uint32_t patch_domain_id = 0;
    {
        auto* md = memory_domain::CurrentDomain();
        if (md != nullptr) patch_domain_id = md->domain_id;
    }

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
        // Save original keep-native flag before SetPatchedBySlot clears it.
        // Phase 3 below restores the flag when keep_native==true, allowing
        // the dispatch entry to keep its kHotpatchKeepNative through the
        // patch/unpatch cycle so revert dispatch matches baseline behavior.
        {
            auto* orig_entry = registry.GetDispatchEntryBySlot(module_id, slot);
            if (orig_entry && HotpatchShouldKeepNative(*orig_entry)) {
                patch_method.keep_native = true;
            }
        }

        registry.SetPatchedBySlot(module_id, slot, true, &patch_method, patch_domain_id);
        HOTPATCH_DIAG("DIAG[APFM]: SetPatchedBySlot OK\n");

        // ── Phase 3: DHE — re-set keep-native flag for unchanged methods ──
        // SetPatchedBySlot unconditionally clears kHotpatchKeepNative. For
        // methods whose IL hasn't changed (keep_native=true), restore the flag
        // so ChaosDispatchMethod skips interpreter entry for AOT-speed execution.
        if (patch_method.keep_native) {
            auto* entry = registry.GetDispatchEntryBySlot(module_id, slot);
            if (entry != nullptr) {
#if defined(_MSC_VER)
                _InterlockedOr(reinterpret_cast<volatile long*>(&entry->flags),
                               static_cast<long>(kHotpatchKeepNative));
#else
                __sync_fetch_and_or(reinterpret_cast<volatile uint32_t*>(&entry->flags),
                                    static_cast<uint32_t>(kHotpatchKeepNative));
#endif
            }

            // ── Cache dispatch entry for O(1) native code dispatch ──────
            // Save the HotpatchEntryV0* for this method so Step A/A0 can
            // read direct_ptr directly without registry lookup.
            // When direct_ptr is a trampoline (pre-JIT), unwrap it via
            // OriginalAotPtrCallback to get the real AOT code address.
            if (entry != nullptr) {
                patch_method.dispatch_entry = entry;
                auto original_cb = GetOriginalAotPtrCallback();
                if (original_cb != nullptr) {
                    auto unwrapped = original_cb(entry);
                    if (unwrapped != nullptr) {
                        entry->direct_ptr = unwrapped;
                    }
                }
            }
        }

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

    // ── Phase 1.2: Pre-lower all methods + reapply inlining ──────────────
    // Pre-warm all patched methods by triggering IR lowering eagerly
    // (eliminates the ~50-200μs first-call latency spike).
    for (uint32_t i = 0; i < patched_count; ++i) {
        PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&ctx->methods[i]));
    }

    // Populate inlining map so InlineLeafCallees can find callee IR.
    for (uint32_t i = 0; i < patched_count; ++i) {
        auto& pm = ctx->methods[i];
        if (pm.token != 0) {
            cache->AddInliningTarget(pm.module_id, pm.token, &pm);
        }
    }

    // Reapply inlining now that all callee IR is available.
    if (patched_count > 0) {
        ReapplyInlining(ctx->methods, patched_count);
    }

    // ── Phase 1.3: Eager JIT compilation ─────────────────────────────
    // For methods with pre-allocated register IR (v2+ .patchdata) that
    // don't already have an AOT entry, JIT-compile them eagerly so they
    // run at native speed on first call via Step A0 dispatch.
    // This is safe only after Phase 1.2 (inlining is complete).
    EagerCompilePatchMethods(ctx->methods, patched_count);

    // Bump patch generation for CallVirt MIC cache invalidation.
    g_patch_generation.fetch_add(1, std::memory_order_relaxed);

    return ctx;
}

PatchContext* ApplyPatchFromMemoryEx(
    const void* data, size_t size,
    const char* host_type_ns,
    const char* const* host_type_names,
    const char* const* host_method_names,
    int method_count) noexcept {
    // Same validation as ApplyPatchFromMemory.
    if (data == nullptr || size < sizeof(PatchDataHeader)) return nullptr;

    auto* header = static_cast<const PatchDataHeader*>(data);
    if (header->magic != PATCH_DATA_MAGIC) return nullptr;
    if (header->version != 1 && header->version != 2 && header->version != 3) return nullptr;
    uint32_t min_header = (header->version == 1) ? 112 :
                          (header->version == 2) ? 124 : sizeof(PatchDataHeader);
    if (header->header_size < min_header) return nullptr;

    uint32_t expected_size = header->body_data_offset + header->body_data_size;
    uint32_t ir_section_end = header->aot_core_ir_offset + header->aot_core_ir_size;
    if (ir_section_end > expected_size)
        expected_size = ir_section_end;
    if (size < expected_size) return nullptr;

    // Multi-module dependency validation (v3+).  Same checks as ApplyPatchFromMemory
    // but passes host_type_ns/host_type_names/host_method_names/method_count for retry.
    if (header->version >= 3 && header->dependency_count > 0)
    {
        auto& registry = GetHotpatchNameRegistry();
        const auto* dep_entries = reinterpret_cast<const PatchDataDependency*>(
            reinterpret_cast<const uint8_t*>(header) + header->dependency_offset);
        bool all_deps_satisfied = true;

        for (uint32_t di = 0; di < header->dependency_count; ++di)
        {
            const char* dep_name = PatchData_String(header, dep_entries[di].assembly_name_offset);
            if (dep_name == nullptr || dep_name[0] == '\0')
                continue;

            bool found = false;
            for (size_t mi = 0; mi < registry.ModuleCount(); ++mi)
            {
                const auto* mod = registry.GetModuleByIndex(mi);
                if (mod != nullptr && mod->module_name != nullptr &&
                    std::strcmp(mod->module_name, dep_name) == 0)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                if (dep_entries[di].min_version == 0)
                    continue;
                all_deps_satisfied = false;
                break;
            }
        }

        if (!all_deps_satisfied)
        {
            auto* data_copy = static_cast<uint8_t*>(
                memory_domain::DomainCurrentAllocateTagged(size));
            if (data_copy == nullptr)
                return nullptr;
            std::memcpy(data_copy, data, size);
            // Note: deferred patches for ApplyPatchFromMemoryEx are stored
            // with nullptr host_type_name (not used).  host_type_ns and
            // per-method arrays are NOT preserved across deferred retry.
            // For v3+ multi-module patches that need namespace overrides,
            // ApplyPatchFromMemoryEx would need deferred patch support.
            // Current use case (v1 patches, no deps) is unaffected.
            g_deferred_patches[reinterpret_cast<uintptr_t>(data_copy)] = DeferredPatchEntry{
                data_copy, size, nullptr, host_method_names, 0, false
            };
            return nullptr;
        }
    }

    auto* ctx = CreatePatchContext(header, size);
    if (ctx == nullptr) return nullptr;

    auto& registry = GetHotpatchNameRegistry();
    auto* cache = ctx->metadata_cache;

    bool has_per_method_overrides = method_count > 0
        && (host_type_names != nullptr || host_method_names != nullptr);

    uint32_t patch_domain_id_ex = 0;
    {
        auto* md = memory_domain::CurrentDomain();
        if (md != nullptr) patch_domain_id_ex = md->domain_id;
    }

    // ── Phase 1: Validation (dry-run, no state mutation) ────────────────
    // (工业级 two-phase: 确保当前 blob 所有 "应有 body" 的方法都能在 AOT 注册表里
    // 解析到 slot. 任一解析失败 → 整体不提交, 不进入 commit, 返回 nullptr.
    // 这是模块级事务保证: 不会出现 "部分方法被打上 kHotpatchActive, 其余静默漏" 的
    // 半残状态 — 这正是前期假阳性审计 FP-4 强调的缺口.)
    //
    // "应为该补丁一部分" = 有 body + 有有效 type/method name 的方法(与 commit
    // 循环同判据). body_size==0 / 缺名 是补丁自己没带, 不是解析失败, 不阻断.
    {
        uint32_t total = cache->MethodCount();
        uint32_t resolve_failures = 0;
        uint32_t candidates = 0;  // 应被解析的方法数
        for (uint32_t i = 0; i < total; ++i) {
            auto* method_entry = cache->GetMethodDef(i);
            if (method_entry == nullptr) continue;
            if (method_entry->body_size == 0) continue;

            const char* type_name = cache->GetTypeName(method_entry);
            const char* type_ns = cache->GetTypeNamespace(method_entry);
            const char* method_name = cache->GetString(method_entry->name_offset);
            if (type_name == nullptr || method_name == nullptr) continue;

            const char* lookup_ns = (host_type_ns != nullptr) ? host_type_ns : type_ns;
            const char* lookup_type = type_name;
            if (has_per_method_overrides && i < static_cast<uint32_t>(method_count)
                && host_type_names != nullptr && host_type_names[i] != nullptr)
                lookup_type = host_type_names[i];
            if (has_per_method_overrides && i < static_cast<uint32_t>(method_count)
                && host_method_names != nullptr && host_method_names[i] != nullptr)
                method_name = host_method_names[i];

            ++candidates;
            uint64_t lookup = registry.LookupMethod(lookup_ns, lookup_type, method_name);
            if (lookup == 0) {
                HOTPATCH_DIAG("DIAG[APFM-P1]: unresolvable method ns='%s' type='%s' method='%s'\n",
                    lookup_ns ? lookup_ns : "(null)",
                    lookup_type ? lookup_type : "(null)",
                    method_name ? method_name : "(null)");
                ++resolve_failures;
                continue;
            }
            uint32_t module_id_tmp = ExtractModuleId(lookup);
            uint32_t aot_token_tmp = ExtractToken(lookup);
            uint32_t slot_tmp = registry.TokenToSlot(module_id_tmp, aot_token_tmp);
            if (slot_tmp == ~0u) {
                HOTPATCH_DIAG("DIAP[APFM-P1]: no slot for ns='%s' method='%s'\n",
                    lookup_ns ? lookup_ns : "(null)", method_name ? method_name : "(null)");
                ++resolve_failures;
            }
        }
        HOTPATCH_DIAG("DIAP[APFM-P1]: validated %u candidates, %u failures\n",
            static_cast<unsigned>(candidates), static_cast<unsigned>(resolve_failures));

        // 事务性判定: 该补丁的候选方法里只要有一个解析失败就不整体提交.
        // (body_size==0 的方法不算候选 —— 那类不含此补丁改的 subject.)
        if (candidates > 0 && resolve_failures > 0) {
            HOTPATCH_DIAG("DIAP[APFM-P1]: transactional abort — %u/%u candidate methods unresolvable\n",
                static_cast<unsigned>(resolve_failures), static_cast<unsigned>(candidates));
            // 不做任何 SetPatchedBySlot. 由调用方(ChaosApplyPatch)映射为 PARTIAL_ROLLBACK/NO_METHODS.
            DestroyPatchContext(ctx);
            return nullptr;
        }
    }

    uint32_t patched_count = 0;
    uint32_t total = cache->MethodCount();
    for (uint32_t i = 0; i < total; ++i) {
        auto* method_entry = cache->GetMethodDef(i);
        if (method_entry == nullptr) continue;
        if (method_entry->body_size == 0) continue;

        // Get metadata names from patch DLL.
        const char* type_name = cache->GetTypeName(method_entry);
        const char* type_ns = cache->GetTypeNamespace(method_entry);
        const char* method_name = cache->GetString(method_entry->name_offset);
        if (type_name == nullptr || method_name == nullptr) continue;

        // Apply overrides: namespace (single), type name (per-method), method name (per-method).
        const char* lookup_ns = (host_type_ns != nullptr) ? host_type_ns : type_ns;

        const char* lookup_type;
        if (has_per_method_overrides && i < static_cast<uint32_t>(method_count)
            && host_type_names != nullptr && host_type_names[i] != nullptr) {
            lookup_type = host_type_names[i];
        } else {
            lookup_type = type_name;
        }

        if (has_per_method_overrides && i < static_cast<uint32_t>(method_count)
            && host_method_names != nullptr && host_method_names[i] != nullptr) {
            method_name = host_method_names[i];
        }

        // Look up in HotpatchNameRegistry with all three overrides applied.
        uint64_t lookup = registry.LookupMethod(lookup_ns, lookup_type, method_name);
        if (lookup == 0) {
            HOTPATCH_DIAG("DIAG[APFM]: LookupMethod failed ns='%s' type='%s' method='%s'\n",
                lookup_ns ? lookup_ns : "(null)",
                lookup_type ? lookup_type : "(null)",
                method_name ? method_name : "(null)");
            continue;
        }

        uint32_t module_id = ExtractModuleId(lookup);
        uint32_t aot_token = ExtractToken(lookup);

        uint32_t slot = registry.TokenToSlot(module_id, aot_token);
        if (slot == ~0u) continue;

        // Set up PatchMethod.
        auto& patch_method = ctx->methods[patched_count];
        patch_method.token = aot_token;
        patch_method.module_id = module_id;
        patch_method.aot_core_ir_json = cache->GetAotCoreIr(i);
        patch_method.aot_core_ir_json_length = 0;

        if (header->version >= 2) {
            auto reg_ir = cache->GetRegisterIr(i);
            if (reg_ir.data != nullptr) {
                patch_method.reg_ir_data         = reg_ir.data;
                patch_method.reg_ir_instr_count  = reg_ir.instr_count;
                patch_method.reg_ir_seh_count    = reg_ir.seh_count;
                patch_method.reg_ir_max_regs     = reg_ir.max_regs;
            }
        }

        patch_method.metadata_cache = cache;

        if (method_entry->signature_offset != 0) {
            patch_method.signature_blob = static_cast<const uint8_t*>(
                cache->GetBlob(method_entry->signature_offset));
            if (patch_method.signature_blob != nullptr)
                patch_method.signature_len = patch_method.signature_blob[0];
        }

        // Save original keep-native flag before SetPatchedBySlot clears it.
        {
            auto* orig_entry = registry.GetDispatchEntryBySlot(module_id, slot);
            if (orig_entry && HotpatchShouldKeepNative(*orig_entry)) {
                patch_method.keep_native = true;
            }
        }

        registry.SetPatchedBySlot(module_id, slot, true, &patch_method, patch_domain_id_ex);

        if (patch_method.keep_native) {
            auto* entry = registry.GetDispatchEntryBySlot(module_id, slot);
            if (entry != nullptr) {
#if defined(_MSC_VER)
                _InterlockedOr(reinterpret_cast<volatile long*>(&entry->flags),
                               static_cast<long>(kHotpatchKeepNative));
#else
                __sync_fetch_and_or(reinterpret_cast<volatile uint32_t*>(&entry->flags),
                                    static_cast<uint32_t>(kHotpatchKeepNative));
#endif
            }
        }

        ++patched_count;
    }

    ctx->method_count = patched_count;
    if (patched_count == 0) {
        HOTPATCH_DIAG("DIAG[APFM]: WARNING — 0 methods patched!\n");
    }
    {
        const auto* bs = chaos::il2cpp::bootstrap::PeekBootstrapState();
        if (bs != nullptr && bs->is_bootstrapped) {
            const auto* bridge = chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
            if (bridge != nullptr && bs->aot_image_handle != 0) {
                ctx->metadata_cache->SetAotBridge(bridge, bs->aot_image_handle);
            }
        }
    }

    for (uint32_t i = 0; i < patched_count; ++i) {
        PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&ctx->methods[i]));
    }
    for (uint32_t i = 0; i < patched_count; ++i) {
        auto& pm = ctx->methods[i];
        if (pm.token != 0)
            cache->AddInliningTarget(pm.module_id, pm.token, &pm);
    }
    if (patched_count > 0)
        ReapplyInlining(ctx->methods, patched_count);

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
