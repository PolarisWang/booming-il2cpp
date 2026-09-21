// WaitHandle static-overload argument validation contracts.
//
// The managed side is generated from subjects whose probes call
// `WaitHandle.WaitAll/WaitAny/SignalAndWait` with deliberately invalid input
// (null array, empty array, null element).  Real .NET raises before any wait
// happens, so the AOT helper only has to reproduce the VALIDATION, not the
// blocking behaviour.  These tests pin that contract.
//
// Expected values were measured against .NET 8 AND net10 (identical on both)
// with a standalone program — see
// docs/dev/in-progress/threading-tasks-hang/waithandle-contracts.md.
//
// Exception detection: RaiseManagedException() aborts when the runtime is not
// initialised, which is the case for a bare contract test.  SetExceptionFallback
// installs a longjmp target instead, so a raise is observable as "a managed
// exception was requested" rather than as process death.
//
// SCOPE OF THESE TESTS — what they do and do NOT prove
// ---------------------------------------------------
// The fallback is `void(void)`: it carries NO exception type, and the type
// cannot be recovered from the callback.  These tests therefore pin the
// VALIDATION CONTROL FLOW — which inputs raise and which pass — and not the
// managed type name of the raise.  Type correctness is asserted end-to-end by
// the threading chunk's fact run, where the subject's Assert.Throws matches the
// real exception class (see waithandle-contracts.md for the measured table).
//
// Stating that boundary is deliberate: a test named
// `..._is_argument_null` that only proves "something was raised" would read as
// a type assertion it does not actually make.
#include <cstdio>
#include <cstring>
#include <setjmp.h>

#include <chaos/native_types.h>

extern "C" {
CHAOS_IL2CPP_INT32 chaos_wait_handle_validate(CHAOS_IL2CPP_INTPTR wait_handles) noexcept;
CHAOS_IL2CPP_INT32 chaos_wait_handle_validate_pair(
    CHAOS_IL2CPP_INTPTR to_signal,
    CHAOS_IL2CPP_INTPTR to_wait_on) noexcept;
void SetExceptionFallback(void (*fn)());
}

// ── Minimal managed-array image ────────────────────────────────────────
//
// Mirrors ManagedArrayAccessor in stub_common.h: 8B header data,
// element_type_shape(1) + padding(7), element_type_info(8), length(8),
// then contiguous element storage.  Allocated as one block so the elements sit
// immediately after the header, exactly as the accessor expects.
namespace {

constexpr size_t kHeaderBytes = 32;

struct ArrayImage {
    unsigned char bytes[kHeaderBytes + 64 * sizeof(void*)];

    void set_length(long long n) {
        std::memcpy(bytes + 24, &n, sizeof(n));
    }
    void set_element(size_t i, CHAOS_IL2CPP_INTPTR v) {
        std::memcpy(bytes + kHeaderBytes + i * sizeof(void*), &v, sizeof(v));
    }
    CHAOS_IL2CPP_INTPTR handle() {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(bytes);
    }
};

jmp_buf g_raise_jmp;
bool g_raised = false;

// The fallback must not return (the exception machinery assumes a transfer of
// control).  longjmp back to the test body.
void raise_capture_fallback() {
    g_raised = true;
    longjmp(g_raise_jmp, 1);
}

/// Run `fn`; return true when it raises.
bool expect_raise(void (*fn)(), const char* what) {
    g_raised = false;
    if (setjmp(g_raise_jmp) == 0) {
        fn();
        std::fprintf(stderr, "FAIL: expected a raise for %s, none happened\n", what);
        return false;
    }
    if (!g_raised) {
        std::fprintf(stderr, "FAIL: longjmp without raise for %s\n", what);
        return false;
    }
    return true;
}

/// Run `fn`; return true when it does NOT raise.
bool expect_no_raise(void (*fn)(), const char* what) {
    g_raised = false;
    if (setjmp(g_raise_jmp) == 0) {
        fn();
        return true;
    }
    std::fprintf(stderr, "FAIL: unexpected raise for %s\n", what);
    return false;
}

// Trampolines — longjmp needs a function pointer, and the payload lives in
// statics to keep the signature simple.
CHAOS_IL2CPP_INTPTR g_arr = 0;
CHAOS_IL2CPP_INTPTR g_pair_a = 0;
CHAOS_IL2CPP_INTPTR g_pair_b = 0;

void call_validate()          { (void)chaos_wait_handle_validate(g_arr); }
void call_validate_pair()     { (void)chaos_wait_handle_validate_pair(g_pair_a, g_pair_b); }

}  // namespace

// ── Tests ─────────────────────────────────────────────────────────────
//
// Names describe the INPUT SHAPE; the raise-vs-pass distinction is what is
// asserted (see "SCOPE OF THESE TESTS" above).

/// null array must raise (real .NET: ArgumentNullException).
static bool test_null_array_raises() {
    g_arr = 0;
    return expect_raise(&call_validate, "null array");
}

/// empty array must raise (real .NET: ArgumentException).
static bool test_empty_array_raises() {
    ArrayImage img{};
    img.set_length(0);
    g_arr = img.handle();
    return expect_raise(&call_validate, "empty array");
}

/// An array holding a null element must raise (real .NET: ArgumentNullException,
/// measured with `WaitAll(new WaitHandle[]{ ev, null! }, 0)`).
static bool test_null_element_raises() {
    static ArrayImage img;
    img.set_length(2);
    img.set_element(0, 0x1000);   // any non-null placeholder
    img.set_element(1, 0);        // the null element
    g_arr = img.handle();
    return expect_raise(&call_validate, "array with a null element");
}

/// A well-formed array passes validation and reports "validated", not "waited".
static bool test_valid_array_passes() {
    static ArrayImage img;
    img.set_length(2);
    img.set_element(0, 0x1000);
    img.set_element(1, 0x2000);
    g_arr = img.handle();

    if (!expect_no_raise(&call_validate, "valid array")) return false;
    const CHAOS_IL2CPP_INT32 rc = chaos_wait_handle_validate(g_arr);
    if (rc != 0) {
        std::fprintf(stderr, "FAIL: valid array returned %d, expected 0\n",
                     static_cast<int>(rc));
        return false;
    }
    return true;
}

/// SignalAndWait: null toSignal must raise.
static bool test_pair_null_first_raises() {
    g_pair_a = 0;
    g_pair_b = 0x1000;
    return expect_raise(&call_validate_pair, "null toSignal");
}

/// SignalAndWait: null toWaitOn must raise.
static bool test_pair_null_second_raises() {
    g_pair_a = 0x1000;
    g_pair_b = 0;
    return expect_raise(&call_validate_pair, "null toWaitOn");
}

/// SignalAndWait with both handles present passes validation.
static bool test_pair_valid_passes() {
    g_pair_a = 0x1000;
    g_pair_b = 0x2000;
    if (!expect_no_raise(&call_validate_pair, "valid pair")) return false;
    const CHAOS_IL2CPP_INT32 rc = chaos_wait_handle_validate_pair(g_pair_a, g_pair_b);
    if (rc != 0) {
        std::fprintf(stderr, "FAIL: valid pair returned %d, expected 0\n",
                     static_cast<int>(rc));
        return false;
    }
    return true;
}

// ── Runner ────────────────────────────────────────────────────────────

int main() {
    SetExceptionFallback(&raise_capture_fallback);

    struct Case { const char* name; bool (*fn)(); };
    const Case cases[] = {
        {"null_array_raises",       &test_null_array_raises},
        {"empty_array_raises",      &test_empty_array_raises},
        {"null_element_raises",     &test_null_element_raises},
        {"valid_array_passes",      &test_valid_array_passes},
        {"pair_null_first_raises",  &test_pair_null_first_raises},
        {"pair_null_second_raises", &test_pair_null_second_raises},
        {"pair_valid_passes",       &test_pair_valid_passes},
    };

    int failed = 0;
    for (const auto& c : cases) {
        const bool ok = c.fn();
        std::printf("%s %s\n", ok ? "PASS" : "FAIL", c.name);
        if (!ok) ++failed;
    }

    std::printf("%zu cases, %d failed\n", sizeof(cases) / sizeof(cases[0]), failed);
    return failed == 0 ? 0 : 1;
}
