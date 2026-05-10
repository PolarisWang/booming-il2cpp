#include "hotpatch_table.h"

#include <algorithm>
#include <atomic>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ── Binary search helpers ──────────────────────────────────────────────

int HotpatchNameRegistry::CompareTypeName(const void* key, const void* elem) noexcept {
    const char* type_name = static_cast<const char*>(key);
    const auto* entry = static_cast<const HotpatchTypeEntryV0*>(elem);
    return std::strcmp(type_name, entry->type_name);
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
}

// ── Lookup ────────────────────────────────────────────────────────────

uint64_t HotpatchNameRegistry::LookupMethod(const char* type_name,
                                             const char* method_name) const noexcept {
    if (type_name == nullptr || method_name == nullptr) return 0;

    for (size_t mi = 0; mi < modules_.size(); ++mi) {
        const auto* mod = modules_[mi];
        if (mod == nullptr) continue;

        const auto* type_entry = static_cast<const HotpatchTypeEntryV0*>(
            std::bsearch(type_name,
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
                // Return composite key: (module_index << 32) | token
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

// ── Dispatch entry access ─────────────────────────────────────────────

HotpatchEntryV0* HotpatchNameRegistry::GetDispatchEntry(uint32_t token) noexcept {
    // Legacy: global token scan. Kept for backward compat.
    // With 200+ DLLs this can resolve the wrong module due to token collisions.
    size_t mi = FindModuleForToken(token);
    if (mi == ~static_cast<size_t>(0)) return nullptr;

    const auto* mod = modules_[mi];
    if (mod == nullptr) return nullptr;
    if (mod->entry_table == nullptr) return nullptr;

    // Re-scan inside the found module.
    const auto* entry = static_cast<const HotpatchSlotEntryV0*>(
        std::bsearch(&token,
                     mod->token_slot_entries,
                     mod->token_slot_entry_count,
                     sizeof(HotpatchSlotEntryV0),
                     CompareTokenSlot));
    if (entry == nullptr) return nullptr;
    if (entry->slot >= mod->entry_table_size) return nullptr;

    return &mod->entry_table[entry->slot];
}

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

size_t HotpatchNameRegistry::FindModuleForToken(uint32_t token) const noexcept {
    for (size_t mi = 0; mi < modules_.size(); ++mi) {
        const auto* mod = modules_[mi];
        if (mod == nullptr) continue;
        if (mod->token_slot_entries == nullptr || mod->token_slot_entry_count == 0) continue;

        const auto* entry = static_cast<const HotpatchSlotEntryV0*>(
            std::bsearch(&token,
                         mod->token_slot_entries,
                         mod->token_slot_entry_count,
                         sizeof(HotpatchSlotEntryV0),
                         CompareTokenSlot));
        if (entry != nullptr) return mi;
    }
    return ~static_cast<size_t>(0);
}

// ── Patch management ──────────────────────────────────────────────────

void HotpatchNameRegistry::SetPatchedBySlot(uint32_t module_id, uint32_t slot, bool patched,
                                             void* method_key) noexcept {
    HotpatchEntryV0* entry = GetDispatchEntryBySlot(module_id, slot);
    if (entry == nullptr) return;

    if (patched) {
        entry->method_key = reinterpret_cast<uintptr_t>(method_key);
        std::atomic_thread_fence(std::memory_order_release);
        entry->flags |= kHotpatchActive;
    } else {
        entry->flags &= ~kHotpatchActive;
        std::atomic_thread_fence(std::memory_order_release);
        entry->method_key = 0;
    }
}

void HotpatchNameRegistry::SetPatched(uint32_t token, bool patched,
                                       void* method_key) noexcept {
    // Legacy fallback: global token lookup (may cause collisions with 200+ DLLs).
    HotpatchEntryV0* entry = GetDispatchEntry(token);
    // NOTE: this path is deprecated for multi-DLL scenarios. Prefer SetPatchedBySlot.
    if (entry == nullptr) return;

    if (patched) {
        entry->method_key = reinterpret_cast<uintptr_t>(method_key);
        std::atomic_thread_fence(std::memory_order_release);
        entry->flags |= kHotpatchActive;
    } else {
        entry->flags &= ~kHotpatchActive;
        std::atomic_thread_fence(std::memory_order_release);
        entry->method_key = 0;
    }
}

// Legacy: global GetDispatchEntry(token) — kept for backward compat and SetPatched fallback.
// NOTE: This calls FindModuleForToken internally, which scans all modules and returns
// the first match. With 200+ DLLs this can resolve the wrong module due to token collisions.
// New code should use GetDispatchEntry(module_id, token) instead.

// ── Global singleton ──────────────────────────────────────────────────

HotpatchNameRegistry& GetHotpatchNameRegistry() noexcept {
    static HotpatchNameRegistry g_hotpatch_name_registry;
    return g_hotpatch_name_registry;
}

void RegisterHotpatchModule(const HotpatchModuleV0* module) noexcept {
    GetHotpatchNameRegistry().RegisterModule(module);
}

void RegisterReversePInvokeWrappers(void* const* wrappers, uint32_t count) noexcept {
    (void)wrappers;
    (void)count;
}

// ── Dispatch helpers ─────────────────────────────────────────────────

void* HotpatchCallViaSlot(uint32_t module_index, uint32_t slot,
                           void* args_buf, void* ret) noexcept {
    (void)args_buf;
    (void)ret;

    auto* entry = GetHotpatchNameRegistry().GetDispatchEntryBySlot(module_index, slot);
    if (entry == nullptr) return nullptr;

    // Acquire fence pairs with the release in SetPatchedBySlot so the reader
    // sees a consistent flags + method_key state.
    std::atomic_thread_fence(std::memory_order_acquire);

    if (entry->flags & kHotpatchActive) {
        return entry->interrupt_ptr;
    }

    return entry->direct_ptr;
}

}  // namespace chaos::il2cpp::runtime_core