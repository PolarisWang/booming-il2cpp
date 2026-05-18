
#include <cstdio>
#include <cstdlib>

typedef long long CHAOS_IL2CPP_INTPTR;
typedef unsigned short CHAOS_IL2CPP_UINT16;

struct chaos_managed_exception {
    CHAOS_IL2CPP_INTPTR object_value = 0;
};

static CHAOS_IL2CPP_INTPTR g_exitCode = 0;

// EXACT pattern from generated code
extern "C" void SubjectFunction() {
    CHAOS_IL2CPP_INTPTR _s0{};
    try {
        _s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
        {
            // Direct throw (not comma expression)
            throw chaos_managed_exception{{reinterpret_cast<CHAOS_IL2CPP_INTPTR>(nullptr)}};
            _s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
        }
        _s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
        {
            auto chaos_value = _s0;
            g_exitCode = chaos_value;
        }
    }
    catch (const chaos_managed_exception& chaos_exception) {
        fprintf(stderr, "CATCH_FIRED obj=%lld\n", (long long)chaos_exception.object_value);
        if (chaos_exception.object_value < 0) { throw; }
        _s0 = chaos_exception.object_value;
    }
}

int main() {
    fprintf(stderr, "=== Test Subject EH (full build pattern) ===\n");
    bool caught = false;
    try {
        SubjectFunction();
    }
    catch (const chaos_managed_exception& e) {
        caught = true;
        fprintf(stderr, "OUTER_CAUGHT obj=%lld\n", (long long)e.object_value);
    }
    catch (...) {
        caught = true;
        fprintf(stderr, "OUTER_CAUGHT ellipsis\n");
    }
    if (caught) {
        fprintf(stderr, "FAILED\n");
        return 1;
    }
    fprintf(stderr, "PASSED\n");
    return 0;
}
