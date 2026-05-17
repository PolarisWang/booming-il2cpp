/// Phase 10: Hotpatch dispatch + method_replacement integration test.
///
/// Separate file to avoid linter interference with interpreter_integration.cpp.
/// Registered in CMakeLists.txt as part of chaos_interpreter_integration.

#include "hotpatch_table.h"
#include "method_replacement.h"

#include <cstdint>
#include <cstring>

// ── Forward declaration ─────────────────────────────────────────────────
bool Test_MethodReplacementDispatch();

// ── Test implementation ─────────────────────────────────────────────────

// Validates the full chain:
//   1. Register a hotpatch module (simulating codegen-emitted data)
//   2. method_replacement::Resolve → nullptr (not registered)
//   3. method_replacement::Register → activates dispatch entry
//   4. method_replacement::Resolve → returns replacement thunk
//   5. Verify HotpatchIsActive on dispatch entry
//   6. method_replacement::Revert → deactivates
//   7. method_replacement::Resolve → nullptr again
//   8. Verify HotpatchIsActive is false after revert

bool Test_MethodReplacementDispatch()
{
    using namespace chaos::il2cpp::runtime_core;

    // ── Sentinel values ──
    void* const kOriginalFn = reinterpret_cast<void*>(
        static_cast<CHAOS_IL2CPP_UINTPTR>(0x1111ULL));
    void* const kReplacementFn = reinterpret_cast<void*>(
        static_cast<CHAOS_IL2CPP_UINTPTR>(0x2222ULL));
    constexpr CHAOS_IL2CPP_UINT32 kMethodToken = 0x42u;
    constexpr CHAOS_IL2CPP_UINT32 kSlot = 0u;

    // ── Step 1: Register a mock hotpatch module ──
    HotpatchMethodEntryV0 method_entries[] = {
        { "MyMethod", kMethodToken, 0u }
    };
    HotpatchTypeEntryV0 type_entries[] = {
        { "MyNS", "MyType", 0u, 1u }
    };
    HotpatchSlotEntryV0 token_slot_entries[] = {
        { kMethodToken, kSlot }
    };
    HotpatchEntryV0 entry_table[] = {
        { kOriginalFn, 0u, 0u }
    };

    HotpatchModuleV0 mock_module;
    std::memset(&mock_module, 0, sizeof(mock_module));
    mock_module.type_entries       = type_entries;
    mock_module.type_entry_count   = 1u;
    mock_module.method_entries     = method_entries;
    mock_module.method_entry_count = 1u;
    mock_module.token_slot_entries     = token_slot_entries;
    mock_module.token_slot_entry_count = 1u;
    mock_module.entry_table        = entry_table;
    mock_module.entry_table_size   = 1u;

    RegisterHotpatchModule(&mock_module);

    // ── Step 2: Resolve before Register → nullptr ──
    void* resolved = chaos::il2cpp::method_replacement::Resolve(kMethodToken);
    if (resolved != nullptr) return false;

    // ── Step 3: Register the replacement → true ──
    if (!chaos::il2cpp::method_replacement::Register(kMethodToken, kReplacementFn))
        return false;

    // ── Step 4: Resolve after Register → returns thunk ──
    resolved = chaos::il2cpp::method_replacement::Resolve(kMethodToken);
    if (resolved != kReplacementFn) return false;

    // ── Step 5: Verify dispatch entry is active ──
    auto& registry = GetHotpatchNameRegistry();
    uint64_t composite = registry.FindToken(kMethodToken);
    if (composite == 0) return false;

    uint32_t mod_id = ExtractModuleId(composite);
    uint32_t slot = registry.TokenToSlot(mod_id, kMethodToken);
    if (slot == ~0u) return false;

    HotpatchEntryV0* entry = registry.GetDispatchEntry(mod_id, kMethodToken);
    if (entry == nullptr) return false;
    if (!HotpatchIsActive(*entry)) return false;
    if (entry->method_key != reinterpret_cast<uintptr_t>(kReplacementFn)) return false;

    // ── Step 6: Revert ──
    if (!chaos::il2cpp::method_replacement::Revert(kMethodToken))
        return false;

    // ── Step 7: Resolve after Revert → nullptr ──
    resolved = chaos::il2cpp::method_replacement::Resolve(kMethodToken);
    if (resolved != nullptr) return false;

    // ── Step 8: Verify dispatch entry is no longer active ──
    if (HotpatchIsActive(*entry)) return false;

    return true;
}
