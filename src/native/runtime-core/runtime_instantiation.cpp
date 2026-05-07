#include "runtime_instantiation.h"
#include "generic_context.h"
#include "generic_method_instantiation.h"
#include "layout_engine.h"
#include "module_registry.h"    // kMaxModules, GetTypeToken
#include "reflection_query_model.h"
#include "runtime_core.h"       // ManagedExceptionCarrier
#include "runtime_vtable.h"
#include "token_resolver.h"     // DefaultTokenResolver, TokenResolverContext
#include "../bootstrap/bootstrap.h"  // GetCodegenBridgeV0

#include <chaos/native_types.h>

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include <string>
#include <vector>

namespace chaos::il2cpp::runtime_instantiation {

namespace {

/* ── Runtime token allocator ────────────────────────────────────────────── */
/* Start at 0x80000000 to leave the AOT token space (0x00000000–0x7FFFFFFF)
 * uncontested.  Each allocation bumps the counter atomically. */

std::atomic<CHAOS_IL2CPP_UINT32> s_next_runtime_token{0x80000000u};

/* ── Track all heap-allocated RuntimeInstantiatedType instances so we can
 *    unregister + free them during hot-update unload.                     ── */
struct RuntimeInstantiatedTypeRecord {
    RuntimeInstantiatedType* type;
    CHAOS_IL2CPP_UINT32 module_id;
};

CHAOS_IL2CPP_MUTEX s_runtime_types_mutex;
CHAOS_IL2CPP_VECTOR(RuntimeInstantiatedTypeRecord) s_runtime_types;

/* ── Track all heap-allocated RuntimeInstantiatedMethod instances so we can
 *    mark them as unloaded during hot-update unload.                     ── */
struct RuntimeInstantiatedMethodRecord {
    RuntimeInstantiatedMethod* method;
    CHAOS_IL2CPP_UINT32 module_id;
};

CHAOS_IL2CPP_MUTEX s_runtime_methods_mutex;
CHAOS_IL2CPP_VECTOR(RuntimeInstantiatedMethodRecord) s_runtime_methods;

/* ── Helper: compute a short display name for a type argument handle.    ── */

static const char* GetTypeDisplayName(TypeInfoHandle handle) {
    if (handle == 0) {
        return "?";
    }
    const auto* desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryTypeHandle(handle);
    if (desc != nullptr && desc->display_name_utf8 != nullptr) {
        return desc->display_name_utf8;
    }
    if (desc != nullptr && desc->name_utf8 != nullptr) {
        return desc->name_utf8;
    }
    return "?";
}

/* ── Helper: copy a C string into a heap-allocated buffer.              ── */

static char* StrDup(const char* src) {
    if (src == nullptr) return nullptr;
    CHAOS_IL2CPP_SIZE len = std::strlen(src);
    auto* buf = static_cast<char*>(std::malloc(len + 1));
    if (buf == nullptr) return nullptr;
    std::memcpy(buf, src, len + 1);
    return buf;
}

/* ── V1: Signature-aware type mapping for argument marshalling ──────── */

/// Map a .NET fully-qualified type name ("System.Int32", etc.) to the
/// corresponding InterpreterValue tag for argument marshalling.
/// Falls back to Int32 (V0 compat) for null/generic-parameter references.
static interpreter::ValueTag MapMemberTypeToValueTag(const char* member_type) {
    if (member_type == nullptr)
        return interpreter::ValueTag::Int32;

    // Generic parameter references ("!N", "!!N") — cannot determine the
    // concrete tag statically; V0 int32 fallback.
    if (member_type[0] == '!') {
        return interpreter::ValueTag::Int32;
    }

    // 4-byte or promoted-to-4-byte primitives
    if (std::strcmp(member_type, "System.Int32") == 0 ||
        std::strcmp(member_type, "System.UInt32") == 0 ||
        std::strcmp(member_type, "System.Boolean") == 0 ||
        std::strcmp(member_type, "System.Byte") == 0 ||
        std::strcmp(member_type, "System.SByte") == 0 ||
        std::strcmp(member_type, "System.Int16") == 0 ||
        std::strcmp(member_type, "System.UInt16") == 0 ||
        std::strcmp(member_type, "System.Char") == 0) {
        return interpreter::ValueTag::Int32;
    }

    // 8-byte primitives
    if (std::strcmp(member_type, "System.Int64") == 0 ||
        std::strcmp(member_type, "System.UInt64") == 0) {
        return interpreter::ValueTag::Int64;
    }

    // 4-byte floating point
    if (std::strcmp(member_type, "System.Single") == 0) {
        return interpreter::ValueTag::Float32;
    }

    // 8-byte floating point
    if (std::strcmp(member_type, "System.Double") == 0) {
        return interpreter::ValueTag::Float64;
    }

    // Native-sized integers
    if (std::strcmp(member_type, "System.IntPtr") == 0 ||
        std::strcmp(member_type, "System.UIntPtr") == 0) {
        return (sizeof(void*) == 8u)
            ? interpreter::ValueTag::Int64
            : interpreter::ValueTag::Int32;
    }

    // Everything else (String, Object, arrays, value types) — ObjectRef
    return interpreter::ValueTag::ObjectRef;
}

/// Scan all registered modules for a type identified by the given metadata
/// token.  Returns the TypeInfoHandle (reflection-query encoding) or 0.
static TypeInfoHandle FindTypeByModuleToken(CHAOS_IL2CPP_UINT32 type_token) {
    for (CHAOS_IL2CPP_UINT32 mid = 0u; mid < runtime_core::kMaxModules; ++mid) {
        const auto* module = runtime_core::LookupModule(mid);
        if (module == nullptr || module->image == nullptr || module->tombstone) {
            continue;
        }
        const auto* type_desc = runtime_core::FindReflectionQueryTypeByToken(
            module->image, type_token);
        if (type_desc != nullptr) {
            return runtime_core::EncodeReflectionQueryTypeHandle(type_desc);
        }
    }
    return 0u;
}

/* ── V1: Type resolution helpers for struct marshalling ───────────────── */

/// Scan all registered modules to find a type by its fully-qualified
/// subject_id (e.g. "System.Guid", "System.Numerics.Vector2").
static TypeInfoHandle FindTypeByName(const char* fully_qualified_name) {
    if (fully_qualified_name == nullptr) return 0u;

    for (CHAOS_IL2CPP_UINT32 mid = 0u; mid < runtime_core::kMaxModules; ++mid) {
        const auto* module = runtime_core::LookupModule(mid);
        if (module == nullptr || module->image == nullptr || module->tombstone) {
            continue;
        }
        const auto* image = module->image;
        if (image == nullptr) continue;

        for (CHAOS_IL2CPP_UINT32 ti = 0u; ti < image->type_count; ++ti) {
            const auto* type_desc = image->types[ti];
            if (type_desc == nullptr) continue;
            if (type_desc->subject_id_utf8 != nullptr &&
                std::strcmp(type_desc->subject_id_utf8,
                    fully_qualified_name) == 0) {
                return runtime_core::MakeTypeHandle(
                    mid, type_desc->metadata_token);
            }
        }
    }
    return 0u;
}

/// Check whether the given TypeInfoHandle refers to a value type (struct).
/// Uses the module registry's type_flags bitfield.
static bool IsValueTypeByHandle(TypeInfoHandle handle) {
    if (handle == 0u) return false;
    const CHAOS_IL2CPP_UINT32 mid = runtime_core::GetModuleId(handle);
    const auto* module = runtime_core::LookupModule(mid);
    if (module == nullptr || module->type_flags == nullptr) return false;
    const CHAOS_IL2CPP_UINT32 type_index = runtime_core::TokenToIndex(
        runtime_core::GetTypeToken(handle));
    if (type_index >= module->type_count) return false;
    return (module->type_flags[type_index] &
            runtime_core::kFlagIsValueType) != 0u;
}

/// Resolve a parameter's member_type_utf8 to a TypeInfoHandle.
/// Handles generic parameter references ("!N", "!!N") and named types.
static TypeInfoHandle ResolveParameterType(
    const char* member_type_utf8,
    const TypeInfoHandle* type_args,
    CHAOS_IL2CPP_UINT32 arg_count)
{
    if (member_type_utf8 == nullptr) return 0u;

    // Generic parameter reference: "!N" (type generic) or "!!N" (method generic)
    if (member_type_utf8[0] == '!') {
        if (type_args == nullptr || arg_count == 0u) return 0u;
        const char* num_str = member_type_utf8 + 1;
        if (member_type_utf8[1] == '!') num_str++;  // "!!N"
        char* end = nullptr;
        long idx = std::strtol(num_str, &end, 10);
        if (end == num_str || idx < 0 ||
            static_cast<CHAOS_IL2CPP_UINT32>(idx) >= arg_count) {
            return 0u;
        }
        return type_args[idx];
    }

    // Named type: scan all registered modules.
    return FindTypeByName(member_type_utf8);
}

/* ── Bridge function implementations ── */

TypeInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveOrInstantiateType(
    TypeInfoHandle         open_type_definition,
    const TypeInfoHandle*  type_args,
    CHAOS_IL2CPP_UINT32   arg_count)
{
    if (open_type_definition == 0) {
        return 0;
    }

    /* Fast path: registry lookup. */
    TypeInfoHandle closed = generic_context::TryResolveClosedType(
        open_type_definition, type_args, arg_count);
    if (closed != 0) {
        return closed;
    }

    /* Slow path: build a RuntimeInstantiatedType descriptor. */
    auto* rt_type = BuildClosedDescriptor(
        open_type_definition, type_args, arg_count);
    if (rt_type == nullptr) {
        return 0;
    }

    /* Compute value type layout if needed. */
    if (rt_type->descriptor.subject_id_utf8 != nullptr) {
        // Check the open type to determine if it's a value type.
        // For now, we assume value types can be identified by convention
        // (the open type descriptor's fields are available).
        ComputeValueTypeLayout(rt_type);
    }

    /* Encode the closed descriptor as a TypeInfoHandle. */
    TypeInfoHandle closed_handle = EncodeReflectionQueryTypeHandle(
        &rt_type->descriptor);
    if (closed_handle == 0) {
        std::free(rt_type);
        return 0;
    }

    /* Register in GenericContextRegistry so subsequent lookups hit the fast path. */
    generic_context::RegisterGenericInstantiation(
        open_type_definition, closed_handle, rt_type->type_args, rt_type->arg_count);
    rt_type->is_registered = true;

    return closed_handle;
}

MethodInfoHandle CHAOS_RUNTIME_ABI_CALL ResolveOrInstantiateMethod(
    MethodInfoHandle       open_method_definition,
    const TypeInfoHandle*  type_args,
    CHAOS_IL2CPP_UINT32   arg_count)
{
    if (open_method_definition == 0u) {
        return 0u;
    }

    /* Fast Path 1: AOT map query (generic method promotion).
     * Check for a pre-compiled AOT instance before falling through to the
     * GenericContextRegistry or building a new RuntimeInstantiatedMethod. */
    {
        CHAOS_IL2CPP_UINT32 open_token = 0u;
        CHAOS_IL2CPP_UINT32 module_id  = static_cast<CHAOS_IL2CPP_UINT32>(
            open_method_definition >> 32);

        const auto* open_desc =
            runtime_core::TryDecodeReflectionQueryMethodHandle(
                open_method_definition);
        if (open_desc != nullptr) {
            open_token = open_desc->metadata_token;
        }
        if (open_token != 0u) {
            const MethodInfoHandle aot_method = QueryAotMethod(
                module_id, open_token, type_args, arg_count);
            if (aot_method != 0u) return aot_method;
        }
    }

    /* Fast Path 2: GenericContextRegistry lookup. */
    MethodInfoHandle closed = generic_context::TryResolveClosedMethod(
        open_method_definition, type_args, arg_count);
    if (closed != 0u) {
        return closed;
    }

    /* Slow path: build a RuntimeInstantiatedMethod descriptor. */
    auto* rt_method = CreateClosedMethodDescriptor(
        open_method_definition, type_args, arg_count);
    if (rt_method == nullptr) {
        return 0u;
    }

    /* Populate token resolution context fields. */
    rt_method->bridge        = chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
    rt_method->source_image  = 0u;  // set from module registration if available
    rt_method->layout_engine = layout::GetLayoutEngine();

    /* Encode the closed descriptor as a MethodInfoHandle. */
    MethodInfoHandle closed_handle = chaos::il2cpp::runtime_core::EncodeReflectionQueryMethodHandle(
        &rt_method->descriptor);
    if (closed_handle == 0u) {
        std::free(const_cast<char*>(rt_method->descriptor.subject_id_utf8));
        std::free(const_cast<char*>(rt_method->descriptor.name_utf8));
        std::free(const_cast<char*>(rt_method->descriptor.member_type_utf8));
        std::free(rt_method->type_args);
        std::free(rt_method);
        return 0u;
    }

    /* Register in GenericContextRegistry so subsequent lookups hit the fast path. */
    generic_context::RegisterGenericMethodInstantiation(
        open_method_definition, closed_handle, rt_method->type_args, rt_method->arg_count);
    rt_method->is_registered = true;

    /* Track for module unload cleanup. */
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_runtime_methods_mutex);
        s_runtime_methods.push_back({rt_method, rt_method->module_id});
    }

    return closed_handle;
}

void CHAOS_RUNTIME_ABI_CALL UnregisterModuleGenerics(
    CHAOS_IL2CPP_UINT32 module_id)
{
    generic_context::UnregisterModuleGenerics(module_id);

    /* Also free RuntimeInstantiatedType records for this module. */
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_runtime_types_mutex);
        for (CHAOS_IL2CPP_SIZE i = 0u; i < s_runtime_types.size(); ) {
            if (s_runtime_types[i].module_id == module_id) {
                auto* rt = s_runtime_types[i].type;
                std::free(const_cast<char*>(rt->descriptor.subject_id_utf8));
                std::free(const_cast<char*>(rt->descriptor.definition_subject_id_utf8));
                std::free(const_cast<char*>(rt->descriptor.namespace_name_utf8));
                std::free(const_cast<char*>(rt->descriptor.name_utf8));
                std::free(const_cast<char*>(rt->descriptor.display_name_utf8));
                std::free(rt->type_args);
                std::free(rt->field_offsets);
                std::free(rt->resolved_field_types);
                std::free(rt);
                s_runtime_types.erase(s_runtime_types.begin() +
                    static_cast<CHAOS_IL2CPP_SIZE>(i));
            } else {
                ++i;
            }
        }
    }

    /* Mark RuntimeInstantiatedMethod records as unloaded for this module.
     * We do NOT free the methods themselves because stale MethodInfoHandle
     * values may still be in use; is_unloaded blocks re-entry. */
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_runtime_methods_mutex);
        for (CHAOS_IL2CPP_SIZE i = 0u; i < s_runtime_methods.size(); ) {
            if (s_runtime_methods[i].module_id == module_id) {
                s_runtime_methods[i].method->is_unloaded = true;
                s_runtime_methods.erase(s_runtime_methods.begin() +
                    static_cast<CHAOS_IL2CPP_SIZE>(i));
            } else {
                ++i;
            }
        }
    }

    /* Mark the module as tombstone in the Module Registry. */
    runtime_core::MarkModuleTombstone(module_id);
}

}  // anonymous namespace

// ── InterpreterDispatch ────────────────────────────────────────────────────
// DispatchCallback implementation for InterpreterVM::Execute.
//
// Converts InterpreterValue arguments to ABI format (void* const* argv array),
// dispatches through MethodInvoke (which routes to AOT or interpreted execution),
// and converts the return value back to DispatchResult.
//
// This is the single point where interpreter Call/CallVirt/CallBridge
// instructions bridge into runtime-core's method dispatch.
//
// NOTE: This is intentionally OUTSIDE the anonymous namespace — the header
// declares it with external linkage, and interpreter_entry.cpp references it
// that way. Putting it in the anonymous namespace gives it internal linkage,
// causing LNK2019 at link time.

interpreter::DispatchResult InterpreterDispatch(
    void*                               call_target,
    const interpreter::InterpreterValue* call_args,
    CHAOS_IL2CPP_UINT32                 arg_count,
    bool                                is_instance_call,
    void*                               dispatch_context)
{
    using interpreter::DispatchResult;
    using interpreter::InterpreterValue;
    using interpreter::ValueTag;

    DispatchResult result = {};

    auto* ctx = static_cast<InterpreterDispatchContext*>(dispatch_context);
    if (ctx == nullptr || ctx->runtime_state == nullptr) {
        return result;
    }

    const MethodInfoHandle method = reinterpret_cast<MethodInfoHandle>(call_target);
    if (method == 0u) {
        return result;
    }

    // ── Depth guard (Phase 7) ──
    if (ctx->recursion_depth >= kInterpreterMaxDispatchDepth) {
        // Exceeded maximum dispatch recursion — signal as an unhandled exception.
        result.threw_exception = true;
        result.exception_value = interpreter::InterpreterValue::null_val();
        return result;
    }
    ++ctx->recursion_depth;

    // ── Separate 'this' pointer from arguments ──
    void* this_ptr = nullptr;
    CHAOS_IL2CPP_UINT32 param_count = arg_count;
    if (is_instance_call && arg_count > 0u) {
        const auto& tv = call_args[0];
        this_ptr = (tv.tag == ValueTag::ObjectRef || tv.tag == ValueTag::Null)
            ? tv.obj : nullptr;
        param_count = arg_count - 1u;
    }

    // ── Build ABI argument array ──
    // argv[i] points to raw bytes of argument i.
    // raw_data stores inline values (small enough for uint64).
    // Struct arguments use the heap-allocated blob pointer directly.
    CHAOS_IL2CPP_VECTOR(void*)     argv(param_count, nullptr);
    CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_UINT64) raw_data(param_count, 0);

    for (CHAOS_IL2CPP_UINT32 ai = 0u, si = (is_instance_call ? 1u : 0u);
         si < arg_count; ++ai, ++si) {
        const auto& arg = call_args[si];
        switch (arg.tag) {
            case ValueTag::Int32:
                std::memcpy(&raw_data[ai], &arg.i32, sizeof(arg.i32));
                argv[ai] = &raw_data[ai];
                break;
            case ValueTag::Int64:
                std::memcpy(&raw_data[ai], &arg.i64, sizeof(arg.i64));
                argv[ai] = &raw_data[ai];
                break;
            case ValueTag::Float32:
                std::memcpy(&raw_data[ai], &arg.f32, sizeof(arg.f32));
                argv[ai] = &raw_data[ai];
                break;
            case ValueTag::Float64:
                std::memcpy(&raw_data[ai], &arg.f64, sizeof(arg.f64));
                argv[ai] = &raw_data[ai];
                break;
            case ValueTag::ObjectRef:
            case ValueTag::Null:
                std::memcpy(&raw_data[ai], &arg.obj, sizeof(arg.obj));
                argv[ai] = &raw_data[ai];
                break;
            case ValueTag::Struct:
                argv[ai] = arg.obj;   // direct pointer to heap-allocated blob
                break;
            default:
                argv[ai] = nullptr;
                break;
        }
    }

    // ── Determine return type ──
    const auto* method_desc =
        runtime_core::TryDecodeReflectionQueryMethodHandle(method);

    // ── Prepare return value buffer ──
    // Scalar returns use a stack uint64; struct returns allocate via LayoutEngine.
    CHAOS_IL2CPP_UINT64      ret_scalar = 0;
    void*                     ret_buf = &ret_scalar;
    CHAOS_IL2CPP_SIZE         ret_size = sizeof(ret_scalar);
    CHAOS_IL2CPP_VECTOR(uint8_t) struct_ret_buf;

    if (method_desc != nullptr && method_desc->member_type_utf8 != nullptr) {
        // Check for struct (value type) return.
        TypeInfoHandle ret_type = ResolveParameterType(
            method_desc->member_type_utf8, nullptr, 0u);
        if (ret_type != 0u && IsValueTypeByHandle(ret_type)) {
            auto* engine = layout::GetLayoutEngine();
            const auto* layout = engine->GetOrComputeLayout(ret_type, nullptr, 0u);
            if (layout != nullptr && layout->value_size > 0u) {
                struct_ret_buf.resize(layout->value_size);
                void* buf_ptr = struct_ret_buf.data();
                ret_buf  = &buf_ptr;   // out_return_value = pointer to buffer ptr
                ret_size = layout->value_size;
            }
        }
    }

    // ── Call MethodInvoke through ABI table ──
    const auto* abi = runtime_core::GetRuntimeAbiV0();
    if (abi == nullptr || abi->method_invoke == nullptr) {
        --ctx->recursion_depth;
        return result;
    }
    ExceptionHandle ex = nullptr;
    const RuntimeStatus status = abi->method_invoke(
        ctx->runtime_state, ctx->thread_state,
        method, this_ptr,
        argv.data(), param_count,
        ret_buf, ret_size, &ex);

    // ── Handle exception ──
    if (status == CHAOS_RUNTIME_STATUS_MANAGED_EXCEPTION) {
        result.threw_exception = true;
        result.exception_value = (ex != nullptr)
            ? InterpreterValue::from_obj(ex)
            : InterpreterValue::null_val();
        --ctx->recursion_depth;
        return result;
    }
    if (status != CHAOS_RUNTIME_STATUS_OK) {
        --ctx->recursion_depth;
        return result;  // unexpected status — no return value
    }

    // ── Convert return value ──
    if (method_desc != nullptr && method_desc->member_type_utf8 != nullptr) {
        ValueTag ret_tag = MapMemberTypeToValueTag(method_desc->member_type_utf8);

        // ObjectRef tag may actually be a value type (struct) — double check.
        if (ret_tag == ValueTag::ObjectRef) {
            TypeInfoHandle ret_type = ResolveParameterType(
                method_desc->member_type_utf8, nullptr, 0u);
            if (ret_type != 0u && IsValueTypeByHandle(ret_type)) {
                ret_tag = ValueTag::Struct;
            }
        }

        switch (ret_tag) {
            case ValueTag::Int32:
                result.has_value = true;
                result.value = InterpreterValue::from_i32(
                    static_cast<CHAOS_IL2CPP_INT32>(ret_scalar));
                break;
            case ValueTag::Int64:
                result.has_value = true;
                result.value = InterpreterValue::from_i64(
                    static_cast<CHAOS_IL2CPP_INT64>(ret_scalar));
                break;
            case ValueTag::Float32: {
                float fval = 0.0f;
                std::memcpy(&fval, &ret_scalar, sizeof(fval));
                result.has_value = true;
                result.value = InterpreterValue::from_f32(fval);
                break;
            }
            case ValueTag::Float64: {
                double dval = 0.0;
                std::memcpy(&dval, &ret_scalar, sizeof(dval));
                result.has_value = true;
                result.value = InterpreterValue::from_f64(dval);
                break;
            }
            case ValueTag::ObjectRef:
            case ValueTag::Null: {
                void* obj_ptr = nullptr;
                std::memcpy(&obj_ptr, &ret_scalar, sizeof(obj_ptr));
                result.has_value = true;
                result.value = (obj_ptr != nullptr)
                    ? InterpreterValue::from_obj(obj_ptr)
                    : InterpreterValue::null_val();
                break;
            }
            case ValueTag::Struct:
                if (!struct_ret_buf.empty()) {
                    result.has_value = true;
                    result.value = InterpreterValue::from_struct(
                        struct_ret_buf.data(),
                        static_cast<CHAOS_IL2CPP_UINT32>(struct_ret_buf.size()));
                }
                break;
            default:
                break;
        }
    } else {
        // V0 fallback: treat return value as int32.
        result.has_value = true;
        result.value = InterpreterValue::from_i32(
            static_cast<CHAOS_IL2CPP_INT32>(ret_scalar));
    }

    --ctx->recursion_depth;
    return result;
}

// ════════════════════════════════════════════════════════════════════════════
// interpret_method_call — bridge from MethodInvoke to InterpreterVM
// ════════════════════════════════════════════════════════════════════════════

RuntimeStatus CHAOS_RUNTIME_ABI_CALL InterpretMethodCall(
    RuntimeState*           runtime_state,
    ThreadState*            thread_state,
    MethodInfoHandle        method,
    void*                   object_instance,
    void* const*            argv,
    CHAOS_IL2CPP_UINT32    argc,
    void*                   out_return_value,
    CHAOS_IL2CPP_SIZE       out_return_value_size,
    ExceptionHandle*        out_exception)
{
    (void)runtime_state;
    (void)thread_state;

    if (out_exception != nullptr) {
        *out_exception = nullptr;
    }

    // ── Recover RuntimeInstantiatedMethod* from MethodInfoHandle ──
    const auto* desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryMethodHandle(method);
    if (desc == nullptr) {
        return CHAOS_RUNTIME_STATUS_NOT_FOUND;
    }
    auto* rt_method = const_cast<RuntimeInstantiatedMethod*>(
        reinterpret_cast<const RuntimeInstantiatedMethod*>(desc));

    // ── Check module unload ──
    if (rt_method->is_unloaded) {
        return CHAOS_RUNTIME_STATUS_NOT_FOUND;
    }

    // ── Ensure IR method body is available ──
    if (rt_method->ir_method_body == nullptr) {
        if (rt_method->il_bytes != nullptr && rt_method->il_length > 0u) {
            interpreter::TokenResolverContext resolver_ctx;
            resolver_ctx.bridge        = rt_method->bridge;
            resolver_ctx.source_image  = rt_method->source_image;
            resolver_ctx.type_args     = rt_method->type_args;
            resolver_ctx.arg_count     = rt_method->arg_count;
            resolver_ctx.layout_engine = rt_method->layout_engine;

            if (!LowerMethodBody(rt_method, rt_method->il_bytes,
                    rt_method->il_length,
                    interpreter::DefaultTokenResolver, &resolver_ctx))
            {
                return CHAOS_RUNTIME_STATUS_INTERNAL_ERROR;
            }
        } else {
            return CHAOS_RUNTIME_STATUS_NOT_FOUND;
        }
    }

    // ── Build ExecutionFrame ──
    // V1 argument marshalling: argv contains pointers to raw argument values.
    // For instance methods, object_instance is the 'this' pointer.
    // Parameter type info from the method descriptor drives the
    // InterpreterValue tag (i32/i64/f32/f64/obj) instead of V0's all-int32.
    interpreter::ExecutionFrame frame;

    if (object_instance != nullptr) {
        frame.arguments.push_back(
            interpreter::InterpreterValue::from_obj(object_instance));
    }

    // Determine available parameter descriptor count.
    const CHAOS_IL2CPP_UINT32 param_count =
        (desc->parameters != nullptr) ? desc->parameter_descriptor_count : 0u;

    for (CHAOS_IL2CPP_UINT32 ai = 0u; ai < argc; ++ai) {
        if (argv != nullptr && argv[ai] != nullptr) {
            // V1: signature-aware marshalling when parameter type info exists.
            if (ai < param_count && desc->parameters[ai].member_type_utf8 != nullptr) {
                const interpreter::ValueTag tag = MapMemberTypeToValueTag(
                    desc->parameters[ai].member_type_utf8);

                switch (tag) {
                    case interpreter::ValueTag::Int32: {
                        CHAOS_IL2CPP_INT32 val;
                        std::memcpy(&val, argv[ai], sizeof(val));
                        frame.arguments.push_back(
                            interpreter::InterpreterValue::from_i32(val));
                        break;
                    }
                    case interpreter::ValueTag::Int64: {
                        CHAOS_IL2CPP_INT64 val;
                        std::memcpy(&val, argv[ai], sizeof(val));
                        frame.arguments.push_back(
                            interpreter::InterpreterValue::from_i64(val));
                        break;
                    }
                    case interpreter::ValueTag::Float32: {
                        float val;
                        std::memcpy(&val, argv[ai], sizeof(val));
                        frame.arguments.push_back(
                            interpreter::InterpreterValue::from_f32(val));
                        break;
                    }
                    case interpreter::ValueTag::Float64: {
                        double val;
                        std::memcpy(&val, argv[ai], sizeof(val));
                        frame.arguments.push_back(
                            interpreter::InterpreterValue::from_f64(val));
                        break;
                    }
                    default: {
                        // Check if the parameter type is a value type (struct).
                        // If so, marshal the struct data by value using the
                        // LayoutEngine-computed size rather than reading a pointer.
                        const TypeInfoHandle param_type = ResolveParameterType(
                            desc->parameters[ai].member_type_utf8,
                            rt_method->type_args, rt_method->arg_count);
                        if (param_type != 0u && IsValueTypeByHandle(param_type)) {
                            auto* engine = layout::GetLayoutEngine();
                            const auto* layout = engine->GetOrComputeLayout(
                                param_type,
                                rt_method->type_args, rt_method->arg_count);
                            if (layout != nullptr && layout->value_size > 0u) {
                                frame.arguments.push_back(
                                    interpreter::InterpreterValue::from_struct(
                                        argv[ai], layout->value_size));
                                break;
                            }
                        }
                        // Fall through: treat as ObjectRef (read pointer).
                        void* obj_ptr = nullptr;
                        std::memcpy(&obj_ptr, argv[ai], sizeof(obj_ptr));
                        if (obj_ptr != nullptr) {
                            frame.arguments.push_back(
                                interpreter::InterpreterValue::from_obj(obj_ptr));
                        } else {
                            frame.arguments.push_back(
                                interpreter::InterpreterValue::null_val());
                        }
                        break;
                    }
                }
            } else {
                // V0 fallback: default to int32 when no type info available.
                frame.arguments.push_back(
                    interpreter::InterpreterValue::from_i32(
                        *static_cast<const CHAOS_IL2CPP_INT32*>(argv[ai])));
            }
        } else {
            frame.arguments.push_back(interpreter::InterpreterValue::null_val());
        }
    }

    // ── Set up dispatch callback for nested Call instructions ──
    // When Execute encounters Call/CallVirt/CallBridge, it invokes
    // InterpreterDispatch synchronously instead of returning
    // needs_external_dispatch.  This enables recursive/mutual calls
    // between interpreted methods through the same MenthodInvoke path.
    InterpreterDispatchContext dispatch_ctx;
    dispatch_ctx.runtime_state = runtime_state;
    dispatch_ctx.thread_state  = thread_state;
    frame.dispatch_fn     = InterpreterDispatch;
    frame.dispatch_context = &dispatch_ctx;

    // ── Execute ──
    const interpreter::InterpreterVM vm = {};
    interpreter::ExecutionResult result = vm.Execute(
        *rt_method->ir_method_body, &frame);

    // ── Exception handling ──
    if (result.threw_exception) {
        // V1: attempt to convert result.exception_value to an ExceptionHandle.
        ExceptionHandle ex_handle = nullptr;

        if (result.exception_value.tag == interpreter::ValueTag::ObjectRef &&
            result.exception_value.obj != nullptr) {
            auto* ex_obj = static_cast<interpreter::InterpreterObject*>(
                result.exception_value.obj);

            if (ex_obj->type_token != 0u) {
                // Resolve type_token → TypeInfoHandle by scanning modules.
                const TypeInfoHandle ex_type = FindTypeByModuleToken(ex_obj->type_token);
                if (ex_type != 0u) {
                    const auto* abi = runtime_core::GetRuntimeAbiV0();
                    if (abi != nullptr && abi->object_new != nullptr) {
                        void* real_obj = abi->object_new(
                            runtime_state, thread_state, ex_type);
                        if (real_obj != nullptr) {
                            ex_handle = static_cast<ExceptionHandle>(real_obj);
                        }
                    }
                }
            }
        }

        // If we couldn't materialize a real handle, ex_handle remains nullptr.
        throw runtime_core::ManagedExceptionCarrier{ex_handle};
    }

    // ── Return value extraction (V0: tag dispatch) ──
    if (result.has_return_value && out_return_value != nullptr) {
        switch (result.return_value.tag) {
            case interpreter::ValueTag::Int32:
                if (out_return_value_size >= sizeof(int32_t)) {
                    CHAOS_IL2CPP_MEMCPY(out_return_value,
                        &result.return_value.i32, sizeof(int32_t));
                }
                break;
            case interpreter::ValueTag::Int64:
                if (out_return_value_size >= sizeof(int64_t)) {
                    CHAOS_IL2CPP_MEMCPY(out_return_value,
                        &result.return_value.i64, sizeof(int64_t));
                }
                break;
            case interpreter::ValueTag::Float32:
                if (out_return_value_size >= sizeof(float)) {
                    CHAOS_IL2CPP_MEMCPY(out_return_value,
                        &result.return_value.f32, sizeof(float));
                }
                break;
            case interpreter::ValueTag::Float64:
                if (out_return_value_size >= sizeof(double)) {
                    CHAOS_IL2CPP_MEMCPY(out_return_value,
                        &result.return_value.f64, sizeof(double));
                }
                break;
            case interpreter::ValueTag::ObjectRef:
            case interpreter::ValueTag::Null:
                if (out_return_value_size >= sizeof(void*)) {
                    CHAOS_IL2CPP_MEMCPY(out_return_value,
                        &result.return_value.obj, sizeof(void*));
                }
                break;
            case interpreter::ValueTag::Struct:
                if (out_return_value != nullptr &&
                    out_return_value_size >= result.return_value.struct_size) {
                    CHAOS_IL2CPP_MEMCPY(out_return_value,
                        result.return_value.obj,
                        result.return_value.struct_size);
                }
                break;
            default:
                break;
        }
    }

    // ── Update diagnostic counter ──
    const_cast<RuntimeInstantiationBridgeV0*>(
        chaos::il2cpp::runtime_instantiation::GetBridgeV0()
    )->interpreted_method_call_count++;

    return CHAOS_RUNTIME_STATUS_OK;
}

/* Process-wide bridge instance.  Non-const because diagnostic counters
 * (interpreted_method_call_count, runtime_instantiation_count) are mutated
 * at runtime via the interpret_method_call and resolve_or_instantiate_*
 * bridge functions. */
RuntimeInstantiationBridgeV0 g_bridge = {
    CHAOS_RUNTIME_INSTANTIATION_BRIDGE_V0,
    sizeof(RuntimeInstantiationBridgeV0),
    ResolveOrInstantiateType,
    ResolveOrInstantiateMethod,
    UnregisterModuleGenerics,
    InterpretMethodCall,
    0u,
    0u
};

// ════════════════════════════════════════════════════════════════════════════
// Public API
// ════════════════════════════════════════════════════════════════════════════

CHAOS_IL2CPP_UINT32 AllocateRuntimeToken() {
    return s_next_runtime_token.fetch_add(1u, std::memory_order_relaxed);
}

char* BuildClosedSubjectId(
    const chaos::il2cpp::runtime_core::ReflectionQueryTypeDescriptor* open_desc,
    const TypeInfoHandle*                type_args,
    CHAOS_IL2CPP_UINT32                  arg_count)
{
    if (open_desc == nullptr) {
        return 0;
    }

    const char* base = open_desc->subject_id_utf8;
    if (base == nullptr) {
        return nullptr;
    }

    // Build: "OpenType`N[Arg1,Arg2,...]"
    std::string result;
    result += base;
    result += '[';

    for (CHAOS_IL2CPP_UINT32 i = 0u; i < arg_count; ++i) {
        if (i > 0u) result += ',';
        result += GetTypeDisplayName(type_args[i]);
    }
    result += ']';

    auto* buf = static_cast<char*>(std::malloc(result.size() + 1));
    if (buf == nullptr) return nullptr;
    std::memcpy(buf, result.data(), result.size() + 1);
    return buf;
}

RuntimeInstantiatedType* BuildClosedDescriptor(
    TypeInfoHandle         open_type_definition,
    const TypeInfoHandle*  type_args,
    CHAOS_IL2CPP_UINT32   arg_count)
{
    if (open_type_definition == 0 || type_args == nullptr || arg_count == 0u) {
        return 0;
    }

    const auto* open_desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryTypeHandle(
        open_type_definition);
    if (open_desc == nullptr) {
        return 0;  // Not a reflection-query type; cannot instantiate.
    }

    // ── Allocate RuntimeInstantiatedType ──
    auto* rt_type = static_cast<RuntimeInstantiatedType*>(
        std::calloc(1u, sizeof(RuntimeInstantiatedType)));
    if (rt_type == nullptr) {
        return 0;
    }

    // ── Build closed subject_id ──
    char* subject_id = BuildClosedSubjectId(open_desc, type_args, arg_count);
    if (subject_id == nullptr) {
        std::free(rt_type);
        return 0;
    }
    rt_type->descriptor.subject_id_utf8 = subject_id;

    // ── Copy other descriptor fields from the open type ──
    rt_type->descriptor.metadata_token           = AllocateRuntimeToken();
    rt_type->descriptor.definition_subject_id_utf8 = StrDup(open_desc->subject_id_utf8);
    rt_type->descriptor.namespace_name_utf8       = StrDup(open_desc->namespace_name_utf8);

    // Name: extract the simple type name (after last '.' or '/')
    const char* base_subject = open_desc->subject_id_utf8;
    const char* name_start = base_subject;
    if (const char* p = std::strrchr(base_subject, '/')) name_start = p + 1;
    else if (const char* p = std::strrchr(base_subject, '.')) name_start = p + 1;
    rt_type->descriptor.name_utf8 = StrDup(name_start);

    // Display name
    {
        std::string display;
        display += name_start;
        display += '[';
        for (CHAOS_IL2CPP_UINT32 i = 0u; i < arg_count; ++i) {
            if (i > 0u) display += ',';
            display += GetTypeDisplayName(type_args[i]);
        }
        display += ']';
        auto* buf = static_cast<char*>(std::malloc(display.size() + 1));
        if (buf) {
            std::memcpy(buf, display.data(), display.size() + 1);
            rt_type->descriptor.display_name_utf8 = buf;
        }
    }

    // Point generic_type_definition to the open type.
    rt_type->descriptor.generic_type_definition = open_desc;

    // Reference the open type's fields and methods directly.
    // (Phase 5 may substitute type arguments in field/method descriptors.)
    rt_type->descriptor.fields         = open_desc->fields;
    rt_type->descriptor.field_count    = open_desc->field_count;
    rt_type->descriptor.properties     = open_desc->properties;
    rt_type->descriptor.property_count = open_desc->property_count;
    rt_type->descriptor.methods        = open_desc->methods;
    rt_type->descriptor.method_count   = open_desc->method_count;

    // ── Copy type_args ──
    auto* args_buf = static_cast<TypeInfoHandle*>(
        std::malloc(sizeof(TypeInfoHandle) * arg_count));
    if (args_buf == nullptr) {
        std::free(const_cast<char*>(rt_type->descriptor.subject_id_utf8));
        std::free(rt_type);
        return 0;
    }
    std::memcpy(args_buf, type_args, sizeof(TypeInfoHandle) * arg_count);
    rt_type->type_args  = args_buf;
    rt_type->arg_count  = arg_count;
    rt_type->module_id  = 0u;  // AOT root by default

    // ── Track for cleanup ──
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(s_runtime_types_mutex);
        s_runtime_types.push_back({rt_type, 0u});
    }

    // ── Build vtable for the closed type (copies open type vtable) ──
    if (rt_type->descriptor.subject_id_utf8 != nullptr &&
        open_desc->subject_id_utf8 != nullptr) {
        CHAOS_IL2CPP_UINT64 closed_sid = chaos_compute_type_stable_id(rt_type->descriptor.subject_id_utf8);
        CHAOS_IL2CPP_UINT64 open_sid = chaos_compute_type_stable_id(open_desc->subject_id_utf8);
        runtime_vtable::BuildRuntimeVTable(closed_sid, open_sid);
    }

    return rt_type;
}

void ComputeValueTypeLayout(RuntimeInstantiatedType* rt_type) {
    if (rt_type == nullptr || rt_type->descriptor.generic_type_definition == nullptr) {
        return;
    }

    // Delegate layout computation to the LayoutEngine.
    TypeInfoHandle closed_handle = EncodeReflectionQueryTypeHandle(&rt_type->descriptor);
    if (closed_handle == 0u) {
        return;
    }

    auto* engine = layout::GetLayoutEngine();
    const auto* layout = engine->GetOrComputeLayout(
        closed_handle, rt_type->type_args, rt_type->arg_count);

    if (layout == nullptr) {
        return;
    }

    // Copy layout results into the RuntimeInstantiatedType record
    // (LayoutEngine owns the layout memory; we copy field offsets and
    // resolved type handles into our own storage).
    rt_type->value_size = layout->value_size;
    rt_type->field_offset_count = layout->field_count;

    if (layout->field_count > 0u && layout->fields != nullptr) {
        // ── Copy field offsets ──
        auto* offsets = static_cast<CHAOS_IL2CPP_UINT32*>(
            std::malloc(sizeof(CHAOS_IL2CPP_UINT32) * layout->field_count));
        if (offsets != nullptr) {
            for (CHAOS_IL2CPP_UINT32 i = 0u; i < layout->field_count; ++i) {
                offsets[i] = layout->fields[i].offset;
            }
            rt_type->field_offsets = offsets;
        }

        // ── Copy resolved field types (optional cache) ──
        auto* resolved = static_cast<TypeInfoHandle*>(
            std::malloc(sizeof(TypeInfoHandle) * layout->field_count));
        if (resolved != nullptr) {
            for (CHAOS_IL2CPP_UINT32 i = 0u; i < layout->field_count; ++i) {
                resolved[i] = layout->fields[i].resolved_type;
            }
            rt_type->resolved_field_types = resolved;
            rt_type->resolved_field_count = layout->field_count;
        }
    }
}

const RuntimeInstantiationBridgeV0* GetBridgeV0() {
    return &g_bridge;
}

// ════════════════════════════════════════════════════════════════════════════
// AOT method map (generic method promotion)
// ════════════════════════════════════════════════════════════════════════════

namespace {

struct PerModuleAotMap {
    const GenericMethodAotEntryV0* entries     = nullptr;
    CHAOS_IL2CPP_UINT32            entry_count = 0u;
    const uint32_t*                args        = nullptr;
    CHAOS_IL2CPP_UINT32            arg_count   = 0u;
};

// Per-module AOT maps indexed by module_id (fixed-size array, O(1) lookup).
PerModuleAotMap s_module_aot_maps[runtime_core::kMaxModules];

}  // anonymous namespace (AOT map)

void RegisterMethodAotEntries(
    CHAOS_IL2CPP_UINT32                module_id,
    const GenericMethodAotEntryV0*     entries,
    CHAOS_IL2CPP_UINT32                count,
    const uint32_t*                    args,
    CHAOS_IL2CPP_UINT32                arg_count)
{
    if (module_id >= runtime_core::kMaxModules) return;
    s_module_aot_maps[module_id].entries     = entries;
    s_module_aot_maps[module_id].entry_count = count;
    s_module_aot_maps[module_id].args        = args;
    s_module_aot_maps[module_id].arg_count   = arg_count;
}

MethodInfoHandle QueryAotMethod(
    CHAOS_IL2CPP_UINT32                module_id,
    CHAOS_IL2CPP_UINT32                open_method_token,
    const TypeInfoHandle*              type_args,
    CHAOS_IL2CPP_UINT32                arg_count)
{
    if (module_id >= runtime_core::kMaxModules) return 0u;

    const auto& mod = s_module_aot_maps[module_id];
    if (mod.entry_count == 0u) return 0u;

    // Binary search by open_token.
    CHAOS_IL2CPP_INT32 lo = 0;
    CHAOS_IL2CPP_INT32 hi = static_cast<CHAOS_IL2CPP_INT32>(mod.entry_count) - 1;

    while (lo <= hi) {
        const CHAOS_IL2CPP_INT32 mid = lo + (hi - lo) / 2;
        const uint32_t mid_token = mod.entries[mid].open_token;

        if (mid_token < open_method_token) {
            lo = mid + 1;
        } else if (mid_token > open_method_token) {
            hi = mid - 1;
        } else {
            /* Found matching open_token — scan type_args.
             * Multiple entries may share the same open_token (different type
             * arguments), so find the contiguous range first. */
            CHAOS_IL2CPP_INT32 range_start = mid;
            while (range_start > 0 &&
                   mod.entries[range_start - 1].open_token == open_method_token) {
                --range_start;
            }
            CHAOS_IL2CPP_UINT32 range_end = static_cast<CHAOS_IL2CPP_UINT32>(mid);
            while (range_end + 1u < mod.entry_count &&
                   mod.entries[range_end + 1u].open_token == open_method_token) {
                ++range_end;
            }

            // Scan the range for a type-arg match.
            for (CHAOS_IL2CPP_UINT32 ei = static_cast<CHAOS_IL2CPP_UINT32>(range_start);
                 ei <= range_end; ++ei) {
                const auto& entry = mod.entries[ei];
                if (entry.arg_count != arg_count) continue;

                bool match = true;
                for (CHAOS_IL2CPP_UINT32 ai = 0u; ai < arg_count; ++ai) {
                    const uint32_t arg_token   = runtime_core::GetTypeToken(type_args[ai]);
                    const uint32_t entry_token = mod.args[entry.args_start_index + ai];
                    if (arg_token != entry_token) {
                        match = false;
                        break;
                    }
                }

                if (match) {
                    // Return the closed token as a simple token-based
                    // MethodInfoHandle (module_id=0 for AOT root).
                    return static_cast<MethodInfoHandle>(
                        (static_cast<CHAOS_IL2CPP_UINT64>(0u) << 32) |
                        entry.closed_token);
                }
            }

            return 0u;  // open_token matched but type args did not
        }
    }

    return 0u;  // open_token not found in this module
}

}  // namespace chaos::il2cpp::runtime_instantiation

extern "C" CHAOS_RUNTIME_ABI_EXPORT const RuntimeInstantiationBridgeV0*
CHAOS_RUNTIME_ABI_CALL ChaosRuntimeInstantiationGetBridgeV0(void) {
    return chaos::il2cpp::runtime_instantiation::GetBridgeV0();
}
