#ifndef CHAOS_IL2CPP_TOKEN_RESOLVER_H_
#define CHAOS_IL2CPP_TOKEN_RESOLVER_H_

#include "il_to_ir_lowerer.h"   // ILTokenResolver, IRInstruction
#include "codegen_bridge.h"     // CodegenBridgeV0, ImageHandle

#include <chaos/native_types.h>

namespace chaos::il2cpp::layout {
class LayoutEngine;  // forward decl — used as pointer in TokenResolverContext
}

namespace chaos::il2cpp::interpreter {

/// Context passed through user_data to the default token resolver.
///
/// Populated by the caller (e.g. the interpreter dispatch or method
/// instantiation path) before calling LowerILToIR / LowerMethodBody.
struct TokenResolverContext {
    /// Process-wide codegen bridge (used for standard token→handle resolution).
    const CodegenBridgeV0* bridge = nullptr;

    /// Source image owning every token in the IL byte stream.
    ImageHandle source_image = 0u;

    /// Generic type arguments for parameter substitution.
    /// Set when lowering the body of a closed generic method.
    const TypeInfoHandle* type_args = nullptr;
    CHAOS_IL2CPP_UINT32  arg_count = 0u;

    /// Optional: layout engine for field offset resolution (LdFld/StFld).
    /// When non-null, the resolver computes real byte offsets for field
    /// access instructions.  May be nullptr during early bring-up.
    chaos::il2cpp::layout::LayoutEngine* layout_engine = nullptr;
};

/// Default token resolver callback.
///
/// Dispatches on instruction.op_code to resolve metadata tokens via the
/// CodegenBridgeV0 functions:
///   - Call / NewObj   → resolve_method_by_token → instruction.call_target
///   - Box / CastClass / IsInst → resolve_type_by_token → instruction.call_target
///   - LdFld / StFld / LdSFld / StSFld → resolve_field_by_token → instruction.call_target
///
/// user_data must be a non-null TokenResolverContext*.
///
/// Returns true on success.  On failure (null context, bridge, or resolution
/// failure) the lowerer aborts and returns an empty IRMethod.
bool CHAOS_RUNTIME_ABI_CALL DefaultTokenResolver(
    CHAOS_IL2CPP_UINT32  token,
    IRInstruction&        instruction,
    void*                 user_data);

}  // namespace chaos::il2cpp::interpreter

#endif  // CHAOS_IL2CPP_TOKEN_RESOLVER_H_
