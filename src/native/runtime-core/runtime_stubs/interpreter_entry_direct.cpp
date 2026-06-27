// interpreter_entry_direct.cpp — extern "C" wrapper for InterpreterEntryDirect
//
// The generated code references &InterpreterEntryDirect with C linkage for
// dispatch table entries. The real implementation lives in interpreter_entry.cpp
// (within chaos::il2cpp::runtime_core namespace). This wrapper provides the
// C-linkage symbol for verification builds.
//
// SEH protection: __try/__except catches STATUS_STACK_OVERFLOW from deeply
// nested IL methods so the process can log and degrade gracefully instead of
// crashing immediately.

#include <cstdint>
#include <cstdio>
#include "interpreter_entry.h"

extern "C" void InterpreterEntryDirect(
    uintptr_t method_key,
    void*     args_buf,
    void*     ret_buf)
{
#if defined(_MSC_VER)
    __try
    {
        chaos::il2cpp::runtime_core::InterpreterEntryDirect(method_key, args_buf, ret_buf);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        std::fprintf(stderr, "[SEH] InterpreterEntryDirect: stack overflow or access violation caught, method_key=%p\n",
            (void*)method_key);
        std::fflush(stderr);
    }
#else
    chaos::il2cpp::runtime_core::InterpreterEntryDirect(method_key, args_buf, ret_buf);
#endif
}