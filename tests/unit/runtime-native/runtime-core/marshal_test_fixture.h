/// marshal_test_fixture.h — Reusable test fixture for P/Invoke & Marshal module tests.
///
/// Provides a fully bootstrapped runtime environment (GC heap, ThreadState,
/// RuntimeState, memory_domain) so that marshal functions (AllocateMarshalBlock,
/// StringNewUtf8, IsAttached, etc.) work correctly.
///
/// Uses a minimal runtime initialization that avoids starting BGC background
/// threads (which crash in test environments due to incomplete thread services).
/// Marshal memory allocations fall back to the RuntimeConfig's default allocator
/// (malloc) when no memory domain is active.
///
/// NOTE: RuntimeState and ThreadState are opaque types (forward-declared in
/// runtime_abi.h, defined only in header_layouts.cpp within the unity build).
/// This fixture allocates sufficient buffers and writes known-offset fields
/// (RuntimeConfig at offset 0, pointer fields at their natural alignment)
/// rather than accessing struct members directly.

#pragma once

#include <gtest/gtest.h>

#include <chaos/common.h>
#include <bootstrap.h>
#include <codegen_bridge.h>
// runtime_core.h is the unified header — includes engine_binding.h + runtime_abi.h
// in the correct dependency order (runtime_abi.h provides CHAOS_TYPE_CAPABILITY_*
// macros used by runtime_capability.h used by engine_binding.h).
#include <runtime_core.h>

#include <cinttypes>
#include <cstdint>
#include <cstdlib>
#include <cstring>

// ── Default allocator for RuntimeConfig (malloc fallback) ────────────────

extern "C" void* TestDefaultAllocate(CHAOS_IL2CPP_SIZE size, void* /*user_data*/) {
    return std::malloc(size);
}

extern "C" void TestDefaultDeallocate(void* ptr, void* /*user_data*/) {
    std::free(ptr);
}

// Known sizes for opaque struct layouts (from header_layouts.cpp):
//   RuntimeConfig : 48 bytes
//   RuntimeState  : RuntimeConfig(48) + RuntimeInternalState*(8) = 56 bytes
//   ThreadState   : RuntimeState*(8) + ThreadInternalState*(8) = 16 bytes

static constexpr size_t kRuntimeStateBufferSize  = 128;
static constexpr size_t kThreadStateBufferSize   = 64;
static constexpr size_t kRuntimeConfigSize       = 48;  // Actual sizeof(RuntimeConfig)
static constexpr size_t kRuntimeConfigAlign      = 8;
static constexpr size_t kOffsetAllocatorFn       = 8;   // offsetof(RuntimeConfig, allocator)
static constexpr size_t kOffsetDeallocatorFn     = 16;  // offsetof(RuntimeConfig, deallocator)
static constexpr size_t kOffsetAllocatorUserData = 24;  // offsetof(RuntimeConfig, allocator_user_data)
static constexpr size_t kOffsetReservedU32       = 32;  // offsetof(RuntimeConfig, reserved_u32)

/// Initialize a RuntimeConfig at the given buffer (must be >= kRuntimeConfigSize bytes).
inline void InitTestRuntimeConfig(void* buf, AllocatorFn alloc_fn, DeallocatorFn dealloc_fn) {
    std::memset(buf, 0, kRuntimeConfigSize);
    std::memcpy(buf, &kRuntimeConfigSize, sizeof(uint32_t));  // struct_size
    std::memcpy(static_cast<char*>(buf) + kOffsetAllocatorFn, &alloc_fn, sizeof(alloc_fn));
    std::memcpy(static_cast<char*>(buf) + kOffsetDeallocatorFn, &dealloc_fn, sizeof(dealloc_fn));
}

/// Initialize the first pointer field of an opaque struct at the 0 offset.
inline void InitOpaquePtrField(void* buf, const void* ptr) {
    std::memcpy(buf, &ptr, sizeof(ptr));
}

// ── Stub data structures (mirror bootstrap test fixture) ──────────────────

struct MarshalMethodPointerEntry {
    uint32_t method_token;
    void* method_pointer;
};

struct MarshalInvokerEntry {
    uint32_t method_token;
    void* invoker_pointer;
};

struct MarshalUnresolvedVirtualCallEntry {
    uint32_t instance_type_token;
    uint32_t declared_method_token;
    void* resolved_method;
};

// ── Base token values for synthetic data ─────────────────────────────────

constexpr uint32_t kMarshalBaseMethodToken = 0x06000001u;
constexpr uint32_t kMarshalBaseTypeToken   = 0x02000001u;
constexpr uint32_t kMarshalBaseInvokerToken = 0x06000001u;
constexpr uint32_t kMarshalMethodCount     = 8u;
constexpr uint32_t kMarshalInvokerCount    = 4u;
constexpr uint32_t kMarshalTypeCapCount    = 4u;
constexpr uint32_t kMarshalVirtualCallCount = 3u;

/// Returns a unique sentinel value per (thunk_index << 16) | thunk_offset.
inline void* MarshalSentinelThunk(uint32_t base, uint32_t index) {
    return reinterpret_cast<void*>(
        static_cast<uintptr_t>(base) + static_cast<uintptr_t>(index));
}

// ── Benchmark helper macros ──────────────────────────────────────────────

/// Wrapper that measures execution time of `expr` over `iterations` calls.
/// Reports to stdout via GTEST_MESSAGE_ at INFO level.
#define MARSHEL_BENCHMARK_OP(label, iterations, expr)                         \
    do {                                                                       \
        const auto bench_start = std::chrono::high_resolution_clock::now();    \
        for (uint64_t _i = 0; _i < (iterations); ++_i) { expr; }              \
        const auto bench_end = std::chrono::high_resolution_clock::now();      \
        const auto bench_ns = std::chrono::duration_cast<std::chrono::nanoseconds>( \
            bench_end - bench_start).count();                                  \
        std::printf("[BENCHMARK] %-40s %5" PRIu64 " iter  %8" PRIu64 " ns  %8" PRIu64 " ns/iter\n", \
                    (label), static_cast<uint64_t>(iterations),                \
                    static_cast<uint64_t>(bench_ns),                           \
                    static_cast<uint64_t>(bench_ns / (iterations)));           \
    } while (false)

// ── Minimal RuntimeState layout (mirrors header_layouts.cpp) ─────────────

namespace chaos { namespace il2cpp { namespace runtime_core {

// Forward declarations needed by marshal_abi and engine_binding functions.
void SetCurrentRuntimeState(RuntimeState* runtime_state);
void SetCurrentThreadState(ThreadState* thread_state);
RuntimeState* GetCurrentRuntimeState();
ThreadState* GetCurrentThreadState();

} } }  // namespace chaos::il2cpp::runtime_core

// ── MarshalTestFixture ───────────────────────────────────────────────────

class MarshalTestFixture : public ::testing::Test {
protected:
    // Shared runtime state (one per suite)
    static RuntimeState* s_runtime_state_;
    static ThreadState*  s_thread_state_;
    static bool          s_initialized_;

    static void SetUpTestSuite() {
        if (s_initialized_) return;

        // Allocate buffer for RuntimeState using default allocator
        auto* rs_buf = static_cast<char*>(std::malloc(kRuntimeStateBufferSize));
        ASSERT_NE(rs_buf, nullptr) << "malloc RuntimeState buffer failed";
        std::memset(rs_buf, 0, kRuntimeStateBufferSize);

        // Write RuntimeConfig at offset 0
        InitTestRuntimeConfig(rs_buf, &TestDefaultAllocate, &TestDefaultDeallocate);

        // Cast buffer to opaque RuntimeState* and set TLS
        s_runtime_state_ = reinterpret_cast<RuntimeState*>(rs_buf);
        chaos::il2cpp::runtime_core::SetCurrentRuntimeState(s_runtime_state_);

        // Allocate buffer for ThreadState
        auto* ts_buf = static_cast<char*>(std::malloc(kThreadStateBufferSize));
        ASSERT_NE(ts_buf, nullptr) << "malloc ThreadState buffer failed";
        std::memset(ts_buf, 0, kThreadStateBufferSize);

        // Write runtime_state pointer at offset 0
        InitOpaquePtrField(ts_buf, s_runtime_state_);

        s_thread_state_ = reinterpret_cast<ThreadState*>(ts_buf);
        chaos::il2cpp::runtime_core::SetCurrentThreadState(s_thread_state_);
        s_initialized_ = true;
    }

    static void TearDownTestSuite() {
        if (s_runtime_state_) {
            std::free(s_runtime_state_);
        }
        if (s_thread_state_) {
            std::free(s_thread_state_);
        }
        s_runtime_state_ = nullptr;
        s_thread_state_ = nullptr;
        s_initialized_ = false;
    }

    // Synthetic data buffers
    MarshalMethodPointerEntry           method_entries_[kMarshalMethodCount];
    MarshalInvokerEntry                 invoker_entries_[kMarshalInvokerCount];
    RuntimeTypeCapabilityEntryV0        type_cap_entries_[kMarshalTypeCapCount];
    MarshalUnresolvedVirtualCallEntry   virtual_call_entries_[kMarshalVirtualCallCount];

    // Registration structures
    CodeRegistrationV0                  code_reg_;
    MetadataRegistrationV0              meta_reg_;
    CodegenRegistrationOptionsV0        options_;

    bool          bootstrapped_  = false;

    void SetUp() override {
        // ── Init synthetic method pointer entries ──
        for (uint32_t i = 0; i < kMarshalMethodCount; i++) {
            method_entries_[i].method_token = kMarshalBaseMethodToken + i;
            method_entries_[i].method_pointer = MarshalSentinelThunk(0xA0000000u, i);
        }

        // ── Init synthetic invoker entries ──
        for (uint32_t i = 0; i < kMarshalInvokerCount; i++) {
            invoker_entries_[i].method_token = kMarshalBaseInvokerToken + i;
            invoker_entries_[i].invoker_pointer = MarshalSentinelThunk(0xB0000000u, i);
        }

        // ── Init synthetic type capability entries ──
        RuntimeTypeCapabilityInfoV0 cap_info{};
        cap_info.struct_size = sizeof(RuntimeTypeCapabilityInfoV0);
        cap_info.capability_bits = 0;
        for (uint32_t i = 0; i < kMarshalTypeCapCount; i++) {
            type_cap_entries_[i].type_token = kMarshalBaseTypeToken + i;
            type_cap_entries_[i].capability_info = cap_info;
            type_cap_entries_[i].capability_info.value_size_bytes = 4u * (i + 1u);
        }

        // ── Init synthetic unresolved virtual call entries ──
        for (uint32_t i = 0; i < kMarshalVirtualCallCount; i++) {
            virtual_call_entries_[i].instance_type_token = kMarshalBaseTypeToken + i;
            virtual_call_entries_[i].declared_method_token = kMarshalBaseMethodToken + i;
            virtual_call_entries_[i].resolved_method = MarshalSentinelThunk(0xC0000000u, i);
        }

        // ── Init registration structures ──
        std::memset(&code_reg_, 0, sizeof(code_reg_));
        code_reg_.struct_size = sizeof(CodeRegistrationV0);
        code_reg_.method_pointers = static_cast<const void*>(method_entries_);
        code_reg_.method_pointer_count = kMarshalMethodCount;
        code_reg_.invoker_pointers = static_cast<const void*>(invoker_entries_);
        code_reg_.invoker_pointer_count = kMarshalInvokerCount;
        code_reg_.type_capabilities = type_cap_entries_;
        code_reg_.type_capability_count = kMarshalTypeCapCount;
        code_reg_.unresolved_virtual_calls = virtual_call_entries_;
        code_reg_.unresolved_virtual_call_count = kMarshalVirtualCallCount;

        std::memset(&meta_reg_, 0, sizeof(meta_reg_));
        meta_reg_.struct_size = sizeof(MetadataRegistrationV0);

        std::memset(&options_, 0, sizeof(options_));
        options_.struct_size = sizeof(CodegenRegistrationOptionsV0);
        options_.image_name_utf8 = "MarshalTestModule";
    }

    void TearDown() override {
        bootstrapped_ = false;
    }

    /// Register synthetic codegen data. Returns the bridge status.
    BridgeStatus RegisterMinimal() {
        const CodegenBridgeV0* bridge = chaos_codegen_get_bridge_v0();
        if (bridge == nullptr) return CHAOS_BRIDGE_STATUS_INTERNAL_ERROR;
        return bridge->register_codegen(&code_reg_, &meta_reg_, &options_);
    }

    /// Bootstrap runtime after registration. Must call RegisterMinimal() first.
    BridgeStatus BootstrapMinimal() {
        const CodegenBridgeV0* bridge = chaos_codegen_get_bridge_v0();
        if (bridge == nullptr) return CHAOS_BRIDGE_STATUS_INTERNAL_ERROR;
        BridgeStatus status = bridge->bootstrap_runtime();
        if (status == CHAOS_BRIDGE_STATUS_OK) {
            bootstrapped_ = true;
        }
        return status;
    }

    /// Full two-phase init: RegisterMinimal() + BootstrapMinimal().
    BridgeStatus FullInit() {
        BridgeStatus status = RegisterMinimal();
        if (status != CHAOS_BRIDGE_STATUS_OK) return status;
        return BootstrapMinimal();
    }

    /// Returns true if the runtime is bootstrapped and GC heap is available.
    bool IsBootstrapped() const { return bootstrapped_; }

    /// Returns the current RuntimeState.
    RuntimeState* GetRuntimeState() const { return s_runtime_state_; }

    /// Returns the current ThreadState.
    ThreadState* GetThreadState() const { return s_thread_state_; }

    /// Returns the process-wide CodegenBridgeV0.
    const CodegenBridgeV0* GetBridge() const {
        return chaos_codegen_get_bridge_v0();
    }
};

// Static member definitions
RuntimeState* MarshalTestFixture::s_runtime_state_ = nullptr;
ThreadState*  MarshalTestFixture::s_thread_state_  = nullptr;
bool          MarshalTestFixture::s_initialized_    = false;
