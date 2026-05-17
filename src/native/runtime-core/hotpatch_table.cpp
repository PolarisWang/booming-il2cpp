#include "hotpatch_table.h"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <intrin.h>

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

void HotpatchNameRegistry::SetPatchedBySlot(uint32_t module_id, uint32_t slot, bool patched,
                                             void* method_key) noexcept {
    HotpatchEntryV0* entry = GetDispatchEntryBySlot(module_id, slot);
    if (entry == nullptr) return;

    if (patched) {
        entry->method_key = reinterpret_cast<uintptr_t>(method_key);
        // release: method_key visible before flags (reader uses acquire fence)
        _InterlockedOr((volatile long*)&entry->flags, kHotpatchActive);
    } else {
        _InterlockedAnd((volatile long*)&entry->flags, ~kHotpatchActive);
        // release: method_key visible before flags (reader uses acquire fence)
        entry->method_key = 0;
    }
}

// ── Global singleton ──────────────────────────────────────────────────

HotpatchNameRegistry& GetHotpatchNameRegistry() noexcept {
    static HotpatchNameRegistry g_hotpatch_name_registry;
    return g_hotpatch_name_registry;
}

void RegisterHotpatchModule(const HotpatchModuleV0* module) noexcept {
    GetHotpatchNameRegistry().RegisterModule(module);
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