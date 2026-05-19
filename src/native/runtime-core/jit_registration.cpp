// ── JIT Mode Method Registration ─────────────────────────────────────────
//
// RegisterJitMethods is called once at program startup (from runtime-entry.cpp)
// when the codegen was run with --mode jit.  It receives an array of
// JitMethodEntry structs emitted by the codegen (each containing the AotCoreIr
// JSON for one method) and activates interpreter dispatch for every entry.
//
// The function:
//   1. Heap-allocates PatchMethod objects (one per entry)
//   2. Populates aot_core_ir_json, token, and module_id
//   3. Calls SetPatchedBySlot() to flag the dispatch entry as active
//
// After this, all methods execute through the interpreter's tiered pipeline
// (InterpreterEntryDirect → FastExecute → InterpreterVM) instead of native
// AOT C++ code.

#include "jit_registration.h"

#include "hotpatch_table.h"
#include "patch_loader.h"

#include <cstdlib>
#include <new>

using namespace chaos::il2cpp::runtime_core;

extern "C" void RegisterJitMethods(const JitMethodEntry* entries, uint32_t count) noexcept {
    if (entries == nullptr || count == 0) return;

    auto& registry = GetHotpatchNameRegistry();

    for (uint32_t i = 0; i < count; i++) {
        const auto& entry = entries[i];
        if (entry.json == nullptr || entry.json_len == 0) continue;

        // Resolve metadata token → dispatch slot.
        uint32_t slot = registry.TokenToSlot(entry.module_id, entry.token);
        if (slot == ~0u) continue;  // skip if no matching dispatch entry

        // Heap-allocate a PatchMethod that lives for the program lifetime.
        // NOTE: Do NOT use memset/memzero — PatchMethod contains atomics and
        // cached pointers that require proper C++ construction (see the
        // tiering-call-count-guard memory rule).
        auto* pm = new (std::nothrow) chaos::il2cpp::runtime_core::PatchMethod();
        if (pm == nullptr) continue;

        pm->aot_core_ir_json        = entry.json;
        pm->aot_core_ir_json_length = entry.json_len;
        pm->token                   = entry.token;
        pm->module_id               = entry.module_id;

        // Activate the dispatch entry: sets kHotpatchActive flag and stores
        // the PatchMethod pointer as method_key so InterpreterEntryDirect
        // can find the AotCoreIr JSON and lower it to IR at runtime.
        registry.SetPatchedBySlot(entry.module_id, slot, true, pm);
    }
}
