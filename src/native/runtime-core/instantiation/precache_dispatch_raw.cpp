namespace chaos::il2cpp::runtime_instantiation {

} // namespace chaos::il2cpp::runtime_instantiation

// Forward declaration for ResolveDirectFnSafe (defined in aot_direct_dispatch.cpp).
// Must be in the chaos::il2cpp::runtime_core namespace. Opening a brief scope
// here instead of inside runtime_instantiation to avoid namespace shadowing.
namespace chaos::il2cpp::runtime_core {
void* ResolveDirectFnSafe(const char* subject_id) noexcept;
}

namespace chaos::il2cpp::runtime_instantiation {

// ── PrecacheCallTarget ──────────────────────────────────────────────────────
// Pre-compute call metadata so InterpreterDispatchRaw can skip all reflection
// queries at runtime. Called during IR lowering (PatchMethodLowerIR).

CachedCallInfo PrecacheCallTarget(void* call_target) noexcept {
    using interpreter::ValueTag;

    CachedCallInfo info{};
    info.ret_tag = 0xFF;  // uninit sentinel

    const auto method = static_cast<MethodInfoHandle>(reinterpret_cast<uintptr_t>(call_target));
    if (method == 0u) return info;

    const auto* method_desc =
        runtime_core::TryDecodeReflectionQueryMethodHandle(method);
    if (method_desc == nullptr || method_desc->member_type_utf8 == nullptr) {
        return info;
    }

    ValueTag ret_tag = MapMemberTypeToValueTag(method_desc->member_type_utf8);

    // Check for struct (value type) return.
    if (ret_tag == ValueTag::ObjectRef) {
        TypeInfoHandle ret_type = ResolveParameterType(
            method_desc->member_type_utf8, nullptr, 0u);
        if (ret_type != 0u && IsValueTypeByHandle(ret_type)) {
            info.is_struct_ret = true;
            auto* engine = layout::GetLayoutEngine();
            const auto* layout = engine->GetOrComputeLayout(ret_type, nullptr, 0u);
            if (layout != nullptr && layout->value_size > 0u) {
                info.struct_size = static_cast<uint32_t>(layout->value_size);
            }
            info.ret_tag = static_cast<uint8_t>(ValueTag::Struct);
            return info;
        }
    }

    info.ret_tag = static_cast<uint8_t>(ret_tag);

    // ── MIC: Resolve direct native function pointer ──────────────────────────
    // When the target method has a non-patched AOT dispatch entry, cache its
    // direct_ptr so Handle_Call can skip InterpreterDispatchRaw/method_invoke.
    if (method_desc != nullptr && method_desc->subject_id_utf8 != nullptr &&
        method_desc->subject_id_utf8[0] != '\0') {
        const char* sid = method_desc->subject_id_utf8;
        // Format: "AssemblyName/Namespace.TypeName:MethodName(Params...)"
        const char* type_start = std::strchr(sid, '/');
        if (type_start != nullptr) {
            ++type_start;
            const char* method_start = std::strstr(type_start, "::");
            if (method_start != nullptr) {
                const char* tname_start = type_start;
                for (const char* p = type_start; p < method_start; ++p) {
                    if (*p == '.') tname_start = p + 1;
                }
                std::string ns;
                if (tname_start > type_start) {
                    ns.assign(type_start, tname_start - type_start - 1);
                }
                std::string type_name(tname_start, method_start - tname_start);
                const char* paren = std::strchr(method_start + 2, '(');
                std::string method_name;
                if (paren != nullptr) {
                    method_name.assign(method_start + 2, paren - method_start - 2);
                } else {
                    method_name.assign(method_start + 2);
                }
                if (!type_name.empty() && !method_name.empty()) {
                    auto& registry = runtime_core::GetHotpatchNameRegistry();
                    uint64_t lookup = registry.LookupMethod(
                        ns.c_str(), type_name.c_str(), method_name.c_str());
                    if (lookup != 0) {
                        info.module_id = runtime_core::ExtractModuleId(lookup);
                        uint32_t token = runtime_core::ExtractToken(lookup);
                        info.method_token = token;
                        info.slot = registry.TokenToSlot(info.module_id, token);
                        if (info.slot != ~0u) {
                            auto* entry = registry.GetDispatchEntryBySlot(
                                info.module_id, info.slot);
                            if (entry != nullptr) {
                                // Acquire fence pairs with SetPatchedBySlot release.
                                std::atomic_thread_fence(std::memory_order_acquire);
                                bool is_active = (entry->flags & kHotpatchActive) != 0;
                                if (!is_active && entry->direct_ptr != nullptr) {
                                    info.direct_ptr = entry->direct_ptr;
                                    info.is_patched = false;
                                } else if (is_active) {
                                    info.is_patched = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Phase 2.2: Fallback to three-tier ResolveDirectFnSafe when manual
    // subjectId parsing + HotpatchNameRegistry lookup missed (handles
    // cross-module calls, external runtime function table entries, etc.).
    if (info.direct_ptr == nullptr && !info.is_struct_ret &&
        method_desc != nullptr && method_desc->subject_id_utf8 != nullptr) {
        info.direct_ptr = runtime_core::ResolveDirectFnSafe(method_desc->subject_id_utf8);
    }

    return info;
}

// ── SEH-safe direct thunk call (separate function avoids C2712) ──────────
// __try/__except cannot coexist with C++ object unwinding in MSVC, so the
// raw AOT thunk call is isolated to this helper.
// Uniform 8-arg AOT thunk signature matching fn(a0...a7) calling convention.
using DirectFn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                              uint64_t, uint64_t, uint64_t, uint64_t);

#if defined(_WIN32)
static uint64_t CallDirectFnSehSafe(
    DirectFn fn,
    uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
    uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7,
    bool* out_caught) noexcept
{
    uint64_t ret = 0;
    __try {
        ret = fn(a0, a1, a2, a3, a4, a5, a6, a7);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        *out_caught = true;
    }
    return ret;
}
#endif


// ── InterpreterDispatchRaw ──────────────────────────────────────────────────
// Optimized dispatch that skips the InterpreterValue[] round-trip.
// Takes raw uint64_t values + ValueTag tags directly from FastFrame stack,
// builds ABI argv[] directly, calls method_invoke, and returns raw result.
//
// Two key optimizations:
//   1) Stack-allocated argv/raw_data for arg_count ≤ 8 (no heap allocation).
//   2) CachedCallInfo skips all TryDecodeReflectionQueryMethodHandle +
//      ResolveParameterType + LayoutEngine queries on each call.
//
// Called by Handle_Call in fast_dispatch.cpp (Layer 4 optimization).

RawDispatchResult InterpreterDispatchRaw(
    void*                               call_target,
    const uint64_t*                     raw_args,
    const uint8_t*                      arg_tags,
    CHAOS_IL2CPP_UINT32                arg_count,
    bool                                is_instance_call,
    void*                               dispatch_context,
    const CachedCallInfo*  cache_info)
{
    using interpreter::ValueTag;

    CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterDispatchRaw");

    RawDispatchResult result = {};
    auto* ctx = static_cast<InterpreterDispatchContext*>(dispatch_context);
    if (ctx == nullptr || ctx->runtime_state == nullptr) {
        return result;
    }

    const MethodInfoHandle method = reinterpret_cast<MethodInfoHandle>(call_target);
    if (method == 0u) {
        return result;
    }

    // ── Depth guard ──
    if (ctx->recursion_depth >= kInterpreterMaxDispatchDepth) {
        result.threw_exception = true;
        result.exception_obj = nullptr;
        return result;
    }
    ++ctx->recursion_depth;

    // ── Separate 'this' pointer ──
    void* this_ptr = nullptr;
    CHAOS_IL2CPP_UINT32 param_count = arg_count;
    if (is_instance_call && arg_count > 0u) {
        auto tag = static_cast<ValueTag>(arg_tags[0]);
        this_ptr = (tag == ValueTag::ObjectRef || tag == ValueTag::Null)
            ? reinterpret_cast<void*>(raw_args[0]) : nullptr;
        param_count = arg_count - 1u;
    }

    // ── Build ABI argument array from raw values ──
    // Use stack buffers for small arg counts to avoid heap allocation.
    void*   argv_stack[8];
    uint64_t raw_data_stack[8];
    void**  argv = (param_count <= 8) ? argv_stack : nullptr;
    uint64_t* raw_data_buf = (param_count <= 8) ? raw_data_stack : nullptr;
    CHAOS_IL2CPP_VECTOR(void*)   argv_heap;
    CHAOS_IL2CPP_VECTOR(uint64_t) raw_heap;

    if (param_count > 8) {
        argv_heap.resize(param_count, nullptr);
        raw_heap.resize(param_count, 0);
        argv = argv_heap.data();
        raw_data_buf = raw_heap.data();
    }

    for (CHAOS_IL2CPP_UINT32 ai = 0u, si = (is_instance_call ? 1u : 0u);
         si < arg_count; ++ai, ++si) {
        auto tag = static_cast<ValueTag>(arg_tags[si]);
        switch (tag) {
            case ValueTag::Int32:
            case ValueTag::Int64:
            case ValueTag::Float32:
            case ValueTag::Float64:
            case ValueTag::ObjectRef:
            case ValueTag::Null:
            case ValueTag::ManagedPtr:
                raw_data_buf[ai] = raw_args[si];
                argv[ai] = &raw_data_buf[ai];
                break;
            case ValueTag::Struct:
                argv[ai] = reinterpret_cast<void*>(raw_args[si]);
                break;
            default:
                argv[ai] = nullptr;
                break;
        }
    }

    // ── Determine return type and prepare buffer ──
    // Use pre-cached info when available (L1: cache_info from Handle_Call).
    // Fall back to runtime reflection (L2: original path).
    bool    is_struct_ret  = false;
    uint32_t struct_size   = 0;
    ValueTag ret_tag       = ValueTag::Void;
    bool    use_cache      = (cache_info != nullptr && cache_info->ret_tag != 0xFF);

    if (use_cache) {
        is_struct_ret = cache_info->is_struct_ret;
        struct_size   = cache_info->struct_size;
        ret_tag       = static_cast<ValueTag>(cache_info->ret_tag);
    } else {
        const auto* method_desc =
            runtime_core::TryDecodeReflectionQueryMethodHandle(method);
        if (method_desc != nullptr && method_desc->member_type_utf8 != nullptr) {
            ret_tag = MapMemberTypeToValueTag(method_desc->member_type_utf8);
            if (ret_tag == ValueTag::ObjectRef) {
                TypeInfoHandle ret_type = ResolveParameterType(
                    method_desc->member_type_utf8, nullptr, 0u);
                if (ret_type != 0u && IsValueTypeByHandle(ret_type)) {
                    is_struct_ret = true;
                    auto* engine = layout::GetLayoutEngine();
                    const auto* layout = engine->GetOrComputeLayout(ret_type, nullptr, 0u);
                    if (layout != nullptr && layout->value_size > 0u) {
                        struct_size = static_cast<uint32_t>(layout->value_size);
                    }
                    ret_tag = ValueTag::Struct;
                }
            }
        }
    }

    // ── MIC: Direct AOT thunk call ───────────────────────────────────────────
    // When cache_info has a valid direct_ptr (pre-resolved dispatch entry with
    // native AOT function), call the AOT thunk directly using the managed calling
    // convention fn(a0...a7).  This completely bypasses MethodInvoke, which
    // cannot handle encoded reflection handles (returns NOT_FOUND for token=0).
    //
    // This is used by the RegisterExecute path which has no direct_fn field in
    // RegisterInstruction and must go through InterpreterDispatchRaw.
    // Handle_Call/DoMIC in fast_dispatch.cpp already uses this same pattern.
    //
    // The AOT thunk expects all managed args (including 'this' for instance
    // calls) as raw uint64_t values via the uniform 8-arg signature.  Struct
    // returns and >8 arg cases fall through to MethodInvoke.
    if (use_cache && cache_info->direct_ptr != nullptr && !cache_info->is_patched &&
        ret_tag != ValueTag::Struct && arg_count <= 8) {
        // Skip MIC for instance methods where 'this' is a value type.
        // AOT thunks expect value type 'this' as a managed pointer (e.g.
        // Int32::ToString calls chaos_resolve_native_int_slot(ptr) which
        // dereferences the pointer).  Passing the raw value crashes.
        bool value_type_this = is_instance_call && arg_count > 0 &&
            static_cast<ValueTag>(arg_tags[0]) != ValueTag::ObjectRef &&
            static_cast<ValueTag>(arg_tags[0]) != ValueTag::Null &&
            static_cast<ValueTag>(arg_tags[0]) != ValueTag::ManagedPtr;
        if (!value_type_this) {
            auto fn = reinterpret_cast<DirectFn>(cache_info->direct_ptr);
            uint64_t a0 = (arg_count > 0) ? raw_args[0] : 0;
            uint64_t a1 = (arg_count > 1) ? raw_args[1] : 0;
            uint64_t a2 = (arg_count > 2) ? raw_args[2] : 0;
            uint64_t a3 = (arg_count > 3) ? raw_args[3] : 0;
            uint64_t a4 = (arg_count > 4) ? raw_args[4] : 0;
            uint64_t a5 = (arg_count > 5) ? raw_args[5] : 0;
            uint64_t a6 = (arg_count > 6) ? raw_args[6] : 0;
            uint64_t a7 = (arg_count > 7) ? raw_args[7] : 0;
            uint64_t ret_scalar = 0;
            bool seh_caught = false;
#if defined(_WIN32)
            ret_scalar = CallDirectFnSehSafe(
                reinterpret_cast<DirectFn>(cache_info->direct_ptr),
                a0, a1, a2, a3, a4, a5, a6, a7, &seh_caught);
#else
            ret_scalar = reinterpret_cast<DirectFn>(cache_info->direct_ptr)(
                a0, a1, a2, a3, a4, a5, a6, a7);
#endif
            if (seh_caught) {
                --ctx->recursion_depth;
                result.threw_exception = true;
                result.exception_obj = nullptr;
                return result;
            }

            if (ret_tag != ValueTag::Void) {
                result.has_value = true;
                result.tag = static_cast<uint8_t>(ret_tag);
                result.value = ret_scalar;
            }
            --ctx->recursion_depth;
            return result;
        }
        // value_type_this: fall through to MethodInvoke
    }

    // ── Call MethodInvoke ──
    uint64_t ret_scalar = 0;
    void*    ret_buf = &ret_scalar;
    size_t   ret_size = sizeof(ret_scalar);

    if (is_struct_ret && struct_size > 0u) {
        result.struct_data = CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(struct_size);
        if (result.struct_data != nullptr) {
            void* buf_ptr = result.struct_data;
            ret_buf = &buf_ptr;
            ret_size = struct_size;
        } else {
            is_struct_ret = false;
        }
    }

    const auto* abi = runtime_core::GetRuntimeAbiV0();
    if (abi == nullptr || abi->method_invoke == nullptr) {
        --ctx->recursion_depth;
        return result;
    }
    ExceptionHandle ex = nullptr;
    const RuntimeStatus status = abi->method_invoke(
        ctx->runtime_state, ctx->thread_state,
        method, this_ptr,
        argv, param_count,
        ret_buf, ret_size, &ex);

    if (status == CHAOS_RUNTIME_STATUS_MANAGED_EXCEPTION) {
        result.threw_exception = true;
        result.exception_obj = ex;
        --ctx->recursion_depth;
        return result;
    }
    if (status != CHAOS_RUNTIME_STATUS_OK) {
        --ctx->recursion_depth;
        return result;
    }

    // ── Convert return value ──
    if (ret_tag != ValueTag::Void) {
        result.has_value = true;
        result.tag = static_cast<uint8_t>(ret_tag);

        switch (ret_tag) {
            case ValueTag::Int32:
            case ValueTag::Int64:
            case ValueTag::Float64:
                result.value = ret_scalar;
                break;
            case ValueTag::Float32:
                result.value = ret_scalar;
                break;
            case ValueTag::ObjectRef:
            case ValueTag::Null: {
                void* obj_ptr = nullptr;
                std::memcpy(&obj_ptr, &ret_scalar, sizeof(obj_ptr));
                result.value = reinterpret_cast<uint64_t>(obj_ptr);
                break;
            }
            case ValueTag::Struct:
                if (result.struct_data != nullptr) {
                    result.struct_size = static_cast<uint32_t>(struct_size);
                    result.tag = static_cast<uint8_t>(ValueTag::Struct);
                }
                break;
            default:
                result.has_value = false;
                break;
        }
    } else if (!use_cache) {
        result.has_value = true;
        result.tag = static_cast<uint8_t>(ValueTag::Int32);
        result.value = ret_scalar;
    }

    --ctx->recursion_depth;
    return result;
}

}  // namespace