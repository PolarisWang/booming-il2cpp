// pal_eh.h — Structured exception handling abstraction
#pragma once

#include <cstdint>

namespace chaos::il2cpp::pal {

/// Call a native function pointer with SEH protection.
/// On Windows: wraps the call in __try/__except to catch hardware
/// exceptions (access violation, stack overflow, etc.).
/// On POSIX: uses sigsetjmp/siglongjmp + SIGSEGV/SIGBUS handler to
/// catch hardware exceptions (segmentation fault, bus error).
///
/// @returns true if a hardware exception was caught.
/// @param out_result  Receives the function's return value on success.
///                    Undefined if the function threw an SEH exception.
bool PalTryCallNoExcept(uint64_t (*fn)(uint64_t, uint64_t, uint64_t, uint64_t,
                                        uint64_t, uint64_t, uint64_t, uint64_t),
                         uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
                         uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7,
                         uint64_t& out_result) noexcept;

}  // namespace chaos::il2cpp::pal
