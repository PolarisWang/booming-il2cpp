#include "token_resolver.h"
#include "reflection_query_model.h"
#include "layout_engine.h"

#include <chaos/native_types.h>

namespace chaos::il2cpp::interpreter {

namespace {

using namespace chaos::il2cpp::runtime_core;

/// Scan an image's types to find the declaring type that contains a field
/// with the given metadata token.  Also returns the field's index within
/// the type's field array.
///
/// Returns nullptr if no type owns the field token.
static const runtime_core::ReflectionQueryTypeDescriptor*
FindDeclaringTypeByFieldToken(
    const runtime_core::ReflectionQueryImageDescriptor* image,
    CHAOS_IL2CPP_UINT32 field_token,
    CHAOS_IL2CPP_UINT32* out_field_index)
{
    if (image == nullptr || image->types == nullptr) {
        return nullptr;
    }
    for (CHAOS_IL2CPP_UINT32 ti = 0u; ti < image->type_count; ++ti) {
        const auto* type = image->types[ti];
        if (type == nullptr || type->fields == nullptr) {
            continue;
        }
        for (CHAOS_IL2CPP_UINT32 fi = 0u; fi < type->field_count; ++fi) {
            if (type->fields[fi].metadata_token == field_token) {
                if (out_field_index != nullptr) {
                    *out_field_index = fi;
                }
                return type;
            }
        }
    }
    return nullptr;
}

}  // anonymous namespace

// ════════════════════════════════════════════════════════════════════════════
// Default token resolver callback
// ════════════════════════════════════════════════════════════════════════════

bool CHAOS_RUNTIME_ABI_CALL DefaultTokenResolver(
    CHAOS_IL2CPP_UINT32  token,
    IRInstruction&        instruction,
    void*                 user_data)
{
    if (user_data == nullptr) {
        return false;
    }

    const auto* ctx = static_cast<const TokenResolverContext*>(user_data);

    // ── Generic parameter token resolution (ELEMENT_TYPE_VAR / ELEMENT_TYPE_MVAR) ──
    // ECMA 335 II.23.1.16: ELEMENT_TYPE_VAR = 0x11, ELEMENT_TYPE_MVAR = 0x12
    // Encoded as metadata tokens: 0x11xxxxxx = class type param, 0x12xxxxxx = method type param
    // These appear in generic method IL bodies where type parameters (!0, !!1, etc.)
    // are referenced (e.g., Box !0, CastClass !!1).
    // We resolve them against the TokenResolverContext's type_args / method_type_args.
    if ((token & 0xFF000000u) == 0x11000000u) {
        // ELEMENT_TYPE_VAR — class-level type parameter (!N)
        const CHAOS_IL2CPP_UINT32 index = token & 0x00FFFFFFu;
        if (ctx->type_args != nullptr && index < ctx->arg_count) {
            instruction.call_target = reinterpret_cast<void*>(
                static_cast<CHAOS_IL2CPP_UINTPTR>(ctx->type_args[index]));
            return true;
        }
        return false;  // index out of range or no type_args
    }
    if ((token & 0xFF000000u) == 0x12000000u) {
        // ELEMENT_TYPE_MVAR — method-level type parameter (!!N)
        const CHAOS_IL2CPP_UINT32 index = token & 0x00FFFFFFu;
        if (ctx->method_type_args != nullptr && index < ctx->method_arg_count) {
            instruction.call_target = reinterpret_cast<void*>(
                static_cast<CHAOS_IL2CPP_UINTPTR>(ctx->method_type_args[index]));
            return true;
        }
        return false;  // index out of range or no method_type_args
    }

    // ── Standard metadata token resolution (requires bridge) ──
    if (ctx->bridge == nullptr) {
        return false;
    }

    switch (instruction.op_code) {
        // ── Method token opcodes ──
        case IROpCode::Call:
        case IROpCode::CallVirt:
        case IROpCode::CallVirtConstrained:
        case IROpCode::NewObj: {
            if (ctx->bridge->resolve_method_by_token == nullptr) {
                return false;
            }
            MethodInfoHandle method_handle = ctx->bridge->resolve_method_by_token(
                ctx->source_image, token);
            if (method_handle == 0u) {
                return false;
            }
            instruction.call_target = reinterpret_cast<void*>(
                static_cast<CHAOS_IL2CPP_UINTPTR>(method_handle));

            // Set arg_count and instance flag from the method descriptor.
            // arg_count = total values on the evaluation stack (including 'this'
            // for instance calls).  CallVirt/CallVirtConstrained are always
            // instance calls; the `call` opcode may be static or instance
            // depending on the method's calling convention.
            const auto* method_desc = TryDecodeReflectionQueryMethodHandle(method_handle);
            if (method_desc != nullptr) {
                const bool is_cv = (instruction.op_code == IROpCode::CallVirt ||
                                    instruction.op_code == IROpCode::CallVirtConstrained);
                // For now, `call` opcode is conservatively treated as static.
                // Phase 5b: add calling-convention detection from the method
                // signature to handle instance `call` correctly.
                instruction.is_instance_call = is_cv;
                instruction.arg_count = static_cast<CHAOS_IL2CPP_UINT32>(
                    method_desc->parameter_count + (is_cv ? 1 : 0));
            }

            // For newobj, try to determine the declaring type's field count
            // from the resolved method's declaring type.
            if (instruction.op_code == IROpCode::NewObj) {
                const auto* method_desc = TryDecodeReflectionQueryMethodHandle(method_handle);
                if (method_desc != nullptr) {
                    // Walk up to find the declaring type's field count.
                    // For now we use 1 as a safe minimum; the token resolver
                    // can be refined as real test cases demand.
                    instruction.secondary_index = 1u;
                }
            }
            return true;
        }

        // ── Type token opcodes ──
        case IROpCode::Box:
        case IROpCode::CastClass:
        case IROpCode::IsInst:
        case IROpCode::Unbox:
        case IROpCode::NewArr:
        case IROpCode::LdElem:
        case IROpCode::StElem: {
            if (ctx->bridge->resolve_type_by_token == nullptr) {
                return false;
            }
            TypeInfoHandle type_handle = ctx->bridge->resolve_type_by_token(
                ctx->source_image, token);
            if (type_handle == 0u) {
                return false;
            }
            instruction.call_target = reinterpret_cast<void*>(
                static_cast<CHAOS_IL2CPP_UINTPTR>(type_handle));
            return true;
        }

        // ── Field token opcodes ──
        case IROpCode::LdFld:
        case IROpCode::StFld:
        case IROpCode::LdSFld:
        case IROpCode::StSFld: {
            if (ctx->bridge->resolve_field_by_token == nullptr) {
                return false;
            }
            FieldInfoHandle field_handle = ctx->bridge->resolve_field_by_token(
                ctx->source_image, token);
            if (field_handle == 0u) {
                return false;
            }
            // Store the field handle; the interpreter or layout pass will
            // extract the byte offset via the layout engine at execution time.
            instruction.call_target = reinterpret_cast<void*>(
                static_cast<CHAOS_IL2CPP_UINTPTR>(field_handle));

            // ── Resolve real field offset via LayoutEngine ──
            // For instance fields (LdFld/StFld), compute the declaring type's
            // layout and find the field's byte offset.  Static fields
            // (LdSFld/StSFld) use a separate global offset scheme and don't
            // participate in struct layout.
            instruction.field_offset = 0u;
            if (instruction.op_code == IROpCode::LdFld ||
                instruction.op_code == IROpCode::StFld) {
                if (ctx->layout_engine != nullptr) {
                    const auto* image_desc =
                        TryDecodeReflectionQueryImageHandle(ctx->source_image);
                    if (image_desc != nullptr) {
                        // Find the declaring type by scanning types for the
                        // field token.
                        CHAOS_IL2CPP_UINT32 field_idx = 0u;
                        const auto* declaring_type = FindDeclaringTypeByFieldToken(
                            image_desc, token, &field_idx);
                        if (declaring_type != nullptr) {
                            TypeInfoHandle decl_handle =
                                EncodeReflectionQueryTypeHandle(declaring_type);
                            const auto* layout =
                                ctx->layout_engine->GetOrComputeLayout(
                                    decl_handle, ctx->type_args, ctx->arg_count);
                            if (layout != nullptr &&
                                field_idx < layout->field_count) {
                                instruction.field_offset =
                                    layout->fields[field_idx].offset;
                            }
                        }
                    }
                }
            }
            return true;
        }

        case IROpCode::LdStr: {
            if (ctx->bridge->resolve_string_by_token == nullptr) {
                return false;
            }
            const char* str = ctx->bridge->resolve_string_by_token(
                ctx->source_image, token);
            if (str == nullptr) {
                return false;
            }
            instruction.string_operand = str;
            return true;
        }

        default:
            // Opcode doesn't carry a token — silently succeed.
            return true;
    }
}

}  // namespace chaos::il2cpp::interpreter
