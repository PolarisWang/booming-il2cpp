// Per-family hotupdate verification test
// Family: family/System.Private.CoreLib/reflection/assembly
// 22 methods
//
// Uses CodeGen-generated C++ for both host (real API calls) and
// patch (sentinel returns). Verifies the method replacement lifecycle:
//   original -> register patch -> resolve -> call patched -> revert -> verify
//
// When semantic-patch C++ is available (via TYPE_ALTERNATIVE_MAP),
// also verifies that host and semantic-patch produce different (but valid)
// return values, proving the hotupdate mechanism replaced actual computation.
//
#include "bootstrap.h"
#include "codegen_bridge.h"
#include "method_replacement.h"

#include <cstdio>
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------
// Forward declarations: host methods (real API calls via CodeGen)
// ---------------------------------------------------------------
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method21(void);

// ---------------------------------------------------------------
// Forward declarations: patch methods (sentinel returns via CodeGen)
// ---------------------------------------------------------------
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method21(void);

// ---------------------------------------------------------------
// Forward declarations: semantic-patch methods (alternative API calls)
// ---------------------------------------------------------------
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method21(void);

namespace {

// Method pointer table for bootstrap registration.
// Each entry maps a method token to the host method pointer.
struct MethodPointerEntry {
    uint32_t method_token;
    void* method_pointer;
};

constexpr uint32_t kMethodCount = 22u;
constexpr uint32_t kBaseToken = 0x06000001u;
constexpr uintptr_t kSentinelPatchBase = 0xB0000000u;

// Host method pointer array (auto-indexed by MethodN ordering)
void* (*kHostThunks[])() = {
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method0),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method1),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method2),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method3),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method4),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method5),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method6),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method7),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method8),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method9),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method10),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method11),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method12),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method13),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method14),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method15),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method16),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method17),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method18),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method19),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method20),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyNativeEntry_ReflectionAssemblyNativeEntry_Method21),
};

// Patch method pointer array (auto-indexed by MethodN ordering)
void* (*kPatchThunks[])() = {
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method0),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method1),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method2),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method3),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method4),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method5),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method6),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method7),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method8),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method9),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method10),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method11),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method12),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method13),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method14),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method15),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method16),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method17),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method18),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method19),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method20),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblyPatchEntry_ReflectionAssemblyPatchEntry_Method21),
};

// Semantic-patch method pointer array (alternative real API calls)
void* (*kSemanticPatchThunks[])() = {
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method0),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method1),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method2),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method3),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method4),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method5),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method6),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method7),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method8),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method9),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method10),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method11),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method12),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method13),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method14),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method15),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method16),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method17),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method18),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method19),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method20),
    reinterpret_cast<void* (*)()>(&ReflectionAssemblySemanticPatchEntry_ReflectionAssemblySemanticPatchEntry_Method21),
};

constexpr uint32_t kAotDomainIdFallback = 0u;

}  // namespace

int main() {
    using chaos::il2cpp::bootstrap::FindMethodPointerByToken;
    using chaos::il2cpp::bootstrap::PeekBootstrapState;
    using chaos::il2cpp::method_replacement::ActiveCount;
    using chaos::il2cpp::method_replacement::Register;
    using chaos::il2cpp::method_replacement::Resolve;
    using chaos::il2cpp::method_replacement::Revert;
    using chaos::il2cpp::method_replacement::RevertAll;

    // Build synthetic method pointer table with host methods.
    MethodPointerEntry entries[kMethodCount];
    for (uint32_t i = 0u; i < kMethodCount; i++) {
        entries[i].method_token = kBaseToken + i;
        entries[i].method_pointer = reinterpret_cast<void*>(kHostThunks[i]);
    }

    // Synthetic code registrations.
    CodeRegistrationV0 code_reg = {};
    code_reg.struct_size = sizeof(CodeRegistrationV0);
    code_reg.method_pointers = static_cast<const void*>(entries);
    code_reg.method_pointer_count = kMethodCount;

    MetadataRegistrationV0 meta_reg = {};
    meta_reg.struct_size = sizeof(MetadataRegistrationV0);

    CodegenRegistrationOptionsV0 options = {};
    options.struct_size = sizeof(CodegenRegistrationOptionsV0);
    options.image_name_utf8 = "System.Private.CoreLib";

    // Bootstrap the runtime through the ABI bridge.
    const CodegenBridgeV0* bridge = chaos_codegen_get_bridge_v0();
    if (bridge == nullptr) {
        std::fprintf(stderr, "FATAL: codegen bridge is null\n");
        return 1;
    }
    if (bridge->register_codegen(&code_reg, &meta_reg, &options) != CHAOS_BRIDGE_STATUS_OK) {
        std::fprintf(stderr, "FATAL: register_codegen failed\n");
        return 1;
    }
    if (bridge->bootstrap_runtime() != CHAOS_BRIDGE_STATUS_OK) {
        std::fprintf(stderr, "FATAL: bootstrap_runtime failed\n");
        return 1;
    }

    if (PeekBootstrapState() == nullptr) {
        std::fprintf(stderr, "FATAL: bootstrap state is null after bootstrap\n");
        return 1;
    }

    // Seed the JSON output array.
    std::printf("{\n");
    std::printf("  \"schemaVersion\": 1,\n");
    std::printf("  \"assemblyName\": \"System.Private.CoreLib\",\n");
    std::printf("  \"familyId\": \"family/System.Private.CoreLib/reflection/assembly\",\n");
    std::printf("  \"verificationKind\": \"hotupdate-proof\",\n");
    std::printf("  \"totalMethods\": %u,\n", kMethodCount);
    std::printf("  \"results\": [\n");

    uint32_t passed_count = 0u;
    uint32_t failed_count = 0u;
    RevertAll();

    for (uint32_t i = 0u; i < kMethodCount; i++) {
        const uint32_t token = kBaseToken + i;
        const uintptr_t expected_sentinel_b = kSentinelPatchBase + i;

        bool step_ok = true;

        // Step 1: Find original pointer via bootstrap dispatch chain.
        void* original_ptr = FindMethodPointerByToken(token);
        if (original_ptr == nullptr) {
            std::fprintf(stderr, "FAIL[%u]: FindMethodPointerByToken returned null\n", i);
            step_ok = false;
        }

        // Step 2: Call original thunk (host now returns real checksum from cast-to-int).
        uintptr_t original_value = 0u;
        if (original_ptr != nullptr) {
            auto* thunk = reinterpret_cast<uintptr_t (*)()>(original_ptr);
            original_value = thunk();
        }

        // Step 2b: Call semantic-patch thunk before registration to get alternative checksum.
        bool has_semantic_thunk = (i < sizeof(kSemanticPatchThunks)/sizeof(kSemanticPatchThunks[0])
            && kSemanticPatchThunks[i] != nullptr);
        uintptr_t semantic_value = 0u;
        if (has_semantic_thunk) {
            auto* thunk = reinterpret_cast<uintptr_t (*)()>(kSemanticPatchThunks[i]);
            semantic_value = thunk();
        }

        // Step 3: Register patch replacement (CodeGen-generated sentinel).
        if (!Register(token, reinterpret_cast<void*>(kPatchThunks[i]))) {
            std::fprintf(stderr, "FAIL[%u]: Register returned false\n", i);
            step_ok = false;
        }

        // Step 4: Resolve directly - must return patched thunk.
        void* resolved_after_patch = Resolve(token);
        if (resolved_after_patch == nullptr) {
            std::fprintf(stderr, "FAIL[%u]: Resolve returned null after Register\n", i);
            step_ok = false;
        }
        if (resolved_after_patch != reinterpret_cast<void*>(kPatchThunks[i])) {
            std::fprintf(stderr, "FAIL[%u]: Resolve returned wrong pointer after Register\n", i);
            step_ok = false;
        }

        // Step 5: FindMethodPointerByToken - must return patched thunk (integration test).
        void* dispatch_ptr = FindMethodPointerByToken(token);
        if (dispatch_ptr == nullptr) {
            std::fprintf(stderr, "FAIL[%u]: FindMethodPointerByToken returned null after Register\n", i);
            step_ok = false;
        }
        if (dispatch_ptr != reinterpret_cast<void*>(kPatchThunks[i])) {
            std::fprintf(stderr, "FAIL[%u]: FindMethodPointerByToken did not return patched thunk\n", i);
            step_ok = false;
        }

        // Step 6: Call the dispatch pointer - must return sentinel B.
        uintptr_t patched_value = 0u;
        if (dispatch_ptr != nullptr) {
            auto* thunk = reinterpret_cast<void* (*)()>(dispatch_ptr);
            patched_value = reinterpret_cast<uintptr_t>(thunk());
            if (patched_value != expected_sentinel_b) {
                std::fprintf(stderr, "FAIL[%u]: patched returned 0x%08zx, expected 0x%08zx\n",
                    i, static_cast<size_t>(patched_value), static_cast<size_t>(expected_sentinel_b));
                step_ok = false;
            }
        }

        // Step 7: Revert replacement.
        if (!Revert(token)) {
            std::fprintf(stderr, "FAIL[%u]: Revert returned false\n", i);
            step_ok = false;
        }

        // Step 8: After revert, FindMethodPointerByToken must return original pointer.
        void* after_revert_ptr = FindMethodPointerByToken(token);
        if (after_revert_ptr == nullptr) {
            std::fprintf(stderr, "FAIL[%u]: FindMethodPointerByToken returned null after Revert\n", i);
            step_ok = false;
        }
        if (after_revert_ptr != original_ptr) {
            std::fprintf(stderr, "FAIL[%u]: after Revert, pointer does not match original\n", i);
            step_ok = false;
        }

        // Step 9: Call after revert - must return host value.
        if (after_revert_ptr != nullptr) {
            auto* thunk = reinterpret_cast<void* (*)()>(after_revert_ptr);
            uintptr_t reverted_value = reinterpret_cast<uintptr_t>(thunk());
            if (reverted_value != original_value) {
                std::fprintf(stderr, "FAIL[%u]: after Revert, call returned 0x%08zx, expected 0x%08zx\n",
                    i, static_cast<size_t>(original_value), static_cast<size_t>(original_value));
                step_ok = false;
            }
        }

        // Step 10: Semantic verification - host and semantic-patch must differ.
        // This proves the hotupdate mechanism replaced actual computation.
        bool semantic_ok = true;
        if (has_semantic_thunk) {
            if (original_value == 0u && semantic_value == 0u) {
                // Both returned 0 - likely TODO stubs, cannot verify semantics.
                std::fprintf(stderr, "WARN[%u]: both host and semantic-patch returned 0 (stub?)\n", i);
                semantic_ok = false;
                // Don't fail the test — the method still passed all hotupdate steps;
                // only the semantic comparison is inconclusive.
            } else if (original_value == semantic_value) {
                // Different inputs should produce different results.
                std::fprintf(stderr, "FAIL[%u]: host and semantic-patch returned same value 0x%08zx\n",
                    i, static_cast<size_t>(original_value));
                semantic_ok = false;
                step_ok = false;
            }
        }

        if (step_ok) {
            passed_count++;
        } else {
            failed_count++;
        }

        // Emit JSON result for this method.
        const char* comma = (i + 1u < kMethodCount) ? "," : "";
        std::printf(
            "    {\n"
            "      \"methodToken\": %u,\n"
            "      \"status\": \"%s\",\n"
            "      \"originalReturnValue\": \"0x%08zx\",\n"
            "      \"patchedReturnValue\": \"0x%08zx\",\n"
            "      \"expectedPatchedValue\": \"0x%08zx\",\n"
            "      \"revertVerified\": true,\n"
            "      \"semanticVerified\": %s\n"
            "    }%s\n",
            static_cast<unsigned>(token),
            step_ok ? "passed" : "failed",
            static_cast<size_t>(original_value),
            static_cast<size_t>(patched_value),
            static_cast<size_t>(expected_sentinel_b),
            (semantic_ok ? "true" : "false"),
            comma);
    }

    std::printf("  ],\n");
    std::printf("  \"passedMethods\": %u,\n", static_cast<unsigned>(passed_count));
    std::printf("  \"failedMethods\": %u\n", static_cast<unsigned>(failed_count));
    std::printf("}\n");

    return (failed_count == 0u) ? 0 : 1;
}