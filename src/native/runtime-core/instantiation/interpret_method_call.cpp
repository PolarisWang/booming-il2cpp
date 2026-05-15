namespace chaos::il2cpp::runtime_instantiation {

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
        if (rt_method->aot_core_ir_json != nullptr && rt_method->aot_core_ir_json_length > 0u) {
            if (!LowerMethodBody(rt_method, rt_method->aot_core_ir_json,
                    rt_method->aot_core_ir_json_length,
                    nullptr, nullptr))
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
        chaos::il2cpp::runtime_core::chaos_raise_exception(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ex_handle));
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

}  // namespace