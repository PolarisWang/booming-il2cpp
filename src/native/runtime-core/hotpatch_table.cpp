#include "hotpatch_table.h"

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

// ── Registration ──────────────────────────────────────────────────────

void HotpatchNameRegistry::RegisterModule(const HotpatchModuleV0* module) noexcept {
    if (module == nullptr) return;

    if (module->type_entries == nullptr && module->type_entry_count > 0) return;
    if (module->method_entries == nullptr && module->method_entry_count > 0) return;
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
        const auto& type_entry = mod->type_entries[ti];
        if (type_entry.method_count == 0) continue;

        // Use namespace from the type entry (never null — codegen emits "" for global ns).
        const char* ns = type_entry.namespace_name;
        if (ns == nullptr) ns = "";

        for (uint16_t mi = 0; mi < type_entry.method_count; ++mi) {
            const auto& method_entry = mod->method_entries[type_entry.first_method_index + mi];

            // Build key: "namespace\0typename\0methodname"
            std::string key;
            key.reserve(std::strlen(ns) + 1 +
                        std::strlen(type_entry.type_name) + 1 +
                        std::strlen(method_entry.method_name) + 1);
            key.append(ns);
            key.push_back('\0');
            key.append(type_entry.type_name);
            key.push_back('\0');
            key.append(method_entry.method_name);

            uint64_t value = (static_cast<uint64_t>(module_index) << 32) | method_entry.method_token;
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

    // Fallback: linear scan with bsearch per module (for modules registered
    // before cache was added, or dynamic registration at runtime).
    TypeNameLookupKey lk{ns, type_name};
    for (size_t mi = 0; mi < modules_.size(); ++mi) {
        const auto* mod = modules_[mi];
        if (mod == nullptr) continue;

        const auto* type_entry = static_cast<const HotpatchTypeEntryV0*>(
            std::bsearch(&lk,
                         mod->type_entries,
                         mod->type_entry_count,
                         sizeof(HotpatchTypeEntryV0),
                         CompareTypeName));
        if (type_entry == nullptr) continue;
        if (type_entry->method_count == 0) continue;

        const HotpatchMethodEntryV0* method_base =
            mod->method_entries + type_entry->first_method_index;

        for (uint16_t i = 0; i < type_entry->method_count; ++i) {
            if (std::strcmp(method_base[i].method_name, method_name) == 0) {
                return (static_cast<uint64_t>(mi) << 32) | method_base[i].method_token;
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
    if (mod == nullptr || mod->method_entries == nullptr) return nullptr;
    for (uint32_t i = 0; i < mod->method_entry_count; ++i) {
        if (mod->method_entries[i].method_token == method_token) {
            return mod->method_entries[i].method_name;
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