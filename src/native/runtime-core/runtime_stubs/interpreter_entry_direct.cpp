// interpreter_entry_direct.cpp — extern "C" wrapper for InterpreterEntryDirect
//
// The generated code references &InterpreterEntryDirect with C linkage for
// dispatch table entries. The real implementation lives in interpreter_entry.cpp
// (within chaos::il2cpp::runtime_core namespace). This wrapper provides the
// C-linkage symbol for verification builds.

#include <cstdint>
#include "interpreter_entry.h"

extern "C" void InterpreterEntryDirect(
    uintptr_t method_key,
    void*     args_buf,
    void*     ret_buf) noexcept
{
    chaos::il2cpp::runtime_core::InterpreterEntryDirect(method_key, args_buf, ret_buf);
}