#include "interpreter_entry.h"
#include "patch_loader.h"
#include "runtime_core.h"
#include "runtime_instantiation.h"
#include "token_resolver.h"
#include "module_registry.h"
#include "reflection_query_model.h"
#include "../bootstrap/bootstrap.h"

#include <il_to_ir_lowerer.h>     // LowerILToIR, ParseMethodBodyHeader, ILTokenResolver
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

// ── PatchTokenResolver ──────────────────────────────────────────────────
// ILTokenResolver callback for patched methods.
// Uses PatchMetadataCache from user_data to resolve metadata tokens locally.
// Falls back through DefaultTokenResolver for tokens outside the patch scope.

static bool PatchTokenResolver(
    CHAOS_IL2CPP_UINT32 token,
    interpreter::IRInstruction& instruction,
    void* user_data) noexcept {

    if (user_data == nullptr) return false;
    auto* cache = static_cast<PatchMetadataCache*>(user_data);

    uint8_t table = static_cast<uint8_t>(token >> 24);

    switch (table) {
    case 0x02: {  // TypeDef — type defined in patch assembly.
        instruction.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(token);
        return true;
    }
    case 0x06: {  // MethodDef — method defined in patch assembly.
        instruction.call_target = nullptr;
        instruction.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(token);
        return true;
    }
    case 0x70: {  // UserString — ldstr token from patch #US heap.
        const char* str = cache->GetUserString(token);
        if (str != nullptr) {
            instruction.string_operand = str;
            return true;
        }
        return false;
    }
    case 0x0A: {  // MemberRef — cross-module method reference.
        // Resolve MemberRef to a concrete MethodInfoHandle from the AOT image.
        // MemberRef tokens (0x0A) cannot be resolved by DefaultTokenResolver
        // because the AOT bridge's resolve_method_by_token only handles MethodDef
        // tokens (0x06).  Instead, we extract the declaring type and method name
        // from the patch data's MemberRef/TypeRef tables, then look up the method
        // in the AOT image's reflection query descriptor tree.
        if (instruction.op_code != interpreter::IROpCode::Call &&
            instruction.op_code != interpreter::IROpCode::CallBridge) {
            break;  // non-call MemberRefs fall through to DefaultTokenResolver
        }

        const auto* member_ref = cache->ResolveMemberRef(token);
        if (member_ref == nullptr) break;

        const char* method_name = cache->GetString(member_ref->name_offset);
        if (method_name == nullptr || method_name[0] == '\0') break;

        // Resolve declaring type name from parent token (TypeRef or TypeDef).
        const uint8_t parent_table = static_cast<uint8_t>(
            member_ref->parent_token >> 24);
        const char* type_name = nullptr;
        const char* ns       = "";

        if (parent_table == 0x02) {
            auto* td = cache->ResolveTypeDef(member_ref->parent_token);
            if (td != nullptr) {
                type_name = cache->GetString(td->type_name_offset);
                ns        = cache->GetString(td->namespace_offset);
            }
        } else if (parent_table == 0x01) {
            auto* tr = cache->ResolveTypeRef(member_ref->parent_token);
            if (tr != nullptr) {
                type_name = cache->GetString(tr->type_name_offset);
                ns        = cache->GetString(tr->namespace_offset);
            }
        }
        if (type_name == nullptr) break;

        // Find the declaring type in the AOT image.
        const auto* aot_image = TryDecodeReflectionQueryImageHandle(
            cache->GetAotImage());
        if (aot_image == nullptr) break;

        const auto* decl_type = FindReflectionQueryTypeByName(
            aot_image, ns, type_name);
        if (decl_type == nullptr) break;

        // Parse parameter count and instance flag from the MemberRef signature.
        CHAOS_IL2CPP_INT32 param_count = -1;
        bool is_instance = false;
        if (member_ref->signature_offset != 0) {
            const auto* sb = static_cast<const uint8_t*>(
                cache->GetBlob(member_ref->signature_offset));
            if (sb != nullptr && sb[0] >= 2) {
                const uint8_t* sig = sb + 1;  // skip #Blob length byte
                is_instance = (sig[0] & 0x20) == 0x20;
                const uint8_t pc = sig[1];     // param count (compressed uint)
                if (pc <= 0x7F) {
                    param_count = static_cast<CHAOS_IL2CPP_INT32>(pc);
                }
            }
        }
        if (param_count < 0) break;

        const auto* method = FindReflectionQueryMethod(
            decl_type, method_name, param_count);
        if (method == nullptr) break;

        instruction.call_target = reinterpret_cast<void*>(
            static_cast<CHAOS_IL2CPP_UINTPTR>(
                EncodeReflectionQueryMethodHandle(method)));
        instruction.is_instance_call = is_instance;
        instruction.arg_count = static_cast<CHAOS_IL2CPP_UINT32>(
            param_count + (is_instance ? 1u : 0u));
        return true;
    }
    default:
        // All other token types (0x01 TypeRef, 0x0A MemberRef, 0x11
        // StandaloneSig, etc.) fall through to DefaultTokenResolver
        // for AOT-side resolution via CodegenBridgeV0.
        break;
    }

    // ── Fallback to DefaultTokenResolver ──
    // Uses the AOT CodegenBridgeV0 + ImageHandle stored in PatchMetadataCache
    // by InterpreterEntryDirect before IL→IR lowering. This enables resolution
    // of string tokens (ldstr), cross-module method references (Convert.ToChar),
    // and type references (System.String) that live in the AOT image rather than
    // the patch data.
    const auto* bridge = cache->GetBridge();
    if (bridge == nullptr) return false;
    const ImageHandle aot_image = cache->GetAotImage();
    if (aot_image == 0) return false;

    interpreter::TokenResolverContext fallback_ctx;
    fallback_ctx.bridge       = bridge;
    fallback_ctx.source_image = aot_image;
    return interpreter::DefaultTokenResolver(token, instruction, &fallback_ctx);
}

// ── PatchMethod lazy IR lowering ────────────────────────────────────────

// Global mutex for double-checked locking of IR lowering.
// Fine-grained: one lock for all methods (low contention since lowering
// typically happens during initial patch activation, not concurrently).
static std::mutex g_lower_ir_mutex;

void PatchMethodLowerIR(uintptr_t method_key) noexcept {
    if (method_key == 0) return;

    auto* patch_method = reinterpret_cast<PatchMethod*>(method_key);

    // Fast path: already lowered.
    if (patch_method->cached_ir != nullptr) return;

    // Slow path: acquire lock and double-check.
    std::lock_guard<std::mutex> lock(g_lower_ir_mutex);

    if (patch_method->cached_ir != nullptr) return;

    // Parse method body header.
    interpreter::MethodBodyHeader header;
    if (!interpreter::ParseMethodBodyHeader(
            patch_method->il_bytes,
            patch_method->il_length,
            header)) {
        // Cannot parse header — create an empty IR with Ret.
        auto* ir = new interpreter::IRMethod();
        ir->instructions.push_back({});
        patch_method->cached_ir = ir;
        return;
    }

    auto* ir = new interpreter::IRMethod();

    // Lower using the IL bytes (after the method body header).
    // The code_start from the parsed header points to the first IL opcode.
    *ir = interpreter::LowerILToIR(
        header.code_start,
        patch_method->il_length - static_cast<uint32_t>(
            reinterpret_cast<const uint8_t*>(header.code_start) -
            patch_method->il_bytes),
        header.code_size,
        header.max_stack,
        PatchTokenResolver,
        patch_method->metadata_cache);

    patch_method->cached_ir = ir;
}

// ── InterpreterEntryDirect ──────────────────────────────────────────────

void InterpreterEntryDirect(
    uintptr_t method_key,
    void*     args_buf,
    void*     ret_buf) noexcept {

    if (method_key == 0) return;

    auto* patch_method = reinterpret_cast<PatchMethod*>(method_key);

    // Step 0: Set up AOT bridge for cross-module token resolution.
    // The PatchMetadataCache needs access to the CodegenBridgeV0 + AOT
    // ImageHandle so that PatchTokenResolver can fall back to
    // DefaultTokenResolver for MemberRef, TypeRef, and LdStr tokens
    // that reference types/methods/strings outside the patch data.
    if (patch_method->metadata_cache != nullptr) {
        const auto* bridge = chaos::il2cpp::bootstrap::GetCodegenBridgeV0();
        const auto* bootstrap_state = chaos::il2cpp::bootstrap::PeekBootstrapState();
        ImageHandle image = 0;
        if (bootstrap_state != nullptr && bootstrap_state->aot_image_handle != 0) {
            image = bootstrap_state->aot_image_handle;
        } else if (bridge != nullptr) {
            // Fallback for standalone hotupdate test context where
            // bootstrap_state->aot_image_handle is 0 (bootstrap not fully
            // initialized). Scan registered modules for the first valid
            // ImageHandle to enable DefaultTokenResolver fallback.
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

    // Step 1: Lazy IL→IR lowering.
    PatchMethodLowerIR(method_key);

    auto* ir = static_cast<interpreter::IRMethod*>(patch_method->cached_ir);
    if (ir == nullptr || ir->instructions.empty()) {
        return;  // Lowering failed — nothing to execute.
    }

    // Step 2: Parse method signature to determine argument count and types.
    // The signature blob format (ECMA 335):
    //   [0] = calling convention (0x00 for static, 0x20 for instance)
    //   [1] = param count (compressed unsigned int)
    //   [2..] = return type, then parameter types
    //
    // For now, use a simple approach: count args from the signature blob.
    CHAOS_IL2CPP_UINT32 arg_count = 0;

    if (patch_method->signature_blob != nullptr &&
        patch_method->signature_len > 1) {
        // Skip calling convention byte.
        // Read compressed unsigned int for param count.
        const uint8_t* sig = patch_method->signature_blob;
        // Skip length prefix (first byte of blob).
        // PatchMethod stores the raw blob starting with the length byte.
        // The actual signature starts at offset 1.
        if (patch_method->signature_blob[0] >= 2) {
            const uint8_t* sig_data = patch_method->signature_blob + 1;
            uint8_t cc = sig_data[0];  // calling convention
            uint8_t count_byte = sig_data[1];

            // Param count (compressed unsigned int, 1-4 bytes).
            if (count_byte <= 0x7F) {
                arg_count = count_byte;
            } else if (count_byte <= 0xBF) {
                arg_count = static_cast<CHAOS_IL2CPP_UINT32>(
                    ((count_byte & 0x3F) << 8) | sig_data[2]);
            }

            // Instance methods have an implicit 'this' argument.
            if ((cc & 0x20) == 0x20) {
                arg_count += 1;  // include 'this'
            }
        }
    }

    // Step 3: Build ExecutionFrame and populate arguments.
    interpreter::ExecutionFrame frame;

    // Parse args from the args_buf.
    ArgBuffer arg_reader(args_buf);
    frame.arguments.reserve(arg_count);

    for (CHAOS_IL2CPP_UINT32 i = 0; i < arg_count; ++i) {
        // Default: treat as IntPtr/object reference (the most common case).
        // Proper type-specific parsing requires full signature traversal.
        // For now, read as pointer-sized value.
        void* raw = arg_reader.ReadPtr();
        frame.arguments.push_back(interpreter::InterpreterValue::from_obj(raw));
    }

    // Initialize locals to default values.
    // (Correct count depends on the method body's local signature, which
    // requires parsing the local_var_sig_tok from the method body header.
    // For now, start with empty locals — InterpreterVM will expand as needed.)
    frame.locals.reserve(8);

    // ── Set up dispatch callback for nested Call instructions ──
    // When Execute encounters Call/CallVirt/CallBridge to cross-module
    // methods (e.g. System.Convert.ToChar), the dispatch_fn routes them
    // through MethodInvoke → AOT function or interpreter, instead of
    // returning needs_external_dispatch. This is the same pattern used
    // by InterpretMethodCall in runtime_instantiation.cpp.
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
        ArgBuffer ret_writer(ret_buf);
        switch (result.return_value.tag) {
        case interpreter::ValueTag::Int32:
            ret_writer.WriteI32(result.return_value.i32);
            break;
        case interpreter::ValueTag::Int64:
            ret_writer.WriteI64(result.return_value.i64);
            break;
        case interpreter::ValueTag::Float32:
            ret_writer.WriteF32(result.return_value.f32);
            break;
        case interpreter::ValueTag::Float64:
            ret_writer.WriteF64(result.return_value.f64);
            break;
        case interpreter::ValueTag::ObjectRef:
        case interpreter::ValueTag::Struct:
        case interpreter::ValueTag::Null:
            ret_writer.WritePtr(result.return_value.obj);
            break;
        default:
            break;
        }
    }
}

}  // namespace chaos::il2cpp::runtime_core
