#include "dispatch_table.h"

#include <algorithm>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ── Binary search helpers ──────────────────────────────────────────────

int NameIndexRegistry::CompareTypeName(const void* key, const void* elem) noexcept {
    const char* type_name = static_cast<const char*>(key);
    const auto* entry = static_cast<const NameIndexTypeEntryV0*>(elem);
    return std::strcmp(type_name, entry->type_name);
}

int NameIndexRegistry::CompareTokenSlot(const void* key, const void* elem) noexcept {
    uint32_t token = *static_cast<const uint32_t*>(key);
    const auto* entry = static_cast<const TokenSlotEntryV0*>(elem);
    if (token < entry->token) return -1;
    if (token > entry->token) return 1;
    return 0;
}

// ── Registration ──────────────────────────────────────────────────────

void NameIndexRegistry::RegisterModule(const NameIndexModuleV0* module) noexcept {
    if (module == nullptr) return;
    if (module_count_ >= kMaxModules) return;

    // Sanity-check the module data.
    if (module->type_entries == nullptr && module->type_entry_count > 0) return;
    if (module->method_entries == nullptr && module->method_entry_count > 0) return;
    if (module->token_slot_entries == nullptr && module->token_slot_entry_count > 0) return;

    modules_[module_count_++] = module;
}

// ── Lookup ────────────────────────────────────────────────────────────

uint32_t NameIndexRegistry::LookupMethod(const char* type_name,
                                          const char* method_name) const noexcept {
    if (type_name == nullptr || method_name == nullptr) return 0;

    for (size_t mi = 0; mi < module_count_; ++mi) {
        const auto* mod = modules_[mi];
        if (mod == nullptr) continue;

        // 1. Find the type entry via bsearch on type_name.
        const auto* type_entry = static_cast<const NameIndexTypeEntryV0*>(
            std::bsearch(type_name,
                         mod->type_entries,
                         mod->type_entry_count,
                         sizeof(NameIndexTypeEntryV0),
                         CompareTypeName));
        if (type_entry == nullptr) continue;

        // 2. Within this type's method range, bsearch for method_name.
        // The method entries for this type are in a contiguous block
        // starting at type_entry->first_method_index.
        if (type_entry->method_count == 0) continue;

        const NameIndexMethodEntryV0* method_base =
            mod->method_entries + type_entry->first_method_index;

        // Linear scan within the type's method block (typically 1-20 methods,
        // so linear is fine; bsearch overhead would dominate).
        for (uint16_t i = 0; i < type_entry->method_count; ++i) {
            if (std::strcmp(method_base[i].method_name, method_name) == 0) {
                return method_base[i].method_token;
            }
        }
    }

    return 0;  // not found
}

uint32_t NameIndexRegistry::TokenToSlot(uint32_t token) const noexcept {
    if (token == 0) return ~0u;

    for (size_t mi = 0; mi < module_count_; ++mi) {
        const auto* mod = modules_[mi];
        if (mod == nullptr) continue;
        if (mod->token_slot_entries == nullptr || mod->token_slot_entry_count == 0) continue;

        const auto* entry = static_cast<const TokenSlotEntryV0*>(
            std::bsearch(&token,
                         mod->token_slot_entries,
                         mod->token_slot_entry_count,
                         sizeof(TokenSlotEntryV0),
                         CompareTokenSlot));
        if (entry != nullptr) {
            return entry->slot;
        }
    }

    return ~0u;  // not found
}

// ── Dispatch entry access ─────────────────────────────────────────────

DispatchEntryV0* NameIndexRegistry::GetDispatchEntry(uint32_t token) const noexcept {
    uint32_t slot = TokenToSlot(token);
    if (slot == ~0u) return nullptr;

    // Find which module owns this token.
    for (size_t mi = 0; mi < module_count_; ++mi) {
        const auto* mod = modules_[mi];
        if (mod == nullptr) continue;
        if (mod->dispatch_table == nullptr) continue;
        if (mod->token_slot_entries == nullptr || mod->token_slot_entry_count == 0) continue;

        // Fast check: is slot within this module's token range?
        // (first token → last token in this module's token_slot_entries)
        if (slot >= mod->dispatch_table_size) continue;

        // Verify token belongs to this module via bsearch.
        const auto* entry = static_cast<const TokenSlotEntryV0*>(
            std::bsearch(&token,
                         mod->token_slot_entries,
                         mod->token_slot_entry_count,
                         sizeof(TokenSlotEntryV0),
                         CompareTokenSlot));
        if (entry != nullptr) {
            return &mod->dispatch_table[slot];
        }
    }

    return nullptr;
}

DispatchEntryV0* NameIndexRegistry::GetDispatchEntryBySlot(
    size_t module_index, uint32_t slot) const noexcept {
    if (module_index >= module_count_) return nullptr;
    const auto* mod = modules_[module_index];
    if (mod == nullptr) return nullptr;
    if (mod->dispatch_table == nullptr) return nullptr;
    if (slot >= mod->dispatch_table_size) return nullptr;
    return &mod->dispatch_table[slot];
}

size_t NameIndexRegistry::FindModuleForToken(uint32_t token) const noexcept {
    for (size_t mi = 0; mi < module_count_; ++mi) {
        const auto* mod = modules_[mi];
        if (mod == nullptr) continue;
        if (mod->token_slot_entries == nullptr || mod->token_slot_entry_count == 0) continue;

        const auto* entry = static_cast<const TokenSlotEntryV0*>(
            std::bsearch(&token,
                         mod->token_slot_entries,
                         mod->token_slot_entry_count,
                         sizeof(TokenSlotEntryV0),
                         CompareTokenSlot));
        if (entry != nullptr) {
            return mi;
        }
    }
    return ~static_cast<size_t>(0);
}

// ── Patch management ──────────────────────────────────────────────────

void NameIndexRegistry::SetPatched(uint32_t token, bool patched,
                                    void* method_key) noexcept {
    DispatchEntryV0* entry = GetDispatchEntry(token);
    if (entry == nullptr) return;

    if (patched) {
        entry->flags |= kDispatchPatched;
        entry->method_key = reinterpret_cast<uintptr_t>(method_key);
    } else {
        entry->flags &= ~kDispatchPatched;
        entry->method_key = 0;
    }
}

// ── Global singleton ──────────────────────────────────────────────────

namespace {
NameIndexRegistry g_name_index_registry;
}  // anonymous namespace

NameIndexRegistry& GetNameIndexRegistry() noexcept {
    return g_name_index_registry;
}

void RegisterModuleNameIndex(const NameIndexModuleV0* module) noexcept {
    g_name_index_registry.RegisterModule(module);
}

// ── Dispatch helpers ─────────────────────────────────────────────────

void* CallViaSlot(uint32_t module_index, uint32_t slot,
                   void* args_buf, void* ret) noexcept {
    (void)args_buf;
    (void)ret;

    auto* entry = g_name_index_registry.GetDispatchEntryBySlot(module_index, slot);
    if (entry == nullptr) return nullptr;

    if (entry->flags & kDispatchPatched) {
        // ── Patched path: route through InterpreterEntryDirect ──
        // Full implementation in Step 5 (interpreter_entry.cpp).
        // For now, return interrupt_ptr as a sentinel.
        return entry->interrupt_ptr;
    }

    // ── AOT direct path ──
    return entry->direct_ptr;
}

}  // namespace chaos::il2cpp::runtime_core
