#ifndef CHAOS_IL2CPP_IL_TO_IR_LOWERER_H_
#define CHAOS_IL2CPP_IL_TO_IR_LOWERER_H_

#include "interpreter_vm.h"

#include <chaos/native_types.h>

namespace chaos::il2cpp::interpreter {

// ── Token resolver ───────────────────────────────────────────────────────────
//
// Called by the lowerer for each metadata token encountered in the IL stream
// (call, newobj, box, ldfld, stfld, etc.).
//
// `token`       — 4-byte metadata token from the IL bytecode
// `instruction` — the IRInstruction being built; the resolver should set
//                 the appropriate fields (call_target, field_offset, etc.)
// `user_data`   — opaque pointer passed through from the LowerILToIR caller
//
// Returns true on success.  If the resolver returns false, the lowerer
// aborts and returns an empty IRMethod containing only a Ret instruction.

using ILTokenResolver = bool (*)(
    CHAOS_IL2CPP_UINT32 token,
    IRInstruction& instruction,
    void* user_data);

// ── Method body header ───────────────────────────────────────────────────────
//
// Parsed ECMA 335 method body header (tiny or fat format, II.15.4.2).
//
// Tiny format (1-byte header):    bits[1:0] = 0x02, bits[7:2] = code_size
// Fat format  (12-byte header):   bits[1:0] = 0x03, full flags/stack/size/sig

struct MethodBodyHeader {
    const uint8_t*        code_start;           // pointer to first byte of IL code
    CHAOS_IL2CPP_SIZE     code_size;            // code body size in bytes
    CHAOS_IL2CPP_UINT16   max_stack;            // max stack depth (8 for tiny)
    CHAOS_IL2CPP_UINT32   local_var_sig_tok;    // local var sig token (0 for tiny)
    bool                  has_more_sects;       // SEH sections follow the code
};

/// Parse a method body header from raw bytes.
///
/// Detects tiny format (1-byte header, max 63 bytes code) and fat format
/// (12-byte header).  Returns false if the bytes don't match a known header
/// format or the buffer is too short.
///
/// \param body_bytes  The raw method body (first byte = flags/header)
/// \param body_length Total buffer length
/// \param out         Receives parsed header fields
/// \return true on successful parse
bool ParseMethodBodyHeader(
    const void*          body_bytes,
    CHAOS_IL2CPP_SIZE    body_length,
    MethodBodyHeader&    out);

// ── IL→IR Lowering ─────────────────────────────────────────────────────────

/// Lower a buffer of .NET IL bytecodes (ECMA 335) into an IRMethod.
///
/// `il_bytes`         — pointer to the start of the IL code (after header)
/// `il_length`        — total buffer size from il_bytes (code + padding + SEH)
/// `code_size`        — code body size from the method header; used to locate
///                      SEH sections (padding = align4(code_size)).
///                      Pass 0 for backward compat (uses il_length for padding).
/// `max_stack`        — declared max stack depth from the method header
/// `token_resolver`   — callback that resolves metadata tokens
/// `user_data`        — passed through to the token_resolver
///
/// Supported IL opcodes (subset):
///   nop, ldarg(.0-.3/.s), ldloc(.0-.3/.s), stloc(.0-.3/.s),
///   ldc.i4(.0-.8/.s), ldc.i8, ldc.r4, ldc.r8, ldnull,
///   ldstr, pop,
///   add, sub, mul, div, rem, neg, ceq, cgt, clt,
///   conv.i4, conv.i8, conv.r4, conv.r8,
///   call, ret,
///   br(.s), brtrue(.s), brfalse(.s),
///   beq(.s), blt(.s), bgt(.s), ble(.s), bge(.s),
///   newobj, box, unbox,
///   castclass, isinst,
///   ldfld, stfld, ldsfld, stsfld,
///   newarr, ldlen, ldelem.*, stelem.*,
///   leave(.s), endfinally,
///   throw, rethrow
///
/// Returns an IRMethod.  On failure (unrecognised opcode, token resolution
/// failure, etc.) the returned method contains a single Ret instruction.
IRMethod LowerILToIR(
    const void*          il_bytes,
    CHAOS_IL2CPP_SIZE    il_length,
    CHAOS_IL2CPP_SIZE    code_size,
    CHAOS_IL2CPP_UINT32  max_stack,
    ILTokenResolver      token_resolver,
    void*                user_data);

}  // namespace chaos::il2cpp::interpreter

#endif  // CHAOS_IL2CPP_IL_TO_IR_LOWERER_H_
