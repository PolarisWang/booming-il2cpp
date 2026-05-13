#include "generic_method_instantiation.h"
#include "aot_core_ir_reader.h"   // DeserializeAotCoreIrMethod
#include "gc_helpers.h"           // DomainStrDup
#include "memory_domain.h"        // CurrentDomain, CHAOS_IL2CPP_DOMAIN_CURRENT_*
#include "runtime_core.h"         // RuntimeMode, RuntimeStatus for gc_helpers.h
#include "instantiation_engine.h" // AllocateRuntimeToken

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

/* ── Helper: copy a C string into a domain-allocated buffer. ── */

static char* StrDup(const char* src) {
    if (src == nullptr) return nullptr;
    CHAOS_IL2CPP_SIZE len = std::strlen(src);
    auto* buf = static_cast<char*>(CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(len + 1));
    if (buf == nullptr) return nullptr;
    CHAOS_IL2CPP_MEMCPY(buf, src, len + 1);
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
    CHAOS_IL2CPP_LOG_TRACE("runtime", "CreateClosedMethodDescriptor", "\"arg_count\"=%u", arg_count);
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
        CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(sizeof(RuntimeInstantiatedMethod)));
    if (rt_method == nullptr) {
        return nullptr;
    }
    CHAOS_IL2CPP_MEMSET(rt_method, 0, sizeof(RuntimeInstantiatedMethod));

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
        rt_method->descriptor.subject_id_utf8 = CHAOS_IL2CPP_DOMAIN_CURRENT_STRDUP(subject_id.c_str());
        if (rt_method->descriptor.subject_id_utf8 == nullptr) {
            CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(rt_method);
            return nullptr;
        }
    }

    // ── Copy descriptor fields from the open method ──
    rt_method->descriptor.metadata_token = AllocateRuntimeToken();
    rt_method->descriptor.name_utf8 = StrDup(open_desc->name_utf8);
    rt_method->descriptor.member_type_utf8 = StrDup(open_desc->member_type_utf8);

    // Reference the open method's parameter descriptors directly.
    rt_method->descriptor.parameter_count = open_desc->parameter_count;
    rt_method->descriptor.parameters = open_desc->parameters;
    rt_method->descriptor.parameter_descriptor_count =
        open_desc->parameter_descriptor_count;

    // ── Copy type_args ──
    auto* args_buf = static_cast<TypeInfoHandle*>(
        CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(sizeof(TypeInfoHandle) * arg_count));
    if (args_buf == nullptr) {
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(const_cast<char*>(rt_method->descriptor.subject_id_utf8));
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(const_cast<char*>(rt_method->descriptor.name_utf8));
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(const_cast<char*>(rt_method->descriptor.member_type_utf8));
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(rt_method);
        return nullptr;
    }
    CHAOS_IL2CPP_MEMCPY(args_buf, type_args, sizeof(TypeInfoHandle) * arg_count);
    rt_method->type_args  = args_buf;
    rt_method->arg_count  = arg_count;
    rt_method->open_method_definition = open_method_definition;
    rt_method->module_id  = 0u;  // AOT root by default
    rt_method->ir_method_body = nullptr;

    return rt_method;
}

// ════════════════════════════════════════════════════════════════════════════
// Deferred AotCoreIr JSON → IR deserialization
// ════════════════════════════════════════════════════════════════════════════

bool LowerMethodBody(
    RuntimeInstantiatedMethod*                      rt_method,
    const char*                                     aot_core_ir_json,
    CHAOS_IL2CPP_SIZE                               aot_core_ir_json_length,
    chaos::il2cpp::runtime_core::ResolveSubjectIdFn  resolve_fn,
    void*                                           resolve_ctx)
{
    CHAOS_IL2CPP_LOG_TRACE("runtime", "LowerMethodBody", "\"json_length\"=%zu", aot_core_ir_json_length);
    if (rt_method == nullptr) {
        return false;
    }

    // Already cached?
    if (rt_method->ir_method_body != nullptr) {
        return true;
    }

    if (aot_core_ir_json == nullptr || aot_core_ir_json_length == 0u) {
        return false;
    }

    // Deserialize AotCoreIr JSON → IRMethod.
    interpreter::IRMethod method = chaos::il2cpp::runtime_core::DeserializeAotCoreIrMethod(
        aot_core_ir_json,
        aot_core_ir_json_length,
        resolve_fn,
        resolve_ctx);

    // Allocate heap-cached IRMethod via placement new
    // (IRMethod contains a std::vector and needs its constructor called).
    void* mem = CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(sizeof(interpreter::IRMethod));
    if (mem == nullptr) {
        return false;
    }
    auto* cached = ::new (mem) interpreter::IRMethod(
        CHAOS_IL2CPP_MOVE(method));

    rt_method->aot_core_ir_json = aot_core_ir_json;
    rt_method->aot_core_ir_json_length = aot_core_ir_json_length;
    rt_method->ir_method_body = cached;
    return true;
}

}  // namespace chaos::il2cpp::runtime_instantiation