// marshal_test_stubs.cpp — Stub definitions for P/Invoke & Marshal test targets.
//
// Provides extern symbols that are normally emitted by AOT codegen but are
// not available in the test environment.
//
// BSTR thunks (SysAllocString/SysFreeString/SysStringLen): On Win32 these
// resolve oleaut32.dll at runtime via LoadLibrary/GetProcAddress inside
// marshal_api.cpp, so no test stubs are needed on Win32. On non-Win32
// the BSTR functions are no-ops and return 0, matching production behavior.

#include <cstdint>
#include <cstddef>

extern "C" {

// chaos_il2cpp_aot_hotpatch_module — normally provided by codegen output.
// Marshal tests don't use hotpatch, so null is safe.
const void* chaos_il2cpp_aot_hotpatch_module = nullptr;

// External runtime dispatch table — normally provided by codegen output.
const char* kChaosExternalRuntimeSubjects[] = { nullptr };
void*       kChaosExternalRuntimeFnTable[]  = { nullptr };
int32_t     kChaosExternalRuntimeCount = 0;

// ChaosJitRegisterAll — normally provided by codegen output (no-op in AOT mode).
void ChaosJitRegisterAll() {}

}  // extern "C"
