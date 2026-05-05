#include "interpreter_entry.h"
#include "patch_loader.h"

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
    case 0x01: {  // TypeRef
        // TypeRef in patch data = locally defined reference.
        // The caller handles resolution via the returned value.
        instruction.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(token);
        return true;
    }
    case 0x02: {  // TypeDef
        // TypeDef in patch data = type defined in patch assembly.
        instruction.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(token);
        return true;
    }
    case 0x06: {  // MethodDef
        // MethodDef in patch data = method defined in patch assembly.
        instruction.call_target = nullptr;  // Will be resolved by interpreter dispatch
        instruction.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(token);
        return true;
    }
    case 0x0A: {  // MemberRef
        // MemberRef: reference to a member of another type.
        instruction.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(token);
        return true;
    }
    case 0x11: {  // StandaloneSig
        instruction.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(token);
        return true;
    }
    default:
        // Unknown token type — return false to let the lowerer handle it.
        return false;
    }
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

    // We need a PatchMetadataCache for token resolution.
    // The PatchMethod doesn't store a cache pointer directly, so we'll
    // create a minimal resolution context for the lowering.
    //
    // Full token resolution requires the PatchContext from which this
    // PatchMethod was created.  For now, use the PatchTokenResolver
    // which handles tokens locally; external references will be
    // unresolved (lowerer falls back to empty IR).
    //
    // TODO(Step 5+): Store PatchMetadataCache* on PatchMethod or
    // pass it through a thread-local/context parameter so that
    // the lowerer can fully resolve cross-assembly references.

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
        nullptr);  // user_data = nullptr since we don't have the cache here

    patch_method->cached_ir = ir;
}

// ── InterpreterEntryDirect ──────────────────────────────────────────────

void InterpreterEntryDirect(
    uintptr_t method_key,
    void*     args_buf,
    void*     ret_buf) noexcept {

    if (method_key == 0) return;

    auto* patch_method = reinterpret_cast<PatchMethod*>(method_key);

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
