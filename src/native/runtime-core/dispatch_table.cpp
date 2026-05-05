#include "dispatch_table.h"

#include <algorithm>
#include <cstdio>
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
    if (module == nullptr) {
        std::fprintf(stderr, "DEBUG RegisterModule: module is null\n");
        return;
    }
    if (module_count_ >= kMaxModules) {
        std::fprintf(stderr, "DEBUG RegisterModule: module_count_ %zu >= kMaxModules\n", module_count_);
        return;
    }

    // Sanity-check the module data.
    if (module->type_entries == nullptr && module->type_entry_count > 0) {
        std::fprintf(stderr, "DEBUG RegisterModule: type_entries is null but count=%u\n", module->type_entry_count);
        return;
    }
    if (module->method_entries == nullptr && module->method_entry_count > 0) {
        std::fprintf(stderr, "DEBUG RegisterModule: method_entries is null but count=%u\n", module->method_entry_count);
        return;
    }
    if (module->token_slot_entries == nullptr && module->token_slot_entry_count > 0) {
        std::fprintf(stderr, "DEBUG RegisterModule: token_slot_entries is null but count=%u\n", module->token_slot_entry_count);
        return;
    }

    std::fprintf(stderr, "DEBUG RegisterModule: registering '%s' at index %zu (dispatch_table=%p, size=%u, token_slot_entries=%p)\n",
        module->module_name ? module->module_name : "(null)",
        module_count_,
        static_cast<const void*>(module->dispatch_table),
        module->dispatch_table_size,
        static_cast<const void*>(module->token_slot_entries));

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
            std::fprintf(stderr, "DEBUG TokenToSlot: token=0x%08x -> slot=%u (found in module %zu)\n", token, entry->slot, mi);
            return entry->slot;
        }
    }

    std::fprintf(stderr, "DEBUG TokenToSlot: token=0x%08x -> NOT FOUND (module_count=%zu)\n", token, module_count_);
    return ~0u;  // not found
}

// ── Dispatch entry access ─────────────────────────────────────────────

DispatchEntryV0* NameIndexRegistry::GetDispatchEntry(uint32_t token) const noexcept {
    uint32_t slot = TokenToSlot(token);
    if (slot == ~0u) {
        std::fprintf(stderr, "DEBUG GetDispatchEntry: token=0x%08x -> TokenToSlot failed\n", token);
        return nullptr;
    }

    // Find which module owns this token.
    for (size_t mi = 0; mi < module_count_; ++mi) {
        const auto* mod = modules_[mi];
        if (mod == nullptr) continue;
        if (mod->dispatch_table == nullptr) {
            std::fprintf(stderr, "DEBUG GetDispatchEntry: module %zu dispatch_table is null\n", mi);
            continue;
        }
        if (mod->token_slot_entries == nullptr || mod->token_slot_entry_count == 0) {
            std::fprintf(stderr, "DEBUG GetDispatchEntry: module %zu token_slot_entries is null/empty\n", mi);
            continue;
        }

        // Fast check: is slot within this module's token range?
        if (slot >= mod->dispatch_table_size) {
            std::fprintf(stderr, "DEBUG GetDispatchEntry: module %zu slot %u >= dispatch_table_size %u\n", mi, slot, mod->dispatch_table_size);
            continue;
        }

        // Verify token belongs to this module via bsearch.
        const auto* entry = static_cast<const TokenSlotEntryV0*>(
            std::bsearch(&token,
                         mod->token_slot_entries,
                         mod->token_slot_entry_count,
                         sizeof(TokenSlotEntryV0),
                         CompareTokenSlot));
        if (entry != nullptr) {
            std::fprintf(stderr, "DEBUG GetDispatchEntry: token=0x%08x -> slot=%u FOUND in module %zu (dispatch_table=%p)\n", token, slot, mi, static_cast<const void*>(mod->dispatch_table));
            return &mod->dispatch_table[slot];
        }
    }

    std::fprintf(stderr, "DEBUG GetDispatchEntry: token=0x%08x -> NOT FOUND in any module (slot=%u)\n", token, slot);
    return nullptr;
}

DispatchEntryV0* NameIndexRegistry::GetDispatchEntryBySlot(
    size_t module_index, uint32_t slot) const noexcept {
    if (module_index >= module_count_) {
        std::fprintf(stderr, "DEBUG GetDispatchEntryBySlot: module_index %zu >= module_count_ %zu\n", module_index, module_count_);
        return nullptr;
    }
    const auto* mod = modules_[module_index];
    if (mod == nullptr) {
        std::fprintf(stderr, "DEBUG GetDispatchEntryBySlot: modules_[%zu] is null\n", module_index);
        return nullptr;
    }
    if (mod->dispatch_table == nullptr) {
        std::fprintf(stderr, "DEBUG GetDispatchEntryBySlot: module %zu dispatch_table is null\n", module_index);
        return nullptr;
    }
    if (slot >= mod->dispatch_table_size) {
        std::fprintf(stderr, "DEBUG GetDispatchEntryBySlot: module %zu slot %u >= dispatch_table_size %u\n", module_index, slot, mod->dispatch_table_size);
        return nullptr;
    }
    std::fprintf(stderr, "DEBUG GetDispatchEntryBySlot: module %zu slot %u -> dispatch_table[%u] flags=0x%08x\n",
        module_index, slot, slot, mod->dispatch_table[slot].flags);
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
    if (entry == nullptr) {
        std::fprintf(stderr, "DEBUG SetPatched: token=0x%08x -> GetDispatchEntry returned null\n", token);
        return;
    }

    if (patched) {
        entry->flags |= kDispatchPatched;
        entry->method_key = reinterpret_cast<uintptr_t>(method_key);
        std::fprintf(stderr, "DEBUG SetPatched: token=0x%08x -> SET flags=0x%08x method_key=%p\n",
            token, entry->flags, method_key);
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
    std::fprintf(stderr, "DEBUG RegisterModuleNameIndex called with module=%p\n", static_cast<const void*>(module));
    g_name_index_registry.RegisterModule(module);
    std::fprintf(stderr, "DEBUG RegisterModuleNameIndex done, module_count_=%zu\n", g_name_index_registry.ModuleCount());
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
