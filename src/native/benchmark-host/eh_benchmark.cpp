// ── EH mechanism micro-benchmark ────────────────────────────────────────
//
// Compares C++ throw/catch vs setjmp/longjmp vs Win32 SEH for managed
// exception delivery.  Both paths measured: throw (raise) and happy
// (no exception, enter/leave).
//
// Build:
//   cl /EHsc /O2 /std:c++17 /DCHAOS_IL2CPP_EH_CPP_THROW eh_benchmark.cpp /Fe:eh_cpp_throw.exe
//   cl /EHsc /O2 /std:c++17 /DCHAOS_IL2CPP_EH_SETJMP eh_benchmark.cpp /Fe:eh_setjmp.exe
//   cl /EHca /O2 /std:c++17 /DCHAOS_IL2CPP_EH_WIN32_SEH eh_benchmark.cpp /Fe:eh_win32_seh.exe

#include <chrono>
#include <csetjmp>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#if defined(CHAOS_IL2CPP_EH_WIN32_SEH)
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#endif

// ── Simulated managed exception type ────────────────────────────────────
using CHAOS_IL2CPP_INTPTR = int64_t;

struct chaos_managed_exception {
    CHAOS_IL2CPP_INTPTR object_value;
};

// ── SETJMP mode TLS (simplified) ────────────────────────────────────────
static thread_local jmp_buf g_chaos_exception_jmp_stack[16];
static thread_local int g_chaos_exception_jmp_depth = 0;
static thread_local void* volatile g_chaos_exception_obj = nullptr;

static jmp_buf* push_exception_jmp_buf() noexcept {
    return &g_chaos_exception_jmp_stack[g_chaos_exception_jmp_depth++];
}

static void pop_exception_jmp_buf() noexcept {
    if (g_chaos_exception_jmp_depth > 0) g_chaos_exception_jmp_depth--;
}

// ── Uniform raise API ───────────────────────────────────────────────────
#if defined(CHAOS_IL2CPP_EH_SETJMP)

[[noreturn]] static void chaos_raise_exception(CHAOS_IL2CPP_INTPTR obj) noexcept {
    g_chaos_exception_obj = reinterpret_cast<void*>(obj);
    if (g_chaos_exception_jmp_depth > 0) {
        longjmp(g_chaos_exception_jmp_stack[g_chaos_exception_jmp_depth - 1], 1);
    }
    std::abort();
}

#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)

static constexpr unsigned long kChaosManagedExceptionCode = 0xE0000001;

// GetExceptionInformation() intrinsic is only valid within __except()
// filter expression.  Define a macro so the expansion stays in filter context.
#define CHAOS_SEH_FILTER_ALL_IMPL \
    (GetExceptionCode() == kChaosManagedExceptionCode ? \
        (g_chaos_exception_obj = reinterpret_cast<void*>( \
            GetExceptionInformation()->ExceptionRecord->ExceptionInformation[0]), \
         EXCEPTION_EXECUTE_HANDLER) : \
        EXCEPTION_CONTINUE_SEARCH)

[[noreturn]] static void chaos_raise_exception(CHAOS_IL2CPP_INTPTR obj) noexcept {
    g_chaos_exception_obj = reinterpret_cast<void*>(obj);
    RaiseException(kChaosManagedExceptionCode, 0, 1,
        reinterpret_cast<const ULONG_PTR*>(&obj));
    std::abort();
}

#else  // CPP_THROW

[[noreturn]] static void chaos_raise_exception(CHAOS_IL2CPP_INTPTR obj) {
    throw chaos_managed_exception{obj};
}

#endif

// ── Benchmark helpers ───────────────────────────────────────────────────
struct Timer {
    using Clock = std::chrono::steady_clock;
    Clock::time_point start;

    Timer() : start(Clock::now()) {}

    double elapsed_ns() const {
        auto end = Clock::now();
        return static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }
};

static int64_t bench_throw_catch(int iterations) {
    int64_t checksum = 0;
    for (int i = 0; i < iterations; i++) {
#if defined(CHAOS_IL2CPP_EH_WIN32_SEH)
        __try {
            chaos_raise_exception(static_cast<CHAOS_IL2CPP_INTPTR>(i));
        } __except(CHAOS_SEH_FILTER_ALL_IMPL) {
            checksum += static_cast<int64_t>(
                reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_chaos_exception_obj));
        }
#elif defined(CHAOS_IL2CPP_EH_CPP_THROW)
        try {
            chaos_raise_exception(static_cast<CHAOS_IL2CPP_INTPTR>(i));
        } catch (const chaos_managed_exception& e) {
            checksum += e.object_value;
        }
#else
        {
            auto* jmp = push_exception_jmp_buf();
            if (setjmp(*jmp) == 0) {
                chaos_raise_exception(static_cast<CHAOS_IL2CPP_INTPTR>(i));
            } else {
                checksum += static_cast<int64_t>(
                    reinterpret_cast<CHAOS_IL2CPP_INTPTR>(g_chaos_exception_obj));
            }
            pop_exception_jmp_buf();
        }
#endif
    }
    return checksum;
}

static int64_t bench_happy_path(int iterations) {
    int64_t checksum = 0;
    for (int i = 0; i < iterations; i++) {
#if defined(CHAOS_IL2CPP_EH_WIN32_SEH)
        __try {
            checksum += i;
        } __finally {
        }
#elif defined(CHAOS_IL2CPP_EH_CPP_THROW)
        try {
            checksum += i;
        } catch (const chaos_managed_exception&) {
            checksum += -1;
        }
#else
        {
            auto* jmp = push_exception_jmp_buf();
            if (setjmp(*jmp) == 0) {
                checksum += i;
            } else {
                checksum += -1;
            }
            pop_exception_jmp_buf();
        }
#endif
    }
    return checksum;
}

// ── Main ────────────────────────────────────────────────────────────────
int main() {
    constexpr int WARMUP = 10'000;
    constexpr int THROW_ITER = 100'000;
    constexpr int HAPPY_ITER = 1'000'000;

    // Warmup
    bench_throw_catch(WARMUP);
    bench_happy_path(WARMUP);

    // Throw/catch benchmark
    {
        Timer t;
        int64_t cs = bench_throw_catch(THROW_ITER);
        double ns = t.elapsed_ns();
        double per_op = ns / THROW_ITER;
        printf("throw/catch:  %10d ops  %10.0f ns total  %8.1f ns/op  (checksum=%lld)\n",
               THROW_ITER, ns, per_op, (long long)cs);
    }

    // Happy path benchmark (no exception, just try/enter)
    {
        Timer t;
        int64_t cs = bench_happy_path(HAPPY_ITER);
        double ns = t.elapsed_ns();
        double per_op = ns / HAPPY_ITER;
        printf("happy path:   %10d ops  %10.0f ns total  %8.1f ns/op  (checksum=%lld)\n",
               HAPPY_ITER, ns, per_op, (long long)cs);
    }

#if defined(CHAOS_IL2CPP_EH_SETJMP)
    printf("Mode: SETJMP\n");
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
    printf("Mode: WIN32_SEH\n");
#else
    printf("Mode: CPP_THROW\n");
#endif

    return 0;
}