/// bootstrap_test_fixture.h — Reusable test fixture for bootstrap module tests.
///
/// Provides synthetic codegen registration data and helper methods to
/// drive the two-phase init (RegisterCodegen → BootstrapRuntime) in tests.
/// Follows the pattern established by hotupdate_verification_test.cpp.

#pragma once

#include <gtest/gtest.h>

#include <chaos/common.h>
#include <bootstrap.h>
#include <codegen_bridge.h>

#include <cstdint>
#include <cstring>

// ── Stub data structures matching the internal bootstrap layout ──────────
// These must match the struct definitions in bootstrap.cpp (anonymous ns).

struct BootstrapMethodPointerEntry {
    uint32_t method_token;
    void* method_pointer;
};

struct BootstrapInvokerEntry {
    uint32_t method_token;
    void* invoker_pointer;
};

struct BootstrapUnresolvedVirtualCallEntry {
    uint32_t instance_type_token;
    uint32_t declared_method_token;
    void* resolved_method;
};

// ── Base token values for synthetic data ─────────────────────────────────

constexpr uint32_t kBaseMethodToken = 0x06000001u;
constexpr uint32_t kBaseTypeToken   = 0x02000001u;
constexpr uint32_t kBaseInvokerToken = 0x06000001u;
constexpr uint32_t kMethodCount     = 8u;
constexpr uint32_t kInvokerCount    = 4u;
constexpr uint32_t kTypeCapCount    = 4u;
constexpr uint32_t kVirtualCallCount = 3u;

// ── Synthetic thunk helpers ──────────────────────────────────────────────

/// Returns a unique sentinel value per (thunk_index << 16) | thunk_offset.
/// Callers cast the result to function pointers as needed.
inline void* SentinelThunk(uint32_t base, uint32_t index) {
    return reinterpret_cast<void*>(
        static_cast<uintptr_t>(base) + static_cast<uintptr_t>(index));
}

// ── BootstrapTestFixture ─────────────────────────────────────────────────

class BootstrapTestFixture : public ::testing::Test {
protected:
    // Synthetic data buffers (stack-allocated in SetUp)
    BootstrapMethodPointerEntry method_entries_[kMethodCount];
    BootstrapInvokerEntry invoker_entries_[kInvokerCount];
    RuntimeTypeCapabilityEntryV0 type_cap_entries_[kTypeCapCount];
    BootstrapUnresolvedVirtualCallEntry virtual_call_entries_[kVirtualCallCount];

    // Registration structures
    CodeRegistrationV0 code_reg_;
    MetadataRegistrationV0 meta_reg_;
    CodegenRegistrationOptionsV0 options_;

    void SetUp() override {
        // Init synthetic method pointer entries
        for (uint32_t i = 0; i < kMethodCount; i++) {
            method_entries_[i].method_token = kBaseMethodToken + i;
            method_entries_[i].method_pointer = SentinelThunk(0xA0000000u, i);
        }

        // Init synthetic invoker entries
        for (uint32_t i = 0; i < kInvokerCount; i++) {
            invoker_entries_[i].method_token = kBaseInvokerToken + i;
            invoker_entries_[i].invoker_pointer = SentinelThunk(0xB0000000u, i);
        }

        // Init synthetic type capability entries
        RuntimeTypeCapabilityInfoV0 cap_info{};
        cap_info.struct_size = sizeof(RuntimeTypeCapabilityInfoV0);
        cap_info.capability_bits = 0;
        cap_info.value_size_bytes = 4;
        for (uint32_t i = 0; i < kTypeCapCount; i++) {
            type_cap_entries_[i].type_token = kBaseTypeToken + i;
            type_cap_entries_[i].capability_info = cap_info;
            type_cap_entries_[i].capability_info.value_size_bytes = 4u * (i + 1u);
        }

        // Init synthetic unresolved virtual call entries
        for (uint32_t i = 0; i < kVirtualCallCount; i++) {
            virtual_call_entries_[i].instance_type_token = kBaseTypeToken + i;
            virtual_call_entries_[i].declared_method_token = kBaseMethodToken + i;
            virtual_call_entries_[i].resolved_method = SentinelThunk(0xC0000000u, i);
        }

        // Init registration structures (zero-initialized, then fill)
        std::memset(&code_reg_, 0, sizeof(code_reg_));
        code_reg_.struct_size = sizeof(CodeRegistrationV0);
        code_reg_.method_pointers = method_entries_;
        code_reg_.method_pointer_count = kMethodCount;
        code_reg_.invoker_pointers = invoker_entries_;
        code_reg_.invoker_pointer_count = kInvokerCount;
        code_reg_.type_capabilities = type_cap_entries_;
        code_reg_.type_capability_count = kTypeCapCount;
        code_reg_.unresolved_virtual_calls = virtual_call_entries_;
        code_reg_.unresolved_virtual_call_count = kVirtualCallCount;

        std::memset(&meta_reg_, 0, sizeof(meta_reg_));
        meta_reg_.struct_size = sizeof(MetadataRegistrationV0);

        std::memset(&options_, 0, sizeof(options_));
        options_.struct_size = sizeof(CodegenRegistrationOptionsV0);
        options_.image_name_utf8 = "TestModule";
    }

    void TearDown() override {
        // No heap allocations in fixture, nothing to clean up.
    }

    /// Register with the default synthetic data. Returns the bridge status.
    BridgeStatus RegisterMinimal() {
        const CodegenBridgeV0* bridge = chaos_codegen_get_bridge_v0();
        if (bridge == nullptr) return CHAOS_BRIDGE_STATUS_INTERNAL_ERROR;
        return bridge->register_codegen(&code_reg_, &meta_reg_, &options_);
    }

    /// Bootstrap with the default synthetic data. Must call RegisterMinimal() first.
    BridgeStatus BootstrapMinimal() {
        const CodegenBridgeV0* bridge = chaos_codegen_get_bridge_v0();
        if (bridge == nullptr) return CHAOS_BRIDGE_STATUS_INTERNAL_ERROR;
        return bridge->bootstrap_runtime();
    }

    /// Full two-phase init: RegisterMinimal() + BootstrapMinimal().
    BridgeStatus FullInit() {
        BridgeStatus status = RegisterMinimal();
        if (status != CHAOS_BRIDGE_STATUS_OK) return status;
        return BootstrapMinimal();
    }

    /// Get the process-wide bridge.
    const CodegenBridgeV0* GetBridge() const {
        return chaos_codegen_get_bridge_v0();
    }
};
