// jit_helpers.cpp — Runtime helper implementations for JIT generated code

#include "jit_helpers.h"
#include "jit_engine.h" // kGprFileOff / kFprFileOff / kFprSlotSize / kGprCount / kFprCount
#include "jit_deopt.h"
#include "jit_seh.h"

#include <cstdint>
#include <cstring>

#include <chaos/profile.h>

#include <gc/gc_root_change.h>
#include <gc/gc_api.h>
#include <gc/gc_helpers.h>
#include <forbid_suspend.h>

#if defined(_MSC_VER)
#include <intrin.h> // _ReturnAddress(), __readgsqword
#else
// _ReturnAddress via __builtin_return_address works on all GCC/Clang targets
// (no x86intrin.h needed — that header is x86-only and breaks ARM64 cross-compile).
#define _ReturnAddress() __builtin_return_address(0)
#endif

// SATB pre-write barrier — provided by gc_bgc.cpp (compiled directly in test
// targets as MSVC-format .obj to avoid __tls_index issues with GNU ar archives).
extern "C" void JitSatbPreWriteBarrier(void** slot) noexcept;

// ── Thread-local deoptimization state ───────────────────────────────────────
namespace chaos::il2cpp::jit {
thread_local DeoptTlsState g_jit_deopt_state;
}

// InterpreterObject and InterpreterValue access for LdFld/StFld.
// The codegen target's include paths reach src/native/interpreter.
#include <interpreter_vm.h>

// PIC dispatch chain and InterpreterDispatchRaw for CallVirt miss path.
#include <instantiation_engine.h>
#include <patch_loader.h>

// Virtual method resolution for CodegenLdVirtFtn.
#include <vtable_registry.h>

// TLAB access inline — full type definition comes from gc/gc_young_gen.h
// via the gc headers included above.  Only the extern declaration is needed.
namespace chaos::il2cpp::runtime_core {
extern thread_local TLAB tls_tlab;
} // namespace chaos::il2cpp::runtime_core

extern "C" uint64_t CodegenLdFld(void* obj, uint32_t field_idx) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LdFld");
    using namespace chaos::il2cpp::interpreter;
    if (obj == nullptr)
        return 0;
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
    using namespace chaos::il2cpp::runtime_core;
    using namespace chaos::il2cpp::runtime_core::threading;
    if (obj == nullptr)
        return;
    auto* io = static_cast<InterpreterObject*>(obj);
    if (field_idx >= io->fields.size()) {
        io->fields.resize(field_idx + 1u);      // C++ heap alloc — outside barrier scope
    } else {
        // SATB pre-write barrier: record old value before overwriting.
        // Guard with heap check: stack-allocated value type slots don't
        // need SATB recording (stack is conservatively scanned by BGC).
        // SATB stays OUTSIDE the barrier scope — it can allocate (emergency
        // GC) and must not be made non-preemptible.
        if (chaos_is_gc_pointer(obj)) {
            JitSatbPreWriteBarrier(reinterpret_cast<void**>(&io->fields[field_idx].obj));
        }
    }
    bool is_gc = chaos_is_gc_pointer(obj);
    // store→card in one non-preemptible critical section (A2b close).
    {
        BarrierCriticalSectionScope barrier;
        io->fields[field_idx] = InterpreterValue::from_i64(static_cast<int64_t>(value));
        if (is_gc) {
            chaos_gc_dirty_card(obj);
        }
    }
}

// Lightweight StFld without SATB pre-write barrier.
// Called when g_bgc_is_marking is false (the common case during steady-state).
// The SATB barrier is only needed during concurrent BGC marking, which is a
// small fraction of total runtime.  Skipping the barrier in the common case
// eliminates SATB buffer management overhead on every StFld.
extern "C" void CodegenStFldNoBarrier(void* obj, uint32_t field_idx, uint64_t value) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::StFldNoBarrier");
    using namespace chaos::il2cpp::interpreter;
    using namespace chaos::il2cpp::runtime_core;
    using namespace chaos::il2cpp::runtime_core::threading;
    if (obj == nullptr)
        return;
    auto* io = static_cast<InterpreterObject*>(obj);
    if (field_idx >= io->fields.size()) {
        io->fields.resize(field_idx + 1u);      // C++ heap alloc — outside barrier scope
    }
    bool is_gc = chaos_is_gc_pointer(obj);
    // store→card in one non-preemptible critical section (A2b close).
    {
        BarrierCriticalSectionScope barrier;
        io->fields[field_idx] = InterpreterValue::from_i64(static_cast<int64_t>(value));
        if (is_gc) {
            chaos_gc_dirty_card(obj);
        }
    }
}

extern "C" uint64_t CodegenCallVirt(const CodegenCallVirtArgs* args) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::CallVirt");
    using namespace chaos::il2cpp::interpreter;
    using namespace chaos::il2cpp::runtime_core;
    using namespace chaos::il2cpp::runtime_instantiation;

    if (args == nullptr)
        return 0;
    uint64_t* gpr_base = args->gpr_base;
    if (gpr_base == nullptr)
        return 0;

    // ── PIC fast path ────────────────────────────────────────────────────
    // Walk pic_dispatch_data for a chain matching this instruction index.
    if (args->pic_data != nullptr) {
        auto* pic_base = static_cast<const uint8_t*>(args->pic_data);
        uint32_t chain_count = *reinterpret_cast<const uint32_t*>(pic_base);
        auto* chains = reinterpret_cast<const PicDispatchChain*>(pic_base + sizeof(uint32_t));

        uint64_t receiver_val = gpr_base[args->first_arg_reg];
        if (receiver_val != 0) {
            auto* obj = reinterpret_cast<InterpreterObject*>(receiver_val);
            uint64_t receiver_token = static_cast<uint64_t>(obj->type_token);

            for (uint32_t ci = 0; ci < chain_count; ++ci) {
                const auto& chain = chains[ci];
                if (chain.instruction_idx != args->instruction_idx)
                    continue;
                if (chain.generation != g_patch_generation.load(std::memory_order_acquire))
                    break;

                // Check PIC slots
                for (uint32_t si = 0; si < 3; ++si) {
                    const auto& slot = chain.slots[si];
                    if (slot.type_token == 0)
                        break; // sentinel
                    if (slot.type_token == receiver_token && slot.direct_fn != nullptr) {
                        // PIC hit — call direct_fn with args from register file.
                        auto fn = reinterpret_cast<uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t,
                                                                uint64_t, uint64_t, uint64_t)>(slot.direct_fn);

                        uint32_t ac = args->arg_count;
                        uint32_t base = args->first_arg_reg;
                        uint64_t result = fn((ac > 0) ? gpr_base[base] : 0, (ac > 1) ? gpr_base[base + 1] : 0,
                                             (ac > 2) ? gpr_base[base + 2] : 0, (ac > 3) ? gpr_base[base + 3] : 0,
                                             (ac > 4) ? gpr_base[base + 4] : 0, (ac > 5) ? gpr_base[base + 5] : 0,
                                             (ac > 6) ? gpr_base[base + 6] : 0, (ac > 7) ? gpr_base[base + 7] : 0);
                        return result;
                    }
                }
                break; // matched chain but no slot hit → fall through
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
                void* resolved_fn =
                    chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(type_token, args->method_token);
                if (resolved_fn != nullptr) {
                    auto fn = reinterpret_cast<uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t,
                                                            uint64_t, uint64_t)>(resolved_fn);
                    uint32_t ac = args->arg_count;
                    uint32_t base = args->first_arg_reg;
                    uint64_t result = fn((ac > 0) ? gpr_base[base] : 0, (ac > 1) ? gpr_base[base + 1] : 0,
                                         (ac > 2) ? gpr_base[base + 2] : 0, (ac > 3) ? gpr_base[base + 3] : 0,
                                         (ac > 4) ? gpr_base[base + 4] : 0, (ac > 5) ? gpr_base[base + 5] : 0,
                                         (ac > 6) ? gpr_base[base + 6] : 0, (ac > 7) ? gpr_base[base + 7] : 0);
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
        using namespace chaos::il2cpp::jit;

        // Copy all 64 GPR values from the register file directly.
        std::memcpy(g_jit_deopt_state.gpr_file, gpr_base, 64 * sizeof(uint64_t));

        // Copy all 32 FPR values (starting after GPRs in the register file).
        std::memcpy(g_jit_deopt_state.fpr_file, gpr_base + 64, 32 * sizeof(double));

        // Extract type tags from the DeoptEntry for this call site.
        // The return address points to the instruction after the call to
        // CodegenCallVirt in the generated code.  Use it to find the
        // JitMethod and its DeoptEntry, then copy per-register tags.
        void* ret_addr = _ReturnAddress();
        const JitMethod* nm = FindNativeCodeByAddress(ret_addr);
        if (nm != nullptr && nm->deopt_values != nullptr) {
            uint64_t code_base = reinterpret_cast<uint64_t>(nm->code);
            uint32_t native_off = static_cast<uint32_t>(reinterpret_cast<uint64_t>(ret_addr) - code_base);
            const DeoptEntry* entry = DeoptRuntime::FindEntry(nm, native_off);
            if (entry != nullptr) {
                // Initialize all GPR tags to Int64, then overwrite with
                // actual tags from the DeoptValue entries.
                std::memset(g_jit_deopt_state.gpr_tags, static_cast<int>(ValueTag::Int64), 64);
                std::memset(g_jit_deopt_state.fpr_tags, static_cast<int>(ValueTag::Float64), 32);
                for (uint32_t i = 0; i < entry->num_values; ++i) {
                    const auto& dv = nm->deopt_values[entry->values_offset + i];
                    if (dv.reg_index < 64) {
                        g_jit_deopt_state.gpr_tags[dv.reg_index] = dv.value_tag;
                    } else {
                        g_jit_deopt_state.fpr_tags[dv.reg_index - 64] = dv.value_tag;
                    }
                }
            }
        }

        // Signal deoptimization to the caller.
        g_jit_deopt_state.instr_pc = args->instruction_idx;
        g_jit_deopt_state.deopt_happened = true;
        *static_cast<uint64_t*>(args->ret_buf) = kDeoptMagic;
    }
    return 0;
}

extern "C" void* CodegenGetTlab() noexcept {
    return &chaos::il2cpp::runtime_core::tls_tlab;
}

// ── Inline TLS TLAB access (eliminates helper call) ────────────────────────
//
// On Windows x64, __declspec(thread) variables are accessed via the GS segment:
//   TEB (gs:[0]) → ThreadLocalStoragePointer at gs:[0x58] → TLS slot array
//   __tls_index indexes into this array to find the module's TLS block base.
//   Each __declspec(thread) variable is at a fixed offset within that block.
//
// We cache both __tls_index and tls_tlab's TLS-block offset at startup so
// the JIT can emit the inline sequence without external symbol references.

// __tls_index is normally defined by the MSVC linker when it encounters .tls$
// sections in linked objects.  However, in static-lib-heavy configurations the
// linker may fail to extract TLS-section objects from static libraries and
// therefore not synthesize __tls_index.  Define it explicitly as a fallback.
// This is safe because InitTlsTlabInfo() caches __tls_index at startup and if
// no JIT codegen ever emits inline TLS access, the value is never used.
extern "C" uint32_t __tls_index = 0;

namespace {
// Cached values, computed once during InitTlsTlabInfo().
uint32_t g_cached_tls_index = 0;
uint32_t g_cached_tls_tlab_offset = 0;
} // namespace

#if defined(_WIN32) || defined(_WIN64)

void chaos::il2cpp::jit::InitTlsTlabInfo() noexcept {
    if (g_cached_tls_index != 0)
        return; // already initialized

    g_cached_tls_index = __tls_index;

    // Compute tls_tlab offset within the TLS block.
    // TLS base = TLS_array[__tls_index]
    uintptr_t tls_array = __readgsqword(0x58);
    uintptr_t tls_base = reinterpret_cast<uintptr_t*>(tls_array)[g_cached_tls_index];

    uintptr_t tls_tlab_addr = reinterpret_cast<uintptr_t>(&chaos::il2cpp::runtime_core::tls_tlab);
    g_cached_tls_tlab_offset = static_cast<uint32_t>(tls_tlab_addr - tls_base);
}

void chaos::il2cpp::jit::EmitLoadTlsTlab(CodeBuffer& buf) noexcept {
    // mov rax, gs:[58h] — ThreadLocalStoragePointer from TEB (x64)
    // Encoding: GS prefix (0x65) + REX.W (0x48) + MOV (0x8B) + ModRM/SIB/disp32
    // ModRM = 00 000 100 (mod=00, reg=0/RAX, r/m=100/SIB)
    // SIB   = 00 100 101 (scale=0, index=100/none, base=101/disp32)
    // disp32 = 0x00000058
    buf.EmitByte(0x65);
    buf.EmitByte(0x48);
    buf.EmitByte(0x8B);
    buf.EmitByte(0x04); // ModRM
    buf.EmitByte(0x25); // SIB
    buf.Emit32(0x58);

    // mov ecx, g_cached_tls_index — load module TLS slot index
    // Encoding: B9 <imm32>
    buf.EmitByte(0xB9);
    buf.Emit32(g_cached_tls_index);

    // mov rax, [rax + rcx*8] — dereference TLS array slot → TLS block base
    // Encoding: REX.W (0x48) + MOV (0x8B) + ModRM/SIB
    // ModRM = 00 000 100 (mod=00, reg=0/RAX, r/m=100/SIB)
    // SIB   = 11 001 000 (scale=3/x8, index=1/RCX, base=0/RAX)
    buf.EmitByte(0x48);
    buf.EmitByte(0x8B);
    buf.EmitByte(0x04);
    buf.EmitByte(0xC8);

    // add rax, g_cached_tls_tlab_offset — adjust to &tls_tlab
    // Only emit if non-zero (common case: tls_tlab is usually first or near-first
    // in the TLS block, but we don't assume).
    // Encoding: REX.W (0x48) + ADD eAX, imm32 (0x05) + <imm32>
    if (g_cached_tls_tlab_offset != 0) {
        buf.EmitByte(0x48);
        buf.EmitByte(0x05);
        buf.Emit32(g_cached_tls_tlab_offset);
    }
}

#elif defined(__linux__) && defined(__aarch64__)

void chaos::il2cpp::jit::InitTlsTlabInfo() noexcept {
    if (g_cached_tls_tlab_offset != 0)
        return; // already initialized

    // On Linux ARM64, __builtin_thread_pointer() expands to:
    //   MRS X0, TPIDR_EL0
    // which returns the thread-pointer (TLS block base address).
    uintptr_t tp = reinterpret_cast<uintptr_t>(__builtin_thread_pointer());
    uintptr_t tls_tlab_addr = reinterpret_cast<uintptr_t>(&chaos::il2cpp::runtime_core::tls_tlab);
    g_cached_tls_tlab_offset = static_cast<uint32_t>(tls_tlab_addr - tp);
}

void chaos::il2cpp::jit::EmitLoadTlsTlab(CodeBuffer& buf) noexcept {
    // MRS X0, TPIDR_EL0 — load thread pointer into X0 (kScratchA)
    // Encoding: 0xD53BD040 + Rd (Rd=0 => 0xD53BD040)
    buf.Emit32(0xD53BD040);

    // ADD X0, X0, #offset — adjust to &tls_tlab
    // 64-bit ADD immediate: 0x91000000 | (imm12 << 10) | (Rn << 5) | Rd
    // Here Rn=Rd=0 (X0), so: 0x91000000 | (offset << 10)
    if (g_cached_tls_tlab_offset != 0) {
        buf.Emit32(0x91000000 | (g_cached_tls_tlab_offset << 10));
    }
}

#endif // _WIN32/WIN64 / __linux__+__aarch64__

extern "C" void* CodegenBox(uint64_t value, uint8_t tag, uint32_t type_token) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::Box");
    using namespace chaos::il2cpp::interpreter;
    using namespace chaos::il2cpp::runtime_core;
    // Use GcAllocateAtomic: BoxedValue has no interior pointers (just an
    // InterpreterValue union), so the GC does not need to scan it.
    auto* boxed = static_cast<BoxedValue*>(GcAllocateAtomic(sizeof(BoxedValue)));
    if (boxed == nullptr)
        return nullptr;
    // GcAllocateAtomic already zeroes memory — InterpreterValue tag defaults
    // to Void (0), which is correct.  No placement new needed.
    // Manually init the value from the raw bits.
    ValueTag vt = static_cast<ValueTag>(tag);
    switch (vt) {
        case ValueTag::Int32:
            boxed->value = InterpreterValue::from_i32(static_cast<int32_t>(value));
            break;
        case ValueTag::Int64:
            boxed->value = InterpreterValue::from_i64(static_cast<int64_t>(value));
            break;
        case ValueTag::Float32: {
            float fv;
            std::memcpy(&fv, &value, sizeof(float));
            boxed->value = InterpreterValue::from_f32(fv);
            break;
        }
        case ValueTag::Float64: {
            double dv;
            std::memcpy(&dv, &value, sizeof(double));
            boxed->value = InterpreterValue::from_f64(dv);
            break;
        }
        default:
            boxed->value = InterpreterValue::from_obj(reinterpret_cast<void*>(value));
            break;
    }
    return boxed;
}

extern "C" void* CodegenNewObj(uint32_t type_token, uint32_t field_count) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::NewObj");
    using namespace chaos::il2cpp::interpreter;
    using namespace chaos::il2cpp::runtime_core;
    // Allocate through the GC heap so the object is tracked for GC scanning
    // and compaction.  GcAllocateProfiled returns zeroed memory.
    auto* obj = static_cast<InterpreterObject*>(GcAllocateProfiled(sizeof(InterpreterObject)));
    if (obj == nullptr)
        return nullptr;
    // Manual init: GcAllocate zeroes everything, so we only need to point
    // fields_ptr_ to the inline buffer and set type_token.
    // Do NOT call placement new (no vector construction on GC memory).
    // Do NOT call fields.resize() — field vector starts empty; the
    // first LdFld/StFld access lazily grows via SmallFieldArray::resize().
    obj->fields.fields_ptr_ = obj->fields.inline_;
    obj->fields.field_count_ = 0;
    obj->fields.field_capacity_ = SmallFieldArray::kInlineCapacity;
    obj->type_token = type_token;
    return obj;
}

extern "C" int32_t CodegenLdLen(void* arr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LdLen");
    using namespace chaos::il2cpp::interpreter;
    if (arr == nullptr)
        return 0;
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
    // g_static_fields is a plain non-GC vector (not registered via
    // GcRegisterStaticRootRange).  Its slots hold interpreter-sandbox
    // InterpreterValue / InterpreterObject* values in the common case, so
    // reverse-registering them as GC roots would make BgcRecordRootChange
    // re-mark an interpreter-heap pointer as a managed object at BGC re-mark
    // — memory corruption.  Consistent with the barrier-free Reg_StSFld /
    // Handle_StSFld paths (fix e6ea531e7).
    //
    // Only when the stored value is actually a GC-managed pointer do we emit
    // the SATB / root-change barrier, so a real GC→static GC edge is never
    // dropped.  Non-GC values skip it, exactly matching the other two paths.
    void* obj = reinterpret_cast<void*>(static_cast<uint64_t>(value));
    if (chaos::il2cpp::runtime_core::chaos_is_gc_pointer(obj)) {
        JitSatbPreWriteBarrier(reinterpret_cast<void**>(&g_static_fields[field_offset].obj));
        chaos::il2cpp::runtime_core::BgcRecordRootChange(
            reinterpret_cast<void**>(&g_static_fields[field_offset].obj),
            g_static_fields[field_offset].obj);
    }
    g_static_fields[field_offset] = InterpreterValue::from_i64(static_cast<int64_t>(value));
}

// ── Array helpers ─────────────────────────────────────────────────────────

extern "C" void* CodegenNewArr(int32_t length) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::NewArr");
    using namespace chaos::il2cpp::interpreter;
    using namespace chaos::il2cpp::runtime_core;
    // Allocate through GC heap.  GcAllocateProfiled returns zeroed memory; on MSVC
    // this is equivalent to a default-constructed empty std::vector (all
    // internal pointers null).  The subsequent resize allocates the vector's
    // internal buffer on C++ heap.
    // TODO(Phase 3.4): Replace std::vector with GC-allocated buffer to
    // eliminate the hidden C++ heap allocation and enable full GC tracking.
    auto* arr = static_cast<ArrayStorage*>(GcAllocateProfiled(sizeof(ArrayStorage)));
    if (arr == nullptr)
        return nullptr;
    // GcAllocate zeroes memory — vector is empty.  No placement new needed.
    arr->elements.resize(static_cast<size_t>(length > 0 ? length : 0));
    return arr;
}

extern "C" void* CodegenNewArrTlab(void* mem, int32_t length) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::NewArrTlab");
    using namespace chaos::il2cpp::interpreter;
    auto* arr = static_cast<ArrayStorage*>(mem);
    if (arr == nullptr)
        return nullptr;
    ::new (arr) ArrayStorage();
    arr->elements.resize(static_cast<size_t>(length > 0 ? length : 0));
    return arr;
}

extern "C" uint64_t CodegenLdElem(void* arr, int32_t index) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LdElem");
    using namespace chaos::il2cpp::interpreter;
    if (arr == nullptr)
        return 0;
    auto* as = static_cast<ArrayStorage*>(arr);
    if (index < 0 || static_cast<size_t>(index) >= as->elements.size())
        return 0;
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
    using namespace chaos::il2cpp::runtime_core;
    using namespace chaos::il2cpp::runtime_core::threading;
    if (arr == nullptr)
        return;
    auto* as = static_cast<ArrayStorage*>(arr);
    auto idx = static_cast<size_t>(index >= 0 ? index : 0);
    if (idx >= as->elements.size()) {
        as->elements.resize(idx + 1u);      // C++ heap alloc — outside barrier scope
    }
    JitSatbPreWriteBarrier(reinterpret_cast<void**>(&as->elements[idx].obj));  // SATB — outside scope (can allocate)
    bool is_gc = chaos_is_gc_pointer(as);
    // store→card in one non-preemptible critical section (A2b close).
    {
        BarrierCriticalSectionScope barrier;
        as->elements[idx] = InterpreterValue::from_i64(static_cast<int64_t>(value));
        if (is_gc) {
            chaos_gc_dirty_card(as);
        }
    }
}

extern "C" uint64_t CodegenLdElemNoCheck(void* arr, int32_t index) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LdElemNoCheck");
    using namespace chaos::il2cpp::interpreter;
    // Skip NULL check and bounds check — BCE has proven safety
    auto* as = static_cast<ArrayStorage*>(arr);
    auto idx = static_cast<size_t>(index >= 0 ? index : 0);
    const auto& iv = as->elements[idx];
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

extern "C" void CodegenStElemNoCheck(void* arr, int32_t index, uint64_t value) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::StElemNoCheck");
    using namespace chaos::il2cpp::interpreter;
    using namespace chaos::il2cpp::runtime_core;
    using namespace chaos::il2cpp::runtime_core::threading;
    // Skip NULL check and bounds check — BCE has proven safety
    auto* as = static_cast<ArrayStorage*>(arr);
    auto idx = static_cast<size_t>(index >= 0 ? index : 0);
    JitSatbPreWriteBarrier(reinterpret_cast<void**>(&as->elements[idx].obj));  // SATB — outside scope (can allocate)
    bool is_gc = chaos_is_gc_pointer(as);
    // store→card in one non-preemptible critical section (A2b close).
    {
        BarrierCriticalSectionScope barrier;
        as->elements[idx] = InterpreterValue::from_i64(static_cast<int64_t>(value));
        if (is_gc) {
            chaos_gc_dirty_card(as);
        }
    }
}

// ── Type check helpers ──────────────────────────────────────────────────────

extern "C" void* CodegenCastClass(void* obj, uint32_t target_token) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::CastClass");
    using namespace chaos::il2cpp::interpreter;
    if (obj == nullptr)
        return nullptr;
    auto* io = static_cast<InterpreterObject*>(obj);
    return (io->type_token == target_token) ? obj : nullptr;
}

extern "C" void* CodegenIsInst(void* obj, uint32_t target_token) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::IsInst");
    using namespace chaos::il2cpp::interpreter;
    if (obj == nullptr)
        return nullptr;
    auto* io = static_cast<InterpreterObject*>(obj);
    return (io->type_token == target_token) ? obj : nullptr;
}

// ── Unbox helper ──────────────────────────────────────────────────────────

extern "C" uint64_t CodegenUnbox(void* obj) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::Unbox");
    using namespace chaos::il2cpp::interpreter;
    if (obj == nullptr)
        return 0;
    // BoxedValue layout: single InterpreterValue (ECMA Box/Unbox semantics).
    // Both the TLAB inline path and CodegenBox create BoxedValue, not
    // InterpreterObject, so we must read the value field directly.
    const auto& iv = static_cast<BoxedValue*>(obj)->value;
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
    if (obj == nullptr)
        return nullptr;
    using namespace chaos::il2cpp::interpreter;
    auto* io = static_cast<InterpreterObject*>(obj);
    uint32_t type_token = io->type_token;
    if (type_token == 0 || method_token == 0)
        return nullptr;
    return chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(type_token, method_token);
}

// ── InitObj helper ─────────────────────────────────────────────────────────

extern "C" void CodegenInitObj(void* ptr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::InitObj");
    if (ptr == nullptr)
        return;
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
    using namespace chaos::il2cpp::runtime_core;
    using namespace chaos::il2cpp::runtime_core::threading;
    if (ptr == nullptr)
        return;
    auto* iv = static_cast<InterpreterValue*>(ptr);
    // SATB pre-write barrier: record old value before overwriting.
    // Guard with heap check: stack-allocated value type slots don't
    // need SATB recording (stack is conservatively scanned by BGC).
    // SATB stays OUTSIDE the barrier scope — it can allocate and must not
    // be made non-preemptible.
    if (chaos_is_gc_pointer(ptr)) {
        JitSatbPreWriteBarrier(reinterpret_cast<void**>(&iv->obj));
    }
    bool is_gc = chaos_is_gc_pointer(ptr);
    // store→card in one non-preemptible critical section (A2b close).
    {
        BarrierCriticalSectionScope barrier;
        *iv = InterpreterValue::from_i64(static_cast<int64_t>(value));
        if (is_gc) {
            chaos_gc_dirty_card(ptr);
        }
    }
}

// ── Cpblk helper ─────────────────────────────────────────────────────────────

extern "C" void CodegenCpblk(void* dst, const void* src, uint32_t count) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::Cpblk");
    if (dst == nullptr || src == nullptr || count == 0)
        return;
    std::memcpy(dst, src, count);
}

// ── InitBlk helper ───────────────────────────────────────────────────────────

extern "C" void CodegenInitBlk(void* dst, uint32_t value, uint32_t count) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::InitBlk");
    if (dst == nullptr || count == 0)
        return;
    std::memset(dst, static_cast<int>(value & 0xFF), count);
}

// ── LdObj helper ────────────────────────────────────────────────────────────

extern "C" uint64_t CodegenLdObj(void* ptr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LdObj");
    using namespace chaos::il2cpp::interpreter;
    if (ptr == nullptr)
        return 0;
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

    // Heap fallback: allocate through GC heap (atomic — raw memory, no GC scanning).
    // GcAllocateAtomic returns zeroed memory.
    using namespace chaos::il2cpp::runtime_core;
    void* mem = GcAllocateAtomic(size);
    if (mem == nullptr)
        return nullptr;
    // GcAllocateAtomic already zeroes memory.
    return mem;
}

// ── Inline deoptimization state saver for EmitDeoptSequence ─────────────────────

extern "C" void DeoptSaveFrameState(uint64_t codegen_rsp) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::DeoptSaveFrameState");
    using namespace chaos::il2cpp::jit;
    using namespace chaos::il2cpp::interpreter;

    if (codegen_rsp == 0)
        return;

    // Get the return address — points to the instruction after the CALL
    // to DeoptSaveFrameState in the generated code.
    void* ret_addr = _ReturnAddress();

    // Find the JitMethod covering this code address.
    const JitMethod* nm = FindNativeCodeByAddress(ret_addr);
    if (nm == nullptr)
        return;

    uint64_t code_base = reinterpret_cast<uint64_t>(nm->code);
    uint32_t native_off = static_cast<uint32_t>(reinterpret_cast<uint64_t>(ret_addr) - code_base);

    // Batch-copy all 64 GPR values from stack frame spill slots.
    // GPR file starts at codegen_rsp + kGprFileOff (= codegen_rsp + 32).
    for (uint32_t vr = 0; vr < kGprCount; ++vr) {
        g_jit_deopt_state.gpr_file[vr] = *reinterpret_cast<const uint64_t*>(codegen_rsp + kGprFileOff + vr * 8);
    }

    // Batch-copy all 32 FPR values from stack frame spill slots.
    // FPR file starts at codegen_rsp + kFprFileOff.  Each virtual FPR occupies
    // a kFprSlotSize-byte slot (32 on x64 / 16 on ARM64) as laid out by
    // FprOff()/StoreFpr — must read with that stride, NOT 8, or vregs >= 65
    // desync (Producer writes stride 32, a stride-8 read re-reads the tail of
    // the previous slot).  Verified vs StoreFpr movejdqa → FprOff(vreg).
    for (uint32_t vr = 0; vr < kFprCount; ++vr) {
        g_jit_deopt_state.fpr_file[vr] =
            *reinterpret_cast<const double*>(codegen_rsp + kFprFileOff + vr * kFprSlotSize);
    }

    // Look up type tags from the DeoptEntry for this native offset.
    const DeoptEntry* entry = DeoptRuntime::FindEntry(nm, native_off);
    if (entry != nullptr && nm->deopt_values != nullptr) {
        std::memset(g_jit_deopt_state.gpr_tags, static_cast<int>(ValueTag::Int64), 64);
        std::memset(g_jit_deopt_state.fpr_tags, static_cast<int>(ValueTag::Float64), 32);
        for (uint32_t i = 0; i < entry->num_values; ++i) {
            const auto& dv = nm->deopt_values[entry->values_offset + i];
            if (dv.reg_index < 64) {
                g_jit_deopt_state.gpr_tags[dv.reg_index] = dv.value_tag;
            } else {
                g_jit_deopt_state.fpr_tags[dv.reg_index - 64] = dv.value_tag;
            }
        }
        g_jit_deopt_state.instr_pc = entry->instr_pc;
        g_jit_deopt_state.osr_resume_pc = entry->osr_resume_pc;
    }

    g_jit_deopt_state.deopt_happened = true;
}

// ── OSR loop header resolver ─────────────────────────────────────────────────

extern "C" void* OsrResolveLoopHeader() noexcept {
    using namespace chaos::il2cpp::jit;
    void* ret_addr = _ReturnAddress();
    const JitMethod* nm = FindNativeCodeByAddress(ret_addr);
    if (nm == nullptr)
        return nullptr;
    if (nm->instr_offsets == nullptr)
        return nullptr;
    if (nm->instr_offset_count == 0)
        return nullptr;

    uint32_t resume_pc = g_jit_deopt_state.osr_resume_pc;
    if (resume_pc >= nm->instr_offset_count) {
        resume_pc = 0; // fall back to instruction 0
    }
    uint32_t native_off = nm->instr_offsets[resume_pc];
    return static_cast<uint8_t*>(nm->code) + native_off;
}

// ── Deoptimization trampoline entry point ─────────────────────────────────────

extern "C" void DeoptTrapEntry(const void* ctx, uint64_t codegen_rsp) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::DeoptTrapEntry");
    using namespace chaos::il2cpp::jit;

    if (ctx == nullptr)
        return;

    // 1. Get the return address within the trampoline — this is the address
    //    that the CALL to DeoptTrapEntry pushed.
    void* ret_addr = _ReturnAddress();

    // 2. Find the JitMethod covering this code address.
    const JitMethod* nm = FindNativeCodeByAddress(ret_addr);
    if (nm == nullptr) {
        return;
    }

    // 3. Compute native offset from the code entry point.
    uint64_t code_base = reinterpret_cast<uint64_t>(nm->code);
    uint32_t native_offset = static_cast<uint32_t>(reinterpret_cast<uint64_t>(ret_addr) - code_base);

    // 4. Deoptimize: reconstruct register file from stack frame spill slots.
    const NativeContext* nctx = static_cast<const NativeContext*>(ctx);
    DeoptRuntime::DeoptTrap(const_cast<JitMethod*>(nm), native_offset, *nctx, codegen_rsp, g_jit_deopt_state.gpr_file,
                            g_jit_deopt_state.fpr_file, g_jit_deopt_state.gpr_tags, g_jit_deopt_state.fpr_tags);

    // 5. Find the DeoptEntry to get the instruction pc.
    const DeoptEntry* entry = DeoptRuntime::FindEntry(nm, native_offset);
    if (entry == nullptr) {
        // No matching deopt entry — can't reconstruct pc.
        return;
    }

    // 6. Set TLS state: the trampoline will write kDeoptMagic to ret_buf[0]
    //    and return.  InterpreterEntryDirect reads this state to reconstruct
    //    the RegisterFrame for RegisterExecute.
    g_jit_deopt_state.instr_pc = entry->instr_pc;
    g_jit_deopt_state.osr_resume_pc = entry->osr_resume_pc;
    g_jit_deopt_state.deopt_happened = true;
}
