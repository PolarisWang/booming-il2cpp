namespace chaos::il2cpp::runtime_instantiation {

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

    CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterDispatch");

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
    //
    // Small-buffer optimization: stack-allocate for ≤8 args (common case),
    // heap-allocate only for methods with unusual parameter counts.
    void* argv_stack[8];
    CHAOS_IL2CPP_UINT64 raw_data_stack[8];
    void** argv;
    CHAOS_IL2CPP_UINT64* raw_data;
    bool heap_allocated = false;
    if (param_count <= 8) {
        argv = argv_stack;
        raw_data = raw_data_stack;
    } else {
        argv = new void*[param_count]();
        raw_data = new CHAOS_IL2CPP_UINT64[param_count]();
        heap_allocated = true;
    }

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
    // Scalar returns use a stack uint64; struct returns allocate a temp buffer.
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

    // Cleanup heap-allocated arg arrays (SBO fallback)
    if (heap_allocated) {
        delete[] argv;
        delete[] raw_data;
    }
    return result;
}

}  // namespace