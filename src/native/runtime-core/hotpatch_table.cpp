#include "hotpatch_table.h"
#include "patch_loader.h"  // PatchMethod

#include <algorithm>
#include <atomic>
#include <cstring>
#include <chaos/log.h>
#if defined(_MSC_VER)
#include <intrin.h>
#else
// GCC/Clang __sync_* builtins provide equivalent atomic ops on uint32_t.
// On LP64 Linux, long is 64-bit, so operate on uint32_t* directly.
#define _InterlockedAnd(ptr, val)    __sync_fetch_and_and(reinterpret_cast<volatile uint32_t*>(ptr), static_cast<uint32_t>(val))
#define _InterlockedOr(ptr, val)     __sync_fetch_and_or(reinterpret_cast<volatile uint32_t*>(ptr), static_cast<uint32_t>(val))
#define _InterlockedIncrement(ptr)   __sync_add_and_fetch(reinterpret_cast<volatile uint32_t*>(ptr), 1u)
#endif

namespace chaos::il2cpp::runtime_core {

// ── Binary search helpers ──────────────────────────────────────────────

// Composite key for namespace+typename binary search.
// The HotpatchTypeEntryV0 array is sorted by (namespace, type_name),
// so bsearch needs a composite key.
struct TypeNameLookupKey {
    const char* ns;
    const char* type_name;
};

int HotpatchNameRegistry::CompareTypeName(const void* key, const void* elem) noexcept {
    const auto* lookup = static_cast<const TypeNameLookupKey*>(key);
    const auto* entry = static_cast<const HotpatchTypeEntryV0*>(elem);

    // Compare namespace first (entries sorted by namespace, then type_name).
    // Both the key and the entry always have valid C strings (codegen emits
    // "" for global namespace types).
    int cmp = std::strcmp(lookup->ns, entry->namespace_name);
    if (cmp != 0) return cmp;

    // Namespace matches, compare type_name.
    return std::strcmp(lookup->type_name, entry->type_name);
}

int HotpatchNameRegistry::CompareTokenSlot(const void* key, const void* elem) noexcept {
    uint32_t token = *static_cast<const uint32_t*>(key);
    const auto* entry = static_cast<const HotpatchSlotEntryV0*>(elem);
    return (token > entry->token) - (token < entry->token);
}

// ── Chunk-aware element access ────────────────────────────────────────
//
// A registration array may be emitted across several translation units, in
// which case the module carries a chunk list instead of one flat block. The
// LOGICAL array — and therefore every index into it — is unchanged: an index
// is still a position in the concatenation of the chunks in order. Only the
// way an index is resolved to an address differs, and that is confined to
// these two functions.
//
// When the chunk list is null the module uses the flat layout, which is what
// every module emitted before chunking does. That path returns the flat
// element directly, so existing behaviour is bit-for-bit preserved and no
// chunk arithmetic runs at all.
//
// Chunk resolution is a binary search over the running prefix sum of the
// chunk counts. The chunk count is small (bounded by the number of declaring
// types) and constant with respect to array length, so lookups stay O(1) in
// the sense that matters for the hot path. If profiling ever shows this
// mattering, the prefix sum can be hoisted into a precomputed array.

namespace {

// Whether this module was built against a header that HAS the chunk fields.
//
// The chunk fields live at the END of HotpatchModuleV0, after `struct_size`
// (which is first). A module produced by an older codegen is physically
// shorter, so reading `.type_chunks` off it would read past its end — the
// `struct_size` check is what makes that read legal.
//
// This is the same versioning idiom bootstrap.cpp already uses for
// CodeRegistrationV0 / MetadataRegistrationV0 (`struct_size >= sizeof(...)`),
// applied per-module because modules can be registered dynamically at
// runtime, not only at bootstrap.
bool ModuleHasChunkFields(const HotpatchModuleV0* mod) noexcept {
    // `struct_size == 0` means the producer did not stamp a size. Treat that
    // as pre-chunking (the safe reading: fall back to the flat fields, which
    // are present in every layout).
    if (mod->struct_size == 0u) return false;
    return mod->struct_size >= sizeof(HotpatchModuleV0);
}

// Locates the chunk containing `index` within a chunk list, or nullptr when
// the list is empty or the index is out of range.
//
// The prefix sum is computed on the fly rather than cached: the binary search
// needs it at each probe, and materialising it would require an allocation on
// a registration path that must not allocate.
const ChaosAbiChunkV0* FindChunk(const ChaosAbiChunkV0* chunks, uint32_t chunk_count,
                                 uint32_t index, uint32_t* out_offset) noexcept {
    if (chunks == nullptr || chunk_count == 0) return nullptr;

    uint32_t lo = 0;
    uint32_t hi = chunk_count;  // exclusive
    while (lo < hi) {
        uint32_t mid = lo + (hi - lo) / 2;
        uint32_t mid_base = 0;
        for (uint32_t k = 0; k < mid; ++k) mid_base += chunks[k].count;

        if (index < mid_base) {
            hi = mid;
        } else if (index >= mid_base + chunks[mid].count) {
            lo = mid + 1;
        } else {
            *out_offset = index - mid_base;
            return &chunks[mid];
        }
    }
    return nullptr;
}

}  // namespace

const HotpatchTypeEntryV0* HotpatchTypeEntryAt(const HotpatchModuleV0* mod, uint32_t index) noexcept {
    if (mod == nullptr) return nullptr;

    // Flat layout — the pre-chunking path, kept free of chunk arithmetic so it
    // is bit-for-bit identical to the original indexing.
    if (!ModuleHasChunkFields(mod) || mod->type_chunks == nullptr || mod->type_chunk_count == 0) {
        if (index >= mod->type_entry_count) return nullptr;
        return &mod->type_entries[index];
    }

    uint32_t offset = 0;
    const ChaosAbiChunkV0* chunk = FindChunk(mod->type_chunks, mod->type_chunk_count, index, &offset);
    if (chunk == nullptr || chunk->base == nullptr || offset >= chunk->count) return nullptr;
    // element_size is carried in the descriptor so this does not hard-code the
    // element type; assert it matches to catch a descriptor/codegen mismatch.
    if (chunk->element_size != sizeof(HotpatchTypeEntryV0)) return nullptr;
    return static_cast<const HotpatchTypeEntryV0*>(chunk->base) + offset;
}

const HotpatchMethodEntryV0* HotpatchMethodEntryAt(const HotpatchModuleV0* mod, uint32_t index) noexcept {
    if (mod == nullptr) return nullptr;

    if (!ModuleHasChunkFields(mod) || mod->method_chunks == nullptr || mod->method_chunk_count == 0) {
        if (index >= mod->method_entry_count) return nullptr;
        return &mod->method_entries[index];
    }

    uint32_t offset = 0;
    const ChaosAbiChunkV0* chunk = FindChunk(mod->method_chunks, mod->method_chunk_count, index, &offset);
    if (chunk == nullptr || chunk->base == nullptr || offset >= chunk->count) return nullptr;
    if (chunk->element_size != sizeof(HotpatchMethodEntryV0)) return nullptr;
    return static_cast<const HotpatchMethodEntryV0*>(chunk->base) + offset;
}

// True when the module exposes a type array at all. Replaces the old
// `type_entries == nullptr` guard, which is wrong under chunking (a chunked
// module legitimately has a null flat pointer) and would silently skip
// registration.
//
// NOTE the `ModuleHasChunkFields` conjunct: a module from an older codegen has
// a shorter struct, so `type_chunks` must not be read at all — the flat
// pointer is authoritative for it, exactly as before chunking existed.
bool HotpatchModuleHasTypeEntries(const HotpatchModuleV0* mod) noexcept {
    if (mod == nullptr || mod->type_entry_count == 0) return false;
    bool chunked = ModuleHasChunkFields(mod)
        && mod->type_chunks != nullptr && mod->type_chunk_count > 0;
    return chunked || mod->type_entries != nullptr;
}

bool HotpatchModuleHasMethodEntries(const HotpatchModuleV0* mod) noexcept {
    if (mod == nullptr || mod->method_entry_count == 0) return false;
    bool chunked = ModuleHasChunkFields(mod)
        && mod->method_chunks != nullptr && mod->method_chunk_count > 0;
    return chunked || mod->method_entries != nullptr;
}

// ── Registration ──────────────────────────────────────────────────────

void HotpatchNameRegistry::RegisterModule(const HotpatchModuleV0* module) noexcept {
    if (module == nullptr) return;

    // Presence is "flat pointer OR chunk list", not the flat pointer alone: a
    // chunked module has a null flat pointer, and testing only that would
    // return early and silently drop the whole module's names.
    if (!HotpatchModuleHasTypeEntries(module) && module->type_entry_count > 0) return;
    if (!HotpatchModuleHasMethodEntries(module) && module->method_entry_count > 0) return;
    if (module->token_slot_entries == nullptr && module->token_slot_entry_count > 0) return;

    modules_.push_back(module);
    BuildLookupCacheForModule(module, modules_.size() - 1);
}

void HotpatchNameRegistry::RegisterAllModules(const HotpatchModuleV0* const* modules, uint32_t count) noexcept {
    if (modules == nullptr || count == 0) return;
    for (uint32_t i = 0; i < count; ++i) {
        RegisterModule(modules[i]);
    }
}

void HotpatchNameRegistry::BuildLookupCacheForModule(const HotpatchModuleV0* mod, size_t module_index) noexcept {
    // Build "ns\0type\0method" → (module_index<<32 | token) cache entries.
    for (uint32_t ti = 0; ti < mod->type_entry_count; ++ti) {
        const auto* type_entry = HotpatchTypeEntryAt(mod, ti);
        if (type_entry == nullptr) continue;
        if (type_entry->method_count == 0) continue;

        // Use namespace from the type entry (never null — codegen emits "" for global ns).
        const char* ns = type_entry->namespace_name;
        if (ns == nullptr) ns = "";

        for (uint16_t mi = 0; mi < type_entry->method_count; ++mi) {
            // `first_method_index` is a GLOBAL index into the logical method
            // array; resolving it goes through the same accessor so a type's
            // methods may span chunk boundaries.
            const auto* method_entry =
                HotpatchMethodEntryAt(mod, type_entry->first_method_index + mi);
            if (method_entry == nullptr) continue;

            // Build key: "namespace\0typename\0methodname"
            std::string key;
            key.reserve(std::strlen(ns) + 1 +
                        std::strlen(type_entry->type_name) + 1 +
                        std::strlen(method_entry->method_name) + 1);
            key.append(ns);
            key.push_back('\0');
            key.append(type_entry->type_name);
            key.push_back('\0');
            key.append(method_entry->method_name);

            uint64_t value = (static_cast<uint64_t>(module_index) << 32) | method_entry->method_token;
            lookup_cache_.emplace(std::move(key), value);
        }
    }
}

// ── Lookup ────────────────────────────────────────────────────────────

uint64_t HotpatchNameRegistry::LookupMethod(const char* ns,
                                             const char* type_name,
                                             const char* method_name) const noexcept {
    if (ns == nullptr || type_name == nullptr || method_name == nullptr) return 0;

    // Fast path: O(1) hash lookup from cache built during registration.
    std::string key;
    key.reserve(std::strlen(ns) + 1 + std::strlen(type_name) + 1 + std::strlen(method_name) + 1);
    key.append(ns);
    key.push_back('\0');
    key.append(type_name);
    key.push_back('\0');
    key.append(method_name);

    auto it = lookup_cache_.find(key);
    if (it != lookup_cache_.end()) {
        return it->second;
    }

    // Fallback: per-module binary search (for modules registered before the
    // cache was built, or dynamic registration at runtime).
    //
    // This was `std::bsearch` over `mod->type_entries`, which requires ONE
    // contiguous block. A chunked module has no such block, so the search is
    // written out over the accessor instead: the algorithm, the comparison and
    // the ordering requirement are unchanged, only element addressing is.
    // (`bsearch` is also removed rather than kept alongside, so there is no
    // second, chunk-unaware path to drift out of sync.)
    TypeNameLookupKey lk{ns, type_name};
    for (size_t mi = 0; mi < modules_.size(); ++mi) {
        const auto* mod = modules_[mi];
        if (mod == nullptr) continue;

        const HotpatchTypeEntryV0* type_entry = nullptr;
        {
            uint32_t lo = 0;
            uint32_t hi = mod->type_entry_count;  // exclusive
            while (lo < hi) {
                uint32_t mid = lo + (hi - lo) / 2;
                const auto* probe = HotpatchTypeEntryAt(mod, mid);
                if (probe == nullptr) break;  // out-of-range => treat as not found

                int cmp = CompareTypeName(&lk, probe);
                if (cmp == 0) { type_entry = probe; break; }
                if (cmp < 0) hi = mid; else lo = mid + 1;
            }
        }
        if (type_entry == nullptr) continue;
        if (type_entry->method_count == 0) continue;

        for (uint16_t i = 0; i < type_entry->method_count; ++i) {
            const auto* method_entry =
                HotpatchMethodEntryAt(mod, type_entry->first_method_index + i);
            if (method_entry == nullptr) continue;
            if (std::strcmp(method_entry->method_name, method_name) == 0) {
                return (static_cast<uint64_t>(mi) << 32) | method_entry->method_token;
            }
        }
    }

    return 0;
}

uint32_t HotpatchNameRegistry::TokenToSlot(uint32_t module_id, uint32_t token) const noexcept {
    if (token == 0 || module_id >= modules_.size()) return ~0u;

    const auto* mod = modules_[module_id];
    if (mod == nullptr) return ~0u;
    if (mod->token_slot_entries == nullptr || mod->token_slot_entry_count == 0) return ~0u;

    const auto* entry = static_cast<const HotpatchSlotEntryV0*>(
        std::bsearch(&token,
                     mod->token_slot_entries,
                     mod->token_slot_entry_count,
                     sizeof(HotpatchSlotEntryV0),
                     CompareTokenSlot));
    return (entry != nullptr) ? entry->slot : ~0u;
}

uint64_t HotpatchNameRegistry::FindToken(uint32_t method_token) const noexcept {
    if (method_token == 0) return 0;

    for (size_t mi = 0; mi < modules_.size(); ++mi) {
        const auto* mod = modules_[mi];
        if (mod == nullptr) continue;
        if (mod->token_slot_entries == nullptr || mod->token_slot_entry_count == 0) continue;

        const auto* entry = static_cast<const HotpatchSlotEntryV0*>(
            std::bsearch(&method_token,
                         mod->token_slot_entries,
                         mod->token_slot_entry_count,
                         sizeof(HotpatchSlotEntryV0),
                         CompareTokenSlot));
        if (entry != nullptr)
            return (static_cast<uint64_t>(mi) << 32) | method_token;
    }

    return 0;
}

/// Reverse of TokenToSlot: given a dispatch table slot index, find the
/// metadata token.  Linear scan (token_slot_entries are sorted by token,
/// not by slot), but only called during hotpatch — not performance-critical.
uint32_t HotpatchNameRegistry::SlotToToken(uint32_t module_id, uint32_t slot) const noexcept {
    if (module_id >= modules_.size()) return ~0u;
    const auto* mod = modules_[module_id];
    if (mod == nullptr || mod->token_slot_entries == nullptr) return ~0u;
    for (uint32_t i = 0; i < mod->token_slot_entry_count; ++i) {
        if (mod->token_slot_entries[i].slot == slot)
            return mod->token_slot_entries[i].token;
    }
    return ~0u;
}

// Free-function wrapper for callers without a registry reference.
uint32_t SlotToToken(uint32_t module_id, uint32_t slot) noexcept {
    return GetHotpatchNameRegistry().SlotToToken(module_id, slot);
}

const char* HotpatchNameRegistry::GetMethodName(uint32_t module_id, uint32_t method_token) const noexcept {
    if (method_token == 0 || module_id >= modules_.size()) return nullptr;
    const auto* mod = modules_[module_id];
    if (!HotpatchModuleHasMethodEntries(mod)) return nullptr;
    for (uint32_t i = 0; i < mod->method_entry_count; ++i) {
        const auto* method_entry = HotpatchMethodEntryAt(mod, i);
        if (method_entry == nullptr) continue;
        if (method_entry->method_token == method_token) {
            return method_entry->method_name;
        }
    }
    return nullptr;
}

// ── Dispatch entry access ─────────────────────────────────────────────

HotpatchEntryV0* HotpatchNameRegistry::GetDispatchEntry(uint32_t module_id, uint32_t token) const noexcept {
    if (token == 0 || module_id >= modules_.size()) return nullptr;

    const auto* mod = modules_[module_id];
    if (mod == nullptr) return nullptr;
    if (mod->entry_table == nullptr) return nullptr;

    uint32_t slot = TokenToSlot(module_id, token);
    if (slot == ~0u) return nullptr;
    if (slot >= mod->entry_table_size) return nullptr;

    return &mod->entry_table[slot];
}

HotpatchEntryV0* HotpatchNameRegistry::GetDispatchEntryBySlot(
    size_t module_index, uint32_t slot) const noexcept {
    if (module_index >= modules_.size()) return nullptr;
    const auto* mod = modules_[module_index];
    if (mod == nullptr) return nullptr;
    if (mod->entry_table == nullptr) return nullptr;
    if (slot >= mod->entry_table_size) return nullptr;
    return &mod->entry_table[slot];
}

// ── Patch management ──────────────────────────────────────────────────

// Global slot update callback (registered by JIT for ReverseSlotMap updates).
static SlotUpdateCallback g_slot_update_cb = nullptr;

void RegisterSlotUpdateCallback(SlotUpdateCallback cb) noexcept {
    g_slot_update_cb = cb;
}

// Global original AOT direct_ptr resolver (registered by JIT for ResolveDirectFn).
static OriginalAotPtrCallback g_original_aot_cb = nullptr;

void SetOriginalAotPtrCallback(OriginalAotPtrCallback cb) noexcept {
    g_original_aot_cb = cb;
}

OriginalAotPtrCallback GetOriginalAotPtrCallback() noexcept {
    return g_original_aot_cb;
}

// ── Precode transfer callback (D+ DP1-a) ────────────────────────────────────
static PrecodeTransferCallback g_precode_transfer_cb = nullptr;

void SetPrecodeTransferCallback(PrecodeTransferCallback cb) noexcept {
    g_precode_transfer_cb = cb;
}

PrecodeTransferCallback GetPrecodeTransferCallback() noexcept {
    return g_precode_transfer_cb;
}

void HotpatchNameRegistry::SetPatchedBySlot(uint32_t module_id, uint32_t slot, bool patched,
                                             void* method_key,
                                             uint32_t domain_id) noexcept {
    HotpatchEntryV0* entry = GetDispatchEntryBySlot(module_id, slot);
    if (entry == nullptr) return;

    if (patched) {
        entry->method_key = reinterpret_cast<uintptr_t>(method_key);
        // Atomic flags transition: set kHotpatchActive + clear kHotpatchKeepNative
        // in a single store to eliminate the TOCTOU window.  The old two-step
        // (_InterlockedAnd then _InterlockedOr) left an intermediate state where
        // neither bit was set, causing readers to fall through to the wrong path.
        // Paired with acquire load in HotpatchIsActive (hotpatch_table.h).
        auto& atomic_flags = *reinterpret_cast<std::atomic<uint32_t>*>(&entry->flags);
        uint32_t new_flags = (atomic_flags.load(std::memory_order_relaxed)
                              | kHotpatchActive) & ~kHotpatchKeepNative;
        atomic_flags.store(new_flags, std::memory_order_release);

        // Gap2: If this entry has a JitPrecode (detected via the original AOT
        // pointer callback), revert direct_ptr from the JIT trampoline/compiled
        // code back to the original AOT code.  This prevents the precode system
        // from independently compiling the method while the tier system manages
        // it through PatchMethod + tier_state.  Without this fix, the precode
        // (Quick JIT via RegisterJitEntryMethods) and the tier system (Full JIT
        // T3→T4 promotion) could independently produce two compiled versions of
        // the same method — a double compilation waste.
        // See: docs/dev/in-progress/tier-promotion-unification/STATUS.md (Gap2)
        auto* orig_cb = GetOriginalAotPtrCallback();
        if (orig_cb != nullptr) {
            void* original_ptr = orig_cb(entry);
            if (original_ptr != nullptr && original_ptr != entry->direct_ptr) {
                entry->direct_ptr = original_ptr;
            }
        }

        // Save original AOT pointer for deopt demotion recovery.
        // This captures the correct AOT code address after Gap2 restore,
        // before DP1-a may override direct_ptr with JIT code.  Phase A
        // deopt demotion restores entry->direct_ptr from this field.
        if (method_key != nullptr) {
            auto* pm = static_cast<PatchMethod*>(method_key);
            pm->original_aot_ptr = entry->direct_ptr;
        }

        // DP1-a: Try to transfer precode-compiled JitMethod ownership to the
        // tier system.  If the precode has already compiled this method (Quick
        // JIT), we transfer the JitMethod* to PatchMethod and set kJitted state,
        // skipping the entire tier promotion path.  The transfer callback is
        // registered by RegisterJitEntryMethods and lives in jit_precode.cpp.
        // If transfer succeeds, direct_ptr is updated to point to the
        // transferred code and the Gap2 revert above is overridden.
        if (auto* transfer_cb = GetPrecodeTransferCallback(); transfer_cb != nullptr) {
            transfer_cb(entry, method_key);
        }

        // Track this patch for domain-unload cleanup.
        if (domain_id > 0) {
            domain_patches_.push_back({domain_id, module_id, slot});
        }
    } else {
        _InterlockedAnd(reinterpret_cast<volatile long*>(&entry->flags), ~kHotpatchActive);
        // release: method_key visible before flags (reader uses acquire fence)
        entry->method_key = 0;

        // Remove from domain tracking (linear scan, small n).
        for (auto it = domain_patches_.begin(); it != domain_patches_.end(); ++it) {
            if (it->module_id == module_id && it->slot == slot) {
                domain_patches_.erase(it);
                break;
            }
        }
        // Restore direct_ptr (revert Gap2 JIT redirect)
        if (auto* cb = GetOriginalAotPtrCallback()) {
            if (void* orig_ptr = cb(entry); orig_ptr && orig_ptr != entry->direct_ptr)
                entry->direct_ptr = orig_ptr;
        }
    }

    // Version bump: signals to JIT-compiled callers that the target may have changed.
    // Paired with acquire load in the dispatch path.
    _InterlockedIncrement(reinterpret_cast<volatile long*>(&entry->version));

    // Notify the JIT slot update callback so ReverseSlotMap can patch RX slot tables.
    // Also enables inline version-staleness checks via HotpatchEntryV0*.
    if (g_slot_update_cb && entry->direct_ptr) {
        // Find the callee token for this slot.
        uint32_t token = SlotToToken(module_id, slot);
        if (token != ~0u) {
            g_slot_update_cb(token, entry->direct_ptr, entry);
        }
    }
}

uint32_t HotpatchNameRegistry::ClearDomainDispatchEntries(uint32_t domain_id) noexcept {
    if (domain_id == 0) return 0;  // core domain, never unloaded
    uint32_t count = 0;
    for (auto it = domain_patches_.begin(); it != domain_patches_.end(); ) {
        if (it->domain_id == domain_id) {
            // Clear the dispatch entry: reset kHotpatchActive and method_key.
            SetPatchedBySlot(it->module_id, it->slot, false, nullptr, 0);
            it = domain_patches_.erase(it);
            count++;
        } else {
            ++it;
        }
    }
    if (count > 0) {
        CHAOS_IL2CPP_LOG_INFO_M("codegen",
            "ClearDomainDispatchEntries: domain={} cleared {} entries", domain_id, count);
    }
    return count;
}

// ── Global singleton ──────────────────────────────────────────────────

static ModuleRegisteredCallback g_module_registered_cb = nullptr;

void SetModuleRegisteredCallback(ModuleRegisteredCallback cb) noexcept {
    g_module_registered_cb = cb;
}

HotpatchNameRegistry& GetHotpatchNameRegistry() noexcept {
    static HotpatchNameRegistry g_hotpatch_name_registry;
    return g_hotpatch_name_registry;
}

void RegisterHotpatchModule(const HotpatchModuleV0* module) noexcept {
    GetHotpatchNameRegistry().RegisterModule(module);

    // Notify PatchLoader to retry deferred patches.
    if (g_module_registered_cb != nullptr) {
        g_module_registered_cb();
    }
}

// P0.1: Reverse P/Invoke wrapper registry.
// Stores wrapper function pointers emitted by codegen for
// [UnmanagedCallersOnly] methods and native-callable delegates.
// Supports patch unload: wrappers from a specific module can be
// bulk-removed by tracking module_id boundaries.
static std::vector<void*> g_reverse_pinvoke_wrappers;

void RegisterReversePInvokeWrappers(void* const* wrappers, uint32_t count) noexcept {
    if (wrappers == nullptr || count == 0) return;

    // Acquire a start index for potential unload tracking.
    size_t start = g_reverse_pinvoke_wrappers.size();
    g_reverse_pinvoke_wrappers.reserve(start + count);
    for (uint32_t i = 0; i < count; ++i) {
        g_reverse_pinvoke_wrappers.push_back(wrappers[i]);
    }
    (void)start;  // Available for future unload tracking (store module→range mapping).
}

}  // namespace chaos::il2cpp::runtime_core