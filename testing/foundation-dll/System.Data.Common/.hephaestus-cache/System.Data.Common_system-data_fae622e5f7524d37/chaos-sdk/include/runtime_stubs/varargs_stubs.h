// ── Varargs P/Invoke stub declarations ──────────────────────────
#pragma once

// V1 Note: Varargs P/Invoke dispatch is handled entirely by the codegen
// (EmitVarargsPInvokeMethod / EmitVarargsPInvokeCall), which generates
// the wrapper function and switch-based dispatch inline.
//
// The codegen wrapper signature:
//   extern "C" RetType WrapperSymbol(
//       fixed_params...,                              // fixed ABI params (excl. sentinel)
//       CHAOS_IL2CPP_INTPTR* chaos_varargs_buf,       // variadic arg buffer
//       uint32_t chaos_varargs_count);                // number of variadic args
//
// Inside the wrapper, a switch on chaos_varargs_count (0-16) calls the
// native function as a pure-variadic FnPtr: RetType(ccAnnotation*)(...).
// Each variadic arg is cast to CHAOS_IL2CPP_UINT64 before forwarding.
//
// Constraints (V1):
//   - Integral/pointer variadic types only (no float/double shadowing).
//   - Max 16 variadic args.
//   - x64 Windows ABI: all integral variadic args are 8-byte values.
//
// Future expansion: this header declares the runtime helpers that
// would be called from the codegen for complex varargs marshalling
// (e.g., float/double XMM register shadowing on x64 Windows, or
//  ARM64 varargs ABI). For V1, no runtime functions are needed.

