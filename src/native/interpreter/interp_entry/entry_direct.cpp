namespace chaos::il2cpp::runtime_core {

// ── InterpreterEntryDirect ──────────────────────────────────────────────

// Forward declarations (defined in other sub-files)
static void CacheSignature(PatchMethod* patch_method) noexcept;
static interpreter::InterpreterValue ReadTypedArg(
    ArgBuffer& reader, interpreter::ValueTag tag) noexcept;
static void WriteTypedRet(void* ret_buf, const interpreter::ExecutionResult& result,
                           interpreter::ValueTag ret_tag) noexcept;

void InterpreterEntryDirect(
    uintptr_t method_key,
    void*     args_buf,
    void*     ret_buf) {

    CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect");

    if (method_key == 0) return;

    auto* patch_method = reinterpret_cast<PatchMethod*>(method_key);

    // A2.3: Increment call count for hot path detection.
    patch_method->call_count.fetch_add(1, std::memory_order_relaxed);

    // Step 1: Lazy AotCoreIr JSON → IR deserialization.
    {
    CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect.Step1_LowerIR");
    PatchMethodLowerIR(method_key);
    }

    // ── Step B: FastExecute path (Layer 1+2) ─────────────────────────
    auto* ir = static_cast<interpreter::IRMethod*>(patch_method->cached_ir);
    if (ir == nullptr) {
        return;  // Deserialization failed — nothing to execute.
    }
    if (ir->instructions.empty()) {
        return;
    }
    const auto instr_count = ir->instructions.size();
    if (instr_count == 1) {
        // Single instruction: must be Ret (empty IR fallback, no AotCoreIr in patchdata).
        return;
    }
    if (instr_count == 2 && ir->seh_clauses.empty()) {
        CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect.Step1c_2InstrFastPath");
        const auto& op0 = ir->instructions[0];
        const auto& op1 = ir->instructions[1];
        if (op1.op_code == interpreter::IROpCode::Ret) {
            // Cache signature on first call if not already done.
            if (!patch_method->cached_sig_valid) {
                CacheSignature(patch_method);
            }

            if (op0.op_code == interpreter::IROpCode::LdArg) {
                // Forward first argument to return buffer.
                // Use type-aware forwarding when cached signature is available.
                if (ret_buf != nullptr) {
                    ArgBuffer args(args_buf);
                    ArgBuffer ret(ret_buf);
                    if (patch_method->cached_sig_valid) {
                        auto ret_tag = static_cast<interpreter::ValueTag>(
                            patch_method->cached_ret_tag);
                        // For LdArg+Ret where arg 0 is the return source, read
                        // from args with the return type's read method.
                        switch (ret_tag) {
                        case interpreter::ValueTag::Int32:
                            ret.WriteI32(args.ReadI32()); return;
                        case interpreter::ValueTag::Int64:
                            ret.WriteI64(args.ReadI64()); return;
                        case interpreter::ValueTag::Float32:
                            ret.WriteF32(args.ReadF32()); return;
                        case interpreter::ValueTag::Float64:
                            ret.WriteF64(args.ReadF64()); return;
                        default:
                            ret.WritePtr(args.ReadPtr()); return;
                        }
                    }
                    // Fallback: pointer forwarding.
                    ret.WritePtr(args.ReadPtr());
                }
                return;
            }
            if (op0.op_code == interpreter::IROpCode::LdcI4) {
                if (ret_buf != nullptr) {
                    ArgBuffer ret(ret_buf);
                    ret.WriteI32(op0.immediate_i4);
                }
                return;
            }
            if (op0.op_code == interpreter::IROpCode::LdNull) {
                if (ret_buf != nullptr) {
                    ArgBuffer ret(ret_buf);
                    ret.WritePtr(nullptr);
                }
                return;
            }
            // A3.2: Additional 2-instr fast-path patterns
            if (op0.op_code == interpreter::IROpCode::LdLoc) {
                // LdLoc(n)+Ret: forward local variable n to return buffer.
                // Local n is stored in PatchMethod::cached_arg_types or
                // must be read from the method's local var metadata.
                // For simple forwarding, treat as pointer (caller will cast).
                if (ret_buf != nullptr && patch_method->cached_sig_valid) {
                    auto ret_tag = static_cast<interpreter::ValueTag>(
                        patch_method->cached_ret_tag);
                    ArgBuffer ret(ret_buf);
                    switch (ret_tag) {
                    case interpreter::ValueTag::Int32:
                        ret.WriteI32(0); return;
                    case interpreter::ValueTag::Int64:
                        ret.WriteI64(0); return;
                    case interpreter::ValueTag::Float32: {
                        float v = 0.0f;
                        ret.WriteF32(v); return;
                    }
                    case interpreter::ValueTag::Float64: {
                        double v = 0.0;
                        ret.WriteF64(v); return;
                    }
                    default:
                        ret.WritePtr(nullptr); return;
                    }
                }
                return;
            }
            if (op0.op_code == interpreter::IROpCode::LdcI8) {
                if (ret_buf != nullptr) {
                    ArgBuffer ret(ret_buf);
                    ret.WriteI64(op0.immediate_i8);
                }
                return;
            }
            if (op0.op_code == interpreter::IROpCode::LdcR4) {
                if (ret_buf != nullptr) {
                    ArgBuffer ret(ret_buf);
                    float v;
                    std::memcpy(&v, &op0.immediate_i4, sizeof(v));
                    ret.WriteF32(v);
                }
                return;
            }
            if (op0.op_code == interpreter::IROpCode::LdcR8) {
                if (ret_buf != nullptr) {
                    ArgBuffer ret(ret_buf);
                    ret.WriteF64(op0.immediate_r8);
                }
                return;
            }
            if (op0.op_code == interpreter::IROpCode::Neg) {
                // Neg+Ret: compute -arg0 for any numeric type
                if (ret_buf != nullptr && patch_method->cached_sig_valid) {
                    auto ret_tag = static_cast<interpreter::ValueTag>(
                        patch_method->cached_ret_tag);
                    ArgBuffer args(args_buf);
                    ArgBuffer ret(ret_buf);
                    switch (ret_tag) {
                    case interpreter::ValueTag::Int32:
                        ret.WriteI32(-args.ReadI32()); return;
                    case interpreter::ValueTag::Int64:
                        ret.WriteI64(-args.ReadI64()); return;
                    case interpreter::ValueTag::Float32:
                        ret.WriteF32(-args.ReadF32()); return;
                    case interpreter::ValueTag::Float64:
                        ret.WriteF64(-args.ReadF64()); return;
                    default:
                        break;  // Fall through to normal execution.
                    }
                }
                // Fall through if no cached_sig or unsupported type.
            }
            if (op0.op_code == interpreter::IROpCode::Not) {
                // Not+Ret: compute ~arg0 (implicit LdArg(0) + Not)
                if (ret_buf != nullptr && patch_method->cached_sig_valid) {
                    auto ret_tag = static_cast<interpreter::ValueTag>(
                        patch_method->cached_ret_tag);
                    ArgBuffer args(args_buf);
                    ArgBuffer ret(ret_buf);
                    switch (ret_tag) {
                    case interpreter::ValueTag::Int32:
                        ret.WriteI32(~args.ReadI32()); return;
                    case interpreter::ValueTag::Int64:
                        ret.WriteI64(~args.ReadI64()); return;
                    default:
                        break;
                    }
                }
            }
            if (op0.op_code == interpreter::IROpCode::Conv_I4) {
                if (ret_buf != nullptr && patch_method->cached_sig_valid) {
                    ArgBuffer args(args_buf);
                    ArgBuffer ret(ret_buf);
                    ret.WriteI32(static_cast<int32_t>(args.ReadI64()));
                }
                return;
            }
            if (op0.op_code == interpreter::IROpCode::Conv_I8) {
                if (ret_buf != nullptr && patch_method->cached_sig_valid) {
                    ArgBuffer args(args_buf);
                    ArgBuffer ret(ret_buf);
                    ret.WriteI64(static_cast<int64_t>(args.ReadI64()));
                }
                return;
            }
            if (op0.op_code == interpreter::IROpCode::Conv_R4) {
                if (ret_buf != nullptr && patch_method->cached_sig_valid) {
                    ArgBuffer args(args_buf);
                    ArgBuffer ret(ret_buf);
                    ret.WriteF32(static_cast<float>(args.ReadF64()));
                }
                return;
            }
            if (op0.op_code == interpreter::IROpCode::Conv_R8) {
                if (ret_buf != nullptr && patch_method->cached_sig_valid) {
                    ArgBuffer args(args_buf);
                    ArgBuffer ret(ret_buf);
                    ret.WriteF64(static_cast<double>(args.ReadF64()));
                }
                return;
            }
        }
    }

    // ── Step B: RegisterExecute path (Layer R) ──────────────────────
    // For methods WITHOUT SEH, use register-based execution (16-byte instrs,
    // explicit dst/src regs).  Faster than stack-based FastExecute.
    auto* reg_method = static_cast<interpreter::RegisterMethod*>(
        patch_method->cached_reg_method);
    if (reg_method != nullptr && reg_method->seh_clauses.empty() &&
        instr_count > 2 && interpreter::CanRegisterExecute(*reg_method)) {
        CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect.RegisterExecute");

        if (!patch_method->cached_sig_valid) {
            CacheSignature(patch_method);
        }

        // Set up register frame on the stack (no pool needed — RegisterFrame is smaller).
        interpreter::RegisterFrame rf = {};
        auto* runtime_state = GetCurrentRuntimeState();
        auto* thread_state  = GetCurrentThreadState();
        rf.args      = args_buf;
        rf.arg_count = patch_method->cached_sig_valid
                       ? patch_method->cached_arg_count : 0;
        rf.dispatch_fn = reinterpret_cast<void*>(
            runtime_instantiation::InterpreterDispatch);
        runtime_instantiation::InterpreterDispatchContext reg_dispatch_ctx;
        reg_dispatch_ctx.runtime_state = runtime_state;
        reg_dispatch_ctx.thread_state  = thread_state;
        rf.dispatch_ctx = &reg_dispatch_ctx;
        rf.call_cache   = patch_method->call_cache;
        rf.call_count   = static_cast<uint32_t>(instr_count);

        bool ok = interpreter::RegisterExecute(
            rf,
            reg_method->instructions.data(),
            static_cast<uint32_t>(reg_method->instructions.size()));

        if (ok) {
            if (rf.has_ret && ret_buf != nullptr) {
                auto ret_tag = static_cast<interpreter::ValueTag>(rf.ret_tag);
                ArgBuffer ret_writer(ret_buf);
                switch (ret_tag) {
                case interpreter::ValueTag::Int32:
                    ret_writer.WriteI32(static_cast<int32_t>(rf.ret_val));
                    return;
                case interpreter::ValueTag::Int64:
                    ret_writer.WriteI64(static_cast<int64_t>(rf.ret_val));
                    return;
                case interpreter::ValueTag::Float32: {
                    float v;
                    std::memcpy(&v, &rf.ret_val, sizeof(float));
                    ret_writer.WriteF32(v);
                    return;
                }
                case interpreter::ValueTag::Float64: {
                    double v;
                    std::memcpy(&v, &rf.ret_val, sizeof(double));
                    ret_writer.WriteF64(v);
                    return;
                }
                default:
                    ret_writer.WritePtr(reinterpret_cast<void*>(rf.ret_val));
                    return;
                }
            }
            return;
        }
        // RegisterExecute failed (unsupported opcode) — fall through to FastExecute.
    }

    // ── Step C: FastExecute path (Layer 1+2) ─────────────────────────
    // For methods WITHOUT SEH, use function-pointer dispatch + FastFrame.
    if (ir->seh_clauses.empty() && instr_count > 2) {
        CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect.FastExecute");

        if (!patch_method->cached_sig_valid) {
            CacheSignature(patch_method);
        }

        // Acquire frame from TLS pool (avoids ~416-byte memset ~200ns).
        FastFrame* ff = tls_frame_pool.Acquire();
        FastFrame ff_fallback;
        bool using_pool = true;
        if (ff == nullptr) {
            ff = &ff_fallback;
            memset(ff, 0, sizeof(*ff));
            using_pool = false;
        }

        // Set up dispatch callback for Call instructions inside FastExecute.
        {
        CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect.SetupFrame");
        auto* runtime_state = GetCurrentRuntimeState();
        auto* thread_state  = GetCurrentThreadState();
        runtime_instantiation::InterpreterDispatchContext dispatch_ctx;
        dispatch_ctx.runtime_state = runtime_state;
        dispatch_ctx.thread_state  = thread_state;

        // Lightweight frame setup — replaces manual field fills.
        SetupFastFrame(ff, patch_method, args_buf, ir,
                       reinterpret_cast<void*>(
                           runtime_instantiation::InterpreterDispatch),
                       &dispatch_ctx);
        }

        bool ok;
        {
        CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect.FastExecuteCall");
        ok = FastExecute(*ff,
                              ir->instructions.data(),
                              static_cast<uint32_t>(ir->instructions.size()));
        }
        if (ok) {
            if (ff->has_ret && ret_buf != nullptr) {
                auto ret_tag = static_cast<interpreter::ValueTag>(ff->ret_tag);
                ArgBuffer ret_writer(ret_buf);
                switch (ret_tag) {
                case interpreter::ValueTag::Int32:
                    ret_writer.WriteI32(static_cast<int32_t>(ff->ret_val));
                    if (using_pool) tls_frame_pool.Release(ff);
                    return;
                case interpreter::ValueTag::Int64:
                    ret_writer.WriteI64(static_cast<int64_t>(ff->ret_val));
                    if (using_pool) tls_frame_pool.Release(ff);
                    return;
                case interpreter::ValueTag::Float32: {
                    float v;
                    std::memcpy(&v, &ff->ret_val, sizeof(float));
                    ret_writer.WriteF32(v);
                    if (using_pool) tls_frame_pool.Release(ff);
                    return;
                }
                case interpreter::ValueTag::Float64: {
                    double v;
                    std::memcpy(&v, &ff->ret_val, sizeof(double));
                    ret_writer.WriteF64(v);
                    if (using_pool) tls_frame_pool.Release(ff);
                    return;
                }
                default:
                    ret_writer.WritePtr(reinterpret_cast<void*>(ff->ret_val));
                    if (using_pool) tls_frame_pool.Release(ff);
                    return;
                }
            }
            if (using_pool) tls_frame_pool.Release(ff);
            return;
        }

        // FastExecute failed (unsupported opcode) — fall through to VM.
        if (using_pool) {
            tls_frame_pool.Release(ff);
        }
    }

    // ── Step 2: Parse/cache method signature ─────────────────────────────
    CHAOS_IL2CPP_UINT32 arg_count = 0;
    bool type_aware_args = false;

    if (!patch_method->cached_sig_valid) {
        CacheSignature(patch_method);
    }

    if (patch_method->cached_sig_valid) {
        arg_count = patch_method->cached_arg_count;
        type_aware_args = true;
    } else {
        // Fallback: legacy signature parsing (arg_count only).
        if (patch_method->signature_blob != nullptr &&
            patch_method->signature_len > 1) {
            const uint8_t* sig = patch_method->signature_blob;
            if (patch_method->signature_blob[0] >= 2) {
                const uint8_t* sig_data = patch_method->signature_blob + 1;
                uint8_t cc = sig_data[0];
                uint8_t count_byte = sig_data[1];

                if (count_byte <= 0x7F) {
                    arg_count = count_byte;
                } else if (count_byte <= 0xBF) {
                    arg_count = static_cast<CHAOS_IL2CPP_UINT32>(
                        ((count_byte & 0x3F) << 8) | sig_data[2]);
                }

                if ((cc & 0x20) == 0x20) {
                    arg_count += 1;
                }
            }
        }
    }

    // Step 3: Build ExecutionFrame and populate arguments.
    interpreter::ExecutionFrame frame;

    ArgBuffer arg_reader(args_buf);
    frame.arguments.reserve(arg_count);

    if (type_aware_args) {
        // Type-aware push: use cached ValueTag per argument.
        for (CHAOS_IL2CPP_UINT32 i = 0; i < arg_count; ++i) {
            auto tag = (i < patch_method->cached_arg_capacity)
                ? static_cast<interpreter::ValueTag>(patch_method->cached_arg_types[i])
                : interpreter::ValueTag::ObjectRef;
            frame.arguments.push_back(ReadTypedArg(arg_reader, tag));
        }
    } else {
        // Legacy: all args as ObjectRef pointers.
        for (CHAOS_IL2CPP_UINT32 i = 0; i < arg_count; ++i) {
            void* raw = arg_reader.ReadPtr();
            frame.arguments.push_back(interpreter::InterpreterValue::from_obj(raw));
        }
    }

    frame.locals.reserve(8);

    // Set up dispatch callback for nested Call instructions.
    auto* runtime_state = GetCurrentRuntimeState();
    auto* thread_state  = GetCurrentThreadState();
    runtime_instantiation::InterpreterDispatchContext dispatch_ctx;
    dispatch_ctx.runtime_state = runtime_state;
    dispatch_ctx.thread_state  = thread_state;
    frame.dispatch_fn     = runtime_instantiation::InterpreterDispatch;
    frame.dispatch_context = &dispatch_ctx;

    // Step 4: Execute via InterpreterVM.
    interpreter::ExecutionResult result;
    {
        interpreter::InterpreterVM vm;
        result = vm.Execute(*ir, &frame);
    }

    // Step 5: Write return value to ret_buf.
    if (ret_buf != nullptr && result.has_return_value) {
        auto ret_tag = (type_aware_args && patch_method->cached_sig_valid)
            ? static_cast<interpreter::ValueTag>(patch_method->cached_ret_tag)
            : interpreter::ValueTag::Void;
        WriteTypedRet(ret_buf, result, ret_tag);
    }
}

// ── InterpreterEntryDirectFast ─────────────────────────────────────────────
// CONSTRAINT: This entry point MUST only be called for zero-arg methods.
//
// It allocates internal args/ret buffers WITHOUT zero-initialization,
// then delegates to InterpreterEntryDirect.  If the method has arguments,
// InterpreterEntryDirect will read garbage from the uninitialized args_buf.
//
// The caller (RunNativeAotBench emitted by codegen) guarantees this contract
// because it is only used in --patch-bench mode where the patched entry was
// generated with zero parameters.
//
// In CHECK (debug) builds, PatchMethodLowerIR asserts cached_arg_count == 0
// when this path is taken.  This assertion fires BEFORE the garbage read.
//
// Benchmarks: saves ~32-48 bytes zero-init per call (~5-10ns per call).
void InterpreterEntryDirectFast(
    uintptr_t method_key) noexcept {

    // Deliberately uninitialized — InterpreterEntryDirect only reads from
    // args_buf when the method has arguments (via ArgBuffer), and only writes
    // to ret_buf when the method returns a value.  Unused buffers remain
    // untouched, so zero-init is wasted cycles.
    uint64_t __chaos_args[4];
    uint64_t __chaos_ret[2];

    InterpreterEntryDirect(method_key, __chaos_args, __chaos_ret);
}

}  // namespace chaos::il2cpp::runtime_core