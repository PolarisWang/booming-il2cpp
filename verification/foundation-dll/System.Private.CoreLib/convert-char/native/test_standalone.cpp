// Standalone Subject_0 test - C++ EH path only
#include <cstdio>

typedef long long CHAOS_IL2CPP_INTPTR;
typedef unsigned int CHAOS_IL2CPP_UINT32;
typedef unsigned short CHAOS_IL2CPP_UINT16;
typedef unsigned long long CHAOS_IL2CPP_UINT64;

#define CHAOS_IL2CPP_ARRAY(T, N)

struct ThinLockableHeader { int dummy; };

struct chaos_managed_exception {
    CHAOS_IL2CPP_INTPTR object_value = 0;
};

static CHAOS_IL2CPP_INTPTR chaos_static_ConvertCharSubjects_ConvertCharSubjects___exitCode = 0;

extern "C" void ConvertCharSubjects_ConvertCharSubjects_Subject_0(void)
{
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
            chaos_static_ConvertCharSubjects_ConvertCharSubjects___exitCode = chaos_value;
        }
    }
    catch (const chaos_managed_exception& chaos_exception)
    {
        if (chaos_exception.object_value < 0) { throw; }
        auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
        if (chaos_header != nullptr)
        {
            // Type check skipped - no type info available in standalone
        }
        _s0 = chaos_exception.object_value;
    }
    return;
}

int main() {
    fprintf(stderr, "=== Standalone Subject_0 test ===\n");
    bool caught = false;
    try {
        ConvertCharSubjects_ConvertCharSubjects_Subject_0();
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
