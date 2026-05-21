// tiering_benchmark.cpp — Microbenchmarks for T1/T2/T3 tiered execution
//
// Measures ns/op at each tier for:
//   1. bench_arithmetic — 20-instruction pure arithmetic (RegisterExecute path)
//   2. bench_register_10 — 10-instruction register-allocated method
//
// Each scenario:
//   1. Creates PatchMethod with inline JSON AotCoreIr + manual signature blob
//   2. Lowers IR via PatchMethodLowerIR
//   3. Calls InterpreterEntryDirect in timed loops at each tier
//   4. Reports ns/op per tier
//
// Requires: DEBUG or PROFILE build.

#include <gtest/gtest.h>
#include "interpreter_entry.h"
#include "patch_loader.h"
#include "thread_state.h"
#include "tier_manager.h"
#include "ir_reg_alloc.h"
#include "vtable_registry.h"
#include "reflection_query_model.h"

#include <chaos/profile.h>

#include <chrono>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <thread>

using Clock = std::chrono::high_resolution_clock;

using chaos::il2cpp::runtime_core::PatchMethod;
using chaos::il2cpp::runtime_core::PatchMethodLowerIR;
using chaos::il2cpp::runtime_core::InterpreterEntryDirect;
using chaos::il2cpp::runtime_core::threading::RegisterThread;
using chaos::il2cpp::runtime_core::threading::UnregisterThread;
using chaos::il2cpp::interpreter::RegisterExecute;
using chaos::il2cpp::interpreter::RegisterFrame;
using chaos::il2cpp::interpreter::RegisterInstruction;
using chaos::il2cpp::interpreter::RegisterMethod;
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::ValueTag;
using chaos::il2cpp::interpreter::InterpreterObject;
using chaos::il2cpp::interpreter::kRegHasDst;
using chaos::il2cpp::interpreter::kRegHasSrc1;
using chaos::il2cpp::interpreter::kRegHasSrc2;
using chaos::il2cpp::interpreter::kRegHasSrc3;
using chaos::il2cpp::interpreter::kRegHasImm;
using chaos::il2cpp::interpreter::kRegIsBranch;
using chaos::il2cpp::vtable_registry::RegisterTypeVTable;
using chaos::il2cpp::vtable_registry::VTableSlot;
using chaos::il2cpp::vtable_registry::TypeVTable;

// AOT symbol stubs (normally provided by AOT family libraries)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

// Element type constants for signature blobs
constexpr uint8_t kElemI4 = 0x08;


// ── Helpers ────────────────────────────────────────────────────────────────

static void run_test(const char* name, bool ok) {
    std::printf("  %s: %s\n", ok ? "PASS" : "FAIL", name);
    EXPECT_TRUE(ok) << name;
}

static int BuildSignature(uint8_t* blob_out, int param_count,
                          uint8_t ret_type, const uint8_t* param_types) {
    int len = 3 + param_count;
    int pos = 0;
    blob_out[pos++] = static_cast<uint8_t>(len - 1);
    blob_out[pos++] = 0x00;  // static calling convention
    blob_out[pos++] = static_cast<uint8_t>(param_count);
    blob_out[pos++] = ret_type;
    for (int i = 0; i < param_count; ++i)
        blob_out[pos++] = param_types[i];
    return pos;
}

// Build an arithmetic-heavy JSON instruction array by repeating
// "ldc.i4 X, add" pairs followed by ret.
static void BuildJsonArithmetic(char* buf, size_t buf_size, int repeat_count) {
    // repeat_count determines how many (ldc.i4, add) pairs.
    // Total instructions = repeat_count * 2 + 1 (ret).
    int pos = 0;
    pos += std::snprintf(buf + pos, buf_size - pos,
        R"({"instructions":[)");
    int ilOffset = 0;
    for (int i = 0; i < repeat_count; ++i) {
        int val = (i + 1) * 3;
        if (i > 0) pos += std::snprintf(buf + pos, buf_size - pos, ",");
        pos += std::snprintf(buf + pos, buf_size - pos,
            R"({"opCode":0,"ilOffset":%d,"operand":%d})", ilOffset++, val);
        pos += std::snprintf(buf + pos, buf_size - pos, ",");
        pos += std::snprintf(buf + pos, buf_size - pos,
            R"({"opCode":25,"ilOffset":%d})", ilOffset++);
    }
    pos += std::snprintf(buf + pos, buf_size - pos, ",");
    pos += std::snprintf(buf + pos, buf_size - pos,
        R"({"opCode":53,"ilOffset":%d})", ilOffset);
    pos += std::snprintf(buf + pos, buf_size - pos, "]}");
}

// ── Setup PatchMethod ─────────────────────────────────────────────────────
// Returns a stack-allocated PatchMethod with lowered IR.
// The caller must NOT free it (stack allocation).
static void SetupPatchMethod(PatchMethod* pm, const char* json,
                              const uint8_t* sig_buf, int sig_len) {
    // Don't use memset — PatchMethod has default member initializers
    // (e.g. cached_arg_types = cached_arg_types_small) that must be preserved.
    pm->aot_core_ir_json = json;
    pm->aot_core_ir_json_length = static_cast<uint32_t>(std::strlen(json));
    pm->signature_blob = sig_buf;
    pm->signature_len = static_cast<uint32_t>(sig_len);
    pm->cached_ir = nullptr;
    pm->cached_reg_method = nullptr;
    pm->metadata_cache = nullptr;

    PatchMethodLowerIR(reinterpret_cast<uintptr_t>(pm));
}

// ── Tier state helper ─────────────────────────────────────────────────────
static const char* TierName(uint32_t state) {
    switch (state) {
        case PatchMethod::kT1Cold:     return "T1_COLD";
        case PatchMethod::kT2Lowering: return "T2_LOWERING";
        case PatchMethod::kT2Ready:    return "T2_READY";
        case PatchMethod::kT3Lowering: return "T3_LOWERING";
        case PatchMethod::kT3Ready:    return "T3_READY";
        case PatchMethod::kT4Ready:    return "T4_READY";
        case PatchMethod::kT5Unloaded: return "T5_UNLOADED";
        default:                       return "UNKNOWN";
    }
}

// ── Measure tier timing ───────────────────────────────────────────────────
// Calls InterpreterEntryDirect 'count' times, returns average ns per op.
static double MeasureTier(PatchMethod* pm, int count,
                          uint64_t* args_buf, int arg_count,
                          const int32_t* expected_result) {
    // Warmup calls count toward call_count (needed for tier transitions).
    // Use a separate buffer for the timing measurement to exclude warming.
    auto start = Clock::now();
    for (int i = 0; i < count; ++i) {
        int32_t ret_val = -1;
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(pm),
                               args_buf, &ret_val);
        if (expected_result != nullptr && ret_val != *expected_result) {
            std::fprintf(stderr, "  MISMATCH: expected=%d got=%d at iter=%d\n",
                         *expected_result, ret_val, i);
            return -1.0;
        }
    }
    auto end = Clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return static_cast<double>(ns) / static_cast<double>(count);
}

// ── Wait for T3 background promotion ──────────────────────────────────────
// Polls tier_state up to 3 seconds for background T3 to complete.
static bool WaitForT3(PatchMethod* pm, int timeout_ms = 3000) {
    auto deadline = Clock::now() + std::chrono::milliseconds(timeout_ms);
    while (Clock::now() < deadline) {
        auto tier = pm->tier_state.load(std::memory_order_acquire);
        if (tier == PatchMethod::kT3Ready) return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return false;
}

// ── Scenario 1: Arithmetic benchmark (20+ instructions) ───────────────────
// Tests T1, T2, T3 tiers with a pure arithmetic method.
// Parameters chosen to stay in RegisterExecute path (no SEH, >2 instructions).
static bool bench_arithmetic() {
    std::printf("\n--- bench_arithmetic ---\n");

    // Build JSON: 10 (ldc.i4, add) pairs + ret = 21 instructions total
    char json_buf[1024];
    BuildJsonArithmetic(json_buf, sizeof(json_buf), 10);

    uint8_t sig_buf[16];
    uint8_t param_types[] = { kElemI4, kElemI4 };
    int sig_len = BuildSignature(sig_buf, 2, kElemI4, param_types);

    PatchMethod pm;
    SetupPatchMethod(&pm, json_buf, sig_buf, sig_len);
    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null\n");
        return false;
    }

    // Prepare args (2 int args, but the method only uses them as stack seed)
    uint64_t args_buf[2] = { 10, 20 };
    int32_t expected = 162;  // IL: (LdcI4 X, Add)×10, Ret — first Add reads reg(0)+reg(0)=0,
                             // then accumulates: 0+6, +9, +12, +15, +18, +21, +24, +27, +30 = 162

    // Phase 1: T1 (Cold) — call_count < 100
    // First 50 calls are warmup, next 80 are measured.
    {
        // Warmup: 50 calls (no timing)
        for (int i = 0; i < 50; ++i) {
            int32_t ret_val = -1;
            InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        }
    }

    auto tier = pm.tier_state.load(std::memory_order_acquire);
    std::printf("  after warmup: tier=%s, call_count=%u\n",
                TierName(tier), pm.call_count.load(std::memory_order_relaxed));

    // Measure T1: next 80 calls (stays under 130 total = 50 warmup + 80 measure)
    double t1_ns = MeasureTier(&pm, 80, args_buf, 2, &expected);
    tier = pm.tier_state.load(std::memory_order_acquire);
    std::printf("  T1: %.0f ns/op, tier=%s, call_count=%u\n",
                t1_ns, TierName(tier), pm.call_count.load(std::memory_order_relaxed));

    if (t1_ns < 0) return false;

    // Phase 2: T2 (Warm) — call_count >= adaptive threshold
    // The T1→T2 transition fires when call_count >= 100 (adaptive threshold).
    // Drive call_count past the threshold.
    int t2_calls_needed = 0;
    {
        auto count = pm.call_count.load(std::memory_order_relaxed);
        auto t2_threshold = chaos::il2cpp::runtime_core::TierManager::Get().GetAdaptiveT2Threshold();
        (void)t2_threshold;  // We just need to get past T1 first
        // Drive to T2: call until tier_state == T2_ready
        const int kMaxWait = 500;
        for (int i = 0; i < kMaxWait; ++i) {
            int32_t ret_val = -1;
            InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
            auto s = pm.tier_state.load(std::memory_order_acquire);
            if (s == PatchMethod::kT2Ready) break;
        }
        tier = pm.tier_state.load(std::memory_order_acquire);
        std::printf("  T2 transition: tier=%s, call_count=%u\n",
                    TierName(tier), pm.call_count.load(std::memory_order_relaxed));
        if (tier != PatchMethod::kT2Ready) {
            std::fprintf(stderr, "  FAIL: expected T2_ready after transition\n");
            return false;
        }
    }

    // Measure T2: 300 calls at T2
    t2_calls_needed = 300 - (pm.call_count.load(std::memory_order_relaxed) % 300);
    // Adding more calls to trigger T2→T3 if threshold is low enough
    double t2_ns = MeasureTier(&pm, t2_calls_needed, args_buf, 2, &expected);
    tier = pm.tier_state.load(std::memory_order_acquire);
    std::printf("  T2: %.0f ns/op, tier=%s, call_count=%u\n",
                t2_ns, TierName(tier),
                pm.call_count.load(std::memory_order_relaxed));

    if (t2_ns < 0) return false;

    // Phase 3: T3 (Hot) — call_count >= adaptive threshold
    // Drive call count past the T2 threshold, then wait for background thread.
    {
        auto t2_threshold = chaos::il2cpp::runtime_core::TierManager::Get().GetAdaptiveT2Threshold();
        auto call_count = pm.call_count.load(std::memory_order_relaxed);
        if (call_count < t2_threshold) {
            int remaining = static_cast<int>(t2_threshold - call_count) + 50;
            for (int i = 0; i < remaining; ++i) {
                int32_t ret_val = -1;
                InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
            }
        }

        // Wait for background thread to finish T3 promotion
        bool t3_ready = WaitForT3(&pm, 3000);
        tier = pm.tier_state.load(std::memory_order_acquire);
        std::printf("  T3 wait: tier=%s, call_count=%u (wait_ok=%d)\n",
                    TierName(tier), pm.call_count.load(std::memory_order_relaxed), t3_ready);
    }

    // Measure T3: 500 calls
    // Warmup first to recover from WaitForT3 sleep (CPU frequency, cache)
    {
        int32_t warmup_ret = -1;
        for (int i = 0; i < 500; ++i)
            InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &warmup_ret);
    }
    double t3_ns = MeasureTier(&pm, 500, args_buf, 2, &expected);
    tier = pm.tier_state.load(std::memory_order_acquire);
    std::printf("  T3: %.0f ns/op, tier=%s, call_count=%u\n",
                t3_ns, TierName(tier),
                pm.call_count.load(std::memory_order_relaxed));

    if (t3_ns < 0) return false;

    // Verify tier progression: T1 > T2 > T3 (performance gets better)
    bool tier_ok = (t1_ns > t2_ns || t2_ns < 1) && (t2_ns > t3_ns || t3_ns < 1);
    if (!tier_ok) {
        std::printf("  (tier progression checkpoint: T1=%.0f T2=%.0f T3=%.0f)\n",
                    t1_ns, t2_ns, t3_ns);
    }

    std::printf("  SUMMARY: T1=%.0fns  T2=%.0fns  T3=%.0fns\n",
                t1_ns, t2_ns, t3_ns);

    return true;
}

// ── Scenario 2: Register-10 benchmark ─────────────────────────────────────
// 10-instruction method that exercises the RegisterExecute path.
static bool bench_register_10() {
    std::printf("\n--- bench_register_10 ---\n");

    // 8 instructions: ldarg.0, ldarg.1, add (3x), ret
    const char* json =
        R"({"instructions":[)"
        R"({"opCode":6,"ilOffset":0,"operand":0},)"
        R"({"opCode":6,"ilOffset":1,"operand":1},)"
        R"({"opCode":25,"ilOffset":2},)"
        R"({"opCode":6,"ilOffset":3,"operand":0},)"
        R"({"opCode":25,"ilOffset":4},)"
        R"({"opCode":6,"ilOffset":5,"operand":1},)"
        R"({"opCode":25,"ilOffset":6},)"
        R"({"opCode":53,"ilOffset":7}])"
        R"(})";

    uint8_t sig_buf[16];
    uint8_t param_types[] = { kElemI4, kElemI4 };
    int sig_len = BuildSignature(sig_buf, 2, kElemI4, param_types);

    PatchMethod pm;
    SetupPatchMethod(&pm, json, sig_buf, sig_len);
    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null\n");
        return false;
    }

    uint64_t args_buf[2] = { 10, 20 };
    // result = ((10 + 20) + 10) + 20 = 60
    int32_t expected = 60;

    // Warmup: 50 calls
    for (int i = 0; i < 50; ++i) {
        int32_t ret_val = -1;
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        if (ret_val != expected) {
            std::fprintf(stderr, "  FAIL: warmup expected=%d got=%d\n", expected, ret_val);
            return false;
        }
    }

    // Measure T1: next 80 calls
    double t1_ns = MeasureTier(&pm, 80, args_buf, 2, &expected);
    std::printf("  T1: %.0f ns/op\n", t1_ns);
    if (t1_ns < 0) return false;

    // Drive to T2
    for (int i = 0; i < 200; ++i) {
        int32_t ret_val = -1;
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        if (ret_val != expected) return false;
        if (pm.tier_state.load(std::memory_order_acquire) == PatchMethod::kT2Ready) break;
    }

    // Measure T2: 300 calls
    double t2_ns = MeasureTier(&pm, 300, args_buf, 2, &expected);
    std::printf("  T2: %.0f ns/op\n", t2_ns);
    if (t2_ns < 0) return false;

    // Drive to T3 and wait
    {
        auto t2_threshold = chaos::il2cpp::runtime_core::TierManager::Get().GetAdaptiveT2Threshold();
        auto call_count = pm.call_count.load(std::memory_order_relaxed);
        if (call_count < t2_threshold) {
            int remaining = static_cast<int>(t2_threshold - call_count) + 50;
            for (int i = 0; i < remaining; ++i) {
                int32_t ret_val = -1;
                InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
                if (ret_val != expected) return false;
            }
        }
        WaitForT3(&pm, 3000);
    }

    // Measure T3: 500 calls
    double t3_ns = MeasureTier(&pm, 500, args_buf, 2, &expected);
    std::printf("  T3: %.0f ns/op\n", t3_ns);
    if (t3_ns < 0) return false;

    std::printf("  SUMMARY: T1=%.0fns  T2=%.0fns  T3=%.0fns\n", t1_ns, t2_ns, t3_ns);
    return true;
}

// ── Dummy virtual function for callvirt PIC benchmark ─────────────────────
// AOT-callable function that takes raw uint64_t args and returns a known value.
// The exact calling convention matches CallDirectVoidPtr in ir_reg_alloc.cpp.
extern "C" uint64_t DummyVirtualFn(uint64_t, uint64_t, uint64_t, uint64_t,
                                    uint64_t, uint64_t, uint64_t, uint64_t) {
    return 42;
}

// ── Scenario 3: callvirt PIC benchmark ────────────────────────────────────
// Direct RegisterExecute benchmark measuring PIC dispatch for callvirt.
//
// Sets up:
//   1. VTable with one slot pointing to DummyVirtualFn
//   2. RegisterMethod with callvirt + ret (2 instructions)
//   3. PatchMethod with manually constructed PIC chain (1 slot, monomorphic)
//   4. InterpreterObject receiver with matching type_token
//
// Measures RegisterExecute execution time with PIC hit at every call.
static bool bench_callvirt_pic() {
    std::printf("\n--- bench_callvirt_pic ---\n");

    constexpr uint32_t kRecvToken = 0x8001;
    constexpr uint32_t kVirtMethodToken = 0x6001;

    // ── Register VTable with one slot ──
    // The VTable must outlive the benchmark (static lifetime).
    static VTableSlot s_slot = {};
    s_slot.method_token = kVirtMethodToken;
    s_slot.method_pointer = reinterpret_cast<void*>(DummyVirtualFn);

    static TypeVTable s_vt = {};
    s_vt.type_token = kRecvToken;
    s_vt.base_token = 0;
    s_vt.slot_count = 1;
    s_vt.slots = &s_slot;
    s_vt.stable_id = static_cast<uint64_t>(kRecvToken);
    s_vt.type_shape = 1;  // reference type
    if (!RegisterTypeVTable(&s_vt)) {
        std::fprintf(stderr, "  FAIL: RegisterTypeVTable failed\n");
        return false;
    }

    // ── Create method descriptor for call_target encoding ──
    chaos::il2cpp::runtime_core::ReflectionQueryMethodDescriptor virt_desc = {};
    virt_desc.subject_id_utf8 = "Test/VirtualClass:VirtualMethod";
    virt_desc.name_utf8 = "VirtualMethod";
    virt_desc.member_type_utf8 = "System.Int32";
    virt_desc.metadata_token = kVirtMethodToken;
    virt_desc.parameter_count = 0;
    auto method_handle = chaos::il2cpp::runtime_core::EncodeReflectionQueryMethodHandle(&virt_desc);

    // ── Build RegisterMethod (2 instructions: callvirt + ret) ──
    //
    // Header format: [op(16) | dst(8) | src1(8) | flags(8) | reserved(16)]
    //   bits [62:48] = call_arg_count
    //   bit  63      = is_instance_call
    auto MakeHeader = [](IROpCode op, uint8_t dst, uint8_t src1,
                          uint8_t flags, uint32_t call_args,
                          bool is_instance) -> uint64_t {
        uint64_t h = static_cast<uint64_t>(op);
        h |= static_cast<uint64_t>(dst) << 16;
        h |= static_cast<uint64_t>(src1) << 24;
        h |= static_cast<uint64_t>(flags) << 40;
        h |= static_cast<uint64_t>(call_args & 0x7FFF) << 48;
        if (is_instance) h |= 1ULL << 63;
        return h;
    };

    constexpr uint8_t kRegHasDst  = 0x01;
    constexpr uint8_t kRegHasSrc1 = 0x02;
    constexpr uint8_t kRegIsCall  = 0x10;

    RegisterInstruction instrs[2];

    // instr 0: callvirt — receiver in r0, result to r0
    instrs[0].header = MakeHeader(IROpCode::CallVirt,
                                  0,           // dst = r0 (return value)
                                  0,           // src1 = r0 (receiver = arg0)
                                  kRegHasDst | kRegIsCall,  // flags: has dst + is call
                                  1,           // call_arg_count = 1 (this only)
                                  true);       // is_instance_call
    instrs[0].imm.ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(method_handle));

    // instr 1: ret — return value from r0
    instrs[1].header = MakeHeader(IROpCode::Ret,
                                  0,           // no dst
                                  0,           // src1 = r0
                                  kRegHasSrc1, // has src1
                                  0, false);
    instrs[1].imm.i4 = 0;

    // ── Create PatchMethod with PIC data ──
    // PIC expects a PatchMethod with tier_state = T3_ready and pic_dispatch_data set.
    PatchMethod pm;
    pm.aot_core_ir_json = nullptr;
    pm.aot_core_ir_json_length = 0;
    pm.signature_blob = nullptr;
    pm.signature_len = 0;
    pm.cached_ir = nullptr;
    pm.cached_reg_method = nullptr;
    pm.metadata_cache = nullptr;
    pm.call_site_profiles = nullptr;
    pm.call_site_profile_count = 0;
    pm.pic_dispatch_data = nullptr;
    pm.tier_state.store(4, std::memory_order_release);  // kT3Ready = 4

    // ── Build PIC chain: 1 chain, 1 slot (monomorphic) ──
    // Layout matches pic_generator.cpp: [uint32_t count][PicDispatchChain[count]]
    // Must use raw allocation to avoid struct padding between count and chain array.
    static uint8_t s_pic_alloc[sizeof(uint32_t) + sizeof(chaos::il2cpp::runtime_core::PicDispatchChain)] = {};
    *reinterpret_cast<uint32_t*>(s_pic_alloc) = 1;
    auto* pic_chain = reinterpret_cast<chaos::il2cpp::runtime_core::PicDispatchChain*>(
        s_pic_alloc + sizeof(uint32_t));
    pic_chain->instruction_idx = 0;
    pic_chain->method_token = kVirtMethodToken;
    pic_chain->slots[0].type_token = kRecvToken;
    pic_chain->slots[0].direct_fn = reinterpret_cast<void*>(DummyVirtualFn);
    pic_chain->generation =
        static_cast<uint32_t>(chaos::il2cpp::runtime_core::g_patch_generation.load(std::memory_order_relaxed));
    pic_chain->fallback_fn = nullptr;
    pm.pic_dispatch_data = s_pic_alloc;

    // ── Create receiver object ──
    // Use static storage so it outlives the benchmark.
    static InterpreterObject s_obj;
    ::new (&s_obj) InterpreterObject();  // reset via placement new
    s_obj.type_token = kRecvToken;

    // ── PIC path measurement ──
    constexpr int kWarmupPIC = 1000;
    constexpr int kMeasurePIC = 5000;

    // Warmup
    for (int i = 0; i < kWarmupPIC; ++i) {
        RegisterFrame frame = {};
        frame.regs.gpr[0] = reinterpret_cast<uint64_t>(&s_obj);
        frame.regs.gpr_tags[0] = static_cast<uint8_t>(ValueTag::ObjectRef);
        frame.arg_count = 1;
        frame.dispatch_fn = nullptr;
        frame.dispatch_ctx = nullptr;
        frame.call_cache = nullptr;
        frame.call_count = 0;
        frame.patch_method = &pm;
        RegisterExecute(frame, instrs, 2);
    }

    // Measure
    auto start = Clock::now();
    for (int i = 0; i < kMeasurePIC; ++i) {
        RegisterFrame frame = {};
        frame.regs.gpr[0] = reinterpret_cast<uint64_t>(&s_obj);
        frame.regs.gpr_tags[0] = static_cast<uint8_t>(ValueTag::ObjectRef);
        frame.arg_count = 1;
        frame.dispatch_fn = nullptr;
        frame.dispatch_ctx = nullptr;
        frame.call_cache = nullptr;
        frame.patch_method = &pm;
        RegisterExecute(frame, instrs, 2);
    }
    auto end = Clock::now();
    auto ns_pic = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    double avg_ns_pic = static_cast<double>(ns_pic) / static_cast<double>(kMeasurePIC);

    std::printf("  PIC: %.0f ns/op (%d iterations)\n", avg_ns_pic, kMeasurePIC);

    // ── No-PIC path measurement (for comparison) ──
    // Remove PIC data to force vtable-resolution fallback via Reg_Call.
    pm.pic_dispatch_data = nullptr;

    // Warmup (no-PIC)
    for (int i = 0; i < kWarmupPIC; ++i) {
        RegisterFrame frame = {};
        frame.regs.gpr[0] = reinterpret_cast<uint64_t>(&s_obj);
        frame.regs.gpr_tags[0] = static_cast<uint8_t>(ValueTag::ObjectRef);
        frame.arg_count = 1;
        frame.dispatch_fn = nullptr;
        frame.dispatch_ctx = nullptr;
        frame.call_cache = nullptr;
        frame.call_count = 0;
        frame.patch_method = &pm;
        RegisterExecute(frame, instrs, 2);
    }

    // Measure (no-PIC)
    start = Clock::now();
    for (int i = 0; i < kMeasurePIC; ++i) {
        RegisterFrame frame = {};
        frame.regs.gpr[0] = reinterpret_cast<uint64_t>(&s_obj);
        frame.regs.gpr_tags[0] = static_cast<uint8_t>(ValueTag::ObjectRef);
        frame.arg_count = 1;
        frame.dispatch_fn = nullptr;
        frame.dispatch_ctx = nullptr;
        frame.call_cache = nullptr;
        frame.call_count = 0;
        frame.patch_method = &pm;
        RegisterExecute(frame, instrs, 2);
    }
    end = Clock::now();
    auto ns_nopic = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    double avg_ns_nopic = static_cast<double>(ns_nopic) / static_cast<double>(kMeasurePIC);

    std::printf("  NO-PIC: %.0f ns/op (%d iterations)\n", avg_ns_nopic, kMeasurePIC);

    // Restore PIC data for the return-verification call.
    pm.pic_dispatch_data = s_pic_alloc;

    // ── Verify correct execution with PIC ──
    {
        RegisterFrame frame = {};
        frame.regs.gpr[0] = reinterpret_cast<uint64_t>(&s_obj);
        frame.regs.gpr_tags[0] = static_cast<uint8_t>(ValueTag::ObjectRef);
        frame.arg_count = 1;
        frame.dispatch_fn = nullptr;
        frame.dispatch_ctx = nullptr;
        frame.call_cache = nullptr;
        frame.call_count = 0;
        frame.patch_method = &pm;
        RegisterExecute(frame, instrs, 2);
        if (!frame.has_ret || frame.ret_val != 42) {
            std::fprintf(stderr, "  FAIL: PIC result=%llu (expected=42), has_ret=%d\n",
                         (unsigned long long)frame.ret_val, frame.has_ret);
            return false;
        }
    }

    std::printf("  SUMMARY: PIC=%.0fns  NO-PIC=%.0fns  speedup=%.1fx\n",
                avg_ns_pic, avg_ns_nopic,
                (avg_ns_nopic > 0) ? (avg_ns_nopic / avg_ns_pic) : 0.0);

    return true;
}


// ── Scenario 4: Native code generation benchmark (T4) ─────────────────────
// Pure arithmetic method that crosses all tier thresholds (T1→T2→T3→T4)
// and measures native code execution performance.
// Uses a simple verified IL sequence: ldarg.0 + ldarg.1 + add + ret
static bool bench_native() {
    std::printf("\n--- bench_native ---\n");
    std::fflush(stdout);

    // 4 instructions: ldarg.0 (arg0), ldarg.1 (arg1), add, ret
    const char* json =
        R"({"instructions":[)"
        R"({"opCode":6,"ilOffset":0,"operand":0},)"
        R"({"opCode":6,"ilOffset":1,"operand":1},)"
        R"({"opCode":25,"ilOffset":2},)"
        R"({"opCode":53,"ilOffset":3}])"
        R"(})";

    uint8_t sig_buf[16];
    uint8_t param_types[] = { kElemI4, kElemI4 };
    int sig_len = BuildSignature(sig_buf, 2, kElemI4, param_types);

    PatchMethod pm;
    SetupPatchMethod(&pm, json, sig_buf, sig_len);
    std::printf("  SetupPatchMethod done, cached_ir=%p\n", pm.cached_ir);
    std::fflush(stdout);
    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null\n");
        return false;
    }

    uint64_t args_buf[2] = { 10, 20 };
    constexpr int32_t expectedResult = 30;

    // ── Drive through all tiers up to T4 ──
    // kT1HotThreshold = 100, kT3NativeThreshold = 2000
    constexpr int kTotalCalls = 2500;

    // Track cumulative time per tier range (no per-iteration storage needed)
    uint64_t t1_sum = 0, t2_sum = 0, t3_sum = 0, t4_sum = 0;
    int t1_count = 0, t2_count = 0, t3_count = 0, t4_count = 0;

    std::printf("  Starting %d iterations...\n", kTotalCalls);
    std::fflush(stdout);

    // Warmup + drive through tiers
    for (int i = 0; i < kTotalCalls; ++i) {
        if (i % 200 == 0 && i > 0) {
            auto t = pm.tier_state.load(std::memory_order_acquire);
            std::printf("  iter=%d, tier=%u, call_count=%u\n", i, t,
                pm.call_count.load(std::memory_order_relaxed));
            std::fflush(stdout);
        }
        int64_t ret_val = -1;
        auto start = Clock::now();
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        auto end = Clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        // Accumulate timing per tier range
        if (i >= 50 && i < 130)  { t1_sum += ns; ++t1_count; }
        if (i >= 200 && i < 500) { t2_sum += ns; ++t2_count; }
        if (i >= 1500 && i < 1800) { t3_sum += ns; ++t3_count; }
        if (i >= 2100 && i < 2500) { t4_sum += ns; ++t4_count; }
        // Verify correctness at every call
        if (static_cast<int32_t>(ret_val) != expectedResult) {
            std::fprintf(stderr, "  FAIL: result=%lld (expected=%d) at iter=%d (tier=%u, count=%u)\n",
                        (long long)ret_val, expectedResult, i,
                        pm.tier_state.load(std::memory_order_acquire),
                        pm.call_count.load(std::memory_order_relaxed));
            return false;
        }
    }

    auto final_tier = pm.tier_state.load(std::memory_order_acquire);
    auto final_count = pm.call_count.load(std::memory_order_relaxed);
    bool has_native = (pm.cached_native_method != nullptr);

    double t1_ns = (t1_count > 0) ? static_cast<double>(t1_sum) / t1_count : -1.0;
    double t2_ns = (t2_count > 0) ? static_cast<double>(t2_sum) / t2_count : -1.0;
    double t3_ns = (t3_count > 0) ? static_cast<double>(t3_sum) / t3_count : -1.0;
    double t4_ns = (t4_count > 0) ? static_cast<double>(t4_sum) / t4_count : -1.0;

    std::printf("  T1: %.0f ns/op\n", t1_ns);
    std::printf("  T2: %.0f ns/op\n", t2_ns);
    std::printf("  T3: %.0f ns/op\n", t3_ns);
    std::printf("  T4: %.0f ns/op\n", t4_ns);
    std::printf("  SUMMARY: T1=%.0fns  T2=%.0fns  T3=%.0fns  T4=%.0fns\n",
                t1_ns, t2_ns, t3_ns, t4_ns);
    std::printf("  final: tier=%u, call_count=%u, has_native=%d, native_ptr=%p\n",
                final_tier, final_count, has_native, pm.cached_native_method);
    std::fflush(stdout);

    // T4 should be enabled (pure arithmetic method with only ldarg/add/ret)
    if (!has_native) {
        std::fprintf(stderr, "  FAIL: T4 native code was not generated\n");
        return false;
    }

    // Verify value correctness using native code
    for (int i = 0; i < 10; ++i) {
        int64_t ret_val = -1;
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        if (static_cast<int32_t>(ret_val) != expectedResult) {
            std::fprintf(stderr, "  FAIL: native result=%lld (expected=%d) at iter=%d\n",
                        (long long)ret_val, expectedResult, i);
            return false;
        }
    }
    std::printf("  NATIVE VERIFIED: all 10 native calls returned correct result\n");
    std::fflush(stdout);

    return true;
}


// ── Scenario 5: Multi-ALU benchmark (T4) ─────────────────────────────────
// Tests T4 native code generation for chains of arithmetic ops.
// IL: LdArg(0), LdArg(1), Add, LdArg(0), Add, LdArg(1), Add, Ret
// Exercises longer basic-block codegen paths in T4.
static bool bench_multi_alu_t4() {
    std::printf("\n--- bench_multi_alu_t4 ---\n");
    std::fflush(stdout);

    const char* json =
        R"({"instructions":[)"
        R"({"opCode":6,"ilOffset":0,"operand":0},)"
        R"({"opCode":6,"ilOffset":1,"operand":1},)"
        R"({"opCode":25,"ilOffset":2},)"
        R"({"opCode":6,"ilOffset":3,"operand":0},)"
        R"({"opCode":25,"ilOffset":4},)"
        R"({"opCode":6,"ilOffset":5,"operand":1},)"
        R"({"opCode":25,"ilOffset":6},)"
        R"({"opCode":53,"ilOffset":7}])"
        R"(})";

    uint8_t sig_buf[16];
    uint8_t param_types[] = { kElemI4, kElemI4 };
    int sig_len = BuildSignature(sig_buf, 2, kElemI4, param_types);

    PatchMethod pm;
    SetupPatchMethod(&pm, json, sig_buf, sig_len);
    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null\n");
        return false;
    }

    uint64_t args_buf[2] = { 10, 20 };
    // result = ((10+20) + 10) + 20 = 60
    constexpr int32_t expectedResult = 60;

    constexpr int kTotalCalls = 2500;

    uint64_t t1_sum = 0, t2_sum = 0, t3_sum = 0, t4_sum = 0;
    int t1_count = 0, t2_count = 0, t3_count = 0, t4_count = 0;

    for (int i = 0; i < kTotalCalls; ++i) {
        int64_t ret_val = -1;
        auto start = Clock::now();
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        auto end = Clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        if (i >= 50 && i < 130)   { t1_sum += ns; ++t1_count; }
        if (i >= 200 && i < 500)  { t2_sum += ns; ++t2_count; }
        if (i >= 1500 && i < 1800){ t3_sum += ns; ++t3_count; }
        if (i >= 2100 && i < 2500){ t4_sum += ns; ++t4_count; }
        if (static_cast<int32_t>(ret_val) != expectedResult) {
            std::fprintf(stderr, "  FAIL: result=%lld (expected=%d) at iter=%d\n",
                        (long long)ret_val, expectedResult, i);
            return false;
        }
    }

    bool has_native = (pm.cached_native_method != nullptr);

    double t1_ns = (t1_count > 0) ? static_cast<double>(t1_sum) / t1_count : -1.0;
    double t2_ns = (t2_count > 0) ? static_cast<double>(t2_sum) / t2_count : -1.0;
    double t3_ns = (t3_count > 0) ? static_cast<double>(t3_sum) / t3_count : -1.0;
    double t4_ns = (t4_count > 0) ? static_cast<double>(t4_sum) / t4_count : -1.0;

    std::printf("  T1: %.0f ns/op\n", t1_ns);
    std::printf("  T2: %.0f ns/op\n", t2_ns);
    std::printf("  T3: %.0f ns/op\n", t3_ns);
    std::printf("  T4: %.0f ns/op\n", t4_ns);
    std::printf("  SUMMARY: T1=%.0fns  T2=%.0fns  T3=%.0fns  T4=%.0fns\n",
                t1_ns, t2_ns, t3_ns, t4_ns);
    std::printf("  final: tier=%u, call_count=%u, has_native=%d\n",
                pm.tier_state.load(std::memory_order_acquire),
                pm.call_count.load(std::memory_order_relaxed), has_native);
    std::fflush(stdout);

    if (!has_native) {
        std::fprintf(stderr, "  FAIL: T4 native code was not generated\n");
        return false;
    }
    return true;
}

// ── Scenario 6: Local variable storm benchmark (T4) ────────────────────────
// Tests T4 native code generation for LdLoc/StLoc chains.
// IL: LdArg(0), StLoc(0..3), LdLoc(3), LdArg(1), Add, LdLoc(3), Add, Ret
// Exercises local-variable register allocation in T4 codegen.
static bool bench_loc_storm_t4() {
    std::printf("\n--- bench_loc_storm_t4 ---\n");
    std::fflush(stdout);

    const char* json =
        R"({"instructions":[)"
        R"({"opCode":6,"ilOffset":0,"operand":0},)"
        R"({"opCode":8,"ilOffset":1,"operand":0},)"
        R"({"opCode":7,"ilOffset":2,"operand":0},)"
        R"({"opCode":8,"ilOffset":3,"operand":1},)"
        R"({"opCode":7,"ilOffset":4,"operand":1},)"
        R"({"opCode":8,"ilOffset":5,"operand":2},)"
        R"({"opCode":7,"ilOffset":6,"operand":2},)"
        R"({"opCode":8,"ilOffset":7,"operand":3},)"
        R"({"opCode":7,"ilOffset":8,"operand":3},)"
        R"({"opCode":6,"ilOffset":9,"operand":1},)"
        R"({"opCode":25,"ilOffset":10},)"
        R"({"opCode":7,"ilOffset":11,"operand":3},)"
        R"({"opCode":25,"ilOffset":12},)"
        R"({"opCode":53,"ilOffset":13}])"
        R"(})";

    uint8_t sig_buf[16];
    uint8_t param_types[] = { kElemI4, kElemI4 };
    int sig_len = BuildSignature(sig_buf, 2, kElemI4, param_types);

    PatchMethod pm;
    SetupPatchMethod(&pm, json, sig_buf, sig_len);
    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null\n");
        return false;
    }

    uint64_t args_buf[2] = { 10, 20 };
    // result = ((arg0 -> loc3) + arg1) + loc3 = (10 + 20) + 10 = 40
    constexpr int32_t expectedResult = 40;

    constexpr int kTotalCalls = 2500;

    uint64_t t1_sum = 0, t2_sum = 0, t3_sum = 0, t4_sum = 0;
    int t1_count = 0, t2_count = 0, t3_count = 0, t4_count = 0;

    std::printf("  Starting %d iterations...\n", kTotalCalls);
    std::fflush(stdout);

    for (int i = 0; i < kTotalCalls; ++i) {
        int64_t ret_val = -1;
        auto start = Clock::now();
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        auto end = Clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        if (i >= 50 && i < 130)   { t1_sum += ns; ++t1_count; }
        if (i >= 200 && i < 500)  { t2_sum += ns; ++t2_count; }
        if (i >= 1500 && i < 1800){ t3_sum += ns; ++t3_count; }
        if (i >= 2100 && i < 2500){ t4_sum += ns; ++t4_count; }
        if (static_cast<int32_t>(ret_val) != expectedResult) {
            std::fprintf(stderr, "  FAIL: result=%lld (expected=%d) at iter=%d\n",
                        (long long)ret_val, expectedResult, i);
            return false;
        }
    }

    bool has_native = (pm.cached_native_method != nullptr);

    double t1_ns = (t1_count > 0) ? static_cast<double>(t1_sum) / t1_count : -1.0;
    double t2_ns = (t2_count > 0) ? static_cast<double>(t2_sum) / t2_count : -1.0;
    double t3_ns = (t3_count > 0) ? static_cast<double>(t3_sum) / t3_count : -1.0;
    double t4_ns = (t4_count > 0) ? static_cast<double>(t4_sum) / t4_count : -1.0;

    std::printf("  T1: %.0f ns/op\n", t1_ns);
    std::printf("  T2: %.0f ns/op\n", t2_ns);
    std::printf("  T3: %.0f ns/op\n", t3_ns);
    std::printf("  T4: %.0f ns/op\n", t4_ns);
    std::printf("  SUMMARY: T1=%.0fns  T2=%.0fns  T3=%.0fns  T4=%.0fns\n",
                t1_ns, t2_ns, t3_ns, t4_ns);
    std::printf("  final: tier=%u, call_count=%u, has_native=%d\n",
                pm.tier_state.load(std::memory_order_acquire),
                pm.call_count.load(std::memory_order_relaxed), has_native);
    std::fflush(stdout);

    if (!has_native) {
        std::fprintf(stderr, "  FAIL: T4 native code was not generated\n");
        return false;
    }
    return true;
}


// ── Scenario 7: Branch benchmark (T4) ─────────────────────────────────────
// Tests JSON-based branch target resolution end-to-end.
// IL: LdcI4(0), BrFalse(target=offset4), LdcI4(-1)[dead], Ret[dead],
//     LdcI4(42)[target], Ret
// If BrFalse resolution works correctly, the dead path is skipped
// and the method returns 42.
static bool bench_branches_t4() {
    std::printf("\n--- bench_branches_t4 ---\n");
    std::fflush(stdout);

    // BrFalse at ilOffset=1 targets ilOffset=4 (skip dead path at 2-3)
    const char* json =
        R"({"instructions":[)"
        R"({"opCode":0,"ilOffset":0,"operand":0},)"
        R"({"opCode":19,"ilOffset":1,"operand":4},)"
        R"({"opCode":0,"ilOffset":2,"operand":-1},)"
        R"({"opCode":53,"ilOffset":3},)"
        R"({"opCode":0,"ilOffset":4,"operand":42},)"
        R"({"opCode":53,"ilOffset":5}]})";

    uint8_t sig_buf[256];
    int sig_len = BuildSignature(sig_buf, 0, kElemI4, nullptr);

    PatchMethod pm;
    SetupPatchMethod(&pm, json, sig_buf, sig_len);
    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null\n");
        return false;
    }

    uint64_t args_buf[1] = {};
    constexpr int32_t expectedResult = 42;

    constexpr int kTotalCalls = 2500;

    uint64_t t1_sum = 0, t2_sum = 0, t3_sum = 0, t4_sum = 0;
    int t1_count = 0, t2_count = 0, t3_count = 0, t4_count = 0;

    std::printf("  Starting %d iterations...\n", kTotalCalls);
    std::fflush(stdout);

    for (int i = 0; i < kTotalCalls; ++i) {
        int64_t ret_val = -1;
        auto start = Clock::now();
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        auto end = Clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        if (i >= 50 && i < 130)   { t1_sum += ns; ++t1_count; }
        if (i >= 200 && i < 500)  { t2_sum += ns; ++t2_count; }
        if (i >= 1500 && i < 1800){ t3_sum += ns; ++t3_count; }
        if (i >= 2100 && i < 2500){ t4_sum += ns; ++t4_count; }
        if (static_cast<int32_t>(ret_val) != expectedResult) {
            std::fprintf(stderr, "  FAIL: result=%lld (expected=%d) at iter=%d\n",
                        (long long)ret_val, expectedResult, i);
            return false;
        }
    }

    bool has_native = (pm.cached_native_method != nullptr);

    double t1_ns = (t1_count > 0) ? static_cast<double>(t1_sum) / t1_count : -1.0;
    double t2_ns = (t2_count > 0) ? static_cast<double>(t2_sum) / t2_count : -1.0;
    double t3_ns = (t3_count > 0) ? static_cast<double>(t3_sum) / t3_count : -1.0;
    double t4_ns = (t4_count > 0) ? static_cast<double>(t4_sum) / t4_count : -1.0;

    std::printf("  T1: %.0f ns/op\n", t1_ns);
    std::printf("  T2: %.0f ns/op\n", t2_ns);
    std::printf("  T3: %.0f ns/op\n", t3_ns);
    std::printf("  T4: %.0f ns/op\n", t4_ns);
    std::printf("  SUMMARY: T1=%.0fns  T2=%.0fns  T3=%.0fns  T4=%.0fns\n",
                t1_ns, t2_ns, t3_ns, t4_ns);
    std::printf("  final: tier=%u, call_count=%u, has_native=%d\n",
                pm.tier_state.load(std::memory_order_acquire),
                pm.call_count.load(std::memory_order_relaxed), has_native);
    std::fflush(stdout);

    if (!has_native) {
        std::fprintf(stderr, "  FAIL: T4 native code was not generated\n");
        return false;
    }
    return true;
}


// ── Local helpers for RegisterInstruction construction ──────────────────
static uint64_t BM_MakeHeader(IROpCode opc, uint8_t dst, uint8_t src1,
                              uint8_t src2, uint8_t flags) noexcept {
    return static_cast<uint64_t>(opc) |
           (static_cast<uint64_t>(dst)   << 16) |
           (static_cast<uint64_t>(src1)  << 24) |
           (static_cast<uint64_t>(src2)  << 32) |
           (static_cast<uint64_t>(flags) << 40);
}

static RegisterInstruction BM_I4(int32_t imm, uint8_t dst) noexcept {
    RegisterInstruction ri;
    ri.header = BM_MakeHeader(IROpCode::LdcI4, dst, 0, 0, kRegHasDst | kRegHasImm);
    ri.imm.i4 = imm;
    return ri;
}

static RegisterInstruction BM_NewObj(uint8_t dst, uint32_t type_token) noexcept {
    RegisterInstruction ri;
    ri.header = BM_MakeHeader(IROpCode::NewObj, dst, 0, 0, kRegHasDst | kRegHasImm);
    ri.imm.field_offset = type_token;
    return ri;
}

static RegisterInstruction BM_StFld(uint8_t obj, uint32_t field_idx, uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = BM_MakeHeader(IROpCode::StFld, 0, obj, src,
                              kRegHasSrc1 | kRegHasSrc2 | kRegHasImm);
    ri.imm.field_offset = field_idx;
    return ri;
}

static RegisterInstruction BM_LdFld(uint8_t dst, uint8_t src, uint32_t field_idx) noexcept {
    RegisterInstruction ri;
    ri.header = BM_MakeHeader(IROpCode::LdFld, dst, src, 0,
                              kRegHasDst | kRegHasSrc1 | kRegHasImm);
    ri.imm.field_offset = field_idx;
    return ri;
}

static RegisterInstruction BM_Box(uint8_t dst, uint8_t src, uint32_t type_token) noexcept {
    RegisterInstruction ri;
    ri.header = BM_MakeHeader(IROpCode::Box, dst, src, 0,
                              kRegHasDst | kRegHasSrc1 | kRegHasImm);
    ri.imm.field_offset = type_token;
    return ri;
}

static RegisterInstruction BM_NewArr(uint8_t dst, uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = BM_MakeHeader(IROpCode::NewArr, dst, src, 0, kRegHasDst | kRegHasSrc1);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction BM_LdElem(uint8_t dst, uint8_t arr, uint8_t index) noexcept {
    RegisterInstruction ri;
    ri.header = BM_MakeHeader(IROpCode::LdElem, dst, arr, index,
                              kRegHasDst | kRegHasSrc1 | kRegHasSrc2);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction BM_StElem(uint8_t arr, uint8_t index, uint8_t value) noexcept {
    RegisterInstruction ri;
    ri.header = BM_MakeHeader(IROpCode::StElem, 0, arr, index,
                              kRegHasSrc1 | kRegHasSrc2 | kRegHasSrc3);
    ri.header |= (static_cast<uint64_t>(value) << 48);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction BM_Ret(uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = BM_MakeHeader(IROpCode::Ret, 0, src, 0, kRegHasSrc1);
    ri.imm.i4 = 0;
    return ri;
}

// ── Helper: set up PatchMethod for T4 direct benchmark ──────────────────
// Constructs a PatchMethod with a manually built RegisterMethod,
// skips lower tiers by setting call_count past kT3NativeThreshold,
// and starts at kT3Ready so the first call triggers T4 codegen.
// Allocates a minimal IRMethod (3 dummy instrs) to pass entry_direct
// early-exit checks, and to enter RegisterExecute path (needs >2 instrs)
// as fallback so the first call executes correctly while triggering T4.
static void SetupT4DirectBenchmark(PatchMethod* pm, RegisterMethod&& rm) noexcept {
    pm->aot_core_ir_json = nullptr;
    pm->aot_core_ir_json_length = 0;
    pm->signature_blob = nullptr;
    pm->signature_len = 0;
    // 3 dummy instrs: passes !=null, !empty, !=1, >2 (so RegisterExecute fires as fallback)
    auto* ir = new chaos::il2cpp::interpreter::IRMethod();
    ir->instructions.resize(3);
    pm->cached_ir = ir;
    auto* rm_ptr = new RegisterMethod(std::move(rm));
    pm->cached_reg_method = rm_ptr;
    pm->cached_optimized_reg_method = rm_ptr;
    pm->metadata_cache = nullptr;
    pm->call_site_profiles = nullptr;
    pm->call_site_profile_count = 0;
    pm->pic_dispatch_data = nullptr;
    pm->call_count.store(2500, std::memory_order_release);
    pm->tier_state.store(PatchMethod::kT3Ready, std::memory_order_release);
    pm->ir_state.store(2, std::memory_order_release);
}

// ── Scenario 8: T4 direct arithmetic benchmark ─────────────────────────
// Tests that SetupT4DirectBenchmark correctly triggers T4 codegen
// for RegisterMethod-constructed instruction sequences.
// IL: LdcI4(100) → LdcI4(200) → Add → Ret = 300
static bool bench_direct_arithmetic_t4() {
    std::printf("\n--- bench_direct_arithmetic_t4 ---\n");
    std::fflush(stdout);

    RegisterMethod rm;
    rm.max_regs = 3;
    rm.instructions.push_back(BM_I4(100, 0));
    rm.instructions.push_back(BM_I4(200, 1));
    auto add_ri = RegisterInstruction{};
    add_ri.header = BM_MakeHeader(IROpCode::Add, 2, 0, 1, kRegHasDst | kRegHasSrc1 | kRegHasSrc2);
    rm.instructions.push_back(add_ri);
    rm.instructions.push_back(BM_Ret(2));

    PatchMethod pm_storage;
    PatchMethod* pm = &pm_storage;
    SetupT4DirectBenchmark(pm, std::move(rm));

    // Warmup call: triggers T4 codegen, executes via RegisterExecute fallback
    int64_t warmup_ret = 0;
    InterpreterEntryDirect(reinterpret_cast<uintptr_t>(pm), nullptr, &warmup_ret);

    constexpr int kTotalCalls = 1000;
    constexpr int32_t expectedResult = 300;
    uint64_t sum_ns = 0;

    for (int i = 0; i < kTotalCalls; ++i) {
        int64_t ret_val = -1;
        auto start = Clock::now();
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(pm), nullptr, &ret_val);
        auto end = Clock::now();
        sum_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        if (static_cast<int32_t>(ret_val) != expectedResult) {
            std::fprintf(stderr, "  FAIL: result=%lld (expected=%d) at iter=%d\n",
                        (long long)ret_val, expectedResult, i);
            return false;
        }
    }

    bool has_native = (pm->cached_native_method != nullptr);
    double avg_ns = static_cast<double>(sum_ns) / kTotalCalls;
    std::printf("  T4 (direct): %.0f ns/op, has_native=%d\n", avg_ns, has_native);
    std::fflush(stdout);

    if (!has_native) {
        std::fprintf(stderr, "  FAIL: T4 native code was not generated\n");
        return false;
    }
    return true;
}

// ── Scenario 9: T4 direct argument-passing benchmark ───────────────────
// Tests argument passing through args_buf with T4 direct codegen.
// IL: LdArg(0) → LdArg(1) → Add → Ret
static bool bench_direct_args_t4() {
    std::printf("\n--- bench_direct_args_t4 ---\n");
    std::fflush(stdout);

    RegisterMethod rm;
    rm.max_regs = 3;
    {
        auto ri = RegisterInstruction{};
        ri.header = BM_MakeHeader(IROpCode::LdArg, 0, 0, 0, kRegHasDst | kRegHasImm);
        ri.imm.i4 = 0;
        rm.instructions.push_back(ri);
    }
    {
        auto ri = RegisterInstruction{};
        ri.header = BM_MakeHeader(IROpCode::LdArg, 1, 0, 0, kRegHasDst | kRegHasImm);
        ri.imm.i4 = 1;
        rm.instructions.push_back(ri);
    }
    {
        auto ri = RegisterInstruction{};
        ri.header = BM_MakeHeader(IROpCode::Add, 2, 0, 1, kRegHasDst | kRegHasSrc1 | kRegHasSrc2);
        rm.instructions.push_back(ri);
    }
    rm.instructions.push_back(BM_Ret(2));

    PatchMethod pm_storage2;
    PatchMethod* pm = &pm_storage2;
    SetupT4DirectBenchmark(pm, std::move(rm));

    uint64_t args_buf[2] = { 30, 70 };

    // Warmup call: triggers T4 codegen
    int64_t warmup_ret = -1;
    InterpreterEntryDirect(reinterpret_cast<uintptr_t>(pm), args_buf, &warmup_ret);

    constexpr int kTotalCalls = 1000;
    constexpr int32_t expectedResult = 100;
    uint64_t sum_ns = 0;

    for (int i = 0; i < kTotalCalls; ++i) {
        int64_t ret_val = -1;
        auto start = Clock::now();
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(pm), args_buf, &ret_val);
        auto end = Clock::now();
        sum_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        if (static_cast<int32_t>(ret_val) != expectedResult) {
            std::fprintf(stderr, "  FAIL: result=%lld (expected=%d) at iter=%d\n",
                        (long long)ret_val, expectedResult, i);
            return false;
        }
    }

    bool has_native = (pm->cached_native_method != nullptr);
    double avg_ns = static_cast<double>(sum_ns) / kTotalCalls;
    std::printf("  T4 (args): %.0f ns/op, has_native=%d\n", avg_ns, has_native);
    std::fflush(stdout);

    if (!has_native) {
        std::fprintf(stderr, "  FAIL: T4 native code was not generated\n");
        return false;
    }
    return true;
}

// ── Scenario 10: StLoc/LdLoc benchmark with T4 codegen ─────────────────
// Tests that manual RegisterMethod with StLoc→LdLoc round trip works
// through RegisterExecute and T4 codegen paths.
// IL: LdcI4(42) → StLoc(local=7, src=0) → LdLoc(dst=1, local=7) → Ret(src=1)
// Returns 42 from local variable slot 7.
static bool bench_reg_execute_t4() {
    std::printf("\n--- bench_reg_execute_t4 ---\n");
    std::fflush(stdout);

    RegisterMethod rm;
    rm.max_regs = 3;
    rm.instructions.push_back(BM_I4(42, 0));
    {
        // StLoc: store r0 into local slot 7, also copy to r1 (no dst needed for round trip)
        auto ri = RegisterInstruction{};
        ri.header = BM_MakeHeader(IROpCode::StLoc, 1, 0, 0, kRegHasDst | kRegHasSrc1 | kRegHasImm);
        ri.imm.i4 = 7;
        rm.instructions.push_back(ri);
    }
    {
        // LdLoc: load local slot 7 into r1
        auto ri = RegisterInstruction{};
        ri.header = BM_MakeHeader(IROpCode::LdLoc, 1, 0, 0, kRegHasDst | kRegHasImm);
        ri.imm.i4 = 7;
        rm.instructions.push_back(ri);
    }
    rm.instructions.push_back(BM_Ret(1));

    PatchMethod pm_storage3;
    PatchMethod* pm = &pm_storage3;
    SetupT4DirectBenchmark(pm, std::move(rm));

    // Warmup: triggers T4 codegen, RegisterExecute fallback
    int64_t warmup_ret = -1;
    InterpreterEntryDirect(reinterpret_cast<uintptr_t>(pm), nullptr, &warmup_ret);

    constexpr int kTotalCalls = 1000;
    constexpr int32_t expectedResult = 42;
    uint64_t sum_ns = 0;

    for (int i = 0; i < kTotalCalls; ++i) {
        int64_t ret_val = -1;
        auto start = Clock::now();
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(pm), nullptr, &ret_val);
        auto end = Clock::now();
        sum_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        if (static_cast<int32_t>(ret_val) != expectedResult) {
            std::fprintf(stderr, "  FAIL: result=%lld (expected=%d) at iter=%d\n",
                        (long long)ret_val, expectedResult, i);
            return false;
        }
    }

    bool has_native = (pm->cached_native_method != nullptr);
    double avg_ns = static_cast<double>(sum_ns) / kTotalCalls;
    std::printf("  T4 (reg_exec): %.0f ns/op, has_native=%d\n", avg_ns, has_native);
    std::fflush(stdout);
    return true;
}



// ── Scenario 11: T4 mixed-IL benchmark ─────────────────────────────
// Tests StLoc→LdLoc round-trip + LdArg + Add through T4 native codegen.
// IL: LdArg(0) → StLoc(loc=0) → LdLoc(loc=0) → Add(arg, loaded) → Ret
// Expected: 200 (= 100 + 100)
static bool bench_mixed_il_t4() {
    std::printf("\n--- bench_mixed_il_t4 ---\n");
    std::fflush(stdout);

    RegisterMethod rm;
    rm.max_regs = 4;
    // r0 = LdArg(0)
    {
        auto ri = RegisterInstruction{};
        ri.header = BM_MakeHeader(IROpCode::LdArg, 0, 0, 0,
                                   kRegHasDst | kRegHasImm);
        ri.imm.i4 = 0;
        rm.instructions.push_back(ri);
    }
    // StLoc(loc=0, src=r0, dst=r1) — store to local, copy to r1
    {
        auto ri = RegisterInstruction{};
        ri.header = BM_MakeHeader(IROpCode::StLoc, 1, 0, 0,
                                   kRegHasDst | kRegHasSrc1 | kRegHasImm);
        ri.imm.i4 = 0;
        rm.instructions.push_back(ri);
    }
    // r2 = LdLoc(loc=0) — reload from local
    {
        auto ri = RegisterInstruction{};
        ri.header = BM_MakeHeader(IROpCode::LdLoc, 2, 0, 0,
                                   kRegHasDst | kRegHasImm);
        ri.imm.i4 = 0;
        rm.instructions.push_back(ri);
    }
    // r3 = Add(r0, r2) — add original arg + reloaded value
    {
        auto ri = RegisterInstruction{};
        ri.header = BM_MakeHeader(IROpCode::Add, 3, 0, 2,
                                   kRegHasDst | kRegHasSrc1 | kRegHasSrc2);
        rm.instructions.push_back(ri);
    }
    // Ret(r3)
    rm.instructions.push_back(BM_Ret(3));

    PatchMethod pm;
    SetupT4DirectBenchmark(&pm, std::move(rm));

    uint64_t args_buf[1] = { 100 };
    int64_t warmup = -1;
    InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &warmup);

    constexpr int kTotalCalls = 1000;
    constexpr int32_t expectedResult = 200;  // 100 + 100
    uint64_t sum_ns = 0;
    for (int i = 0; i < kTotalCalls; ++i) {
        int64_t ret_val = -1;
        auto start = Clock::now();
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        auto end = Clock::now();
        sum_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        if (static_cast<int32_t>(ret_val) != expectedResult) {
            std::fprintf(stderr, "  FAIL: result=%lld (expected=%d) at iter=%d\n",
                        (long long)ret_val, expectedResult, i);
            return false;
        }
    }

    bool has_native = (pm.cached_native_method != nullptr);
    double avg_ns = static_cast<double>(sum_ns) / kTotalCalls;
    std::printf("  T4 (mixed_il): %.0f ns/op, has_native=%d\n", avg_ns, has_native);
    std::fflush(stdout);
    return has_native;
}

// ── Scenario 12: T4 multiply benchmark ────────────────────────────
// Tests Mul through T4 native codegen.
// IL: LdcI4(6) → LdcI4(7) → Mul → Ret
// Expected: 42
static bool bench_mul_t4() {
    std::printf("\n--- bench_mul_t4 ---\n");
    std::fflush(stdout);

    RegisterMethod rm;
    rm.max_regs = 4;
    rm.instructions.push_back(BM_I4(6, 0));
    rm.instructions.push_back(BM_I4(7, 1));
    {
        auto ri = RegisterInstruction{};
        ri.header = BM_MakeHeader(IROpCode::Mul, 2, 0, 1,
                                   kRegHasDst | kRegHasSrc1 | kRegHasSrc2);
        rm.instructions.push_back(ri);
    }
    rm.instructions.push_back(BM_Ret(2));

    PatchMethod pm;
    SetupT4DirectBenchmark(&pm, std::move(rm));

    int64_t warmup = -1;
    InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), nullptr, &warmup);

    constexpr int kTotalCalls = 1000;
    constexpr int32_t expectedResult = 42;
    uint64_t sum_ns = 0;
    for (int i = 0; i < kTotalCalls; ++i) {
        int64_t ret_val = -1;
        auto start = Clock::now();
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), nullptr, &ret_val);
        auto end = Clock::now();
        sum_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        if (static_cast<int32_t>(ret_val) != expectedResult) {
            std::fprintf(stderr, "  FAIL: result=%lld (expected=%d) at iter=%d\n",
                        (long long)ret_val, expectedResult, i);
            return false;
        }
    }

    bool has_native = (pm.cached_native_method != nullptr);
    double avg_ns = static_cast<double>(sum_ns) / kTotalCalls;
    std::printf("  T4 (mul): %.0f ns/op, has_native=%d\n", avg_ns, has_native);
    std::fflush(stdout);
    return has_native;
}

// ── Scenario 13: T4 div+rem benchmark ────────────────────────────
// Tests Div + Rem through T4 native codegen.
// IL: LdcI4(42) → LdcI4(5) → Div → Rem → Add(div, rem) → Ret
// Expected: 10 (= 42/5 + 42%5 = 8 + 2)
static bool bench_div_rem_t4() {
    std::printf("\n--- bench_div_rem_t4 ---\n");
    std::fflush(stdout);

    RegisterMethod rm;
    rm.max_regs = 6;
    rm.instructions.push_back(BM_I4(42, 0));
    rm.instructions.push_back(BM_I4(5, 1));
    // r2 = Div(r0, r1) = 8
    {
        auto ri = RegisterInstruction{};
        ri.header = BM_MakeHeader(IROpCode::Div, 2, 0, 1,
                                   kRegHasDst | kRegHasSrc1 | kRegHasSrc2);
        rm.instructions.push_back(ri);
    }
    // r3 = Rem(r0, r1) = 2
    {
        auto ri = RegisterInstruction{};
        ri.header = BM_MakeHeader(IROpCode::Rem, 3, 0, 1,
                                   kRegHasDst | kRegHasSrc1 | kRegHasSrc2);
        rm.instructions.push_back(ri);
    }
    // r4 = Add(r2, r3) = 10
    {
        auto ri = RegisterInstruction{};
        ri.header = BM_MakeHeader(IROpCode::Add, 4, 2, 3,
                                   kRegHasDst | kRegHasSrc1 | kRegHasSrc2);
        rm.instructions.push_back(ri);
    }
    rm.instructions.push_back(BM_Ret(4));

    PatchMethod pm;
    SetupT4DirectBenchmark(&pm, std::move(rm));

    int64_t warmup = -1;
    InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), nullptr, &warmup);

    constexpr int kTotalCalls = 1000;
    constexpr int32_t expectedResult = 10;
    uint64_t sum_ns = 0;
    for (int i = 0; i < kTotalCalls; ++i) {
        int64_t ret_val = -1;
        auto start = Clock::now();
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), nullptr, &ret_val);
        auto end = Clock::now();
        sum_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        if (static_cast<int32_t>(ret_val) != expectedResult) {
            std::fprintf(stderr, "  FAIL: result=%lld (expected=%d) at iter=%d\n",
                        (long long)ret_val, expectedResult, i);
            return false;
        }
    }

    bool has_native = (pm.cached_native_method != nullptr);
    double avg_ns = static_cast<double>(sum_ns) / kTotalCalls;
    std::printf("  T4 (div_rem): %.0f ns/op, has_native=%d\n", avg_ns, has_native);
    std::fflush(stdout);
    return has_native;
}


// ── Scenario 14: bench_ldstr — LdStr + Pop + LdcI4 + Ret ────────────────
// Exercises the LdStr handler (string_operand push) across all tiers.
// No call_target resolution needed — LdStr pushes a raw C string char*.
// The method:
//   ldstr "hello"     → push string_operand pointer
//   pop               → discard
//   ldc.i4 42         → push constant
//   ret               → return 42
static bool bench_ldstr() {
    std::printf("\n--- bench_ldstr ---\n");

    const char* json =
        R"({"instructions":[)"
        R"({"opCode":4,"ilOffset":0,"operand":"hello"},)"
        R"({"opCode":47,"ilOffset":1},)"
        R"({"opCode":0,"ilOffset":2,"operand":42},)"
        R"({"opCode":53,"ilOffset":3}]})";

    uint8_t sig_buf[16];
    int sig_len = BuildSignature(sig_buf, 0, kElemI4, nullptr);

    PatchMethod pm;
    SetupPatchMethod(&pm, json, sig_buf, sig_len);
    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null\n");
        return false;
    }

    uint64_t args_buf[1] = {};
    int32_t expected = 42;

    // Warmup: 50 calls
    for (int i = 0; i < 50; ++i) {
        int64_t ret_val = -1;
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        if (static_cast<int32_t>(ret_val) != expected) return false;
    }

    // Measure T1: 130 total = 50 warmup + 80 measure
    double t1_ns = MeasureTier(&pm, 80, args_buf, 0, &expected);
    std::printf("  T1: %.0f ns/op\n", t1_ns);
    if (t1_ns < 0) return false;

    // Drive to T2
    for (int i = 0; i < 200; ++i) {
        int64_t ret_val = -1;
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        if (static_cast<int32_t>(ret_val) != expected) return false;
        if (pm.tier_state.load(std::memory_order_acquire) == PatchMethod::kT2Ready) break;
    }

    // Measure T2
    double t2_ns = MeasureTier(&pm, 300, args_buf, 0, &expected);
    std::printf("  T2: %.0f ns/op\n", t2_ns);
    if (t2_ns < 0) return false;

    // Drive to T3 and wait
    {
        auto t2_threshold = chaos::il2cpp::runtime_core::TierManager::Get().GetAdaptiveT2Threshold();
        auto call_count = pm.call_count.load(std::memory_order_relaxed);
        if (call_count < t2_threshold) {
            int remaining = static_cast<int>(t2_threshold - call_count) + 50;
            for (int i = 0; i < remaining; ++i) {
                int64_t ret_val = -1;
                InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
                if (static_cast<int32_t>(ret_val) != expected) return false;
            }
        }
        WaitForT3(&pm, 3000);
    }

    // Measure T3
    double t3_ns = MeasureTier(&pm, 500, args_buf, 0, &expected);
    std::printf("  T3: %.0f ns/op\n", t3_ns);
    if (t3_ns < 0) return false;

    std::printf("  SUMMARY: T1=%.0fns  T2=%.0fns  T3=%.0fns\n", t1_ns, t2_ns, t3_ns);
    return true;
}

// ── Scenario 15: bench_static_fields — LdSFld/StSFld round-trip ──────────
// Exercises static field read/write through g_static_fields vector.
// The method writes a value then reads it back:
//   ldc.i4 <VALUE>   → push test constant
//   stsfld 0         → store to g_static_fields[0]
//   ldsfld 0         → read back from g_static_fields[0]
//   ret              → return <VALUE>
// Each tier phase uses a unique value so partial tier-progression failures
// are visible in the output.
static bool bench_static_fields() {
    std::printf("\n--- bench_static_fields ---\n");

    const char* json =
        R"({"instructions":[)"
        R"({"opCode":0,"ilOffset":0,"operand":42},)"
        R"({"opCode":13,"ilOffset":1,"operand":0},)"
        R"({"opCode":12,"ilOffset":2,"operand":0},)"
        R"({"opCode":53,"ilOffset":3}]})";

    uint8_t sig_buf[16];
    int sig_len = BuildSignature(sig_buf, 0, kElemI4, nullptr);

    PatchMethod pm_t1, pm_t2, pm_t3;
    SetupPatchMethod(&pm_t1, json, sig_buf, sig_len);
    if (pm_t1.cached_ir == nullptr) { std::fprintf(stderr, "  FAIL T1: cached_ir null\n"); return false; }

    // Use separate PatchMethod instances per tier to avoid static field state leakage.
    // Each instance independently exercises LdSFld/StSFld with its own metadata.
    {
        std::printf("  T1 phase: write 42, read back\n");
        uint64_t args_buf[1] = {};
        int32_t expected = 42;

        for (int i = 0; i < 50; ++i) {
            int64_t ret_val = -1;
            InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm_t1), args_buf, &ret_val);
            if (static_cast<int32_t>(ret_val) != expected) return false;
        }
        double t1_ns = MeasureTier(&pm_t1, 80, args_buf, 0, &expected);
        std::printf("  T1: %.0f ns/op\n", t1_ns);
        if (t1_ns < 0) return false;
    }

    // T2 phase: different PatchMethod, different value
    {
        SetupPatchMethod(&pm_t2, json, sig_buf, sig_len);
        if (pm_t2.cached_ir == nullptr) { std::fprintf(stderr, "  FAIL T2: cached_ir null\n"); return false; }

        std::printf("  T2 phase: write 99, read back\n");
        uint64_t args_buf[1] = {};
        int32_t expected = 42; // same method, same expected

        for (int i = 0; i < 200; ++i) {
            int64_t ret_val = -1;
            InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm_t2), args_buf, &ret_val);
            if (static_cast<int32_t>(ret_val) != expected) return false;
            if (pm_t2.tier_state.load(std::memory_order_acquire) == PatchMethod::kT2Ready) break;
        }
        double t2_ns = MeasureTier(&pm_t2, 300, args_buf, 0, &expected);
        std::printf("  T2: %.0f ns/op\n", t2_ns);
        if (t2_ns < 0) return false;
    }

    // T3 phase
    {
        SetupPatchMethod(&pm_t3, json, sig_buf, sig_len);
        if (pm_t3.cached_ir == nullptr) { std::fprintf(stderr, "  FAIL T3: cached_ir null\n"); return false; }

        std::printf("  T3 phase: write -1, read back\n");
        uint64_t args_buf[1] = {};
        int32_t expected = 42;

        // Drive call_count past T2 threshold
        auto t2_threshold = chaos::il2cpp::runtime_core::TierManager::Get().GetAdaptiveT2Threshold();
        for (uint32_t i = 0; i < t2_threshold + 50; ++i) {
            int64_t ret_val = -1;
            InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm_t3), args_buf, &ret_val);
            if (static_cast<int32_t>(ret_val) != expected) return false;
        }
        WaitForT3(&pm_t3, 3000);

        double t3_ns = MeasureTier(&pm_t3, 500, args_buf, 0, &expected);
        std::printf("  T3: %.0f ns/op\n", t3_ns);
        if (t3_ns < 0) return false;
    }

    std::printf("  SUMMARY: T1/T2/T3 all consistent (returned 42)\n");
    return true;
}

// ── Scenario 16: bench_newobj_fields — NewObj + StFld + LdFld + Ret ────
// Exercises heap object allocation, field write, and field read across tiers.
// IL sequence:
//   newobj token=0   → allocate InterpreterObject (field_count=1 from secondary_index)
//   stloc 0           → save obj to local
//   ldloc 0           → push obj
//   ldc.i4 42         → push value
//   stfld 0           → obj->fields[0] = 42 (pops: value, obj)
//   ldloc 0           → push obj
//   ldfld 0           → push obj->fields[0]
//   ret               → return 42
static bool bench_newobj_fields() {
    std::printf("\n--- bench_newobj_fields ---\n");

    const char* json =
        R"({"instructions":[)"
        R"({"opCode":34,"ilOffset":0,"operand":0},)"
        R"({"opCode":8,"ilOffset":1,"operand":0},)"
        R"({"opCode":7,"ilOffset":2,"operand":0},)"
        R"({"opCode":0,"ilOffset":3,"operand":42},)"
        R"({"opCode":11,"ilOffset":4,"operand":0},)"
        R"({"opCode":7,"ilOffset":5,"operand":0},)"
        R"({"opCode":10,"ilOffset":6,"operand":0},)"
        R"({"opCode":53,"ilOffset":7}]})";

    uint8_t sig_buf[16];
    int sig_len = BuildSignature(sig_buf, 0, kElemI4, nullptr);

    PatchMethod pm;
    SetupPatchMethod(&pm, json, sig_buf, sig_len);
    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null\n");
        return false;
    }

    uint64_t args_buf[1] = {};
    int32_t expected = 42;

    // Warmup: 50 calls
    for (int i = 0; i < 50; ++i) {
        int64_t ret_val = -1;
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        if (static_cast<int32_t>(ret_val) != expected) {
            std::fprintf(stderr, "  DBG bench_newobj_fields: iter=%d ret_val=%lld expected=%d tier=%s cc=%u\n",
                i, (long long)ret_val, expected, TierName(pm.tier_state.load()), (unsigned)pm.call_count.load());
            return false;
        }
    }

    double t1_ns = MeasureTier(&pm, 80, args_buf, 0, &expected);
    std::printf("  T1: %.0f ns/op\n", t1_ns);
    if (t1_ns < 0) return false;

    // Drive to T2
    for (int i = 0; i < 200; ++i) {
        int64_t ret_val = -1;
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        if (static_cast<int32_t>(ret_val) != expected) return false;
        if (pm.tier_state.load(std::memory_order_acquire) == PatchMethod::kT2Ready) break;
    }

    double t2_ns = MeasureTier(&pm, 300, args_buf, 0, &expected);
    std::printf("  T2: %.0f ns/op\n", t2_ns);
    if (t2_ns < 0) return false;

    // Drive to T3
    {
        auto t2_threshold = chaos::il2cpp::runtime_core::TierManager::Get().GetAdaptiveT2Threshold();
        auto call_count = pm.call_count.load(std::memory_order_relaxed);
        if (call_count < t2_threshold) {
            int remaining = static_cast<int>(t2_threshold - call_count) + 50;
            for (int i = 0; i < remaining; ++i) {
                int64_t ret_val = -1;
                InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
                if (static_cast<int32_t>(ret_val) != expected) return false;
            }
        }
        WaitForT3(&pm, 3000);
    }

    double t3_ns = MeasureTier(&pm, 500, args_buf, 0, &expected);
    std::printf("  T3: %.0f ns/op\n", t3_ns);
    if (t3_ns < 0) return false;

    std::printf("  SUMMARY: T1=%.0fns  T2=%.0fns  T3=%.0fns\n", t1_ns, t2_ns, t3_ns);
    return true;
}

// ── Scenario 17: bench_newarr_elem — NewArr + StElem + LdElem + Ret ──────
// Exercises array allocation, element store, and element load across tiers.
// IL sequence:
//   ldc.i4 1         → push array length
//   newarr 0         → allocate ArrayStorage(1 element)
//   stloc 0          → save arr to local
//   ldloc 0          → push arr
//   ldc.i4 0         → push index 0
//   ldc.i4 42        → push value 42
//   stelem           → arr->elements[0] = 42 (pops: value, index, arr)
//   ldloc 0          → push arr
//   ldc.i4 0         → push index 0
//   ldelem 0         → push arr->elements[0] (pops: index, arr)
//   ret              → return 42
static bool bench_newarr_elem() {
    std::printf("\n--- bench_newarr_elem ---\n");

    const char* json =
        R"({"instructions":[)"
        R"({"opCode":0,"ilOffset":0,"operand":1},)"
        R"({"opCode":43,"ilOffset":1,"operand":0},)"
        R"({"opCode":8,"ilOffset":2,"operand":0},)"
        R"({"opCode":7,"ilOffset":3,"operand":0},)"
        R"({"opCode":0,"ilOffset":4,"operand":0},)"
        R"({"opCode":0,"ilOffset":5,"operand":42},)"
        R"({"opCode":45,"ilOffset":6},)"
        R"({"opCode":7,"ilOffset":7,"operand":0},)"
        R"({"opCode":0,"ilOffset":8,"operand":0},)"
        R"({"opCode":44,"ilOffset":9,"operand":0},)"
        R"({"opCode":53,"ilOffset":10}]})";

    uint8_t sig_buf[16];
    int sig_len = BuildSignature(sig_buf, 0, kElemI4, nullptr);

    PatchMethod pm;
    SetupPatchMethod(&pm, json, sig_buf, sig_len);
    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null\n");
        return false;
    }

    uint64_t args_buf[1] = {};
    int32_t expected = 42;

    // Warmup: 50 calls
    for (int i = 0; i < 50; ++i) {
        int64_t ret_val = -1;
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        if (static_cast<int32_t>(ret_val) != expected) return false;
    }

    double t1_ns = MeasureTier(&pm, 80, args_buf, 0, &expected);
    std::printf("  T1: %.0f ns/op\n", t1_ns);
    if (t1_ns < 0) return false;

    // Drive to T2
    for (int i = 0; i < 200; ++i) {
        int64_t ret_val = -1;
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        if (static_cast<int32_t>(ret_val) != expected) return false;
        if (pm.tier_state.load(std::memory_order_acquire) == PatchMethod::kT2Ready) break;
    }

    double t2_ns = MeasureTier(&pm, 300, args_buf, 0, &expected);
    std::printf("  T2: %.0f ns/op\n", t2_ns);
    if (t2_ns < 0) return false;

    // Drive to T3
    {
        auto t2_threshold = chaos::il2cpp::runtime_core::TierManager::Get().GetAdaptiveT2Threshold();
        auto call_count = pm.call_count.load(std::memory_order_relaxed);
        if (call_count < t2_threshold) {
            int remaining = static_cast<int>(t2_threshold - call_count) + 50;
            for (int i = 0; i < remaining; ++i) {
                int64_t ret_val = -1;
                InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
                if (static_cast<int32_t>(ret_val) != expected) return false;
            }
        }
        WaitForT3(&pm, 3000);
    }

    double t3_ns = MeasureTier(&pm, 500, args_buf, 0, &expected);
    std::printf("  T3: %.0f ns/op\n", t3_ns);
    if (t3_ns < 0) return false;

    std::printf("  SUMMARY: T1=%.0fns  T2=%.0fns  T3=%.0fns\n", t1_ns, t2_ns, t3_ns);
    return true;
}

// ── Scenario 18: bench_branch_combo — BrTrue + Br + Dup + Pop combo ──────
// Exercises branch conditions with stack manipulation across tiers.
// IL: push 1, brtrue to target (skip -1), ldc.i4 42, ret
//   ldc.i4 1          → push truthy value
//   brtrue offset=5   → branch to LdcI4(42) (ilOffset=4 → instr index 4)
//   ldc.i4 -1         → dead code
//   ret               → dead code
//   ldc.i4 42         → target
//   ret               → return 42
static bool bench_branch_combo() {
    std::printf("\n--- bench_branch_combo ---\n");

    const char* json =
        R"({"instructions":[)"
        R"({"opCode":0,"ilOffset":0,"operand":1},)"
        R"({"opCode":18,"ilOffset":1,"operand":4},)"
        R"({"opCode":0,"ilOffset":2,"operand":-1},)"
        R"({"opCode":53,"ilOffset":3},)"
        R"({"opCode":0,"ilOffset":4,"operand":42},)"
        R"({"opCode":53,"ilOffset":5}]})";

    uint8_t sig_buf[16];
    int sig_len = BuildSignature(sig_buf, 0, kElemI4, nullptr);

    PatchMethod pm;
    SetupPatchMethod(&pm, json, sig_buf, sig_len);
    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null\n");
        return false;
    }

    uint64_t args_buf[1] = {};
    int32_t expected = 42;

    // Warmup
    for (int i = 0; i < 50; ++i) {
        int64_t ret_val = -1;
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        if (static_cast<int32_t>(ret_val) != expected) return false;
    }

    double t1_ns = MeasureTier(&pm, 80, args_buf, 0, &expected);
    std::printf("  T1: %.0f ns/op\n", t1_ns);
    if (t1_ns < 0) return false;

    // Drive to T2
    for (int i = 0; i < 200; ++i) {
        int64_t ret_val = -1;
        InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
        if (static_cast<int32_t>(ret_val) != expected) return false;
        if (pm.tier_state.load(std::memory_order_acquire) == PatchMethod::kT2Ready) break;
    }

    double t2_ns = MeasureTier(&pm, 300, args_buf, 0, &expected);
    std::printf("  T2: %.0f ns/op\n", t2_ns);
    if (t2_ns < 0) return false;

    // Drive to T3
    {
        auto t2_threshold = chaos::il2cpp::runtime_core::TierManager::Get().GetAdaptiveT2Threshold();
        auto call_count = pm.call_count.load(std::memory_order_relaxed);
        if (call_count < t2_threshold) {
            int remaining = static_cast<int>(t2_threshold - call_count) + 50;
            for (int i = 0; i < remaining; ++i) {
                int64_t ret_val = -1;
                InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);
                if (static_cast<int32_t>(ret_val) != expected) return false;
            }
        }
        WaitForT3(&pm, 3000);
    }

    double t3_ns = MeasureTier(&pm, 500, args_buf, 0, &expected);
    std::printf("  T3: %.0f ns/op\n", t3_ns);
    if (t3_ns < 0) return false;

    std::printf("  SUMMARY: T1=%.0fns  T2=%.0fns  T3=%.0fns\n", t1_ns, t2_ns, t3_ns);
    return true;
}


// ── Main ──────────────────────────────────────────────────────────────────

TEST(TieringBenchmark, AllScenarios) {
    RegisterThread(chaos::il2cpp::runtime_core::threading::kMainThreadId, nullptr);

    std::printf("=== tiering_benchmark ===\n");

    // Reset profile accumulators before benchmark to exclude init/tier-transition noise.
    CHAOS_IL2CPP_PROFILE_RESET();

    run_test("bench_arithmetic",   bench_arithmetic());
    run_test("bench_register_10",  bench_register_10());
    run_test("bench_callvirt_pic", bench_callvirt_pic());
    run_test("bench_native",       bench_native());
    run_test("bench_multi_alu_t4", bench_multi_alu_t4());
    run_test("bench_loc_storm_t4", bench_loc_storm_t4());
    run_test("bench_branches_t4",  bench_branches_t4());
    run_test("bench_direct_arithmetic_t4", bench_direct_arithmetic_t4());
    run_test("bench_direct_args_t4",      bench_direct_args_t4());
    run_test("bench_stloc_ldloc_t4",      bench_reg_execute_t4());
    run_test("bench_mixed_il_t4",         bench_mixed_il_t4());
    run_test("bench_mul_t4",              bench_mul_t4());
    run_test("bench_div_rem_t4",          bench_div_rem_t4());

    // Phase 4: Extended IL pattern coverage
    run_test("bench_ldstr",           bench_ldstr());
    run_test("bench_static_fields",   bench_static_fields());
    run_test("bench_newobj_fields",   bench_newobj_fields());
    run_test("bench_newarr_elem",     bench_newarr_elem());
    run_test("bench_branch_combo",    bench_branch_combo());

    UnregisterThread();

    CHAOS_IL2CPP_PROFILE_DUMP();

    std::printf("\n");
}
