// ir_reg_gc_helpers.cpp — GC write-barrier primitives for Reg_StInd/Reg_StObj/static field access
// (textually included from ir_reg_alloc.cpp, INSIDE namespace chaos::il2cpp::interpreter)
//
// GC headers (gc_api.h, gc_bgc_inline.h, gc_card_table.h, forbid_suspend.h) are included
// from ir_reg_alloc.cpp at global scope before this textual include, so they are NOT
// repeated here — repeating them inside the interpreter namespace would nest their
// `chaos::il2cpp::runtime_core` declarations incorrectly.

// GCC/Clang: suppress -Wparentheses for Reg_StInd's `if (is_gc) { barrier; } else { ... }` pattern.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses"
#endif

// ── LdSFld / StSFld: static field access ───────────────────────────────
// g_static_fields is defined in interpreter_vm.cpp at namespace scope.
extern CHAOS_IL2CPP_VECTOR(InterpreterValue) g_static_fields;

static void Reg_LdSFld(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdSFld");
    uint32_t offset = instr.imm.field_offset;
    auto& sfields = g_static_fields;
    if (sfields.size() <= offset) {
        sfields.resize(offset + 1u);
    }
    const auto& iv = sfields[offset];
    switch (iv.tag) {
        case ValueTag::Int32:
            frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(iv.i32)),
                               static_cast<uint8_t>(ValueTag::Int32));
            break;
        case ValueTag::Int64:
            frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(iv.i64), static_cast<uint8_t>(ValueTag::Int64));
            break;
        case ValueTag::Float32: {
            uint64_t val;
            std::memcpy(&val, &iv.f32, sizeof(float));
            frame.regs.set_reg(instr.dst_reg(), val, static_cast<uint8_t>(ValueTag::Float32));
            break;
        }
        case ValueTag::Float64: {
            uint64_t val;
            std::memcpy(&val, &iv.f64, sizeof(double));
            frame.regs.set_reg(instr.dst_reg(), val, static_cast<uint8_t>(ValueTag::Float64));
            break;
        }
        default:
            frame.regs.set_reg(instr.dst_reg(), reinterpret_cast<uint64_t>(iv.obj),
                               static_cast<uint8_t>(ValueTag::ObjectRef));
            break;
    }
    ++frame.pc;
}

static void Reg_StSFld(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_StSFld");
    uint32_t offset = instr.imm.field_offset;
    auto& sfields = g_static_fields;
    if (sfields.size() <= offset) {
        sfields.resize(offset + 1u);
    }
    uint8_t tag = frame.regs.reg_tag(instr.src1_reg());
    uint64_t val = frame.regs.reg(instr.src1_reg());
    switch (static_cast<ValueTag>(tag)) {
        case ValueTag::Int32:
            sfields[offset] = InterpreterValue::from_i32(static_cast<int32_t>(val));
            break;
        case ValueTag::Int64:
            sfields[offset] = InterpreterValue::from_i64(static_cast<int64_t>(val));
            break;
        case ValueTag::Float32: {
            float fv;
            std::memcpy(&fv, &val, sizeof(float));
            sfields[offset] = InterpreterValue::from_f32(fv);
            break;
        }
        case ValueTag::Float64: {
            double dv;
            std::memcpy(&dv, &val, sizeof(double));
            sfields[offset] = InterpreterValue::from_f64(dv);
            break;
        }
        default:
            sfields[offset] = InterpreterValue::from_obj(reinterpret_cast<void*>(val));
            break;
    }
    ++frame.pc;
}

// ── StInd: indirect store ───────────────────────────────────────────────
static void Reg_StInd(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_StInd");
    uint64_t val = frame.regs.reg(instr.src1_reg());
    void* ptr = reinterpret_cast<void*>(frame.regs.reg(instr.src2_reg()));
    if (ptr != nullptr) {
        const bool is_gc = chaos::il2cpp::runtime_core::chaos_is_gc_pointer(ptr);
        if (is_gc) {
            using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
            BgcSatbPreWriteBarrier(reinterpret_cast<void**>(ptr));
        }
        if (is_gc) {
            using chaos::il2cpp::runtime_core::threading::BarrierCriticalSectionScope;
            BarrierCriticalSectionScope barrier;
            *static_cast<uint64_t*>(ptr) = val;
            chaos_gc_dirty_card(ptr);
        } else {
            *static_cast<uint64_t*>(ptr) = val;
        }
    }
    ++frame.pc;
}

// ── StObj: store object to managed pointer ──────────────────────────────
static void Reg_StObj(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_StObj");
    uint64_t val = frame.regs.reg(instr.src1_reg());
    uint8_t tag = frame.regs.reg_tag(instr.src1_reg());
    void* ptr = reinterpret_cast<void*>(frame.regs.reg(instr.src2_reg()));
    if (ptr != nullptr) {
        auto* iv = static_cast<InterpreterValue*>(ptr);
        if (chaos::il2cpp::runtime_core::chaos_is_gc_pointer(iv)) {
            using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
            using chaos::il2cpp::runtime_core::threading::BarrierCriticalSectionScope;
            BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&iv->obj));
            {
                BarrierCriticalSectionScope barrier;
                iv->tag = static_cast<ValueTag>(tag);
                iv->i64 = static_cast<int64_t>(val);
                chaos_gc_dirty_card(iv);
            }
        } else {
            iv->tag = static_cast<ValueTag>(tag);
            iv->i64 = static_cast<int64_t>(val);
        }
    }
    ++frame.pc;
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif