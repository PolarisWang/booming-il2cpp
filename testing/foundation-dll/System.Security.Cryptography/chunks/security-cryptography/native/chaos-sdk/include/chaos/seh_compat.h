#ifndef CHAOS_IL2CPP_SEH_COMPAT_H_
#define CHAOS_IL2CPP_SEH_COMPAT_H_

#include <cstdint>

// Cross-platform SEH (Structured Exception Handling) macros.
// On Windows, maps to __try/__except/GetExceptionCode.
// On non-Windows, expands to a no-op that compiles the handler as dead code.

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define CHAOS_SEH_TRY             __try
#define CHAOS_SEH_EXCEPT(e)       __except(e)
#define CHAOS_SEH_GET_CODE()      GetExceptionCode()
#else
// On Linux: CHAOS_SEH_EXCEPT(e) expands to else, making the handler
// unreachable dead code. The filter expression is consumed by the macro
// parameter and not evaluated at runtime.
#define CHAOS_SEH_TRY             if (true)
#define CHAOS_SEH_EXCEPT(e)       else
#define CHAOS_SEH_GET_CODE()      static_cast<uint32_t>(0)
#endif

#endif  // CHAOS_IL2CPP_SEH_COMPAT_H_
