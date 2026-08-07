/// hotupdate_verification_test.cpp — HotUpdate dispatch chain verification (gtest)
///
/// Tests the full register/resolve/revert cycle via the bootstrap dispatch chain,
/// verifying that FindMethodPointerByToken returns original → patched → original
/// pointers through the method replacement lifecycle.

#include "bootstrap.h"
#include "codegen_bridge.h"
#include "method_replacement.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <cstdint>
#include <cstring>

// AOT symbol stubs (required by chaos_interpreter and chaos_bootstrap linkage)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

namespace {

// Synthetic original thunks — each returns a known sentinel value (0xA0000000 + index).
void* OriginalThunk_00() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 0u)); }
void* OriginalThunk_01() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 1u)); }
void* OriginalThunk_02() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 2u)); }
void* OriginalThunk_03() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 3u)); }
void* OriginalThunk_04() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 4u)); }
void* OriginalThunk_05() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 5u)); }
void* OriginalThunk_06() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 6u)); }
void* OriginalThunk_07() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 7u)); }
void* OriginalThunk_08() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 8u)); }
void* OriginalThunk_09() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 9u)); }
void* OriginalThunk_10() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 10u)); }
void* OriginalThunk_11() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 11u)); }
void* OriginalThunk_12() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 12u)); }
void* OriginalThunk_13() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 13u)); }
void* OriginalThunk_14() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 14u)); }
void* OriginalThunk_15() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 15u)); }
void* OriginalThunk_16() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 16u)); }
void* OriginalThunk_17() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xA0000000u + 17u)); }

void* (*kOriginalThunks[])() = {
    &OriginalThunk_00, &OriginalThunk_01, &OriginalThunk_02, &OriginalThunk_03,
    &OriginalThunk_04, &OriginalThunk_05, &OriginalThunk_06, &OriginalThunk_07,
    &OriginalThunk_08, &OriginalThunk_09, &OriginalThunk_10, &OriginalThunk_11,
    &OriginalThunk_12, &OriginalThunk_13, &OriginalThunk_14, &OriginalThunk_15,
    &OriginalThunk_16, &OriginalThunk_17,
};

// Synthetic patched thunks — return sentinel B value (0xB0000000 + index).
void* PatchedThunk_00() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 0u)); }
void* PatchedThunk_01() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 1u)); }
void* PatchedThunk_02() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 2u)); }
void* PatchedThunk_03() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 3u)); }
void* PatchedThunk_04() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 4u)); }
void* PatchedThunk_05() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 5u)); }
void* PatchedThunk_06() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 6u)); }
void* PatchedThunk_07() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 7u)); }
void* PatchedThunk_08() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 8u)); }
void* PatchedThunk_09() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 9u)); }
void* PatchedThunk_10() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 10u)); }
void* PatchedThunk_11() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 11u)); }
void* PatchedThunk_12() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 12u)); }
void* PatchedThunk_13() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 13u)); }
void* PatchedThunk_14() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 14u)); }
void* PatchedThunk_15() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 15u)); }
void* PatchedThunk_16() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 16u)); }
void* PatchedThunk_17() { return reinterpret_cast<void*>(static_cast<uintptr_t>(0xB0000000u + 17u)); }

void* (*kPatchedThunks[])() = {
    &PatchedThunk_00, &PatchedThunk_01, &PatchedThunk_02, &PatchedThunk_03,
    &PatchedThunk_04, &PatchedThunk_05, &PatchedThunk_06, &PatchedThunk_07,
    &PatchedThunk_08, &PatchedThunk_09, &PatchedThunk_10, &PatchedThunk_11,
    &PatchedThunk_12, &PatchedThunk_13, &PatchedThunk_14, &PatchedThunk_15,
    &PatchedThunk_16, &PatchedThunk_17,
};

// Struct layout must match chaos::il2cpp::bootstrap's internal MethodPointerEntry.
struct MethodPointerEntry {
    uint32_t method_token;
    void* method_pointer;
};

constexpr uint32_t kMethodCount = 18u;
constexpr uint32_t kBaseToken = 0x06000001u;
constexpr uintptr_t kSentinelOriginalBase = 0xA0000000u;
constexpr uintptr_t kSentinelPatchedBase = 0xB0000000u;
constexpr uint32_t kAotDomainIdFallback = 0u;

}  // namespace

TEST(HotUpdate_Verification, FullLifecycle) {
    using chaos::il2cpp::bootstrap::FindMethodPointerByToken;
    using chaos::il2cpp::bootstrap::PeekBootstrapState;
    using chaos::il2cpp::method_replacement::ActiveCount;
    using chaos::il2cpp::method_replacement::Register;
    using chaos::il2cpp::method_replacement::Resolve;
    using chaos::il2cpp::method_replacement::Revert;
    using chaos::il2cpp::method_replacement::RevertAll;

    // Build synthetic method pointer table.
    MethodPointerEntry entries[kMethodCount];
    for (uint32_t i = 0u; i < kMethodCount; i++) {
        entries[i].method_token = kBaseToken + i;
        entries[i].method_pointer = reinterpret_cast<void*>(kOriginalThunks[i]);
    }

    // Synthetic code registrations — minimal viable state.
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
    ASSERT_NE(bridge, nullptr) << "codegen bridge is null";

    ASSERT_EQ(bridge->register_codegen(&code_reg, &meta_reg, &options), CHAOS_BRIDGE_STATUS_OK)
        << "register_codegen failed";

    ASSERT_EQ(bridge->bootstrap_runtime(), CHAOS_BRIDGE_STATUS_OK)
        << "bootstrap_runtime failed";

    ASSERT_NE(PeekBootstrapState(), nullptr)
        << "bootstrap state is null after bootstrap";

    RevertAll();
    EXPECT_EQ(ActiveCount(), 0);

    uint32_t passed_count = 0u;
    uint32_t failed_count = 0u;

    for (uint32_t i = 0u; i < kMethodCount; i++) {
        const uint32_t token = kBaseToken + i;
        const uintptr_t expected_sentinel_a = kSentinelOriginalBase + i;
        const uintptr_t expected_sentinel_b = kSentinelPatchedBase + i;

        bool step_ok = true;

        // Step 1: Find original pointer via bootstrap dispatch chain.
        void* original_ptr = FindMethodPointerByToken(token);
        ASSERT_NE(original_ptr, nullptr) << "FindMethodPointerByToken returned null for token " << token;

        // Step 2: Call original thunk — must return sentinel A.
        auto* original_thunk = reinterpret_cast<void* (*)()>(original_ptr);
        uintptr_t original_value = reinterpret_cast<uintptr_t>(original_thunk());
        if (original_value != expected_sentinel_a) {
            ADD_FAILURE() << "[" << i << "] original returned 0x" << std::hex << original_value
                          << ", expected 0x" << expected_sentinel_a;
            step_ok = false;
        }

        // Step 3: Register patch replacement.
        if (!Register(token, reinterpret_cast<void*>(kPatchedThunks[i]))) {
            ADD_FAILURE() << "[" << i << "] Register returned false";
            step_ok = false;
        }

        // Step 4: Resolve directly — must return patched thunk.
        void* resolved_after_patch = Resolve(token);
        if (resolved_after_patch == nullptr) {
            ADD_FAILURE() << "[" << i << "] Resolve returned null after Register";
            step_ok = false;
        }
        if (resolved_after_patch != reinterpret_cast<void*>(kPatchedThunks[i])) {
            ADD_FAILURE() << "[" << i << "] Resolve returned wrong pointer after Register";
            step_ok = false;
        }

        // Step 5: FindMethodPointerByToken — must return patched thunk (integration test).
        void* dispatch_ptr = FindMethodPointerByToken(token);
        if (dispatch_ptr == nullptr) {
            ADD_FAILURE() << "[" << i << "] FindMethodPointerByToken returned null after Register";
            step_ok = false;
        }
        if (dispatch_ptr != reinterpret_cast<void*>(kPatchedThunks[i])) {
            ADD_FAILURE() << "[" << i << "] FindMethodPointerByToken did not return patched thunk";
            step_ok = false;
        }

        // Step 6: Call the dispatch pointer — must return sentinel B.
        uintptr_t patched_value = 0u;
        if (dispatch_ptr != nullptr) {
            auto* thunk = reinterpret_cast<void* (*)()>(dispatch_ptr);
            patched_value = reinterpret_cast<uintptr_t>(thunk());
            if (patched_value != expected_sentinel_b) {
                ADD_FAILURE() << "[" << i << "] patched returned 0x" << std::hex << patched_value
                              << ", expected 0x" << expected_sentinel_b;
                step_ok = false;
            }
        }

        // Step 7: Revert replacement.
        if (!Revert(token)) {
            ADD_FAILURE() << "[" << i << "] Revert returned false";
            step_ok = false;
        }

        // Step 8: After revert, FindMethodPointerByToken must return original pointer.
        void* after_revert_ptr = FindMethodPointerByToken(token);
        if (after_revert_ptr == nullptr) {
            ADD_FAILURE() << "[" << i << "] FindMethodPointerByToken returned null after Revert";
            step_ok = false;
        }
        if (after_revert_ptr != original_ptr) {
            ADD_FAILURE() << "[" << i << "] after Revert, pointer does not match original";
            step_ok = false;
        }

        // Step 9: Call after revert — must return sentinel A again.
        if (after_revert_ptr != nullptr) {
            auto* thunk = reinterpret_cast<void* (*)()>(after_revert_ptr);
            uintptr_t reverted_value = reinterpret_cast<uintptr_t>(thunk());
            if (reverted_value != expected_sentinel_a) {
                ADD_FAILURE() << "[" << i << "] after Revert, call returned 0x" << std::hex
                              << reverted_value << ", expected 0x" << expected_sentinel_a;
                step_ok = false;
            }
        }

        if (step_ok) {
            passed_count++;
        } else {
            failed_count++;
        }
    }

    EXPECT_EQ(passed_count, kMethodCount);
    EXPECT_EQ(failed_count, 0u);
}
