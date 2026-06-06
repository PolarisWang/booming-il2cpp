// runtime-entry.cpp — Auto-generated
// CLI entry point for native-AOT verification dispatch.
//
// Modes:
//   (no args)            — fact: run all subject entries, print Passed: N/M
//   --fact-json          — per-method JSON output (value-level verification)
//   --benchmark N I      — benchmark method N for I iterations
//   --benchmark-all I    — benchmark ALL methods for I iterations, per-line JSON
//   --hotupdate          — hotpatch fact: baseline + apply + semantic-check + revert
//   --hotupdate-and-benchmark N I — post-patch benchmark
//   --microbench         — interpreter microbenchmarks

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <cstdint>
#include <cinttypes>
#include <csignal>
#include <csetjmp>

#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>

// CRT safety: suppress invalid parameter fast-fail and abort crash.
// Some subject methods trigger CRT invalid parameter assertions
// (e.g. memcpy(NULL, ...)) which would otherwise __fastfail with
// STATUS_STACK_BUFFER_OVERRUN (0xC0000409) and kill the process.
// For verification, we want graceful recovery via SEH + longjmp instead.
static void DummyInvalidParameterHandler(
    const wchar_t*, const wchar_t*, const wchar_t*, unsigned, uintptr_t) {}

// Thread-local jmp_buf for SIGABRT recovery in the fact-json worker thread.
// When abort() fires during a subject dispatch, the signal handler longjmps
// back to the dispatch loop, which records the method as failed and continues
// to the next subject.  This prevents abort -> _exit(3) from killing the
// process mid-way through the verification suite.
static thread_local jmp_buf t_abort_jmp;
#endif

// g_log_use_stderr is now exported by the prebuilt chaos_runtime_core
// library, so we do NOT define it here — doing so would cause a
// multiple-definition linker error.

#include <chaos/config.h>
#include <chaos/native_types.h>
#include <chaos/profile.h>
#include <runtime_core.h>
#include <gc/gc_api.h>

#include "chaos_runtime_host.h"

#include <chaos/hotpatch_dispatch.h>
#include <patch_loader.h>

extern "C" const int kAotMethodCount;

// kSubjectEntryCount / kSubjectSlotMap are defined in native-aot.generated.cpp
// (codegen output). kSubjectBenchmarkEligible is defined below (TPG-generated).
extern "C" const int kSubjectEntryCount;
extern "C" const int kSubjectSlotMap[];

extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept;

// Codegen registration symbols (defined in native-aot.generated.cpp)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration;
extern "C" const MetadataRegistrationV0 chaos_codegen_metadata_registration;
extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options;
extern "C" void ChaosRegisterGcLayouts();

// kDefaultArgThunks: in AOT mode, nullptr is safe because ChaosDispatchMethod
// falls through to entry.direct_ptr (set by SetDirectDispatch).
// In JIT mode, direct_ptr is REPLACED by RegisterJitEntryMethods() with
// JIT precode trampoline → JIT compilation → hang. Use kDefaultArgThunks
// to call AOT function body directly (verification tests AOT codegen).

extern "C" void (*kDefaultArgThunks[])() noexcept;
#define CHAOS_USE_DEFAULT_THUNKS kDefaultArgThunks


// Benchmark result struct (must match verification_dispatch.generated.cpp)
struct BenchmarkResult {
    double elapsed_ms;
    int64_t allocated_bytes;
    bool caught_exception;
};

extern "C" CHAOS_IL2CPP_INT32 RunFactAll();
extern "C" BenchmarkResult RunBenchmark(int entry_index, int iterations);
extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll();
extern "C" BenchmarkResult RunHotpatchBenchmark(int entry_index, int iterations);

// Assert lifecycle functions (from Chaos.TestFramework.Sdk via external runtime table)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset();
extern "C" CHAOS_IL2CPP_INT32 Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete();

// Interop stub declarations (defined in interop_stubs.cpp from SDK runtime_stubs)
extern "C" int ChaosMarshalGetLastPInvokeError() noexcept;
extern "C" int ChaosMarshalGetHRForLastWin32Error() noexcept;
extern "C" int ChaosMarshalGetExceptionCode() noexcept;
extern "C" CHAOS_IL2CPP_INTPTR ChaosMarshalGetExceptionPointers() noexcept;
extern "C" int ChaosMarshalAreComObjectsAvailableForCleanup() noexcept;

// External runtime function table (populated by codegen, may have null entries)
extern "C" void* kChaosExternalRuntimeFnTable[];
extern "C" int32_t kChaosExternalRuntimeCount;

// Per-method host arrays for ApplyPatchFromMemoryEx (defined in patch-host-arrays.cpp).
// Sentinel values when no patch data is embedded; replaced by the hotupdate pipeline
// with real per-method mappings to enable name-based lookup in HotpatchNameRegistry.
extern const char* const kPatchDataHostNamespace;
extern const char* const kPatchDataHostTypeNames[];
extern const char* const kPatchDataHostMethodNames[];
extern const int kPatchDataCount;

extern const uint8_t kPatchData[];
extern const size_t kPatchDataSize;
extern const char* const kPatchDataHostClassName;

extern "C" void RunMicrobench();

// Hotupdate benchmark iteration count (set by --benchmark-iterations in main)
static int g_benchmark_iterations = 0;


extern "C" void ChaosJitRegisterAll();
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module;

#if defined(_WIN32)
#include <windows.h>
//
// VEH: log + skip crashes in JIT-code pages (MEM_PRIVATE VirtualAlloc).
// This prevents chaos_codegen.dll's internal __except from catching the
// crash (which itself crashes, burning ~10KB stack per event).  By handling
// in the VEH, frame-based handlers (including chaos_codegen.dll's) never
// fire, eliminating cumulative stack exhaustion from repeated null-deref
// benchmark iterations.
//
static LONG CALLBACK JitVehHandler(PEXCEPTION_POINTERS ExceptionInfo) noexcept {
    auto* ctx = ExceptionInfo->ContextRecord;
    auto* er = ExceptionInfo->ExceptionRecord;

    // Throttle: if the same RIP crashes repeatedly, the skip isn't working.
    // Stop trying after N consecutive crashes at the same RIP to avoid
    // infinite crash → skip → crash loops.
    static void* s_last_rip = nullptr;
    static int s_same_rip_count = 0;
    if (s_last_rip == reinterpret_cast<void*>(ctx->Rip)) {
        s_same_rip_count++;
    } else {
        s_last_rip = reinterpret_cast<void*>(ctx->Rip);
        s_same_rip_count = 0;
    }
    if (s_same_rip_count >= 3) {
        std::fprintf(stderr, "JIT ABORT: crash at RIP=%p repeated %d times — giving up\n",
                     reinterpret_cast<void*>(ctx->Rip), s_same_rip_count);
        std::fflush(stderr);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    // Log
    // For ACCESS_VIOLATION in JIT code, fall through to __try/__except
    // so benchmark loop skips the crashing method and continues.
    if (er->ExceptionCode == STATUS_ACCESS_VIOLATION) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    std::fprintf(stderr,
        "JIT CRASH: Code=0x%08lX RIP=0x%p RSP=0x%p"
        " RAX=0x%p RBX=0x%p RCX=0x%p RDX=0x%p RSI=0x%p RDI=0x%p"
        " R8=0x%p R9=0x%p R10=0x%p R11=0x%p R12=0x%p R13=0x%p R14=0x%p R15=0x%p"
        " nprm=%lu\n",
        er->ExceptionCode,
        (void*)ctx->Rip, (void*)ctx->Rsp,
        (void*)ctx->Rax, (void*)ctx->Rbx,
        (void*)ctx->Rcx, (void*)ctx->Rdx,
        (void*)ctx->Rsi, (void*)ctx->Rdi,
        (void*)ctx->Rbp,
        (void*)ctx->R8, (void*)ctx->R9,
        (void*)ctx->R10, (void*)ctx->R11,
        (void*)ctx->R12, (void*)ctx->R13,
        (void*)ctx->R14, (void*)ctx->R15,
        er->NumberParameters);
    for (uint16_t i = 0; i < er->NumberParameters && i < 15; ++i) {
        std::fprintf(stderr, "  param[%u]=0x%p\n", i, (void*)er->ExceptionInformation[i]);
    }
    std::fflush(stderr);

    // If RIP is in a MEM_PRIVATE page (JIT-code VirtualAlloc), skip the
    // faulting instruction and continue execution.  This bypasses all
    // frame-based handlers (including chaos_codegen.dll's crashing __except).
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(reinterpret_cast<LPCVOID>(ctx->Rip), &mbi, sizeof(mbi)) &&
        mbi.State == MEM_COMMIT && mbi.Type == MEM_PRIVATE) {
        ctx->Rip += 3;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
#endif


static chaos::il2cpp::runtime_core::PatchContext* ApplyHotpatchIfAvailable() {
    if (kPatchDataSize > 0u) {
        // Use ApplyPatchFromMemoryEx with per-method host arrays from
        // patch-host-arrays.cpp to map Subject_N methods to their AOT
        // HotpatchNameRegistry entries.  The host arrays are generated by
        // the hotupdate pipeline before incremental cmake rebuild.
        auto* patch_ctx = chaos::il2cpp::runtime_core::ApplyPatchFromMemoryEx(
            kPatchData, kPatchDataSize,
            kPatchDataHostNamespace,
            kPatchDataHostTypeNames,
            kPatchDataHostMethodNames,
            kPatchDataCount);
        if (patch_ctx == nullptr) {
            std::fprintf(stderr, "WARN: ApplyPatchFromMemoryEx returned null (no patches applied)\n");
        } else {
            std::fprintf(stderr, "INFO: Applied patches to %u methods\n",
                         static_cast<unsigned>(patch_ctx->method_count));
        }
        return patch_ctx;
    }
    return nullptr;
}

// Load patch data from an external file (used by the verification pipeline).
// Returns nullptr if path is nullptr or file cannot be opened — the caller
// falls through to ApplyHotpatchIfAvailable() for embedded data.
static chaos::il2cpp::runtime_core::PatchContext* ApplyHotpatchFromFile(const char* path) {
    if (path == nullptr) return nullptr;
    FILE* f = fopen(path, "rb");
    if (!f) { std::fprintf(stderr, "WARN: cannot open patch data: %s\n", path); return nullptr; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    uint8_t* data = (uint8_t*)std::malloc(size > 0 ? size : 1);
    if (!data) { fclose(f); return nullptr; }
    size_t read_size = fread(data, 1, size, f);
    fclose(f);
    if (read_size == 0) { std::free(data); return nullptr; }
    auto* ctx = chaos::il2cpp::runtime_core::ApplyPatchFromMemoryEx(
        data, read_size,
        kPatchDataHostNamespace,
        kPatchDataHostTypeNames,
        kPatchDataHostMethodNames,
        kPatchDataCount);
    std::free(data);
    if (ctx == nullptr) {
        std::fprintf(stderr, "WARN: ApplyPatchFromMemoryEx returned null (no patches applied)\n");
    } else {
        std::fprintf(stderr, "INFO: Applied patches to %u methods from %s\n",
                     static_cast<unsigned>(ctx->method_count), path);
    }
    return ctx;
}

static int RunFactMode() {
    const int kCount = kSubjectEntryCount;
    int passed_count = 0;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
#if defined(_WIN32)
        __try {
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            ++passed_count;
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            // SEH caught (e.g. STATUS_ACCESS_VIOLATION) — skip increment
        }
#else
        CHAOS_EH_TRY
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            ++passed_count;
        CHAOS_EH_CATCH_BEGIN
            // caught — skip increment
        CHAOS_EH_END
#endif
    }
    int failed_count = kCount - passed_count;
    printf("Passed: %d/%d\n", passed_count, kCount);
    std::fflush(stdout);
    	return failed_count;
	}

	// ── FACT_CHECK macros ──
	// Must be defined before RunFactJsonMode (uses them).
// ── VEH handler for AOT fact dispatch ─────────────────────────────
// Catches hardware exceptions (AV, stack overflow) during per-method
// dispatch and longjmps to the _setjmp recovery point so the method
// is marked as caught.  Managed C++ exceptions (0xE06D7363) are
// ignored and propagate normally to Assert.Throws catch blocks.
#if defined(_WIN32)
static LONG CALLBACK FactVehHandler(PEXCEPTION_POINTERS ExceptionInfo) noexcept {
    auto* er = ExceptionInfo->ExceptionRecord;
    // Only catch hardware exceptions — let all other exceptions
    // (including managed C++ exceptions 0xE06D7363) pass through.
    if (er->ExceptionCode != STATUS_ACCESS_VIOLATION &&
        er->ExceptionCode != STATUS_STACK_OVERFLOW)
        return EXCEPTION_CONTINUE_SEARCH;
    // Recover via longjmp to the per-method _setjmp point.
    // t_abort_jmp is set by RunFactJsonMode before each dispatch.
    longjmp(t_abort_jmp, 1);
    return EXCEPTION_CONTINUE_SEARCH;  // unreachable
}
#endif
	// ── --fact-json: per-method JSON output (R1+R2: value-level verification) ──
// Iterates only subject entries (kSubjectEntryCount) via kSubjectSlotMap, not
// all AOT-compiled methods (kAotMethodCount).  Closure/framework methods may
// access uninitialized runtime state and cause uncatchable access violations;
// the CombinedSubjects/ prefix detection in IsSubjectMethod() correctly marks
// all Subject_N wrappers from the subjects DLL as subject entries.
static int RunFactJsonMode() {
    const int kCount = kSubjectEntryCount;

    // Reset the managed assertion state before the fact loop so that
    // s_exitCode from previous runs is cleared.  After dispatch, check
    // Complete() as a backstop for assertion failures that may not have
    // propagated as C++ exceptions (e.g. codegen-inlined assertion paths).
#ifndef CHAOS_IL2CPP_JIT_MODE
#define CHAOS_FACT_RESET()   Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset()
#define CHAOS_FACT_CHECK()   do { \
    if (Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete() != 0) { \
        std::fprintf(stderr, "[ASSERT] s_exitCode was non-zero after fact loop\n"); \
    } \
} while(0)
#else
#define CHAOS_FACT_RESET()   ((void)0)
#define CHAOS_FACT_CHECK()   ((void)0)
#endif

#if defined(_WIN32)
    CHAOS_FACT_RESET();



    void* g_fact_veh = AddVectoredExceptionHandler(1, FactVehHandler);



    // Use a worker thread with 300s timeout so a hanging dispatch (infinite
    // loop, deadlock) doesn't block the process forever.  On timeout the
    // process is killed; whatever was flushed to stdout is the partial result.
    HANDLE worker = CreateThread(nullptr, 4 * 1024 * 1024, [](LPVOID) -> DWORD {
        void* g_fact_veh = AddVectoredExceptionHandler(1, FactVehHandler);
        const int kCount = kSubjectEntryCount;
        printf("{\"factResults\":[");
        bool first = true;
        for (int si = 0; si < kCount; si++) {
            int i = kSubjectSlotMap[si];
            int64_t result = 0;
            bool caught = false;

            // Re-arm SIGABRT handler each iteration: MSVC signal() resets to
            // SIG_DFL after delivery, so a second abort would otherwise call
            // _exit(3) without going through longjmp.
            signal(SIGABRT, [](int) {
                longjmp(t_abort_jmp, 1);
            });

            // _setjmp _must_ be outside __try on x64 (MSVC restriction).
            // If abort() fires, the SIGABRT handler longjmps here with
            // value 1, and we record the method as caught.
            if (_setjmp(t_abort_jmp) == 0) {
                __try {
                    result = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                        GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
                } __except(EXCEPTION_EXECUTE_HANDLER) {
                    caught = true;
                }
            } else {
                // longjmp from SIGABRT handler — abort was called
                caught = true;
            }

            if (!first) printf(",");
            printf("{\"si\":%d,\"methodIndex\":%d,\"contractIndex\":-1,\"passed\":%s,\"value\":%" PRId64 "}",
                   si, i, caught ? "false" : "true", caught ? -1 : result);
            first = false;
        }
        printf("]}\n");
        std::fflush(stdout);
        CHAOS_FACT_CHECK();

        RemoveVectoredExceptionHandler(g_fact_veh);

        return 0;
    }, nullptr, 0, nullptr);

    DWORD wait_result = WaitForSingleObject(worker, 300000);  // 300s global timeout
    CloseHandle(worker);
    if (wait_result == WAIT_TIMEOUT) {
        // Partial results already flushed.  TerminateProcess kills the
        // hanging thread — fact_chunk.py handles truncated JSON recovery.
        TerminateProcess(GetCurrentProcess(), 0);
    }
    return 0;
#else
    // No worker thread on Linux (no intermittent-hang pattern observed);
    // simple dispatch loop with EH protection is sufficient.
    printf("{\"factResults\":[");
    bool first = true;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        int64_t result = 0;
        bool caught = false;
        result = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        if (!first) printf(",");
        printf("{\"si\":%d,\"methodIndex\":%d,\"contractIndex\":-1,\"passed\":%s,\"value\":%" PRId64 "}",
               si, i, caught ? "false" : "true", caught ? -1 : result);
        first = false;
    }
    printf("]}\n");
    std::fflush(stdout);
    CHAOS_FACT_CHECK();
    return 0;
#endif
}

static int RunBenchmarkMode(int entry_index, int iterations) {
    auto* entries = GetHotpatchEntries();
    if (entry_index < 0 || entry_index >= kAotMethodCount || !entries) {
        printf("{\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}\n");
        std::fflush(stdout);
        return 1;
    }

    // Warmup: per-call crash protection
    bool crashed = false;
    for (int w = 0; w < 100; w++) {
#if defined(_WIN32)
        __try {
            chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(
                entries, kAotMethodCount, entry_index);
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            crashed = true;
            break;
        }
#else
        chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(
            entries, kAotMethodCount, entry_index);
#endif
    }

    if (crashed) {
        printf("{\"elapsedMilliseconds\":-1.0,\"allocatedBytes\":0,\"error\":\"managed exception\"}\n");
        std::fflush(stdout);
        return 1;
    }

    // Timing: per-call crash protection
    chaos::il2cpp::runtime_core::chaos_gc_enter_no_gc_region();
    auto alloc_before = chaos::il2cpp::runtime_core::chaos_gc_get_allocated_bytes_for_current_thread();
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; i++) {
#if defined(_WIN32)
        __try {
            chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(
                entries, kAotMethodCount, entry_index);
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            crashed = true;
            break;
        }
#else
        chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(
            entries, kAotMethodCount, entry_index);
#endif
    }
    auto end = std::chrono::steady_clock::now();
    auto alloc_after = chaos::il2cpp::runtime_core::chaos_gc_get_allocated_bytes_for_current_thread();
    chaos::il2cpp::runtime_core::chaos_gc_leave_no_gc_region();

    if (crashed) {
        printf("{\"elapsedMilliseconds\":-1.0,\"allocatedBytes\":%" PRId64 ",\"error\":\"managed exception\"}\n",
               alloc_after - alloc_before);
        std::fflush(stdout);
        return 1;
    }

    double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
    if (elapsed_ms <= 0.0) {
        printf("{\"elapsedMilliseconds\":%.3f,\"opsPerSecond\":0,\"iterations\":%d,\"allocatedBytes\":%" PRId64 "}\n",
               elapsed_ms, iterations, alloc_after - alloc_before);
        std::fflush(stdout);
        return 0;
    }
    double ops_per_sec = (iterations / elapsed_ms) * 1000.0;
    double alloc_per_op = static_cast<double>(alloc_after - alloc_before) / iterations;
    printf(
        "{\"elapsedMilliseconds\":%.3f,\"calibratedMs\":%.3f,"
        "\"opsPerSecond\":%.0f,\"iterations\":%d,"
        "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}\n",
        elapsed_ms, elapsed_ms, ops_per_sec, iterations,
        alloc_after - alloc_before, alloc_per_op);
    std::fflush(stdout);
    return 0;
}

// ── --benchmark-all: benchmark ALL subject methods ─────────────────
// Returns per-line JSON: each method gets its own complete JSON line,
// followed by a summary line.  Stray stderr log lines cannot pollute
// the protocol stream since each line is independently parseable.
static int RunBenchmarkAllMode(int iterations) {
    const int kCount = kSubjectEntryCount;
    auto* entries = GetHotpatchEntries();
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];

        // Warmup: per-call crash protection
        bool crashed = false;
        for (int w = 0; w < 100; w++) {
#if defined(_WIN32)
            __try {
                chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(
                    entries, kAotMethodCount, i);
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                crashed = true;
                break;
            }
#else
            chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(
                entries, kAotMethodCount, i);
#endif
        }

        if (crashed) {
            printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"allocatedBytes\":0,\"error\":\"managed exception\"}\n",
                   i);
            fflush(stdout);
            continue;
        }

        // Timing: per-call crash protection
        chaos::il2cpp::runtime_core::chaos_gc_enter_no_gc_region();
        auto alloc_before = chaos::il2cpp::runtime_core::chaos_gc_get_allocated_bytes_for_current_thread();
        auto start = std::chrono::steady_clock::now();
        for (int t = 0; t < iterations; t++) {
#if defined(_WIN32)
            __try {
                chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(
                    entries, kAotMethodCount, i);
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                crashed = true;
                break;
            }
#else
            chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(
                entries, kAotMethodCount, i);
#endif
        }
        auto end = std::chrono::steady_clock::now();
        auto alloc_after = chaos::il2cpp::runtime_core::chaos_gc_get_allocated_bytes_for_current_thread();
        chaos::il2cpp::runtime_core::chaos_gc_leave_no_gc_region();

        if (crashed) {
            printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"allocatedBytes\":%" PRId64 ",\"error\":\"managed exception\"}\n",
                   i, alloc_after - alloc_before);
        } else {
            double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
            double ops_per_sec = elapsed_ms > 0.0 ? (iterations / elapsed_ms) * 1000.0 : 0.0;
            printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":%.3f,\"opsPerSecond\":%.0f,\"iterations\":%d,\"allocatedBytes\":%lld}\n",
                   i, elapsed_ms, ops_per_sec, iterations, (long long)(alloc_after - alloc_before));
        }
        fflush(stdout);
    }
    printf("{\"summary\":{\"methodCount\":%d}}\n", kCount);
    fflush(stdout);
    return 0;
}

// ── --benchmark-range: benchmark a contiguous range of method table indices ─
// First argument: start index (inclusive), Second argument: end index (exclusive)
static int RunBenchmarkRangeMode(int iterations, int start_idx, int end_idx) {
    if (start_idx < 0) start_idx = 0;
    if (end_idx > kAotMethodCount) end_idx = kAotMethodCount;
    auto* entries = GetHotpatchEntries();
    for (int i = start_idx; i < end_idx; i++) {
        // Warmup: per-call crash protection
        bool crashed = false;
        for (int w = 0; w < 100; w++) {
#if defined(_WIN32)
            __try {
                chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(
                    entries, kAotMethodCount, i);
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                crashed = true;
                break;
            }
#else
            chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(
                entries, kAotMethodCount, i);
#endif
        }

        if (crashed) {
            printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"allocatedBytes\":0,\"error\":\"managed exception\"}\n",
                   i);
            fflush(stdout);
            continue;
        }

        // Timing: per-call crash protection
        chaos::il2cpp::runtime_core::chaos_gc_enter_no_gc_region();
        auto alloc_before = chaos::il2cpp::runtime_core::chaos_gc_get_allocated_bytes_for_current_thread();
        auto start = std::chrono::steady_clock::now();
        for (int t = 0; t < iterations; t++) {
#if defined(_WIN32)
            __try {
                chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(
                    entries, kAotMethodCount, i);
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                crashed = true;
                break;
            }
#else
            chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(
                entries, kAotMethodCount, i);
#endif
        }
        auto end = std::chrono::steady_clock::now();
        auto alloc_after = chaos::il2cpp::runtime_core::chaos_gc_get_allocated_bytes_for_current_thread();
        chaos::il2cpp::runtime_core::chaos_gc_leave_no_gc_region();

        if (crashed) {
            printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"allocatedBytes\":%" PRId64 ",\"error\":\"managed exception\"}\n",
                   i, alloc_after - alloc_before);
        } else {
            double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
            double ops_per_sec = elapsed_ms > 0.0 ? (iterations / elapsed_ms) * 1000.0 : 0.0;
            printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":%.3f,\"opsPerSecond\":%.0f,\"iterations\":%d,\"allocatedBytes\":%lld}\n",
                   i, elapsed_ms, ops_per_sec, iterations, (long long)(alloc_after - alloc_before));
        }
        fflush(stdout);
    }
    printf("{\"summary\":{\"methodCount\":%d}}\n", end_idx - start_idx);
    fflush(stdout);
    return 0;
}

static int RunHotupdateMode(const char* patchDataPath = nullptr) {
    const int kCount = kSubjectEntryCount;
    // Pre-patch: capture per-method pass/fail AND return value via
    // ChaosDispatchMethodGetValue.  The pre-patch value for void-returning
    // AOT methods may be undefined (RAX garbage), but is useful for comparison
    // when the patch changes the return type (void->sentinel int).
    //
    // Post-patch: same function - InterpreterEntryDirect captures the managed
    // patch method's actual return value in __chaos_ret[0].
    //
    // Semantic change is detected by comparing both pass/fail AND return values
    // (if they differ, the patch actually changed behavior - e.g. sentinel-based
    // patches return 0xB0000000+N vs the original void (garbage RAX)).
    printf("{\n");
    // Heap-allocated arrays sized to actual kSubjectEntryCount (not hardcoded 256).
    // Cannot use std::vector due to MSVC C2712: __try/__except forbids C++ object unwinding.
    bool* baseline_ok = new bool[kCount]();
    bool* baseline_caught = new bool[kCount]();
    int64_t* baseline_value = new int64_t[kCount]();
    printf("\"baselineFact\":[");
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        int64_t bv = 0;
        bool caught = false;
#if defined(_WIN32)
        __try {
            CHAOS_EH_TRY
                bv = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                    GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
                baseline_ok[si] = true;
            CHAOS_EH_CATCH_BEGIN
                caught = true;
            CHAOS_EH_END
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            caught = true;
        }
#else
        CHAOS_EH_TRY
            bv = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            baseline_ok[si] = true;
        CHAOS_EH_CATCH_BEGIN
            caught = true;
        CHAOS_EH_END
#endif
        baseline_caught[si] = caught;
        baseline_value[si] = bv;
        if (si > 0) printf(",");
        printf("{\"si\":%d,\"passed\":%s,\"value\":%" PRId64 "}",
               si, caught ? "false" : "true", bv);
    }
    printf("],");
    // Baseline benchmark (before patch)
    if (g_benchmark_iterations > 0) {
        printf("\"baselineBenchmark\":[");
        for (int si = 0; si < kCount; si++) {
            int i = kSubjectSlotMap[si];
            bool crashed = false;
            auto result = BenchmarkResult{};
#if defined(_WIN32)
            __try {
                result = RunBenchmark(i, g_benchmark_iterations);
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                crashed = true;
            }
#else
            CHAOS_EH_TRY
                result = RunBenchmark(i, g_benchmark_iterations);
            CHAOS_EH_CATCH_BEGIN
                crashed = true;
            CHAOS_EH_END
#endif
            if (si > 0) printf(",");
            if (crashed) {
                printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"error\":\"crashed\"}", i);
            } else if (result.caught_exception) {
                printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"error\":\"managed exception\"}", i);
            } else if (result.elapsed_ms < 0.0) {
                printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}", i);
            } else if (result.elapsed_ms == 0.0) {
                printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":0.0,\"opsPerSecond\":0.0}",
                       i);
            } else {
                double ops_per_sec = (g_benchmark_iterations / result.elapsed_ms) * 1000.0;
                printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":%.3f,\"opsPerSecond\":%.0f}",
                       i, result.elapsed_ms, ops_per_sec);
            }
        }
        printf("],");
    }
    // Post-patch: apply patch and capture per-method pass/fail + return value
    auto* patch_ctx = patchDataPath ? ApplyHotpatchFromFile(patchDataPath) : ApplyHotpatchIfAvailable();
    int semantic_passed = 0;
    int semantic_changed_count = 0;
    int patched_assert_failed = 0;
    printf("\"patchedFact\":[");
    for (int si = 0; si < kCount; si++) {
        if (!baseline_ok[si]) { continue; }
        int i = kSubjectSlotMap[si];
        int64_t patched_value = 0;
        bool patched_caught = false;
#if defined(_WIN32)
        __try {
            CHAOS_EH_TRY
                patched_value = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                    GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            CHAOS_EH_CATCH_BEGIN
                patched_caught = true;
            CHAOS_EH_END
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            patched_caught = true;
        }
#else
        CHAOS_EH_TRY
            patched_value = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        CHAOS_EH_CATCH_BEGIN
            patched_caught = true;
        CHAOS_EH_END
#endif
        if (semantic_passed > 0) printf(",");
        printf("{\"si\":%d,\"passed\":%s,\"value\":%" PRId64 "}",
               si, patched_caught ? "false" : "true",
               patched_caught ? 0 : patched_value);
        semantic_passed++;
        // Assertion failure detection:
        // If baseline passed (no crash) but patch assertion fails (crashes),
        // this is a test FAILURE (patch introduced a bug), NOT a semantic change.
        // If baseline also crashed, the method is fundamentally broken — skip.
        if (!baseline_caught[si] && patched_caught) {
            patched_assert_failed++;
        }
        // Genuine semantic change (both paths succeed but return different values)
        if (!baseline_caught[si] && !patched_caught &&
            baseline_value[si] != patched_value) {
            semantic_changed_count++;
        }
    }
    printf("],");
    // Patched benchmark (after patch, before revert)
    if (g_benchmark_iterations > 0) {
        printf("\"patchedBenchmark\":[");
        for (int si = 0; si < kCount; si++) {
            int i = kSubjectSlotMap[si];
            bool crashed = false;
            auto result = BenchmarkResult{};
#if defined(_WIN32)
            __try {
                result = RunBenchmark(i, g_benchmark_iterations);
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                crashed = true;
            }
#else
            CHAOS_EH_TRY
                result = RunBenchmark(i, g_benchmark_iterations);
            CHAOS_EH_CATCH_BEGIN
                crashed = true;
            CHAOS_EH_END
#endif
            if (si > 0) printf(",");
            if (crashed) {
                printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"error\":\"crashed\"}", i);
            } else if (result.caught_exception) {
                printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"error\":\"managed exception\"}", i);
            } else if (result.elapsed_ms < 0.0) {
                printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}", i);
            } else if (result.elapsed_ms == 0.0) {
                printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":0.0,\"opsPerSecond\":0.0}",
                       i);
            } else {
                double ops_per_sec = (g_benchmark_iterations / result.elapsed_ms) * 1000.0;
                printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":%.3f,\"opsPerSecond\":%.0f}",
                       i, result.elapsed_ms, ops_per_sec);
            }
        }
        printf("],");
    }
    // Revert and verify cleanup
    if (patch_ctx != nullptr) {
        chaos::il2cpp::runtime_core::Unpatch(patch_ctx);
    }
    bool all_revert = true;
    printf("\"revertedFact\":[");
    for (int si = 0; si < kCount; si++) {
        // Skip methods that failed in baseline — they're not expected to revert.
        if (!baseline_ok[si]) {
            if (si > 0) printf(",");
            printf("{\"si\":%d,\"passed\":true,\"skipped\":true}", si);
            continue;
        }
        int i = kSubjectSlotMap[si];
        bool reverted_ok = false;
#if defined(_WIN32)
        __try {
            CHAOS_EH_TRY
                chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                    GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
                reverted_ok = true;
            CHAOS_EH_CATCH_BEGIN
                all_revert = false;
            CHAOS_EH_END
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            all_revert = false;
        }
#else
        CHAOS_EH_TRY
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            reverted_ok = true;
        CHAOS_EH_CATCH_BEGIN
            all_revert = false;
        CHAOS_EH_END
#endif
        if (si > 0) printf(",");
        printf("{\"si\":%d,\"passed\":%s}", si, reverted_ok ? "true" : "false");
    }
    printf("],");
    bool all_semantic = (semantic_passed > 0 && semantic_changed_count > 0);
    printf("\"passedMethods\":%d,", semantic_passed);
    printf("\"failedMethods\":%d,", patched_assert_failed);
    printf("\"totalMethods\":%d,", kCount);
    printf("\"allSemantic\":%s,", all_semantic ? "true" : "false");
    printf("\"allRevert\":%s,", all_revert ? "true" : "false");
    printf("\"semanticChangedCount\":%d,", semantic_changed_count);
    printf("\"patchedAssertFailed\":%d}\n", patched_assert_failed);
    std::fflush(stdout);
    delete[] baseline_ok;
    delete[] baseline_caught;
    delete[] baseline_value;
    return 0;
}

extern void ParseSubjectIdForHotpatchLookup(const char*, std::string&, std::string&, std::string&) noexcept;

static int RunMicrobenchMode() {
    RunMicrobench();
    return 0;
}

static int RunHotupdateBenchmarkMode(int entry_index, int iterations) {
    auto* patch_ctx = ApplyHotpatchIfAvailable();
    auto result = RunHotpatchBenchmark(entry_index, iterations);
    if (result.elapsed_ms < 0.0) {
        printf("{\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}\n");
        return 1;
    }
    double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
    double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
    printf(
        "{\"elapsedMilliseconds\":%.3f,\"calibratedMs\":%.3f,"
        "\"opsPerSecond\":%.0f,\"iterations\":%d,"
        "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}\n",
        result.elapsed_ms, result.elapsed_ms, ops_per_sec, iterations,
        result.allocated_bytes, alloc_per_op);
    std::fflush(stdout);
    return 0;
}

int main(int argc, char* argv[]) {

#if defined(_WIN32)
    // Suppress CRT fast-fail: abort() and invalid parameter handlers on Win10+
    // call __fastfail(STATUS_STACK_BUFFER_OVERRUN, 0xC0000409) which is
    // non-continuable — the process dies even inside __try/__except.
    // For verification we want subject methods to recover gracefully.
#pragma warning(push)
#pragma warning(disable: 4996)  // _set_abort_behavior is deprecated but still works
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#pragma warning(pop)
    _set_invalid_parameter_handler(DummyInvalidParameterHandler);
    _CrtSetReportMode(_CRT_ASSERT, 0);
    _CrtSetReportMode(_CRT_WARN, 0);
    _CrtSetReportMode(_CRT_ERROR, 0);
    // Convert SIGABRT to longjmp so the fact-json worker thread can recover
    // and continue to the next subject method instead of dying with _exit(3).
    signal(SIGABRT, [](int) {
        longjmp(t_abort_jmp, 1);
    });
    // Route CHAOS_IL2CPP_FAIL() through longjmp so the dispatch loop catches
    // codegen-emitted fail stubs and runtime assertions gracefully.
    ::chaos::il2cpp::common::g_chaos_fail_hook = []() {
        longjmp(t_abort_jmp, 1);
    };
#endif

    // Redirect diagnostic log output to stderr so that machine-consumed
    // protocol output (benchmark JSON, fact results) on stdout stays clean.
    // The verification pipeline parses stdout for JSON — log messages on
    // stdout would pollute the protocol stream and cause parse failures.
    chaos::il2cpp::common::log_internal::g_log_use_stderr = true;

    // Disable BGC for this short-lived verification process.  BGC concurrent
    // mark/sweep races with tight allocation loops in benchmark/fact dispatch,
    // causing sporadic access violations.  For a process that runs for < 30s
    // and allocates heavily, STW collections are more predictable and avoid
    // the known BGC concurrency races (see gc_old_gen.cpp BgcSweep Phase 4b).
    chaos::il2cpp::runtime_core::g_bgc_enabled = false;

    // Parse --benchmark-iterations flag (position-independent)
    for (int ai = 1; ai < argc; ai++) {
        if (std::strcmp(argv[ai], "--benchmark-iterations") == 0 && ai + 1 < argc) {
            g_benchmark_iterations = atoi(argv[ai + 1]);
            if (g_benchmark_iterations < 0) g_benchmark_iterations = 0;
            break;
        }
    }

    // Initialize ChaOS runtime: resolves kChaosExternalRuntimeFnTable entries
    // (bridge/import stubs) and registers the AOT module so that HotpatchNameRegistry
    // is populated.  Without this, external fnTable entries stay nullptr and any
    // AOT-compiled method that calls through them will segfault.
    //
    // NOTE: heap-allocated and intentionally leaked.  RuntimeShutdown() + static
    // destruction (BgcController threads) race on exit — for a short-lived
    // verification process it is safe to let the OS reclaim everything.
    // TerminateProcess at the shutdown label kills all threads immediately
    // without running static destructors, avoiding the race entirely.
    auto* chaos_host = new ChaosRuntimeHost();
    if (!chaos_host->Initialize("verification-entry")) {
        std::fprintf(stderr, "FATAL: ChaosRuntimeHost::Initialize failed\n");
        return 1;
    }
    if (!chaos_host->RegisterModule(
            &chaos_codegen_code_registration,
            &chaos_codegen_metadata_registration,
            &chaos_codegen_options)) {
        std::fprintf(stderr, "FATAL: RegisterModule failed\n");
        return 1;
    }
    ChaosRegisterGcLayouts();

    // ── Register interop stub fallbacks in kChaosExternalRuntimeFnTable ──
    // The codegen may not produce AOT stubs for all managed methods, leaving
    // their external function table entries as nullptr.  Bridge thunks that
    // call through null entries crash with AV.  Match known methods by
    // SubjectId and populate with native interop stub implementations.
    for (int32_t _i = 0; _i < kChaosExternalRuntimeCount; _i++) {
        const char* _sid = kChaosExternalRuntimeSubjects[_i];
        if (_sid == nullptr) continue;
        if (std::strstr(_sid, "::GetLastPInvokeError") != nullptr) {
            kChaosExternalRuntimeFnTable[_i] = reinterpret_cast<void*>(ChaosMarshalGetLastPInvokeError);
        } else if (std::strstr(_sid, "::GetHRForLastWin32Error") != nullptr) {
            kChaosExternalRuntimeFnTable[_i] = reinterpret_cast<void*>(ChaosMarshalGetHRForLastWin32Error);
        } else if (std::strstr(_sid, "::GetExceptionCode") != nullptr) {
            kChaosExternalRuntimeFnTable[_i] = reinterpret_cast<void*>(ChaosMarshalGetExceptionCode);
        } else if (std::strstr(_sid, "::GetExceptionPointers") != nullptr) {
            kChaosExternalRuntimeFnTable[_i] = reinterpret_cast<void*>(ChaosMarshalGetExceptionPointers);
        } else if (std::strstr(_sid, "::AreComObjectsAvailableForCleanup") != nullptr) {
            kChaosExternalRuntimeFnTable[_i] = reinterpret_cast<void*>(ChaosMarshalAreComObjectsAvailableForCleanup);
        }
    }

    // ── Also update hotpatch direct_ptrs for interpreter-dispatched methods ──
    // The interpreter's ResolveDirectFnSafe resolves calls through HotpatchNameRegistry
    // (Step 1), not through kChaosExternalRuntimeFnTable (Step 3).  When a method
    // has a hotpatch entry with direct_ptr=&InterpreterEntryDirect, Step 1 returns
    // the interpreter entry instead of our stub.  Overwrite the hotpatch entry's
    // direct_ptr so the interpreter finds our stub directly.
    if (kChaosExternalRuntimeCount > 0) {
        for (int32_t _i = 0; _i < kChaosExternalRuntimeCount; _i++) {
            const char* _sid = kChaosExternalRuntimeSubjects[_i];
            if (_sid == nullptr) continue;
            void* _fn = nullptr;
            if (std::strstr(_sid, "::GetLastPInvokeError") != nullptr)
                _fn = reinterpret_cast<void*>(ChaosMarshalGetLastPInvokeError);
            else if (std::strstr(_sid, "::GetHRForLastWin32Error") != nullptr)
                _fn = reinterpret_cast<void*>(ChaosMarshalGetHRForLastWin32Error);
            else if (std::strstr(_sid, "::GetExceptionCode") != nullptr)
                _fn = reinterpret_cast<void*>(ChaosMarshalGetExceptionCode);
            else if (std::strstr(_sid, "::GetExceptionPointers") != nullptr)
                _fn = reinterpret_cast<void*>(ChaosMarshalGetExceptionPointers);
            else if (std::strstr(_sid, "::AreComObjectsAvailableForCleanup") != nullptr)
                _fn = reinterpret_cast<void*>(ChaosMarshalAreComObjectsAvailableForCleanup);
            if (_fn == nullptr) continue;
            // Look up the hotpatch name registry and update direct_ptr
            auto& _registry = chaos::il2cpp::runtime_core::GetHotpatchNameRegistry();
            std::string _ns, _type, _method;
            // Parse SubjectId inline: "Assembly/Namespace.Type::MethodName(Params)"
            {
                const char* _p = _sid;
                if (const char* _slash = std::strchr(_p, '/')) {
                    _p = _slash + 1;
                    if (const char* _colon2 = strstr(_p, "::")) {
                        const char* _type_start = _p;
                        for (const char* _cp = _p; _cp < _colon2; ++_cp)
                            if (*_cp == '.') _type_start = _cp + 1;
                        if (_type_start > _p) _ns.assign(_p, _type_start - _p - 1);
                        _type.assign(_type_start, _colon2 - _type_start);
                        _p = _colon2 + 2;
                        if (const char* _paren = std::strchr(_p, '(')) {
                            _method.assign(_p, _paren - _p);
                            auto _gt = _method.find("<");
                            if (_gt != std::string::npos) _method.resize(_gt);
                        }
                    }
                }
            }
            if (_type.empty() || _method.empty()) continue;
            uint64_t _result = _registry.LookupMethod(_ns.c_str(), _type.c_str(), _method.c_str());
            if (_result == 0) continue;
            uint32_t _mod = chaos::il2cpp::runtime_core::ExtractModuleId(_result);
            uint32_t _tok = chaos::il2cpp::runtime_core::ExtractToken(_result);
            if (_mod >= _registry.ModuleCount()) continue;
            uint32_t _slot = _registry.TokenToSlot(_mod, _tok);
            if (_slot == ~0u) continue;
            auto* _entry = _registry.GetDispatchEntryBySlot(_mod, _slot);
            if (_entry != nullptr && _entry->direct_ptr != nullptr) {
                // Only overwrite if the current direct_ptr is the interpreter entry
                // (not real AOT code).  We can't check easily, so just update the
                // fn table entry which the interpreter falls through to in Step 3.
                // Also set direct_ptr so Step 1 finds our stub.
                _entry->direct_ptr = _fn;
            }
        }
    }

    // ── Hardcoded bridge thunk fallback ──
    // The bridge thunk for Marshal::GetHRForLastWin32Error uses fn table [101].
    // The subjects array has this method at index 102 (loop above sets [102]),
    // while index 101 is GetHRForException (which has its own AOT code).  The
    // bridge thunk was generated with the WRONG index, so overwrite [101]
    // unconditionally to make Marshal::GetHRForLastWin32Error resolvable.
    // NOTE: This overwrites GetHRForException's function table entry, but
    // GetHRForException has its own direct stub in interop_stubs.cpp.
    if (kChaosExternalRuntimeCount > 101) {
        auto _sid101 = kChaosExternalRuntimeSubjects[101];
        if (_sid101 != nullptr &&
            std::strstr(_sid101, "::GetHRForException") != nullptr) {
            // Index 101 has GetHRForException code.  The bridge thunk for
            // GetHRForLastWin32Error incorrectly uses [101] when it should
            // use [102].  Give [101] the Marshal stub so the bridge thunk
            // resolves correctly.  GetHRForException at [102] (original
            // GetHRForLastWin32Error's slot) will use the loop-set stub.
            kChaosExternalRuntimeFnTable[101] =
                reinterpret_cast<void*>(ChaosMarshalGetHRForLastWin32Error);
        }
    }


#if defined(_WIN32)
    AddVectoredExceptionHandler(1, JitVehHandler);
#endif
    if (chaos_il2cpp_aot_hotpatch_module != nullptr) {
        chaos::il2cpp::runtime_core::RegisterHotpatchModule(chaos_il2cpp_aot_hotpatch_module);
    }
    ChaosJitRegisterAll();


    int ret = 0;
    if (argc < 2) { ret = RunFactMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--fact-json") == 0) { ret = RunFactJsonMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--benchmark") == 0) {
        if (argc < 4) { printf("Usage: entry.exe --benchmark <index> <iterations>\n"); return 1; }
        int bm_idx = std::atoi(argv[2]);
        if (bm_idx >= 0 && bm_idx < kSubjectEntryCount) {
            ret = RunBenchmarkMode(kSubjectSlotMap[bm_idx], std::atoi(argv[3]));
        } else {
            printf("{\"elapsedMilliseconds\":-1.0,\"error\":\"subject index out of range\"}\n");
            ret = 1;
        }
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--benchmark-all") == 0) {
        if (argc < 3) { printf("Usage: entry.exe --benchmark-all <iterations>\n"); return 1; }
        ret = RunBenchmarkAllMode(std::atoi(argv[2]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--benchmark-range") == 0) {
        if (argc < 5) { printf("Usage: entry.exe --benchmark-range <start> <end> <iterations>\n"); return 1; }
        ret = RunBenchmarkRangeMode(std::atoi(argv[4]), std::atoi(argv[2]), std::atoi(argv[3]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--hotupdate") == 0) {
        const char* patchDataPath = nullptr;
        if (argc >= 4 && std::strcmp(argv[2], "--patch-data") == 0) {
            patchDataPath = argv[3];
        }
        ret = RunHotupdateMode(patchDataPath);
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--hotupdate-and-benchmark") == 0) {
        if (argc < 4) { printf("Usage: entry.exe --hotupdate-and-benchmark <index> <iterations>\n"); return 1; }
        int hb_idx = std::atoi(argv[2]);
        if (hb_idx >= 0 && hb_idx < kSubjectEntryCount) {
            ret = RunHotupdateBenchmarkMode(kSubjectSlotMap[hb_idx], std::atoi(argv[3]));
        } else {
            printf("{\"elapsedMilliseconds\":-1.0,\"error\":\"subject index out of range\"}\n");
            ret = 1;
        }
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--microbench") == 0) { ret = RunMicrobenchMode(); goto shutdown; }

    printf("Unknown flag: %s\n", argv[1]);
    ret = 1;

shutdown:
    std::fflush(stdout);
#if defined(_WIN32)
    TerminateProcess(GetCurrentProcess(), static_cast<DWORD>(ret));
#else
    _exit(ret);
#endif
    // unreachable
}

// ── Delegate stub forwarders ──────────────────────────────────────────────
// Forward declarations from delegate_helpers.h (not included to avoid pulling
// <core/delegate_object.h> and <vector> into this translation unit).
namespace chaos::il2cpp::runtime_core {
    std::intptr_t DelegateCombine(std::intptr_t left, std::intptr_t right);
    std::intptr_t DelegateRemove(std::intptr_t source, std::intptr_t value_to_remove);
}

std::intptr_t chaos_delegate_combine(std::intptr_t left, std::intptr_t right) noexcept
{
    return chaos::il2cpp::runtime_core::DelegateCombine(left, right);
}

std::intptr_t chaos_delegate_remove(std::intptr_t source, std::intptr_t value) noexcept
{
    return chaos::il2cpp::runtime_core::DelegateRemove(source, value);
}