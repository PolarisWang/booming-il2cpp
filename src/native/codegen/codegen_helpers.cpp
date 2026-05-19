// codegen_helpers.cpp — Runtime helper implementations for T4 generated code

#include "codegen_helpers.h"
#include "deopt_runtime.h"
#include "t4_seh_handler.h"

#include <cstdint>
#include <cstring>

#include <chaos/profile.h>

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

// TLAB (Thread-Local Allocation Buffer) for inline bump-pointer allocation.
// We avoid including gc/gc_young_gen.h here because it pulls in GcScheduler
// and other GC internals that the codegen library should not depend on.
// Instead, we forward-declare the TLAB struct and extern tls_tlab.
namespace chaos::il2cpp::runtime_core {
struct TLAB {
    char* start{nullptr};
    char* current{nullptr};
    char* end{nullptr};
    char* start_scan{nullptr};
    char* current_scan{nullptr};
};
extern thread_local TLAB tls_tlab;
}  // namespace chaos::il2cpp::runtime_core

extern "C" uint64_t CodegenLdFld(void* obj, uint32_t field_idx) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LdFld");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::StFld");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::CallVirt");
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

    // ── Megamorphic fallback: vtable resolution ───────────────────────────
    // PIC miss — try ResolveVirtualMethodPointer before deoptimizing.
    // This is slower than PIC hit (vtable walk) but much faster than
    // deoptimization (T4 demotion + T3 RegisterExecute rebuild).
    if (args->method_token != 0) {
        uint64_t receiver_val = gpr_base[args->first_arg_reg];
        if (receiver_val != 0) {
            auto* obj = reinterpret_cast<InterpreterObject*>(receiver_val);
            uint32_t type_token = obj->type_token;
            if (type_token != 0) {
                void* resolved_fn = chaos::il2cpp::vtable_registry::
                    ResolveVirtualMethodPointer(type_token, args->method_token);
                if (resolved_fn != nullptr) {
                    auto fn = reinterpret_cast<uint64_t (*)(
                        uint64_t, uint64_t, uint64_t, uint64_t,
                        uint64_t, uint64_t, uint64_t, uint64_t)>(
                        resolved_fn);
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
        }
    }

    // ── Fallback: Deoptimization path ────────────────────────────────────
    // PIC cache miss — deoptimize to RegisterExecute instead of calling
    // the VM path (InterpreterDispatchRaw).  Saves all register file values
    // to TLS state, writes kDeoptMagic to ret_buf, and returns.
    // The call site (code_generator) checks ret_buf[0] and jumps to
    // deopt_return, which unwinds the frame back to entry_direct.
    if (args->ret_buf != nullptr) {
        using namespace chaos::il2cpp::codegen;

        // Copy all 64 GPR values from the register file directly.
        std::memcpy(t_deopt_state.gpr_file, gpr_base,
                    64 * sizeof(uint64_t));

        // Copy all 32 FPR values (starting after GPRs in the register file).
        std::memcpy(t_deopt_state.fpr_file, gpr_base + 64,
                    32 * sizeof(double));

        // Extract type tags from the DeoptEntry for this call site.
        // The return address points to the instruction after the call to
        // CodegenCallVirt in the generated code.  Use it to find the
        // NativeMethod and its DeoptEntry, then copy per-register tags.
        void* ret_addr = _ReturnAddress();
        const NativeMethod* nm = FindT4CodeByAddress(ret_addr);
        if (nm != nullptr && nm->deopt_values != nullptr) {
            uint64_t code_base = reinterpret_cast<uint64_t>(nm->code);
            uint32_t native_off = static_cast<uint32_t>(
                reinterpret_cast<uint64_t>(ret_addr) - code_base);
            const DeoptEntry* entry = DeoptRuntime::FindEntry(nm, native_off);
            if (entry != nullptr) {
                // Initialize all GPR tags to Int64, then overwrite with
                // actual tags from the DeoptValue entries.
                std::memset(t_deopt_state.gpr_tags,
                    static_cast<int>(ValueTag::Int64), 64);
                std::memset(t_deopt_state.fpr_tags,
                    static_cast<int>(ValueTag::Float64), 32);
                for (uint32_t i = 0; i < entry->num_values; ++i) {
                    const auto& dv = nm->deopt_values[entry->values_offset + i];
                    if (dv.reg_index < 64) {
                        t_deopt_state.gpr_tags[dv.reg_index] = dv.value_tag;
                    } else {
                        t_deopt_state.fpr_tags[dv.reg_index - 64] = dv.value_tag;
                    }
                }
            }
        }

        // Signal deoptimization to the caller.
        t_deopt_state.instr_pc = args->instruction_idx;
        t_deopt_state.deopt_happened = true;
        *static_cast<uint64_t*>(args->ret_buf) = kDeoptMagic;
    }
    return 0;
}

extern "C" void* CodegenGetTlab() noexcept {
    return &chaos::il2cpp::runtime_core::tls_tlab;
}

extern "C" void* CodegenBox(uint64_t value, uint8_t tag, uint32_t type_token) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::Box");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::NewObj");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LdLen");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LdSFld");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::StSFld");
    using namespace chaos::il2cpp::interpreter;
    if (field_offset >= g_static_fields.size()) {
        g_static_fields.resize(field_offset + 1u);
    }
    g_static_fields[field_offset] = InterpreterValue::from_i64(static_cast<int64_t>(value));
}

// ── Array helpers ─────────────────────────────────────────────────────────

extern "C" void* CodegenNewArr(int32_t length) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::NewArr");
    using namespace chaos::il2cpp::interpreter;
    auto* arr = static_cast<ArrayStorage*>(CHAOS_IL2CPP_MALLOC(sizeof(ArrayStorage)));
    if (arr == nullptr) return nullptr;
    ::new (arr) ArrayStorage();
    arr->elements.resize(static_cast<size_t>(length > 0 ? length : 0));
    return arr;
}

extern "C" uint64_t CodegenLdElem(void* arr, int32_t index) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LdElem");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::StElem");
    using namespace chaos::il2cpp::interpreter;
    if (arr == nullptr) return;
    auto* as = static_cast<ArrayStorage*>(arr);
    auto idx = static_cast<size_t>(index >= 0 ? index : 0);
    if (idx >= as->elements.size()) {
        as->elements.resize(idx + 1u);
    }
    as->elements[idx] = InterpreterValue::from_i64(static_cast<int64_t>(value));
}

// ── Type check helpers ──────────────────────────────────────────────────────

extern "C" void* CodegenCastClass(void* obj, uint32_t target_token) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::CastClass");
    using namespace chaos::il2cpp::interpreter;
    if (obj == nullptr) return nullptr;
    auto* io = static_cast<InterpreterObject*>(obj);
    return (io->type_token == target_token) ? obj : nullptr;
}

extern "C" void* CodegenIsInst(void* obj, uint32_t target_token) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::IsInst");
    using namespace chaos::il2cpp::interpreter;
    if (obj == nullptr) return nullptr;
    auto* io = static_cast<InterpreterObject*>(obj);
    return (io->type_token == target_token) ? obj : nullptr;
}

// ── Unbox helper ──────────────────────────────────────────────────────────

extern "C" uint64_t CodegenUnbox(void* obj) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::Unbox");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LdVirtFtn");
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
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::InitObj");
    if (ptr == nullptr) return;
    // 16-byte zero fill — two uint64_t stores instead of std::memset.
    // InterpreterValue is 16 bytes (tag + pad + struct_size + union).
    auto* p = static_cast<uint64_t*>(ptr);
    p[0] = 0;
    p[1] = 0;
}

// ── StObj helper ─────────────────────────────────────────────────────────────

extern "C" void CodegenStObj(void* ptr, uint64_t value) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::StObj");
    using namespace chaos::il2cpp::interpreter;
    if (ptr == nullptr) return;
    auto* iv = static_cast<InterpreterValue*>(ptr);
    *iv = InterpreterValue::from_i64(static_cast<int64_t>(value));
}

// ── Cpblk helper ─────────────────────────────────────────────────────────────

extern "C" void CodegenCpblk(void* dst, const void* src, uint32_t count) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::Cpblk");
    if (dst == nullptr || src == nullptr || count == 0) return;
    std::memcpy(dst, src, count);
}

// ── InitBlk helper ───────────────────────────────────────────────────────────

extern "C" void CodegenInitBlk(void* dst, uint32_t value, uint32_t count) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::InitBlk");
    if (dst == nullptr || count == 0) return;
    std::memset(dst, static_cast<int>(value & 0xFF), count);
}

// ── LdObj helper ────────────────────────────────────────────────────────────

extern "C" uint64_t CodegenLdObj(void* ptr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LdObj");
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

extern "C" void* CodegenLocAlloc(uint32_t size, void* base, uint32_t* bump) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LocAlloc");

    // Stack-backed allocation: bump-pointer from pre-allocated frame reserve.
    if (base != nullptr && bump != nullptr) {
        uint32_t old = *bump;
        uint32_t new_bump = old + size;
        // 4KB reserve should be enough for typical localloc usage.
        // If exceeded, fall through to heap below.
        if (new_bump <= 4096) {
            *bump = new_bump;
            void* ptr = static_cast<char*>(base) + old;
            std::memset(ptr, 0, size);
            return ptr;
        }
    }

    // Heap fallback (no stack reserve, or reserve exhausted).
    void* mem = CHAOS_IL2CPP_MALLOC(size);
    if (mem == nullptr) return nullptr;
    std::memset(mem, 0, size);
    return mem;
}

// ── Inline deoptimization state saver for EmitDeoptSequence ─────────────────────

extern "C" void DeoptSaveFrameState(uint64_t codegen_rsp) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::DeoptSaveFrameState");
    using namespace chaos::il2cpp::codegen;
    using namespace chaos::il2cpp::interpreter;

    if (codegen_rsp == 0) return;

    // Get the return address — points to the instruction after the CALL
    // to DeoptSaveFrameState in the generated code.
    void* ret_addr = _ReturnAddress();

    // Find the NativeMethod covering this code address.
    const NativeMethod* nm = FindT4CodeByAddress(ret_addr);
    if (nm == nullptr) return;

    uint64_t code_base = reinterpret_cast<uint64_t>(nm->code);
    uint32_t native_off = static_cast<uint32_t>(
        reinterpret_cast<uint64_t>(ret_addr) - code_base);

    // Batch-copy all 64 GPR values from stack frame spill slots.
    // GPR file starts at codegen_rsp + kGprFileOff (= codegen_rsp + 32).
    for (uint32_t vr = 0; vr < 64; ++vr) {
        t_deopt_state.gpr_file[vr] = *reinterpret_cast<const uint64_t*>(
            codegen_rsp + 32 + vr * 8);
    }

    // Batch-copy all 32 FPR values from stack frame spill slots.
    // FPR file starts at codegen_rsp + kFprFileOff (= codegen_rsp + 544).
    for (uint32_t vr = 0; vr < 32; ++vr) {
        t_deopt_state.fpr_file[vr] = *reinterpret_cast<const double*>(
            codegen_rsp + 544 + vr * 8);
    }

    // Look up type tags from the DeoptEntry for this native offset.
    const DeoptEntry* entry = DeoptRuntime::FindEntry(nm, native_off);
    if (entry != nullptr && nm->deopt_values != nullptr) {
        std::memset(t_deopt_state.gpr_tags,
            static_cast<int>(ValueTag::Int64), 64);
        std::memset(t_deopt_state.fpr_tags,
            static_cast<int>(ValueTag::Float64), 32);
        for (uint32_t i = 0; i < entry->num_values; ++i) {
            const auto& dv = nm->deopt_values[entry->values_offset + i];
            if (dv.reg_index < 64) {
                t_deopt_state.gpr_tags[dv.reg_index] = dv.value_tag;
            } else {
                t_deopt_state.fpr_tags[dv.reg_index - 64] = dv.value_tag;
            }
        }
        t_deopt_state.instr_pc = entry->instr_pc;
    }

    t_deopt_state.deopt_happened = true;
}

// ── Deoptimization trampoline entry point ─────────────────────────────────────

extern "C" void DeoptTrapEntry(const void* ctx, uint64_t codegen_rsp) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::DeoptTrapEntry");
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
        *nctx,
        codegen_rsp,
        t_deopt_state.gpr_file,
        t_deopt_state.fpr_file,
        t_deopt_state.gpr_tags,
        t_deopt_state.fpr_tags);

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
