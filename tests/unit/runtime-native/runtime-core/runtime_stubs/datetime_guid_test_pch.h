// datetime_guid_test_pch.h — Forced include (/FI) for datetime/guid direct compilation
//
// datetime_stubs.cpp and guid_stubs.cpp include Windows.h before the include
// chain reaches thread_state.h.  The Windows SDK's processthreadsapi.h declares
// GetCurrentThreadId/GetCurrentThread as extern "C" at global scope, which
// prevents thread_state.h from declaring functions with the same names inside
// the chaos::il2cpp::runtime_core::threading namespace (MSVC limitation with
// extern "C" names).
//
// Solution: rename the Windows API declarations away before Windows.h sees them,
// then undefine so thread_state.h's namespace-scoped names compile normally.
#ifdef _MSC_VER
#pragma push_macro("GetCurrentThreadId")
#pragma push_macro("GetCurrentThread")
#endif

// These macros rename the Windows SDK declarations before processthreadsapi.h
// is included.  The actual API is never called in our test context.
#define GetCurrentThreadId __ChaosTest_GetCurrentThreadId
#define GetCurrentThread __ChaosTest_GetCurrentThread

#include <Windows.h>

#ifdef _MSC_VER
#pragma pop_macro("GetCurrentThreadId")
#pragma pop_macro("GetCurrentThread")
#endif
