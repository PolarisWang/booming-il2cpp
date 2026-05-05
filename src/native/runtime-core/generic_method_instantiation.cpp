#include "generic_method_instantiation.h"
#include "il_to_ir_lowerer.h"       // LowerILToIR
#include "runtime_instantiation.h"  // AllocateRuntimeToken

#include <chaos/native_types.h>
#include <chaos/trace.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <new>      // placement new
#include <string>

namespace chaos::il2cpp::runtime_instantiation {

namespace {

/* ── Helper: compute a short display name for a type argument handle. ── */

static const char* GetTypeDisplayName(TypeInfoHandle handle) {
    if (handle == 0u) {
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

/* ── Helper: copy a C string into a heap-allocated buffer. ── */

static char* StrDup(const char* src) {
    if (src == nullptr) return nullptr;
    CHAOS_IL2CPP_SIZE len = std::strlen(src);
    auto* buf = static_cast<char*>(std::malloc(len + 1));
    if (buf == nullptr) return nullptr;
    std::memcpy(buf, src, len + 1);
    return buf;
}

}  // anonymous namespace

// ════════════════════════════════════════════════════════════════════════════
// Public API
// ════════════════════════════════════════════════════════════════════════════

RuntimeInstantiatedMethod* CreateClosedMethodDescriptor(
    MethodInfoHandle        open_method_definition,
    const TypeInfoHandle*   type_args,
    CHAOS_IL2CPP_UINT32    arg_count)
{
    CHAOS_IL2CPP_TRACE("runtime", "CreateClosedMethodDescriptor", "\"arg_count\"=%u", arg_count);
    if (open_method_definition == 0u || type_args == nullptr || arg_count == 0u) {
        return nullptr;
    }

    /* Decode the open method descriptor.  Must be a tag-encoded
     * ReflectionQueryMethodDescriptor pointer (the codegen bridge returns
     * handles in this format for generic method definitions). */
    const auto* open_desc = chaos::il2cpp::runtime_core::TryDecodeReflectionQueryMethodHandle(
        open_method_definition);
    if (open_desc == nullptr) {
        return nullptr;  // Not a reflection-query method; cannot instantiate.
    }

    // ── Allocate RuntimeInstantiatedMethod ──
    auto* rt_method = static_cast<RuntimeInstantiatedMethod*>(
        std::calloc(1u, sizeof(RuntimeInstantiatedMethod)));
    if (rt_method == nullptr) {
        return nullptr;
    }

    // ── Build closed subject_id ──
    // Format: "OpenMethodSubjectId[arg1,arg2,...]"
    {
        std::string subject_id;
        if (open_desc->subject_id_utf8 != nullptr) {
            subject_id += open_desc->subject_id_utf8;
        }
        subject_id += '[';
        for (CHAOS_IL2CPP_UINT32 i = 0u; i < arg_count; ++i) {
            if (i > 0u) subject_id += ',';
            subject_id += GetTypeDisplayName(type_args[i]);
        }
        subject_id += ']';
        rt_method->descriptor.subject_id_utf8 = StrDup(subject_id.c_str());
        if (rt_method->descriptor.subject_id_utf8 == nullptr) {
            std::free(rt_method);
            return nullptr;
        }
    }

    // ── Copy descriptor fields from the open method ──
    rt_method->descriptor.metadata_token = AllocateRuntimeToken();
    rt_method->descriptor.name_utf8 = StrDup(open_desc->name_utf8);
    rt_method->descriptor.member_type_utf8 = StrDup(open_desc->member_type_utf8);

    // Reference the open method's parameter descriptors directly.
    // (Phase 5b may substitute type arguments in parameter descriptors.)
    rt_method->descriptor.parameter_count = open_desc->parameter_count;
    rt_method->descriptor.parameters = open_desc->parameters;
    rt_method->descriptor.parameter_descriptor_count =
        open_desc->parameter_descriptor_count;

    // ── Copy type_args ──
    auto* args_buf = static_cast<TypeInfoHandle*>(
        std::malloc(sizeof(TypeInfoHandle) * arg_count));
    if (args_buf == nullptr) {
        std::free(const_cast<char*>(rt_method->descriptor.subject_id_utf8));
        std::free(const_cast<char*>(rt_method->descriptor.name_utf8));
        std::free(const_cast<char*>(rt_method->descriptor.member_type_utf8));
        std::free(rt_method);
        return nullptr;
    }
    std::memcpy(args_buf, type_args, sizeof(TypeInfoHandle) * arg_count);
    rt_method->type_args  = args_buf;
    rt_method->arg_count  = arg_count;
    rt_method->open_method_definition = open_method_definition;
    rt_method->module_id  = 0u;  // AOT root by default
    rt_method->ir_method_body = nullptr;

    return rt_method;
}

// ════════════════════════════════════════════════════════════════════════════
// Deferred IL→IR lowering
// ════════════════════════════════════════════════════════════════════════════

bool LowerMethodBody(
    RuntimeInstantiatedMethod*                      rt_method,
    const void*                                     il_bytes,
    CHAOS_IL2CPP_SIZE                               il_length,
    chaos::il2cpp::interpreter::ILTokenResolver      token_resolver,
    void*                                           user_data)
{
    CHAOS_IL2CPP_TRACE("runtime", "LowerMethodBody", "\"il_length\"=%zu", il_length);
    if (rt_method == nullptr) {
        return false;
    }

    // Already cached?
    if (rt_method->ir_method_body != nullptr) {
        return true;
    }

    if (il_bytes == nullptr || il_length == 0u) {
        return false;
    }

    // ── Attempt to parse as ECMA-335 method body header ──
    // Auto-detect tiny/fat format.  If il_bytes starts with a valid header,
    // extract the code body, max_stack, and code_size for correct SEH
    // section location.  If parsing fails, treat il_bytes as raw code body
    // (backward compat with callers that pre-strip the header).
    const uint8_t* raw = static_cast<const uint8_t*>(il_bytes);
    const uint8_t* code_body    = raw;
    CHAOS_IL2CPP_SIZE code_size = 0u;  // 0 = backward compat in LowerILToIR
    CHAOS_IL2CPP_SIZE buf_remaining = il_length;
    CHAOS_IL2CPP_UINT16 max_stack = 8u;

    interpreter::MethodBodyHeader header;
    if (interpreter::ParseMethodBodyHeader(il_bytes, il_length, header)) {
        code_body     = header.code_start;
        code_size     = header.code_size;
        max_stack     = header.max_stack;
        // Remaining buffer from code_start onward (code + padding + SEH)
        buf_remaining = il_length - static_cast<CHAOS_IL2CPP_SIZE>(
            code_body - raw);
    }

    // Lower IL → IR using the (possibly parsed) max_stack and code_size.
    interpreter::IRMethod method = interpreter::LowerILToIR(
        code_body, buf_remaining, code_size, max_stack,
        token_resolver, user_data);

    // Allocate heap-cached IRMethod via placement new
    // (IRMethod contains a std::vector and needs its constructor called).
    void* mem = std::malloc(sizeof(interpreter::IRMethod));
    if (mem == nullptr) {
        return false;
    }
    auto* cached = ::new (mem) interpreter::IRMethod(
        CHAOS_IL2CPP_MOVE(method));

    rt_method->il_bytes  = il_bytes;
    rt_method->il_length = il_length;
    rt_method->ir_method_body = cached;
    return true;
}

}  // namespace chaos::il2cpp::runtime_instantiation
