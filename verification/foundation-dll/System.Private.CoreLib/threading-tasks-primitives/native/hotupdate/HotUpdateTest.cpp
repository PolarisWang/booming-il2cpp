// Per-family hotupdate verification test
// Family: family/System.Private.CoreLib/threading/tasks/primitives
// 15 methods
//
// Uses CodeGen-generated C++ for both host (real API calls) and
// patch (sentinel returns). Verifies the method replacement lifecycle:
//   original -> register patch -> resolve -> call patched -> revert -> verify
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
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method14(void);

// ---------------------------------------------------------------
// Forward declarations: patch methods (sentinel returns via CodeGen)
// ---------------------------------------------------------------
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method14(void);

namespace {

// Method pointer table for bootstrap registration.
// Each entry maps a method token to the host method pointer.
struct MethodPointerEntry {
    uint32_t method_token;
    void* method_pointer;
};

constexpr uint32_t kMethodCount = 15u;
constexpr uint32_t kBaseToken = 0x06000001u;
constexpr uintptr_t kSentinelPatchBase = 0xB0000000u;

// Host method pointer array (auto-indexed by MethodN ordering)
void* (*kHostThunks[])() = {
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method0),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method1),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method2),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method3),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method4),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method5),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method6),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method7),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method8),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method9),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method10),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method11),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method12),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method13),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesNativeEntry_ThreadingTasksPrimitivesNativeEntry_Method14),
};

// Patch method pointer array (auto-indexed by MethodN ordering)
void* (*kPatchThunks[])() = {
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method0),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method1),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method2),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method3),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method4),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method5),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method6),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method7),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method8),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method9),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method10),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method11),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method12),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method13),
    reinterpret_cast<void* (*)()>(&ThreadingTasksPrimitivesPatchEntry_ThreadingTasksPrimitivesPatchEntry_Method14),
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
    std::printf("  \"familyId\": \"family/System.Private.CoreLib/threading/tasks/primitives\",\n");
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
            "      \"semanticVerified\": false\n"
            "    }%s\n",
            static_cast<unsigned>(token),
            step_ok ? "passed" : "failed",
            static_cast<size_t>(original_value),
            static_cast<size_t>(patched_value),
            static_cast<size_t>(expected_sentinel_b),
            comma);
    }

    std::printf("  ],\n");
    std::printf("  \"passedMethods\": %u,\n", static_cast<unsigned>(passed_count));
    std::printf("  \"failedMethods\": %u\n", static_cast<unsigned>(failed_count));
    std::printf("}\n");

    return (failed_count == 0u) ? 0 : 1;
}