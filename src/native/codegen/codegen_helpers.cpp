// codegen_helpers.cpp — Runtime helper implementations for T4 generated code

#include "codegen_helpers.h"
#include "deopt_runtime.h"
#include "t4_seh_handler.h"

#include <cstdint>
#include <cstring>

#include <intrin.h>  // _ReturnAddress()

// ── Thread-local deoptimization state ───────────────────────────────────────
namespace chaos::il2cpp::codegen {
thread_local DeoptTlsState t_deopt_state;
}

// InterpreterObject and InterpreterValue access for LdFld/StFld.
// The codegen target's include paths reach src/native/interpreter.
#include <interpreter_vm.h>

// PIC dispatch chain and InterpreterDispatchRaw for CallVirt miss path.
#include <instantiation_engine.h>
#include <patch_loader.h>

// Virtual method resolution for CodegenLdVirtFtn.
#include <vtable_registry.h>

extern "C" uint64_t CodegenLdFld(void* obj, uint32_t field_idx) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (obj == nullptr) return 0;
    auto* io = static_cast<InterpreterObject*>(obj);
    if (field_idx >= io->fields.size()) {
        io->fields.resize(field_idx + 1u);
    }
    const auto& iv = io->fields[field_idx];
    // Return the raw 64-bit representation regardless of tag.
    // The union ensures i32, i64, f32, f64, and obj all occupy the same bits.
    switch (iv.tag) {
    case ValueTag::Int32:
        return static_cast<uint64_t>(iv.i32);
    case ValueTag::Int64:
        return static_cast<uint64_t>(iv.i64);
    case ValueTag::Float32: {
        uint64_t val;
        std::memcpy(&val, &iv.f32, sizeof(float));
        return val;
    }
    case ValueTag::Float64: {
        uint64_t val;
        std::memcpy(&val, &iv.f64, sizeof(double));
        return val;
    }
    default:
        return reinterpret_cast<uint64_t>(iv.obj);
    }
}

extern "C" void CodegenStFld(void* obj, uint32_t field_idx, uint64_t value) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (obj == nullptr) return;
    auto* io = static_cast<InterpreterObject*>(obj);
    if (field_idx >= io->fields.size()) {
        io->fields.resize(field_idx + 1u);
    }
    // Store as Int64 (bit-preserving). The call site may have set Int32 or
    // ObjectRef bits, but the uint64_t representation is what T4 tracks.
    io->fields[field_idx] = InterpreterValue::from_i64(static_cast<int64_t>(value));
}

extern "C" uint64_t CodegenCallVirt(const CodegenCallVirtArgs* args) noexcept {
    using namespace chaos::il2cpp::interpreter;
    using namespace chaos::il2cpp::runtime_core;
    using namespace chaos::il2cpp::runtime_instantiation;

    if (args == nullptr) return 0;
    uint64_t* gpr_base = args->gpr_base;
    if (gpr_base == nullptr) return 0;

    // ── PIC fast path ────────────────────────────────────────────────────
    // Walk pic_dispatch_data for a chain matching this instruction index.
    if (args->pic_data != nullptr) {
        auto* pic_base = static_cast<const uint8_t*>(args->pic_data);
        uint32_t chain_count = *reinterpret_cast<const uint32_t*>(pic_base);
        auto* chains = reinterpret_cast<const PicDispatchChain*>(
            pic_base + sizeof(uint32_t));

        uint64_t receiver_val = gpr_base[args->first_arg_reg];
        if (receiver_val != 0) {
            auto* obj = reinterpret_cast<InterpreterObject*>(receiver_val);
            uint64_t receiver_token = static_cast<uint64_t>(obj->type_token);

            for (uint32_t ci = 0; ci < chain_count; ++ci) {
                const auto& chain = chains[ci];
                if (chain.instruction_idx != args->instruction_idx) continue;
                if (chain.generation != g_patch_generation.load(
                        std::memory_order_acquire)) break;

                // Check PIC slots
                for (uint32_t si = 0; si < 3; ++si) {
                    const auto& slot = chain.slots[si];
                    if (slot.type_token == 0) break;  // sentinel
                    if (slot.type_token == receiver_token && slot.direct_fn != nullptr) {
                        // PIC hit — call direct_fn with args from register file.
                        auto fn = reinterpret_cast<uint64_t (*)(
                            uint64_t, uint64_t, uint64_t, uint64_t,
                            uint64_t, uint64_t, uint64_t, uint64_t)>(
                            slot.direct_fn);

                        uint32_t ac = args->arg_count;
                        uint32_t base = args->first_arg_reg;
                        uint64_t result = fn(
                            (ac > 0) ? gpr_base[base]     : 0,
                            (ac > 1) ? gpr_base[base + 1] : 0,
                            (ac > 2) ? gpr_base[base + 2] : 0,
                            (ac > 3) ? gpr_base[base + 3] : 0,
                            (ac > 4) ? gpr_base[base + 4] : 0,
                            (ac > 5) ? gpr_base[base + 5] : 0,
                            (ac > 6) ? gpr_base[base + 6] : 0,
                            (ac > 7) ? gpr_base[base + 7] : 0);
                        return result;
                    }
                }
                break;  // matched chain but no slot hit → fall through
            }
        }
    }

    // ── Fallback: Deoptimization path ────────────────────────────────────
    // PIC cache miss — deoptimize to RegisterExecute instead of calling
    // the VM path (InterpreterDispatchRaw).  Saves all register file values
    // to TLS state, writes kDeoptMagic to ret_buf, and returns.
    // The call site (code_generator) checks ret_buf[0] and jumps to
    // deopt_return, which unwinds the frame back to entry_direct.
    if (args->ret_buf != nullptr) {
        using chaos::il2cpp::codegen::t_deopt_state;
        using chaos::il2cpp::codegen::kDeoptMagic;

        // Copy all 64 GPR values from the register file directly.
        std::memcpy(t_deopt_state.gpr_file, gpr_base,
                    64 * sizeof(uint64_t));

        // Copy all 32 FPR values (starting after GPRs in the register file).
        std::memcpy(t_deopt_state.fpr_file, gpr_base + 64,
                    32 * sizeof(double));

        // Signal deoptimization to the caller.
        t_deopt_state.deopt_happened = true;
        *static_cast<uint64_t*>(args->ret_buf) = kDeoptMagic;
    }
    return 0;
}

extern "C" void* CodegenBox(uint64_t value, uint8_t tag, uint32_t type_token) noexcept {
    using namespace chaos::il2cpp::interpreter;
    auto* boxed = static_cast<BoxedValue*>(CHAOS_IL2CPP_MALLOC(sizeof(BoxedValue)));
    if (boxed == nullptr) return nullptr;
    ::new (boxed) BoxedValue();
    ValueTag vt = static_cast<ValueTag>(tag);
    switch (vt) {
    case ValueTag::Int32:
        boxed->value = InterpreterValue::from_i32(static_cast<int32_t>(value)); break;
    case ValueTag::Int64:
        boxed->value = InterpreterValue::from_i64(static_cast<int64_t>(value)); break;
    case ValueTag::Float32: {
        float fv; std::memcpy(&fv, &value, sizeof(float));
        boxed->value = InterpreterValue::from_f32(fv); break;
    }
    case ValueTag::Float64: {
        double dv; std::memcpy(&dv, &value, sizeof(double));
        boxed->value = InterpreterValue::from_f64(dv); break;
    }
    default:
        boxed->value = InterpreterValue::from_obj(reinterpret_cast<void*>(value)); break;
    }
    return boxed;
}

extern "C" void* CodegenNewObj(uint32_t type_token, uint32_t field_count) noexcept {
    using namespace chaos::il2cpp::interpreter;
    auto* obj = static_cast<InterpreterObject*>(CHAOS_IL2CPP_MALLOC(sizeof(InterpreterObject)));
    if (obj == nullptr) return nullptr;
    ::new (obj) InterpreterObject();
    obj->type_token = type_token;
    if (field_count == 0) field_count = 1;
    obj->fields.resize(field_count);
    return obj;
}

extern "C" int32_t CodegenLdLen(void* arr) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (arr == nullptr) return 0;
    auto* as = static_cast<ArrayStorage*>(arr);
    return static_cast<int32_t>(as->elements.size());
}

// ── Static field helpers ──────────────────────────────────────────────────

namespace chaos::il2cpp::interpreter {
extern CHAOS_IL2CPP_VECTOR(InterpreterValue) g_static_fields;
}

extern "C" uint64_t CodegenLdSFld(uint32_t field_offset) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (field_offset >= g_static_fields.size()) {
        g_static_fields.resize(field_offset + 1u);
    }
    const auto& iv = g_static_fields[field_offset];
    switch (iv.tag) {
    case ValueTag::Int32:
        return static_cast<uint64_t>(iv.i32);
    case ValueTag::Int64:
        return static_cast<uint64_t>(iv.i64);
    case ValueTag::Float32: {
        uint64_t val;
        std::memcpy(&val, &iv.f32, sizeof(float));
        return val;
    }
    case ValueTag::Float64: {
        uint64_t val;
        std::memcpy(&val, &iv.f64, sizeof(double));
        return val;
    }
    default:
        return reinterpret_cast<uint64_t>(iv.obj);
    }
}

extern "C" void CodegenStSFld(uint32_t field_offset, uint64_t value) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (field_offset >= g_static_fields.size()) {
        g_static_fields.resize(field_offset + 1u);
    }
    g_static_fields[field_offset] = InterpreterValue::from_i64(static_cast<int64_t>(value));
}

// ── Array helpers ─────────────────────────────────────────────────────────

extern "C" void* CodegenNewArr(int32_t length) noexcept {
    using namespace chaos::il2cpp::interpreter;
    auto* arr = static_cast<ArrayStorage*>(CHAOS_IL2CPP_MALLOC(sizeof(ArrayStorage)));
    if (arr == nullptr) return nullptr;
    ::new (arr) ArrayStorage();
    arr->elements.resize(static_cast<size_t>(length > 0 ? length : 0));
    return arr;
}

extern "C" uint64_t CodegenLdElem(void* arr, int32_t index) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (arr == nullptr) return 0;
    auto* as = static_cast<ArrayStorage*>(arr);
    if (index < 0 || static_cast<size_t>(index) >= as->elements.size()) return 0;
    const auto& iv = as->elements[static_cast<size_t>(index)];
    switch (iv.tag) {
    case ValueTag::Int32:
        return static_cast<uint64_t>(iv.i32);
    case ValueTag::Int64:
        return static_cast<uint64_t>(iv.i64);
    case ValueTag::Float32: {
        uint64_t val;
        std::memcpy(&val, &iv.f32, sizeof(float));
        return val;
    }
    case ValueTag::Float64: {
        uint64_t val;
        std::memcpy(&val, &iv.f64, sizeof(double));
        return val;
    }
    default:
        return reinterpret_cast<uint64_t>(iv.obj);
    }
}

extern "C" void CodegenStElem(void* arr, int32_t index, uint64_t value) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (arr == nullptr) return;
    auto* as = static_cast<ArrayStorage*>(arr);
    auto idx = static_cast<size_t>(index >= 0 ? index : 0);
    if (idx >= as->elements.size()) {
        as->elements.resize(idx + 1u);
    }
    as->elements[idx] = InterpreterValue::from_i64(static_cast<int64_t>(value));
}

// ── Type check helpers (no-op — matches FastExecute convention) ──────────

extern "C" void* CodegenCastClass(void* obj, uint32_t /*target_token*/) noexcept {
    return obj;  // FastExecute convention: no type checking
}

extern "C" void* CodegenIsInst(void* obj, uint32_t /*target_token*/) noexcept {
    return obj;  // FastExecute convention: no type checking
}

// ── Unbox helper ──────────────────────────────────────────────────────────

extern "C" uint64_t CodegenUnbox(void* obj) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (obj == nullptr) return 0;
    auto* io = static_cast<InterpreterObject*>(obj);
    if (io->fields.empty()) return 0;
    const auto& iv = io->fields[0];
    switch (iv.tag) {
    case ValueTag::Int32:
        return static_cast<uint64_t>(iv.i32);
    case ValueTag::Int64:
        return static_cast<uint64_t>(iv.i64);
    case ValueTag::Float32: {
        uint64_t val;
        std::memcpy(&val, &iv.f32, sizeof(float));
        return val;
    }
    case ValueTag::Float64: {
        uint64_t val;
        std::memcpy(&val, &iv.f64, sizeof(double));
        return val;
    }
    default:
        return reinterpret_cast<uint64_t>(iv.obj);
    }
}

// ── LdVirtFtn helper ───────────────────────────────────────────────────────

extern "C" void* CodegenLdVirtFtn(void* obj, uint32_t method_token) noexcept {
    if (obj == nullptr) return nullptr;
    using namespace chaos::il2cpp::interpreter;
    auto* io = static_cast<InterpreterObject*>(obj);
    uint32_t type_token = io->type_token;
    if (type_token == 0 || method_token == 0) return nullptr;
    return chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(
        type_token, method_token);
}

// ── InitObj helper ─────────────────────────────────────────────────────────

extern "C" void CodegenInitObj(void* ptr) noexcept {
    if (ptr == nullptr) return;
    std::memset(ptr, 0, sizeof(chaos::il2cpp::interpreter::InterpreterValue));
}

// ── StObj helper ─────────────────────────────────────────────────────────────

extern "C" void CodegenStObj(void* ptr, uint64_t value) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (ptr == nullptr) return;
    auto* iv = static_cast<InterpreterValue*>(ptr);
    *iv = InterpreterValue::from_i64(static_cast<int64_t>(value));
}

// ── Cpblk helper ─────────────────────────────────────────────────────────────

extern "C" void CodegenCpblk(void* dst, const void* src, uint32_t count) noexcept {
    if (dst == nullptr || src == nullptr || count == 0) return;
    std::memcpy(dst, src, count);
}

// ── InitBlk helper ───────────────────────────────────────────────────────────

extern "C" void CodegenInitBlk(void* dst, uint32_t value, uint32_t count) noexcept {
    if (dst == nullptr || count == 0) return;
    std::memset(dst, static_cast<int>(value & 0xFF), count);
}

// ── LdObj helper ────────────────────────────────────────────────────────────

extern "C" uint64_t CodegenLdObj(void* ptr) noexcept {
    using namespace chaos::il2cpp::interpreter;
    if (ptr == nullptr) return 0;
    auto* iv = static_cast<InterpreterValue*>(ptr);
    switch (iv->tag) {
    case ValueTag::Int32:
        return static_cast<uint64_t>(iv->i32);
    case ValueTag::Int64:
        return static_cast<uint64_t>(iv->i64);
    case ValueTag::Float32: {
        uint64_t val;
        std::memcpy(&val, &iv->f32, sizeof(float));
        return val;
    }
    case ValueTag::Float64: {
        uint64_t val;
        std::memcpy(&val, &iv->f64, sizeof(double));
        return val;
    }
    default:
        return reinterpret_cast<uint64_t>(iv->obj);
    }
}

// ── LocAlloc helper ─────────────────────────────────────────────────────────

extern "C" void* CodegenLocAlloc(uint32_t size) noexcept {
    void* mem = CHAOS_IL2CPP_MALLOC(size);
    if (mem == nullptr) return nullptr;
    std::memset(mem, 0, size);
    return mem;
}

// ── Deoptimization trampoline entry point ─────────────────────────────────────

extern "C" void DeoptTrapEntry(const void* ctx, uint64_t codegen_rsp) noexcept {
    using namespace chaos::il2cpp::codegen;

    if (ctx == nullptr) return;

    // 1. Get the return address within the trampoline — this is the address
    //    that the CALL to DeoptTrapEntry pushed.
    void* ret_addr = _ReturnAddress();

    // 2. Find the NativeMethod covering this code address.
    const NativeMethod* nm = FindT4CodeByAddress(ret_addr);
    if (nm == nullptr) {
        return;
    }

    // 3. Compute native offset from the code entry point.
    uint64_t code_base = reinterpret_cast<uint64_t>(nm->code);
    uint32_t native_offset = static_cast<uint32_t>(
        reinterpret_cast<uint64_t>(ret_addr) - code_base);

    // 4. Deoptimize: reconstruct register file from stack frame spill slots.
    const NativeContext* nctx = static_cast<const NativeContext*>(ctx);
    DeoptRuntime::DeoptTrap(
        const_cast<NativeMethod*>(nm),
        native_offset,
        *nctx);

    // 5. Find the DeoptEntry to get the instruction pc.
    const DeoptEntry* entry = DeoptRuntime::FindEntry(nm, native_offset);
    if (entry == nullptr) {
        // No matching deopt entry — can't reconstruct pc.
        return;
    }

    // 6. Set TLS state: the trampoline will write kDeoptMagic to ret_buf[0]
    //    and return.  InterpreterEntryDirect reads this state to reconstruct
    //    the RegisterFrame for RegisterExecute.
    t_deopt_state.instr_pc = entry->instr_pc;
    t_deopt_state.deopt_happened = true;
}
