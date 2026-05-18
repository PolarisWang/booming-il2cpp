#ifndef CHAOS_IL2CPP_FAST_DISPATCH_H_
#define CHAOS_IL2CPP_FAST_DISPATCH_H_

// ── FastFrame + function-pointer dispatch table ─────────────────────────
//
// Lightweight execution frame for methods without SEH (no setjmp/longjmp).
// Uses fixed-size arrays instead of std::vector<InterpreterValue>.
// Two-layer optimization:
//   Layer 1: function pointer array (kHandlers[99]) replaces while-switch
//   Layer 2: FastFrame (fixed-size stack/locals) replaces ExecutionFrame
//
// FastExecute checks for SEH at entry; SEH methods fall back to InterpreterVM.

#include "patch_loader.h"
#include "runtime_core.h"

#include <interpreter_vm.h>

#include <chaos/log.h>

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ── FastFrame ──────────────────────────────────────────────────────────
// Fixed-size value stack and local variables. All values stored as uint64_t
// with a separate tag byte. No heap allocation during execution.
struct FastFrame {
    static constexpr uint32_t kMaxStack  = 16;
    static constexpr uint32_t kMaxLocals = 8;

    uint64_t  stack[kMaxStack]        = {};
    uint8_t   stack_tags[kMaxStack]   = {};
    uint32_t  sp                      = 0;

    uint64_t  locals[kMaxLocals]      = {};
    uint8_t   local_tags[kMaxLocals]  = {};
    uint32_t  local_count             = 0;

    const void* args                  = nullptr;
    uint32_t    arg_count             = 0;

    // Optional pointer to per-argument type tags (from PatchMethod::cached_arg_types).
    // Used by Handle_LdArg to emit the correct ValueTag instead of assuming ObjectRef.
    // nullptr = all args treated as ObjectRef (legacy behavior).
    const uint8_t* arg_type_tags       = nullptr;

    uint64_t    ret_val               = 0;
    uint8_t     ret_tag               = 0;
    bool        has_ret               = false;
    bool        threw_exception       = false;
    void*       exception_obj_val     = nullptr;

    // Dispatch callback for Call instructions.
    // Set by InterpreterEntryDirect before calling FastExecute.
    void*       dispatch_fn           = nullptr;
    void*       dispatch_ctx          = nullptr;

    // Optional call-site metadata cache (CachedCallInfo[]).
    // Populated during IR lowering. One entry per IR instruction.
    // Handle_Call reads cache[pc] to skip runtime reflection.
    const void* call_cache             = nullptr;
    uint32_t    call_count             = 0;

    uint32_t    pc                    = 0;

    // ── Tracked object cleanup for interpreter heap objects ──────────
    // Fast dispatch heap-allocates InterpreterObject/ArrayStorage via
    // CHAOS_IL2CPP_MALLOC+placement new.  We track pointers here so
    // FastExecute / callers can free everything on normal/fallback exit,
    // eliminating the R1 operator-new leak.
    static constexpr uint32_t kMaxTracked = 8;
    void*     tracked_objs[kMaxTracked]{};
    void (*tracked_dtors[kMaxTracked])(void*){};
    bool      tracked_is_pool[kMaxTracked]{};  // true = pool-allocated (skip free)
    uint32_t  tracked_cnt            = 0;

    template<typename T>
    static void Dtor(void* p) noexcept { static_cast<T*>(p)->~T(); }

    void Track(void* ptr, void (*dtor)(void*)) noexcept {
        if (tracked_cnt < kMaxTracked) {
            tracked_objs[tracked_cnt]    = ptr;
            tracked_dtors[tracked_cnt]   = dtor;
            tracked_is_pool[tracked_cnt] = false;
            ++tracked_cnt;
        } else {
            CHAOS_IL2CPP_LOG_WARN_M("FastFrame", "Track overflow, ptr={0}", reinterpret_cast<const void*>(ptr));
        }
    }

    /// Track a pool-allocated object. CleanupTracked calls the dtor but
    /// skips the CHAOS_IL2CPP_FREE (the object returns to its pool).
    void TrackPool(void* ptr, void (*dtor)(void*)) noexcept {
        if (tracked_cnt < kMaxTracked) {
            tracked_objs[tracked_cnt]    = ptr;
            tracked_dtors[tracked_cnt]   = dtor;
            tracked_is_pool[tracked_cnt] = true;
            ++tracked_cnt;
        } else {
            CHAOS_IL2CPP_LOG_WARN_M("FastFrame", "TrackPool overflow, ptr={0}", reinterpret_cast<const void*>(ptr));
        }
    }

    void CleanupTracked() noexcept {
        for (uint32_t i = 0; i < tracked_cnt; ++i) {
            tracked_dtors[i](tracked_objs[i]);
            if (!tracked_is_pool[i]) {
                CHAOS_IL2CPP_FREE(tracked_objs[i]);
            }
        }
        tracked_cnt = 0;
    }

    // ── Push helpers ─────────────────────────────────────────────────
    void PushI32(int32_t v) noexcept {
        CHAOS_IL2CPP_ASSERT(sp < kMaxStack && "FastFrame stack overflow");
        if (sp >= kMaxStack) { threw_exception = true; return; }
        stack[sp] = static_cast<uint64_t>(v);
        stack_tags[sp] = static_cast<uint8_t>(interpreter::ValueTag::Int32);
        ++sp;
    }

    void PushI64(int64_t v) noexcept {
        CHAOS_IL2CPP_ASSERT(sp < kMaxStack && "FastFrame stack overflow");
        if (sp >= kMaxStack) { threw_exception = true; return; }
        stack[sp] = static_cast<uint64_t>(v);
        stack_tags[sp] = static_cast<uint8_t>(interpreter::ValueTag::Int64);
        ++sp;
    }

    void PushF32(float v) noexcept {
        CHAOS_IL2CPP_ASSERT(sp < kMaxStack && "FastFrame stack overflow");
        if (sp >= kMaxStack) { threw_exception = true; return; }
        std::memcpy(&stack[sp], &v, sizeof(float));
        stack_tags[sp] = static_cast<uint8_t>(interpreter::ValueTag::Float32);
        ++sp;
    }

    void PushF64(double v) noexcept {
        CHAOS_IL2CPP_ASSERT(sp < kMaxStack && "FastFrame stack overflow");
        if (sp >= kMaxStack) { threw_exception = true; return; }
        std::memcpy(&stack[sp], &v, sizeof(double));
        stack_tags[sp] = static_cast<uint8_t>(interpreter::ValueTag::Float64);
        ++sp;
    }

    void PushObj(void* v) noexcept {
        CHAOS_IL2CPP_ASSERT(sp < kMaxStack && "FastFrame stack overflow");
        if (sp >= kMaxStack) { threw_exception = true; return; }
        stack[sp] = reinterpret_cast<uint64_t>(v);
        stack_tags[sp] = static_cast<uint8_t>(interpreter::ValueTag::ObjectRef);
        ++sp;
    }

    void PushNull() noexcept {
        CHAOS_IL2CPP_ASSERT(sp < kMaxStack && "FastFrame stack overflow");
        if (sp >= kMaxStack) { threw_exception = true; return; }
        stack[sp] = 0;
        stack_tags[sp] = static_cast<uint8_t>(interpreter::ValueTag::Null);
        ++sp;
    }

    void PushIV(const interpreter::InterpreterValue& iv) noexcept;

    // ── Pop helpers ──────────────────────────────────────────────────
    int32_t PopI32() noexcept {
        --sp;
        return static_cast<int32_t>(stack[sp]);
    }

    int64_t PopI64() noexcept {
        --sp;
        return static_cast<int64_t>(stack[sp]);
    }

    float PopF32() noexcept {
        --sp;
        float v;
        std::memcpy(&v, &stack[sp], sizeof(float));
        return v;
    }

    double PopF64() noexcept {
        --sp;
        double v;
        std::memcpy(&v, &stack[sp], sizeof(double));
        return v;
    }

    void* PopObj() noexcept {
        --sp;
        return reinterpret_cast<void*>(stack[sp]);
    }

    interpreter::InterpreterValue PopIV() noexcept;
};

// ── OpHandler ──────────────────────────────────────────────────────────
// Each handler processes one IR instruction on FastFrame.
// Handler must advance frame.pc (typically ++frame.pc, or set to target).
using OpHandler = void (*)(FastFrame& frame,
                           const interpreter::IRInstruction& instr) noexcept;

// ── Dispatch table ─────────────────────────────────────────────────────
// Indexed by IROpCode value (0..98). One handler per opcode.
// Unimplemented opcodes point to HandleUnsupported (triggers fallback).
extern const OpHandler kHandlers[99];

// ── FastExecute ────────────────────────────────────────────────────────
// Execute a method's IR using function-pointer dispatch + FastFrame.
// Returns true if execution completed normally, false if an unsupported
// opcode was encountered (caller should fall back to InterpreterVM).
//
// On normal return: frame.has_ret indicates whether a value was produced.
// On exception: frame.threw_exception is set.
bool FastExecute(FastFrame& frame,
                 const interpreter::IRInstruction* instrs,
                 uint32_t instr_count) noexcept;

// ── Fallback: check if FastExecute can handle this method ───────────────
// FastExecute supports methods WITHOUT SEH.  The caller should pre-check
// and only call FastExecute when seh_clauses is empty.
inline bool CanFastExecute(const interpreter::IRMethod& ir) noexcept {
    return ir.seh_clauses.empty();
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_FAST_DISPATCH_H_