#include "interpreter_entry.h"
#include "patch_loader.h"
#include "runtime_core.h"
#include "runtime_instantiation.h"
#include "token_resolver.h"
#include "module_registry.h"
#include "reflection_query_model.h"
#include "fast_dispatch.h"
#include "../bootstrap/bootstrap.h"

#include <aot_core_ir_reader.h>   // DeserializeAotCoreIrMethod
#include <interpreter_vm.h>       // ExecutionFrame, InterpreterVM, IRMethod, InterpreterValue

#include <atomic>
#include <cstring>
#include <mutex>

namespace chaos::il2cpp::runtime_core {

// ── ArgBuffer implementation ────────────────────────────────────────────

ArgBuffer::ArgBuffer(void* buffer) noexcept
    : buffer_(buffer) {
}

CHAOS_IL2CPP_INT32 ArgBuffer::ReadI32() noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    auto value = *reinterpret_cast<CHAOS_IL2CPP_INT32*>(ptr);
    offset_ += sizeof(CHAOS_IL2CPP_INT32);
    return value;
}

CHAOS_IL2CPP_INT64 ArgBuffer::ReadI64() noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    auto value = *reinterpret_cast<CHAOS_IL2CPP_INT64*>(ptr);
    offset_ += sizeof(CHAOS_IL2CPP_INT64);
    return value;
}

CHAOS_IL2CPP_FLOAT32 ArgBuffer::ReadF32() noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    auto value = *reinterpret_cast<CHAOS_IL2CPP_FLOAT32*>(ptr);
    offset_ += sizeof(CHAOS_IL2CPP_FLOAT32);
    return value;
}

CHAOS_IL2CPP_FLOAT64 ArgBuffer::ReadF64() noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    auto value = *reinterpret_cast<CHAOS_IL2CPP_FLOAT64*>(ptr);
    offset_ += sizeof(CHAOS_IL2CPP_FLOAT64);
    return value;
}

void* ArgBuffer::ReadPtr() noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    auto value = *reinterpret_cast<void**>(ptr);
    offset_ += sizeof(void*);
    return value;
}

void ArgBuffer::WriteI32(CHAOS_IL2CPP_INT32 value) noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    *reinterpret_cast<CHAOS_IL2CPP_INT32*>(ptr) = value;
    offset_ += sizeof(CHAOS_IL2CPP_INT32);
}

void ArgBuffer::WriteI64(CHAOS_IL2CPP_INT64 value) noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    *reinterpret_cast<CHAOS_IL2CPP_INT64*>(ptr) = value;
    offset_ += sizeof(CHAOS_IL2CPP_INT64);
}

void ArgBuffer::WriteF32(CHAOS_IL2CPP_FLOAT32 value) noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    *reinterpret_cast<CHAOS_IL2CPP_FLOAT32*>(ptr) = value;
    offset_ += sizeof(CHAOS_IL2CPP_FLOAT32);
}

void ArgBuffer::WriteF64(CHAOS_IL2CPP_FLOAT64 value) noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    *reinterpret_cast<CHAOS_IL2CPP_FLOAT64*>(ptr) = value;
    offset_ += sizeof(CHAOS_IL2CPP_FLOAT64);
}

void ArgBuffer::WritePtr(void* value) noexcept {
    auto* ptr = static_cast<char*>(buffer_) + offset_;
    *reinterpret_cast<void**>(ptr) = value;
    offset_ += sizeof(void*);
}

// ── Subject ID resolver callback for DeserializeAotCoreIr ────────────────
// Maps subject IDs to call_target pointers using PatchMetadataCache + AOT reflection query.
// This replaces the old PatchTokenResolver which resolved raw metadata tokens.

static void* ResolveSubjectId(
    const char* subject_id,
    void* user_data) noexcept
{
    if (subject_id == nullptr || user_data == nullptr) return nullptr;

    auto* cache = static_cast<PatchMetadataCache*>(user_data);
    const auto* bridge = cache->GetBridge();
    ImageHandle aot_image = cache->GetAotImage();

    // Try to resolve through the AOT reflection query model.
    // Subject IDs follow the format: "AssemblyName/Namespace.TypeName:MethodName"
    // or just "SubjectId" for types.
    if (bridge != nullptr && aot_image != 0) {
        const auto* image = TryDecodeReflectionQueryImageHandle(aot_image);
        if (image != nullptr) {
            // Search types by subject_id.
            for (CHAOS_IL2CPP_UINT32 ti = 0; ti < image->type_count; ++ti) {
                const auto* type_desc = image->types[ti];
                if (type_desc == nullptr) continue;
                if (type_desc->subject_id_utf8 != nullptr &&
                    std::strcmp(type_desc->subject_id_utf8, subject_id) == 0) {
                    return reinterpret_cast<void*>(
                        static_cast<CHAOS_IL2CPP_UINTPTR>(
                            EncodeReflectionQueryTypeHandle(type_desc)));
                }
                // Search methods in this type.
                if (type_desc->methods != nullptr) {
                    for (CHAOS_IL2CPP_INT32 mi = 0; mi < type_desc->method_count; ++mi) {
                        const auto* method_desc = &type_desc->methods[mi];
                        if (method_desc->subject_id_utf8 != nullptr &&
                            std::strcmp(method_desc->subject_id_utf8, subject_id) == 0) {
                            return reinterpret_cast<void*>(
                                static_cast<CHAOS_IL2CPP_UINTPTR>(
                                    EncodeReflectionQueryMethodHandle(method_desc)));
                        }
                    }
                }
            }
        }
    }

    return nullptr;
}

// ── ECMA element_type → ValueTag mapper ────────────────────────────────────
// Maps a single ECMA element type byte from a method signature to an
// interpreter::ValueTag. Returns ValueTag::ObjectRef for complex types
// (Class, ValueType, SzArray) that need pointer-based access.
static interpreter::ValueTag ElementTypeToValueTag(uint8_t elem_type) noexcept {
    using interpreter::ValueTag;
    switch (elem_type) {
    case 0x02: // Boolean
    case 0x03: // Char
    case 0x04: // I1 (SByte)
    case 0x05: // U1 (Byte)
    case 0x06: // I2 (Int16)
    case 0x07: // U2 (UInt16)
    case 0x08: // I4 (Int32)
    case 0x09: // U4 (UInt32)
        return ValueTag::Int32;
    case 0x0A: // I8 (Int64)
    case 0x0B: // U8 (UInt64)
    case 0x18: // I (IntPtr)
    case 0x19: // U (UIntPtr)
        return ValueTag::Int64;
    case 0x0C: // R4 (Single)
        return ValueTag::Float32;
    case 0x0D: // R8 (Double)
        return ValueTag::Float64;
    default:
        // 0x0E (String), 0x0F (Ptr), 0x10 (ByRef), 0x11 (ValueType),
        // 0x12 (Class), 0x1C (Object), 0x1D (SzArray) → pointer-sized
        return ValueTag::ObjectRef;
    }
}

// ── Read one argument from ArgBuffer with correct type ──────────────────────
// Reads from ArgBuffer using the type-appropriate read method and returns
// an InterpreterValue with the correct tag (not ObjectRef for primitives).
static interpreter::InterpreterValue ReadTypedArg(
    ArgBuffer& reader, interpreter::ValueTag tag) noexcept {
    switch (tag) {
    case interpreter::ValueTag::Int32:
        return interpreter::InterpreterValue::from_i32(reader.ReadI32());
    case interpreter::ValueTag::Int64:
        return interpreter::InterpreterValue::from_i64(reader.ReadI64());
    case interpreter::ValueTag::Float32:
        return interpreter::InterpreterValue::from_f32(reader.ReadF32());
    case interpreter::ValueTag::Float64:
        return interpreter::InterpreterValue::from_f64(reader.ReadF64());
    default:
        return interpreter::InterpreterValue::from_obj(reader.ReadPtr());
    }
}

// ── Write a return value to ret_buf with type-aware write ──────────────────
// Skips the tag switch when the return type is known.
static void WriteTypedRet(void* ret_buf, const interpreter::ExecutionResult& result,
                           interpreter::ValueTag ret_tag) noexcept {
    if (ret_buf == nullptr || !result.has_return_value) return;
    ArgBuffer ret_writer(ret_buf);
    switch (ret_tag) {
    case interpreter::ValueTag::Int32:
        ret_writer.WriteI32(result.return_value.i32);
        return;
    case interpreter::ValueTag::Int64:
        ret_writer.WriteI64(result.return_value.i64);
        return;
    case interpreter::ValueTag::Float32:
        ret_writer.WriteF32(result.return_value.f32);
        return;
    case interpreter::ValueTag::Float64:
        ret_writer.WriteF64(result.return_value.f64);
        return;
    default:
        break; // fall through to tag-switch
    }
    // Fallback: use tag from the return value itself.
    switch (result.return_value.tag) {
    case interpreter::ValueTag::Int32:
        ret_writer.WriteI32(result.return_value.i32); break;
    case interpreter::ValueTag::Int64:
        ret_writer.WriteI64(result.return_value.i64); break;
    case interpreter::ValueTag::Float32:
        ret_writer.WriteF32(result.return_value.f32); break;
    case interpreter::ValueTag::Float64:
        ret_writer.WriteF64(result.return_value.f64); break;
    default:
        ret_writer.WritePtr(result.return_value.obj); break;
    }
}

// ── Parse and cache method signature ───────────────────────────────────────
// Walks ECMA #Blob signature format, caches arg count, per-arg types,
// and return type in PatchMethod. Sets cached_sig_valid = true on success.
static void CacheSignature(PatchMethod* patch_method) noexcept {
    if (patch_method == nullptr ||
        patch_method->signature_blob == nullptr ||
        patch_method->signature_len <= 1) {
        return;
    }

    const uint8_t* sig = patch_method->signature_blob;
    uint32_t sig_len = patch_method->signature_len;

    // [blob_length] [calling_convention] [param_count] [ret_type] [param_types...]
    const uint8_t* ptr = sig + 1; // skip blob length
    uint32_t remaining = sig_len - 1;
    if (remaining < 2) return;

    uint8_t cc = ptr[0];
    uint8_t count_byte = ptr[1];

    uint32_t param_count = 0;
    uint32_t consumed = 2;
    if (count_byte <= 0x7F) {
        param_count = count_byte;
    } else if (count_byte <= 0xBF && remaining >= 3) {
        param_count = static_cast<uint32_t>(((count_byte & 0x3F) << 8) | ptr[2]);
        consumed = 3;
    } else {
        return; // 4-byte encoding, rare, skip
    }

    bool has_this = (cc & 0x20) == 0x20;
    uint32_t total_arg_count = has_this ? param_count + 1 : param_count;

    ptr += consumed;
    remaining -= consumed;
    if (remaining < 1) return;

    // Parse return type.
    patch_method->cached_ret_tag = static_cast<uint8_t>(
        ElementTypeToValueTag(ptr[0]));
    ptr += 1;
    remaining -= 1;

    // Parse parameter types.
    // Only cache first 8 args (vast majority of methods).
    uint32_t args_cached = (total_arg_count > 8) ? 8 : total_arg_count;
    for (uint32_t i = 0; i < args_cached && remaining > 0; ++i) {
        uint8_t elem = ptr[0];
        auto tag = ElementTypeToValueTag(elem);
        patch_method->cached_arg_types[i] = static_cast<uint8_t>(tag);

        // Advance past this parameter type.
        ptr += 1;
        remaining -= 1;

        // For complex types with trailing token bytes, skip past them.
        if (elem == 0x11 || elem == 0x12) { // ValueType or Class
            // Skip TypeDefOrRef coded index (compressed unsigned int).
            if (remaining >= 1 && ptr[0] >= 0x80) {
                if ((ptr[0] & 0xC0) == 0xC0 && remaining >= 4) {
                    ptr += 4; remaining -= 4;
                } else if (remaining >= 2) {
                    ptr += 2; remaining -= 2;
                }
            }
            // 1-byte tok: ptr already advanced by ptr+=1 above, any 0x7F or less is 1 byte
        }
    }

    patch_method->cached_arg_count = total_arg_count;
    patch_method->cached_sig_valid = true;
}

// ── PatchMethod lazy IR deserialization ─────────────────────────────────

// Global mutex for double-checked locking of IR deserialization.
static std::mutex g_lower_ir_mutex;

void PatchMethodLowerIR(uintptr_t method_key) noexcept {
    if (method_key == 0) return;

    auto* patch_method = reinterpret_cast<PatchMethod*>(method_key);

    // Fast path: already lowered.
    if (patch_method->cached_ir != nullptr) return;

    // Slow path: acquire lock and double-check.
    std::lock_guard<std::mutex> lock(g_lower_ir_mutex);

    if (patch_method->cached_ir != nullptr) return;

    // Deserialize AotCoreIr JSON → IRMethod.
    const char* json = patch_method->aot_core_ir_json;
    if (json == nullptr || json[0] == '\0') {
        // No JSON — create an empty IR with Ret.
        auto* ir = new interpreter::IRMethod();
        ir->instructions.push_back({});
        patch_method->cached_ir = ir;
        return;
    }

    size_t json_length = std::strlen(json);
    auto* ir = new interpreter::IRMethod(
        DeserializeAotCoreIrMethod(
            json,
            json_length,
            ResolveSubjectId,
            patch_method->metadata_cache));

    patch_method->cached_ir = ir;

    // Pre-cache signature so the fast path can use it immediately.
    if (!patch_method->cached_sig_valid) {
        CacheSignature(patch_method);
    }

    // Pre-cache call-site metadata for every Call instruction.
    // This eliminates TryDecodeReflectionQueryMethodHandle + ResolveParameterType
    // + IsValueTypeByHandle + LayoutEngine at each call dispatch.
    uint32_t instr_count = static_cast<uint32_t>(ir->instructions.size());
    if (instr_count > 0) {
        auto* call_cache = new runtime_instantiation::CachedCallInfo[instr_count];
        for (uint32_t i = 0; i < instr_count; ++i) {
            const auto& instr = ir->instructions[i];
            if (instr.op_code == interpreter::IROpCode::Call ||
                instr.op_code == interpreter::IROpCode::CallVirt ||
                instr.op_code == interpreter::IROpCode::CallBridge ||
                instr.op_code == interpreter::IROpCode::CallVirtConstrained) {
                if (instr.call_target != nullptr) {
                    call_cache[i] = runtime_instantiation::PrecacheCallTarget(
                        instr.call_target);
                } else {
                    call_cache[i].ret_tag = 0xFF; // not cached
                }
            } else {
                call_cache[i].ret_tag = 0xFF; // not a call
            }
        }
        patch_method->call_cache = call_cache;
    }
}

// ── InterpreterEntryDirect ──────────────────────────────────────────────

void InterpreterEntryDirect(
    uintptr_t method_key,
    void*     args_buf,
    void*     ret_buf) noexcept {

    if (method_key == 0) return;

    auto* patch_method = reinterpret_cast<PatchMethod*>(method_key);

    // Step 0: Set up AOT bridge for cross-module subject ID resolution.
    // This only runs once per PatchMetadataCache (all methods in a patch
    // context share the same cache). After the first call, GetBridge()
    // returns non-null and the entire block is skipped.
    if (patch_method->metadata_cache != nullptr &&
        patch_method->metadata_cache->GetBridge() == nullptr) {
        const auto* bridge = chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
        const auto* bootstrap_state = chaos::il2cpp::bootstrap::PeekBootstrapState();
        ImageHandle image = 0;
        if (bootstrap_state != nullptr && bootstrap_state->aot_image_handle != 0) {
            image = bootstrap_state->aot_image_handle;
        } else {
            // In hotupdate test contexts the AOT image handle is obtained
            // from the first registered module that has types loaded.
            for (uint32_t mid = 0; mid < kMaxModules; ++mid) {
                const auto* module = LookupModule(mid);
                if (module != nullptr && !module->tombstone &&
                    module->image != nullptr && module->type_count > 0) {
                    image = EncodeReflectionQueryImageHandle(module->image);
                    break;
                }
            }
        }
        if (bridge != nullptr && image != 0) {
            patch_method->metadata_cache->SetAotBridge(bridge, image);
        }
    }

    // Step 1: Lazy AotCoreIr JSON → IR deserialization.
    PatchMethodLowerIR(method_key);

    auto* ir = static_cast<interpreter::IRMethod*>(patch_method->cached_ir);
    if (ir == nullptr || ir->instructions.empty()) {
        return;  // Deserialization failed — nothing to execute.
    }

    // ── Step A: Trivial fast path ────────────────────────────────────────
    // Detect simple patterns (LdArg+Ret, LdcI4+Ret, LdNull+Ret) and handle
    // them inline without building ExecutionFrame or calling InterpreterVM.
    // Also handles single-instruction Ret (empty IR fallback when AotCoreIr
    // JSON is absent from patchdata).
    if (ir->instructions.empty()) {
        return;  // No instructions — nothing to execute.
    }
    const auto instr_count = ir->instructions.size();
    if (instr_count == 1) {
        // Single instruction: must be Ret (empty IR fallback, no AotCoreIr in patchdata).
        return;
    }
    if (instr_count == 2 && ir->seh_clauses.empty()) {
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
        }
    }

    // ── Step B: FastExecute path (Layer 1+2) ─────────────────────────
    // For methods WITHOUT SEH, use function-pointer dispatch + FastFrame.
    if (ir->seh_clauses.empty() && instr_count > 2) {
        if (!patch_method->cached_sig_valid) {
            CacheSignature(patch_method);
        }

        FastFrame ff;
        if (patch_method->cached_sig_valid) {
            ff.arg_count = patch_method->cached_arg_count;
        } else {
            ff.arg_count = 0;
            if (patch_method->signature_blob != nullptr &&
                patch_method->signature_len > 1) {
                const uint8_t* sig = patch_method->signature_blob;
                const uint8_t* sig_data = sig + 1;
                uint8_t count_byte = sig_data[1];
                if (count_byte <= 0x7F) {
                    ff.arg_count = count_byte;
                } else if (count_byte <= 0xBF) {
                    ff.arg_count = static_cast<uint32_t>(
                        ((count_byte & 0x3F) << 8) | sig_data[2]);
                }
                if ((sig_data[0] & 0x20) == 0x20) {
                    ff.arg_count += 1;
                }
            }
        }
        ff.args = args_buf;

        // Set up dispatch callback for Call instructions inside FastExecute.
        auto* runtime_state = GetCurrentRuntimeState();
        auto* thread_state  = GetCurrentThreadState();
        runtime_instantiation::InterpreterDispatchContext dispatch_ctx;
        dispatch_ctx.runtime_state = runtime_state;
        dispatch_ctx.thread_state  = thread_state;
        ff.dispatch_fn = reinterpret_cast<void*>(
            runtime_instantiation::InterpreterDispatch);
        ff.dispatch_ctx = &dispatch_ctx;

        // Wire call-site metadata cache — one CachedCallInfo per instruction.
        ff.call_cache = patch_method->call_cache;
        ff.call_count = static_cast<uint32_t>(ir->instructions.size());

        bool ok = FastExecute(ff,
                              ir->instructions.data(),
                              static_cast<uint32_t>(ir->instructions.size()));
        if (ok) {
            if (ff.has_ret && ret_buf != nullptr) {
                auto ret_tag = static_cast<interpreter::ValueTag>(ff.ret_tag);
                ArgBuffer ret_writer(ret_buf);
                switch (ret_tag) {
                case interpreter::ValueTag::Int32:
                    ret_writer.WriteI32(static_cast<int32_t>(ff.ret_val));
                    return;
                case interpreter::ValueTag::Int64:
                    ret_writer.WriteI64(static_cast<int64_t>(ff.ret_val));
                    return;
                case interpreter::ValueTag::Float32: {
                    float v;
                    std::memcpy(&v, &ff.ret_val, sizeof(float));
                    ret_writer.WriteF32(v);
                    return;
                }
                case interpreter::ValueTag::Float64: {
                    double v;
                    std::memcpy(&v, &ff.ret_val, sizeof(double));
                    ret_writer.WriteF64(v);
                    return;
                }
                default:
                    ret_writer.WritePtr(reinterpret_cast<void*>(ff.ret_val));
                    return;
                }
            }
            return;
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
            auto tag = (i < 8)
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

}  // namespace chaos::il2cpp::runtime_core