// interpreter_entry_direct.cpp — extern "C" wrapper for InterpreterEntryDirect
//
// The generated code references &InterpreterEntryDirect with C linkage for
// dispatch table entries. The real implementation lives in interpreter_entry.cpp
// (within chaos::il2cpp::runtime_core namespace). This wrapper provides the
// C-linkage symbol for verification builds.
//
// Also wraps the call in SEH __try/__except to catch STATUS_STACK_OVERFLOW
// from deeply nested IL methods (e.g. Convert, Half, Double in system-5)
// executing via the interpreter during hotpatch mode.  Without this, a
// StackOverflow kills the whole process with no recovery.

#include <cstdint>
#include <cstdio>
#include "interpreter_entry.h"
#include <chaos/eh.h>

extern "C" void InterpreterEntryDirect(
    uintptr_t method_key,
    void*     args_buf,
    void*     ret_buf)
{
    __try
    {
        chaos::il2cpp::runtime_core::InterpreterEntryDirect(method_key, args_buf, ret_buf);
    }
    __except (CHAOS_SEH_FILTER_ALL)
    {
        // Stack overflow during hotpatch interpreter dispatch.  Log and return
        // — the caller (RunHotpatchMode) will detect the failed fact/benchmark
        // result and report it as a crash rather than crashing the whole process.
        std::fprintf(stderr, "[InterpreterEntryDirect] SEH caught: method_key=0x%llx\n",
                     (unsigned long long)method_key);
    }
}