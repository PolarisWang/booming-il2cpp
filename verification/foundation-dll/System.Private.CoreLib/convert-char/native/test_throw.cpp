// Test: does MSVC elide try/catch with unconditional throw + no-op catch?
#include <cstdio>
#include <cstdlib>

// Exact types from the project
typedef long long CHAOS_IL2CPP_INTPTR;
typedef unsigned short CHAOS_IL2CPP_UINT16;

struct chaos_managed_exception {
    CHAOS_IL2CPP_INTPTR object_value = 0;
};

static CHAOS_IL2CPP_INTPTR chaos_static_exitCode = 0;

extern "C" void TestSubject0() {
    CHAOS_IL2CPP_INTPTR _s0{};

    try
    {
        _s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
        {
            const auto chaos_inline_result = (throw chaos_managed_exception{reinterpret_cast<CHAOS_IL2CPP_INTPTR>(nullptr)}, static_cast<CHAOS_IL2CPP_UINT16>(0));
            _s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_inline_result);
        }
        _s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
        {
            auto chaos_value = _s0;
            chaos_static_exitCode = chaos_value;
        }
    }
    catch (const chaos_managed_exception& chaos_exception)
    {
        if (chaos_exception.object_value < 0) { throw; }
        // NO fprintf, no side effect - EXACTLY like the generated code
        _s0 = chaos_exception.object_value;
    }
}

// Call through function pointers like the generated code does
static void (*kAotMethods[1])() = {
    &TestSubject0
};

static void (*kBenchmarkWrappers[1])() = {
    []() { kAotMethods[0](); }
};

extern "C" void RunNativeAot(int i) {
    kBenchmarkWrappers[i]();
}

int main() {
    fprintf(stderr, "=== Starting exact pattern test ===\n");
    bool caught = false;
    try {
        RunNativeAot(0);
    }
    catch (const chaos_managed_exception& e) {
        caught = true;
        fprintf(stderr, "OUTER_CATCH obj=%lld\n", (long long)e.object_value);
    }
    catch (...) {
        caught = true;
        fprintf(stderr, "OUTER_CATCH_UNKNOWN\n");
    }
    if (caught) {
        fprintf(stderr, "TEST FAILED: exception propagated out\n");
        return 1;
    }
    fprintf(stderr, "TEST PASSED\n");
    return 0;
}
