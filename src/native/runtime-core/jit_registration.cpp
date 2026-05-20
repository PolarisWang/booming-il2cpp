// -- JIT Mode Method Registration -------------------------------------------
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
// (InterpreterEntryDirect -> FastExecute -> InterpreterVM) instead of native
// AOT C++ code.

#include "jit_registration.h"

#include "hotpatch_table.h"
#include "patch_loader.h"

#include "bootstrap/bootstrap.h"

#include <cstdlib>
#include <new>

using namespace chaos::il2cpp::runtime_core;

extern "C" void RegisterJitMethods(const JitMethodEntry* entries, uint32_t count) noexcept {
    if (entries == nullptr || count == 0) return;

    auto& registry = GetHotpatchNameRegistry();

    // -- Create a shared metadata cache for JIT methods ----------------------
    // PatchMetadataCache normally wraps a .patchdata binary's metadata tables,
    // but in JIT mode we don't have one -- we only need it as a container for
    // the AOT bridge + image handle that ResolveSubjectId uses to walk the
    // reflection query model and resolve subject IDs to call_target pointers.
    //
    // Passing nullptr as the header is safe because:
    //   - GetBridge() / GetAotImage() don't touch header_
    //   - All patch-data-specific methods (GetString, GetBlob, ResolveToken,
    //     etc.) check for null header_ and return 0/nullptr gracefully.
    //   - These methods are NOT called in JIT mode (IR is built from
    //     AotCoreIr JSON, not from raw IL + .patchdata metadata tables).
    auto* jit_cache = new (std::nothrow) chaos::il2cpp::runtime_core::PatchMetadataCache(nullptr);
    if (jit_cache == nullptr) return;

    // Set the AOT bridge + image so ResolveSubjectId can decode subject IDs
    // via TryDecodeReflectionQueryImageHandle during lazy IR lowering.
    {
        const auto* bs = chaos::il2cpp::bootstrap::PeekBootstrapState();
        if (bs != nullptr && bs->is_bootstrapped) {
            const auto* bridge = chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
            if (bridge != nullptr && bs->aot_image_handle != 0) {
                jit_cache->SetAotBridge(bridge, bs->aot_image_handle);
            }
        }
    }

    for (uint32_t i = 0; i < count; i++) {
        const auto& entry = entries[i];
        if (entry.json == nullptr || entry.json_len == 0) continue;

        // Resolve metadata token to dispatch slot.
        uint32_t slot = registry.TokenToSlot(entry.module_id, entry.token);
        if (slot == ~0u) {
            std::fprintf(stderr, "[jit_reg] SKIP token=0x%08X module=%u (no matching slot)\n",
                static_cast<unsigned>(entry.token), static_cast<unsigned>(entry.module_id));
            continue;  // skip if no matching dispatch entry
        }

        // Heap-allocate a PatchMethod that lives for the program lifetime.
        // NOTE: Do NOT use memset/memzero -- PatchMethod contains atomics and
        // cached pointers that require proper C++ construction (see the
        // tiering-call-count-guard memory rule).
        auto* pm = new (std::nothrow) chaos::il2cpp::runtime_core::PatchMethod();
        if (pm == nullptr) continue;

        pm->aot_core_ir_json        = entry.json;
        pm->aot_core_ir_json_length = entry.json_len;
        pm->token                   = entry.token;
        pm->module_id               = entry.module_id;
        pm->metadata_cache          = jit_cache;  // enable call_target resolution

        // Activate the dispatch entry: sets kHotpatchActive flag and stores
        // the PatchMethod pointer as method_key so InterpreterEntryDirect
        // can find the AotCoreIr JSON and lower it to IR at runtime.
        registry.SetPatchedBySlot(entry.module_id, slot, true, pm);
    }
}
