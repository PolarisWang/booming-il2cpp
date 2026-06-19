#include "interpreter_vm.h"

#include <cstring>
#include <stdexcept>
#include <vector>

#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include "vtable_registry.h"
#include "generated_code_compat.h"
#include <memory_domain.h>

#include <gc/gc_bgc_inline.h>

// ── Allocation counter tracking ───────────────────────────────────
// Incremented by interpreter NewObj/NewArr to make interpreted method
// allocations visible to chaos_gc_get_allocated_bytes_for_current_thread().
// Declared in gc_alloc_stubs.h but included here directly for independence.
namespace chaos { namespace il2cpp { namespace runtime_core {
    extern thread_local CHAOS_IL2CPP_SIZE tls_alloc_fast_count;
    extern thread_local CHAOS_IL2CPP_SIZE tls_alloc_fast_bytes;
}}}
#include <gc/gc_root_change.h>
#include <gc/gc_helpers.h>
#include <chaos/pal/pal_eh.h>

#if CHAOS_IL2CPP_DEBUGGER
#include <diagnostics/debugger/dbg_runtime.h>
#endif

namespace chaos::il2cpp::interpreter {

namespace {

using ObjectStorage = InterpreterObject;

/// Max argument count for stack-allocated call_args buffer in Call handler.
/// The vast majority of managed methods have < 8 parameters.
static constexpr CHAOS_IL2CPP_UINT32 kMaxCallArgs = 8u;

}  // anonymous namespace

/// Definition of the process-global static fields vector.
/// Extern declaration in fast_dispatch.cpp.
CHAOS_IL2CPP_VECTOR(InterpreterValue) g_static_fields;

namespace {

CHAOS_IL2CPP_INT32 ReadInt32(const InterpreterValue& value) {
    switch (value.tag) {
        case ValueTag::Int32:
            return value.i32;
        case ValueTag::Int64:
            return static_cast<CHAOS_IL2CPP_INT32>(value.i64);
        case ValueTag::Float32:
            return static_cast<CHAOS_IL2CPP_INT32>(value.f32);
        case ValueTag::Float64:
            return static_cast<CHAOS_IL2CPP_INT32>(value.f64);
        case ValueTag::Null:
            return 0;
        default:
            throw CHAOS_IL2CPP_RUNTIME_ERROR("value is not convertible to int32");
    }
}

CHAOS_IL2CPP_INT64 ReadInt64(const InterpreterValue& value) {
    switch (value.tag) {
        case ValueTag::Int32:
            return static_cast<CHAOS_IL2CPP_INT64>(value.i32);
        case ValueTag::Int64:
            return value.i64;
        case ValueTag::Float32:
            return static_cast<CHAOS_IL2CPP_INT64>(value.f32);
        case ValueTag::Float64:
            return static_cast<CHAOS_IL2CPP_INT64>(value.f64);
        case ValueTag::Null:
            return 0;
        default:
            throw CHAOS_IL2CPP_RUNTIME_ERROR("value is not convertible to int64");
    }
}

float ReadFloat32(const InterpreterValue& value) {
    switch (value.tag) {
        case ValueTag::Int32:
            return static_cast<float>(value.i32);
        case ValueTag::Int64:
            return static_cast<float>(value.i64);
        case ValueTag::Float32:
            return value.f32;
        case ValueTag::Float64:
            return static_cast<float>(value.f64);
        case ValueTag::Null:
            return 0.0f;
        default:
            throw CHAOS_IL2CPP_RUNTIME_ERROR("value is not convertible to float32");
    }
}

double ReadFloat64(const InterpreterValue& value) {
    switch (value.tag) {
        case ValueTag::Int32:
            return static_cast<double>(value.i32);
        case ValueTag::Int64:
            return static_cast<double>(value.i64);
        case ValueTag::Float32:
            return static_cast<double>(value.f32);
        case ValueTag::Float64:
            return value.f64;
        case ValueTag::Null:
            return 0.0;
        default:
            throw CHAOS_IL2CPP_RUNTIME_ERROR("value is not convertible to float64");
    }
}

ObjectStorage* RequireObject(const InterpreterValue& value) {
    if (value.tag != ValueTag::ObjectRef || value.obj == nullptr) {
        throw CHAOS_IL2CPP_RUNTIME_ERROR("object reference required");
    }

    return static_cast<ObjectStorage*>(value.obj);
}

ArrayStorage* RequireArray(const InterpreterValue& value) {
    if (value.tag != ValueTag::ObjectRef || value.obj == nullptr) {
        throw CHAOS_IL2CPP_RUNTIME_ERROR("array reference required");
    }

    return static_cast<ArrayStorage*>(value.obj);
}

// ── SEH helper: find innermost try clause covering idx with finally/fault ──
static int FindEnclosingFinally(const IRMethod& method, CHAOS_IL2CPP_SIZE idx) {
    for (int i = static_cast<int>(method.seh_clauses.size()) - 1; i >= 0; --i) {
        const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(i)];
        if (idx >= clause.try_start_idx && idx < clause.try_end_idx) {
            const auto flags = static_cast<uint32_t>(clause.flags);
            if (flags == static_cast<uint32_t>(SEHFlags::Finally) ||
                flags == static_cast<uint32_t>(SEHFlags::Fault)) {
                return i;
            }
        }
    }
    return -1;
}

// ── SEH helper: find innermost try clause covering idx with catch handler ──
static int FindEnclosingCatch(const IRMethod& method, CHAOS_IL2CPP_SIZE idx) {
    for (int i = static_cast<int>(method.seh_clauses.size()) - 1; i >= 0; --i) {
        const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(i)];
        if (idx >= clause.try_start_idx && idx < clause.try_end_idx) {
            const auto flags = static_cast<uint32_t>(clause.flags);
            if (flags == static_cast<uint32_t>(SEHFlags::Exception) ||
                flags == static_cast<uint32_t>(SEHFlags::Filter)) {
                return i;
            }
        }
    }
    return -1;
}

}  // namespace

// ── InterpreterValue lifecycle ──────────────────────────────────────────

void InterpreterValue::FreeStruct() {
    if (tag == ValueTag::Struct && obj != nullptr) {
        CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(obj);
        obj = nullptr;
        struct_size = 0u;
    }
}

InterpreterValue::InterpreterValue(const InterpreterValue& other)
    : tag(other.tag)
    , struct_size(other.struct_size)
{
    if (tag == ValueTag::Struct && other.obj != nullptr) {
        // Deep-copy struct data.
        obj = CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(struct_size);
        if (obj != nullptr) {
            std::memcpy(obj, other.obj, struct_size);
        } else {
            struct_size = 0u;
        }
    } else if (tag == ValueTag::Int64) {
        i64 = other.i64;
    } else if (tag == ValueTag::Float64) {
        f64 = other.f64;
    } else {
        obj = other.obj;
    }
}

InterpreterValue& InterpreterValue::operator=(const InterpreterValue& other) {
    if (this == &other) {
        return *this;
    }

    // Free existing struct data before overwriting.
    FreeStruct();

    tag = other.tag;
    struct_size = other.struct_size;

    if (tag == ValueTag::Struct && other.obj != nullptr) {
        obj = CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(struct_size);
        if (obj != nullptr) {
            std::memcpy(obj, other.obj, struct_size);
        } else {
            struct_size = 0u;
        }
    } else if (tag == ValueTag::Int64) {
        i64 = other.i64;
    } else if (tag == ValueTag::Float64) {
        f64 = other.f64;
    } else {
        obj = other.obj;
    }

    return *this;
}

InterpreterValue::~InterpreterValue() {
    FreeStruct();
}

// ── Factory methods ────────────────────────────────────────────────────

InterpreterValue InterpreterValue::from_i32(CHAOS_IL2CPP_INT32 value) {
    return InterpreterValue(value);
}

InterpreterValue InterpreterValue::from_i64(CHAOS_IL2CPP_INT64 value) {
    InterpreterValue result = {};
    result.tag = ValueTag::Int64;
    result.i64 = value;
    return result;
}

InterpreterValue InterpreterValue::from_f32(float value) {
    InterpreterValue result = {};
    result.tag = ValueTag::Float32;
    result.f32 = value;
    return result;
}

InterpreterValue InterpreterValue::from_f64(double value) {
    InterpreterValue result = {};
    result.tag = ValueTag::Float64;
    result.f64 = value;
    return result;
}

InterpreterValue InterpreterValue::from_obj(void* value) {
    InterpreterValue result = {};
    result.tag = ValueTag::ObjectRef;
    result.obj = value;
    return result;
}

InterpreterValue InterpreterValue::from_struct(const void* data, CHAOS_IL2CPP_UINT32 size) {
    InterpreterValue result = {};
    result.tag = ValueTag::Struct;
    result.struct_size = size;
    if (data != nullptr && size > 0u) {
        result.obj = CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(size);
        if (result.obj != nullptr) {
            std::memcpy(result.obj, data, size);
        } else {
            result.struct_size = 0u;
        }
    }
    return result;
}

InterpreterValue InterpreterValue::null_val() {
    InterpreterValue result = {};
    result.tag = ValueTag::Null;
    result.obj = nullptr;
    return result;
}

// ── ExecutionFrame destructor ──────────────────────────────────────────

ExecutionFrame::~ExecutionFrame() {
    // Free struct data in all stored values.
    for (auto& v : arguments) { v.FreeStruct(); }
    for (auto& v : locals)    { v.FreeStruct(); }
    for (auto& v : stack)     { v.FreeStruct(); }
    // Free localloc allocations.
    for (auto* block : localloc_blocks) {
        CHAOS_IL2CPP_FREE(block);
    }
}

CHAOS_IL2CPP_SIZE InterpreterVM::GetBranchTarget(const IRMethod& method, CHAOS_IL2CPP_SIZE target) {
    if (target >= method.instructions.size()) {
        throw CHAOS_IL2CPP_OUT_OF_RANGE("branch_target");
    }

    return target;
}

void InterpreterVM::EnsureLocal(CHAOS_IL2CPP_VECTOR(InterpreterValue)* locals, CHAOS_IL2CPP_SIZE index) {
    if (locals == nullptr) {
        throw CHAOS_IL2CPP_INVALID_ARGUMENT("locals");
    }

    if (locals->size() <= index) {
        locals->resize(index + 1u, InterpreterValue{});
    }
}

InterpreterValue InterpreterVM::Pop(CHAOS_IL2CPP_VECTOR(InterpreterValue)* stack) {
    if (stack == nullptr || stack->empty()) {
        throw CHAOS_IL2CPP_RUNTIME_ERROR("evaluation stack underflow");
    }

    const InterpreterValue value = stack->back();
    stack->pop_back();
    return value;
}

ExecutionResult InterpreterVM::Execute(const IRMethod& method, ExecutionFrame* frame) const {
    if (frame == nullptr) {
        throw CHAOS_IL2CPP_INVALID_ARGUMENT("frame");
    }

    ExecutionResult result = {};
    CHAOS_IL2CPP_SIZE instruction_index = 0;

    // SEH execution state.
    bool        in_handler              = false;
    bool        pending_leave           = false;
    CHAOS_IL2CPP_SIZE pending_leave_target = 0u;
    int32_t     active_handler_clause   = -1;

    // Two-phase exception state.
    bool        exception_in_flight     = false;
    InterpreterValue exception_obj      = {};
    int32_t     unwind_catch_clause     = -1;
    static constexpr int kMaxUnwindDepth = 8;
    int32_t     unwind_finally_list[kMaxUnwindDepth] = {};
    int32_t     unwind_finally_count    = 0;
    int32_t     unwind_finally_current  = 0;

    // ── Filter evaluation state ───────────────────────────────────────────
    // When a Filter clause is encountered during exception search, we transfer
    // control to its filter expression (filter_start_idx) instead of directly
    // to the handler.  EndFilter then reads the filter result:
    //   - non-zero (accept) → enter the handler for filter_active_clause
    //   - zero (reject)     → resume search from filter_search_resume_idx-1
    //
    // filter_evaluating is true while the filter expression is running.
    // Original throw point (filter_throw_ip) is saved for the resume scan.
    bool        filter_evaluating       = false;
    int32_t     filter_active_clause    = -1;
    int32_t     filter_search_resume_idx = 0;          // next clause idx (exclusive end) to scan
    CHAOS_IL2CPP_SIZE filter_throw_ip   = 0u;

    // ── SEH helper lambdas (capture locals by reference) ──────────────────

    // Phase 1: find the innermost catch/filter handler covering ip.
    // If exc_val is provided and is a ThreadAbort/ThreadInterrupt sentinel
    // (ObjectRef with obj < 0), typed catch clauses are skipped — sentinels
    // must propagate to the managed exception dispatch layer.
    //
    // start_idx_excl is the upper bound (exclusive) for the reverse scan.
    // Default (size of seh_clauses) means "scan all clauses".  When resuming
    // after a filter rejected the exception, this is set to the rejected
    // clause index, so we skip it and any inner clauses already considered.
    auto findCatchHandler = [&](CHAOS_IL2CPP_SIZE ip,
                                const InterpreterValue* exc_val = nullptr,
                                int start_idx_excl = -1) -> int {
        const int upper = (start_idx_excl < 0)
            ? static_cast<int>(method.seh_clauses.size())
            : start_idx_excl;
        for (int i = upper - 1; i >= 0; --i) {
            const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(i)];
            if (ip >= clause.try_start_idx && ip < clause.try_end_idx) {
                const auto flags = static_cast<uint32_t>(clause.flags);
                if (flags == static_cast<uint32_t>(SEHFlags::Exception) ||
                    flags == static_cast<uint32_t>(SEHFlags::Filter)) {
                    // Phase 5: sentinel exception check.
                    // ThreadAbort (-1) / ThreadInterrupt (-2) / COMException (-3) sentinels
                    // non-null ObjectRef whose pointer value encodes the sentinel.
                    // Typed catch clauses cannot handle sentinels — skip them.
                    if (exc_val != nullptr && exc_val->tag == ValueTag::ObjectRef) {
                        const auto ptr_as_int = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(exc_val->obj);
                        if (ptr_as_int < 0 && ptr_as_int >= kManagedExceptionComFailure) {
                            const bool is_typed = (flags & static_cast<uint32_t>(SEHFlags::Typed)) != 0;
                            if (is_typed) continue;  // Skip typed catch for sentinels.
                        }
                    }
                    return i;
                }
            }
        }
        return -1;
    };

    // Phase 2: build the finally/fault unwind list for a given catch clause.
    // Stores indices in unwind_finally_list and returns the count.
    auto setupFinallyUnwind = [&](int catch_idx, CHAOS_IL2CPP_SIZE ip) {
        unwind_finally_count = 0;
        const auto& catch_clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(catch_idx)];
        for (int i = 0; i < static_cast<int>(method.seh_clauses.size()); ++i) {
            if (i == catch_idx) continue;
            const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(i)];
            const auto flags = static_cast<uint32_t>(clause.flags);
            if (flags == static_cast<uint32_t>(SEHFlags::Finally) ||
                flags == static_cast<uint32_t>(SEHFlags::Fault)) {
                if (ip >= clause.try_start_idx &&
                    ip < clause.try_end_idx &&
                    clause.try_start_idx >= catch_clause.try_start_idx &&
                    clause.try_end_idx <= catch_clause.try_end_idx &&
                    unwind_finally_count < kMaxUnwindDepth) {
                    unwind_finally_list[unwind_finally_count++] = i;
                }
            }
        }
    };

    // Dispatch an in-flight exception to the next matching handler.
    // Returns true if a handler (or filter) was selected and execution should
    // resume.  Returns false if no handler exists (caller should propagate).
    //
    // start_idx_excl: upper bound for the search.  -1 = scan all clauses.
    // After a filter rejects the exception, this is set to the rejected
    // filter clause index so that clause is excluded from the resumed search.
    //
    // For Filter clauses, this routine pushes the exception onto the
    // evaluation stack and transfers to the filter expression
    // (filter_start_idx).  EndFilter then commits or rejects.
    //
    // For Exception clauses, this routine performs the existing
    // setupFinallyUnwind + handler entry sequence.
    auto dispatchException = [&](int start_idx_excl) -> bool {
        const int catch_idx = findCatchHandler(instruction_index, &exception_obj, start_idx_excl);
        if (catch_idx < 0) {
            return false;
        }
        const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(catch_idx)];
        const auto cflags = static_cast<uint32_t>(clause.flags);
        if (cflags == static_cast<uint32_t>(SEHFlags::Filter)) {
            // Begin filter evaluation — push exception object onto stack and
            // transfer to filter_start_idx.  EndFilter pops the result and
            // commits (handler) or rejects (resume search).
            filter_evaluating = true;
            filter_active_clause = catch_idx;
            filter_search_resume_idx = catch_idx;     // exclude this clause on reject
            filter_throw_ip = instruction_index;
            frame->stack.push_back(exception_obj);
            instruction_index = clause.handler_start_idx;
            in_handler = true;
            active_handler_clause = catch_idx;
            return true;
        }
        // Plain catch (Exception flag): walk finally/fault unwind first, then
        // hand off to the handler.
        setupFinallyUnwind(catch_idx, instruction_index);
        if (unwind_finally_count > 0) {
            exception_in_flight = true;
            unwind_catch_clause = catch_idx;
            unwind_finally_current = 0;
            instruction_index = method.seh_clauses[
                static_cast<CHAOS_IL2CPP_SIZE>(unwind_finally_list[0])].handler_start_idx;
            in_handler = true;
            active_handler_clause = unwind_finally_list[0];
        } else {
            instruction_index = clause.handler_start_idx;
            in_handler = true;
            active_handler_clause = catch_idx;
            frame->stack.push_back(exception_obj);
        }
        return true;
    };

    // ── Action for handleDispatchResult — controls switch flow in call handlers. ──
    enum class DispatchAction { Return, Continue, Break };

    // Shared post-dispatch handler for Call/CallVirt/CallBridge.
    // Encapsulates SEH catch/finally dispatch and tail-call optimization.
    // Called after frame->dispatch_fn returns a DispatchResult.
    // Returns DispatchAction to control the outer switch/loop flow.
    auto handleDispatchResult = [&](const DispatchResult& dret) -> DispatchAction {
        if (dret.threw_exception) {
            exception_obj = dret.exception_value;
            if (dispatchException(/*start_idx_excl=*/-1)) {
                return DispatchAction::Continue;
            }
            // No catch handler — propagate to caller.
            result.threw_exception = true;
            result.exception_value = exception_obj;
            return DispatchAction::Return;
        }

        if (dret.has_value) {
            // Tail call: if the next instruction is Ret, skip the stack round-trip.
            if (instruction_index + 1u < method.instructions.size() &&
                method.instructions[instruction_index + 1u].op_code == IROpCode::Ret) {
                result.has_return_value = true;
                result.return_value = dret.value;
                return DispatchAction::Return;
            }
            frame->stack.push_back(dret.value);
        }
        return DispatchAction::Break;
    };

    while (instruction_index < method.instructions.size()) {
        const IRInstruction& instruction = method.instructions[instruction_index];

#if CHAOS_IL2CPP_DEBUGGER
        // Debugger breakpoint/stepping check at each instruction boundary.
        if (diagnostics::DbgShouldPause(frame->method_token,
                                        static_cast<uint32_t>(instruction_index), 0)) {
            // Capture frame snapshot (top frame only; RegisterExecute doesn't
            // have prev_frame chain access.  The FastExecute path captures the
            // full chain for FastFrames.)
            diagnostics::DbgClearFrameSnapshot();
            auto& snap = diagnostics::DbgGetFrameSnapshot();
            snap.frames[0].method_token = frame->method_token;
            snap.frames[0].il_offset   = static_cast<uint32_t>(instruction_index);
            snap.frame_count = 1;
            snap.local_count = 0;

            diagnostics::DbgNotifyPaused(frame->method_token,
                                         static_cast<uint32_t>(instruction_index));
        }
#endif

        switch (instruction.op_code) {
            [[likely]]
            case IROpCode::LdcI4:
                frame->stack.push_back(InterpreterValue::from_i32(instruction.immediate_i4));
                break;
            case IROpCode::LdcI8:
                frame->stack.push_back(InterpreterValue::from_i64(instruction.immediate_i8));
                break;
            case IROpCode::LdcR4:
                frame->stack.push_back(InterpreterValue::from_f32(static_cast<float>(instruction.immediate_r8)));
                break;
            case IROpCode::LdcR8:
                frame->stack.push_back(InterpreterValue::from_f64(instruction.immediate_r8));
                break;
            case IROpCode::LdStr:
                frame->stack.push_back(InterpreterValue::from_obj(const_cast<char*>(instruction.string_operand)));
                break;
            case IROpCode::LdNull:
                frame->stack.push_back(InterpreterValue::null_val());
                break;
            [[likely]]
            case IROpCode::LdArg:
                if (instruction.operand_index < 0 ||
                    static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index) >= frame->arguments.size()) {
                    throw CHAOS_IL2CPP_OUT_OF_RANGE("argument");
                }

                frame->stack.push_back(frame->arguments[static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index)]);
                break;
            [[likely]]
            case IROpCode::LdLoc:
                EnsureLocal(&frame->locals, static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index));
                frame->stack.push_back(frame->locals[static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index)]);
                break;
            [[likely]]
            case IROpCode::StLoc: {
                EnsureLocal(&frame->locals, static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index));
                frame->locals[static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index)] = Pop(&frame->stack);
                break;
            }
            case IROpCode::StArg: {
                if (instruction.operand_index < 0) {
                    throw CHAOS_IL2CPP_OUT_OF_RANGE("argument");
                }

                const CHAOS_IL2CPP_SIZE argument_index = static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index);
                if (frame->arguments.size() <= argument_index) {
                    frame->arguments.resize(argument_index + 1u);
                }

                frame->arguments[argument_index] = Pop(&frame->stack);
                break;
            }
            case IROpCode::LdSFld: {
                if (g_static_fields.size() <= instruction.field_offset) {
                    g_static_fields.resize(instruction.field_offset + 1u);
                }

                frame->stack.push_back(g_static_fields[instruction.field_offset]);
                break;
            }
            case IROpCode::StSFld: {
                if (g_static_fields.size() <= instruction.field_offset) {
                    g_static_fields.resize(instruction.field_offset + 1u);
                }
                auto& slot = g_static_fields[instruction.field_offset];
                using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
                BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&slot.obj));
                chaos::il2cpp::runtime_core::BgcRecordRootChange(
                    reinterpret_cast<void**>(&slot.obj), slot.obj);
                slot = Pop(&frame->stack);
                break;
            }
            case IROpCode::Beq: {
                const InterpreterValue right = Pop(&frame->stack);
                const InterpreterValue left = Pop(&frame->stack);
                if (ReadFloat64(left) == ReadFloat64(right)) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::Blt: {
                const InterpreterValue right_val = Pop(&frame->stack);
                const InterpreterValue left_val = Pop(&frame->stack);
                if (left_val.tag == ValueTag::Float32 || left_val.tag == ValueTag::Float64 ||
                    right_val.tag == ValueTag::Float32 || right_val.tag == ValueTag::Float64) {
                    if (ReadFloat64(left_val) < ReadFloat64(right_val)) {
                        instruction_index = GetBranchTarget(method, instruction.branch_target);
                        continue;
                    }
                } else {
                    if (ReadInt32(left_val) < ReadInt32(right_val)) {
                        instruction_index = GetBranchTarget(method, instruction.branch_target);
                        continue;
                    }
                }
                break;
            }
            case IROpCode::Bgt: {
                const InterpreterValue right_val = Pop(&frame->stack);
                const InterpreterValue left_val = Pop(&frame->stack);
                if (left_val.tag == ValueTag::Float32 || left_val.tag == ValueTag::Float64 ||
                    right_val.tag == ValueTag::Float32 || right_val.tag == ValueTag::Float64) {
                    if (ReadFloat64(left_val) > ReadFloat64(right_val)) {
                        instruction_index = GetBranchTarget(method, instruction.branch_target);
                        continue;
                    }
                } else {
                    if (ReadInt32(left_val) > ReadInt32(right_val)) {
                        instruction_index = GetBranchTarget(method, instruction.branch_target);
                        continue;
                    }
                }
                break;
            }
            case IROpCode::Ble: {
                const InterpreterValue right_val = Pop(&frame->stack);
                const InterpreterValue left_val = Pop(&frame->stack);
                if (left_val.tag == ValueTag::Float32 || left_val.tag == ValueTag::Float64 ||
                    right_val.tag == ValueTag::Float32 || right_val.tag == ValueTag::Float64) {
                    if (ReadFloat64(left_val) <= ReadFloat64(right_val)) {
                        instruction_index = GetBranchTarget(method, instruction.branch_target);
                        continue;
                    }
                } else {
                    if (ReadInt32(left_val) <= ReadInt32(right_val)) {
                        instruction_index = GetBranchTarget(method, instruction.branch_target);
                        continue;
                    }
                }
                break;
            }
            case IROpCode::Bge: {
                const InterpreterValue right_val = Pop(&frame->stack);
                const InterpreterValue left_val = Pop(&frame->stack);
                if (left_val.tag == ValueTag::Float32 || left_val.tag == ValueTag::Float64 ||
                    right_val.tag == ValueTag::Float32 || right_val.tag == ValueTag::Float64) {
                    if (ReadFloat64(left_val) >= ReadFloat64(right_val)) {
                        instruction_index = GetBranchTarget(method, instruction.branch_target);
                        continue;
                    }
                } else {
                    if (ReadInt32(left_val) >= ReadInt32(right_val)) {
                        instruction_index = GetBranchTarget(method, instruction.branch_target);
                        continue;
                    }
                }
                break;
            [[likely]]
            }
            case IROpCode::Add: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left + right));
                [[likely]]
                break;
            }
            case IROpCode::Sub: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left - right));
                break;
            }
            case IROpCode::Mul: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left * right));
                break;
            }
            case IROpCode::Div: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left / right));
                break;
            }
            case IROpCode::Rem: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left % right));
                break;
            }
            case IROpCode::Neg: {
                const CHAOS_IL2CPP_INT32 value = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(-value));
                break;
            }
            case IROpCode::Ceq: {
                const InterpreterValue right = Pop(&frame->stack);
                const InterpreterValue left = Pop(&frame->stack);
                const bool equal = left.tag == ValueTag::ObjectRef || left.tag == ValueTag::Null
                    ? left.obj == right.obj
                    : ReadFloat64(left) == ReadFloat64(right);
                frame->stack.push_back(InterpreterValue::from_i32(equal ? 1 : 0));
                break;
            }
            case IROpCode::Clt: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left < right ? 1 : 0));
                break;
            }
            case IROpCode::Cgt: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left > right ? 1 : 0));
                break;
            }
            [[likely]]
            case IROpCode::Br:
                instruction_index = GetBranchTarget(method, instruction.branch_target);
                continue;
            [[likely]]
            case IROpCode::BrTrue:
                if (ReadInt32(Pop(&frame->stack)) != 0) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            [[likely]]
            case IROpCode::BrFalse:
                if (ReadInt32(Pop(&frame->stack)) == 0) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            case IROpCode::NewObj: {
                auto* storage = static_cast<ObjectStorage*>(CHAOS_IL2CPP_MALLOC(sizeof(ObjectStorage)));
                if (storage == nullptr) break;
                ::new (storage) ObjectStorage();
                storage->fields.resize(instruction.secondary_index == 0u ? 1u : instruction.secondary_index);
                // Type token is set by the token resolver (or test) via immediate_i4.
                storage->type_token = static_cast<CHAOS_IL2CPP_UINT32>(instruction.immediate_i4);
                frame->stack.push_back(InterpreterValue::from_obj(storage));
                // Track interpreter allocation in TLS counter
                chaos::il2cpp::runtime_core::tls_alloc_fast_count++;
                chaos::il2cpp::runtime_core::tls_alloc_fast_bytes += sizeof(ObjectStorage) + storage->fields.capacity() * sizeof(InterpreterValue);
                break;
            }
            case IROpCode::NewArr: {
                const CHAOS_IL2CPP_SIZE length = static_cast<CHAOS_IL2CPP_SIZE>(ReadInt32(Pop(&frame->stack)));
                auto* storage = static_cast<ArrayStorage*>(CHAOS_IL2CPP_MALLOC(sizeof(ArrayStorage)));
                if (storage == nullptr) break;
                ::new (storage) ArrayStorage();
                storage->elements.resize(length);
                frame->stack.push_back(InterpreterValue::from_obj(storage));
                // Track interpreter allocation in TLS counter
                chaos::il2cpp::runtime_core::tls_alloc_fast_count++;
                chaos::il2cpp::runtime_core::tls_alloc_fast_bytes += sizeof(ArrayStorage) + length * sizeof(InterpreterValue);
                break;
            }
            case IROpCode::LdFld: {
                const InterpreterValue instance = Pop(&frame->stack);
                if (instance.tag == ValueTag::Struct) {
                    // Struct field access: read bytes at field_offset.
                    // field_size comes from immediate_i4 (set by the codegen emitter).
                    const CHAOS_IL2CPP_SIZE offset   = static_cast<CHAOS_IL2CPP_SIZE>(instruction.field_offset);
                    const CHAOS_IL2CPP_SIZE field_sz = static_cast<CHAOS_IL2CPP_SIZE>(instruction.immediate_i4);
                    if (instance.obj != nullptr && offset + field_sz <= instance.struct_size) {
                        const void* src = static_cast<const char*>(instance.obj) + offset;
                        if (field_sz <= sizeof(CHAOS_IL2CPP_INT64)) {
                            // Small field: copy as appropriate integer type.
                            CHAOS_IL2CPP_INT64 raw = 0;
                            std::memcpy(&raw, src, field_sz);
                            if (field_sz <= 4) {
                                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(raw)));
                            } else {
                                frame->stack.push_back(InterpreterValue::from_i64(raw));
                            }
                        } else {
                            // Large field: copy as struct (from_struct will malloc+copy internally).
                            auto val = InterpreterValue::from_struct(src, static_cast<CHAOS_IL2CPP_UINT32>(field_sz));
                            frame->stack.push_back(val);
                        }
                    } else {
                        frame->stack.push_back(InterpreterValue::null_val());
                    }
                } else {
                    // Object field access (existing behavior).
                    auto* object = RequireObject(instance);
                    if (object->fields.size() <= instruction.field_offset) {
                        object->fields.resize(instruction.field_offset + 1u);
                    }
                    frame->stack.push_back(object->fields[instruction.field_offset]);
                }
                break;
            }
            case IROpCode::StFld: {
                const InterpreterValue value = Pop(&frame->stack);
                const InterpreterValue instance = Pop(&frame->stack);
                if (instance.tag == ValueTag::Struct) {
                    // Struct field write: write bytes at field_offset.
                    const CHAOS_IL2CPP_SIZE offset = static_cast<CHAOS_IL2CPP_SIZE>(instruction.field_offset);
                    if (instance.obj != nullptr && offset <= instance.struct_size) {
                        void* dst = static_cast<char*>(instance.obj) + offset;
                        const CHAOS_IL2CPP_SIZE remaining = instance.struct_size - offset;
                        if (value.tag == ValueTag::Struct && value.obj != nullptr) {
                            // Large struct field: copy full struct data (clamped to remaining space).
                            const CHAOS_IL2CPP_SIZE copy_size = (value.struct_size < remaining)
                                ? value.struct_size : remaining;
                            std::memcpy(dst, value.obj, copy_size);
                        } else {
                            // Small field: read from scalar value.
                            CHAOS_IL2CPP_INT64 raw = 0;
                            if (value.tag == ValueTag::Int32 || value.tag == ValueTag::Float32) {
                                raw = value.i32;
                            } else if (value.tag == ValueTag::Int64 || value.tag == ValueTag::Float64) {
                                raw = value.i64;
                            } else {
                                raw = reinterpret_cast<CHAOS_IL2CPP_INT64>(value.obj);
                            }
                            const CHAOS_IL2CPP_SIZE write_size = (remaining < sizeof(void*))
                                ? remaining
                                : sizeof(void*);
                            std::memcpy(dst, &raw, write_size);
                        }
                    }
                } else {
                    // Object field access — pre-write barrier (SATB) + post-write barrier (card dirty).
                    auto* object = RequireObject(instance);
                    if (object->fields.size() <= instruction.field_offset) {
                        object->fields.resize(instruction.field_offset + 1u);
                    }
                    using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
                    BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&object->fields[instruction.field_offset].obj));
                    object->fields[instruction.field_offset] = value;
                    chaos_gc_dirty_card(object);
                }
                break;
            }
            case IROpCode::LdElem: {
                const CHAOS_IL2CPP_SIZE index = static_cast<CHAOS_IL2CPP_SIZE>(ReadInt32(Pop(&frame->stack)));
                auto* array = RequireArray(Pop(&frame->stack));
                if (index >= array->elements.size()) {
                    throw CHAOS_IL2CPP_OUT_OF_RANGE("array_index");
                }
                frame->stack.push_back(array->elements[index]);
                break;
            }
            case IROpCode::StElem: {
                const InterpreterValue value = Pop(&frame->stack);
                const CHAOS_IL2CPP_SIZE index = static_cast<CHAOS_IL2CPP_SIZE>(ReadInt32(Pop(&frame->stack)));
                auto* array = RequireArray(Pop(&frame->stack));
                if (index >= array->elements.size()) {
                    throw CHAOS_IL2CPP_OUT_OF_RANGE("array_index");
                }
                using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
                BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&array->elements[index].obj));
                array->elements[index] = value;
                chaos_gc_dirty_card(array);
                break;
            }
            case IROpCode::LdLen: {
                auto* array = RequireArray(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(array->elements.size())));
                break;
            }
            case IROpCode::Pop:
                (void)Pop(&frame->stack);
                break;
            case IROpCode::Box: {
                auto* boxed = static_cast<BoxedValue*>(CHAOS_IL2CPP_MALLOC(sizeof(BoxedValue)));
                if (boxed == nullptr) break;
                ::new (boxed) BoxedValue();
                boxed->value = Pop(&frame->stack);
                frame->stack.push_back(InterpreterValue::from_obj(boxed));
                break;
            }
            case IROpCode::Unbox: {
                const InterpreterValue boxed_value = Pop(&frame->stack);
                if (boxed_value.tag != ValueTag::ObjectRef || boxed_value.obj == nullptr) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("boxed value required");
                }
                frame->stack.push_back(static_cast<BoxedValue*>(boxed_value.obj)->value);
                break;
            }
            case IROpCode::CastClass:
            case IROpCode::IsInst: {
                const InterpreterValue cast_val = Pop(&frame->stack);
                // Null: CastClass passes through, IsInst returns null
                if (cast_val.tag == ValueTag::Null) {
                    if (instruction.op_code == IROpCode::IsInst) {
                        frame->stack.push_back(InterpreterValue::null_val());
                    } else {
                        frame->stack.push_back(cast_val);
                    }
                    break;
                }
                // Non-object: CastClass passes through, IsInst returns null
                if (cast_val.tag != ValueTag::ObjectRef || cast_val.obj == nullptr) {
                    if (instruction.op_code == IROpCode::IsInst) {
                        frame->stack.push_back(InterpreterValue::null_val());
                    } else {
                        frame->stack.push_back(cast_val);
                    }
                    break;
                }
                // Target type token stored in immediate_i4 by the IR builder
                const CHAOS_IL2CPP_UINT32 target_type_token =
                    static_cast<CHAOS_IL2CPP_UINT32>(instruction.immediate_i4);
                if (target_type_token == 0u) {
                    // No target type info — pass through (safe fallback)
                    frame->stack.push_back(cast_val);
                    break;
                }
                // Walk the inheritance chain using vtable_registry
                auto* obj = static_cast<InterpreterObject*>(cast_val.obj);
                const CHAOS_IL2CPP_UINT32 obj_type_token = obj->type_token;
                bool compatible = false;
                CHAOS_IL2CPP_UINT32 current_token = obj_type_token;
                while (current_token != 0u) {
                    if (current_token == target_type_token) {
                        compatible = true;
                        break;
                    }
                    // Walk to base type via vtable_registry
                    const auto* vtable = chaos::il2cpp::vtable_registry::TryGetTypeVTable(current_token);
                    if (vtable == nullptr) {
                        break;
                    }
                    current_token = vtable->base_token;
                }
                // Interface compatibility check: scan implemented interfaces
                if (!compatible) {
                    const auto* target_vtable = chaos::il2cpp::vtable_registry::TryGetTypeVTable(target_type_token);
                    if (target_vtable != nullptr && target_vtable->type_shape == chaos::il2cpp::common::chaos_type_shape_interface) {
                        // Target is an interface — check if instance type implements it.
                        // Walk instance type's interface map (through all ancestors).
                        CHAOS_IL2CPP_UINT32 scan_token = obj_type_token;
                        while (scan_token != 0u && !compatible) {
                            const auto* scan_vtable = chaos::il2cpp::vtable_registry::TryGetTypeVTable(scan_token);
                            if (scan_vtable == nullptr) break;
                            if (scan_vtable->iface_map != nullptr && scan_vtable->iface_count > 0u) {
                                const auto* iface_entries = static_cast<const chaos::il2cpp::common::InterfaceMapEntry*>(scan_vtable->iface_map);
                                for (CHAOS_IL2CPP_UINT32 ifi = 0u; ifi < scan_vtable->iface_count; ++ifi) {
                                    // Compare stable_id against target's stable_id.
                                    const auto* target_vt2 = chaos::il2cpp::vtable_registry::TryGetTypeVTable(target_type_token);
                                    if (target_vt2 != nullptr &&
                                        iface_entries[ifi].iface_stable_id == target_vt2->stable_id) {
                                        compatible = true;
                                        break;
                                    }
                                }
                            }
                            // Also check runtime_iface_map (hot-update interface additions).
                            if (!compatible && scan_vtable->runtime_iface_map != nullptr &&
                                scan_vtable->runtime_iface_count > 0u) {
                                const auto* runtime_entries = static_cast<const chaos::il2cpp::common::InterfaceMapEntry*>(scan_vtable->runtime_iface_map);
                                for (CHAOS_IL2CPP_UINT32 ifi = 0u; ifi < scan_vtable->runtime_iface_count; ++ifi) {
                                    const auto* target_vt2 = chaos::il2cpp::vtable_registry::TryGetTypeVTable(target_type_token);
                                    if (target_vt2 != nullptr &&
                                        runtime_entries[ifi].iface_stable_id == target_vt2->stable_id) {
                                        compatible = true;
                                        break;
                                    }
                                }
                            }
                            scan_token = scan_vtable->base_token;
                        }
                    }
                }
                if (compatible) {
                    frame->stack.push_back(cast_val);
                } else if (instruction.op_code == IROpCode::IsInst) {
                    frame->stack.push_back(InterpreterValue::null_val());
                } else {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("InvalidCastException: type mismatch in CastClass");
                }
                break;
            }
            [[likely]]
            case IROpCode::Call:
            case IROpCode::CallBridge: {
                // AotDirectDispatch: if direct_fn is set, call the pre-resolved
                // chaos_external_runtime_* function pointer directly without going
                // through dispatch_fn / call_target resolution.  This is needed
                // for SEH-containing methods that fall back from FastExecute.
                if (instruction.direct_fn != nullptr) {
                    // Check arg_count bounds.
                    CHAOS_IL2CPP_SIZE ac = static_cast<CHAOS_IL2CPP_SIZE>(instruction.arg_count);
                    if (ac > 8) {
                        break;
                    }
                    if (frame->stack.size() < ac) {
                        break;
                    }

                    // Pop args as raw uint64_t values from InterpreterValue stack.
                    uint64_t raw_args[8] = {};
                    for (CHAOS_IL2CPP_SIZE ai = ac; ai > 0u && ai <= 8; --ai) {
                        const auto& val = frame->stack.back();
                        switch (val.tag) {
                        case ValueTag::Int32:
                            raw_args[ai - 1u] = static_cast<uint64_t>(static_cast<uint32_t>(val.i32));
                            break;
                        case ValueTag::Int64:
                            raw_args[ai - 1u] = static_cast<uint64_t>(val.i64);
                            break;
                        case ValueTag::Float32:
                            std::memcpy(&raw_args[ai - 1u], &val.f32, sizeof(float));
                            break;
                        case ValueTag::Float64:
                            std::memcpy(&raw_args[ai - 1u], &val.f64, sizeof(double));
                            break;
                        default:
                            raw_args[ai - 1u] = reinterpret_cast<uint64_t>(val.obj);
                            break;
                        }
                        frame->stack.pop_back();
                    }

                    // Determine return tag from IRInstruction.
                    uint8_t ret_tag = instruction.direct_ret_tag;
                    if (ret_tag == 0xFF)
                        ret_tag = static_cast<uint8_t>(ValueTag::Int32);

                    // Call via uniform 8-arg signature.
                    // The shaped chaos_external_runtime_* functions may throw
                    // chaos_managed_exception (e.g. InvalidCastException).
                    // Catch and propagate through the VM's SEH mechanism.
                    using DirectFn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                                                  uint64_t, uint64_t, uint64_t, uint64_t);
                    auto fn = reinterpret_cast<DirectFn>(instruction.direct_fn);
                    uint64_t raw_ret = 0;
                    bool direct_fn_threw = false;
                    uint64_t direct_fn_exception_obj = 0;
                    bool pal_caught = chaos::il2cpp::pal::PalTryCallNoExcept(
                        fn, raw_args[0], raw_args[1], raw_args[2], raw_args[3],
                        raw_args[4], raw_args[5], raw_args[6], raw_args[7], raw_ret);
                    if (pal_caught) {
                        direct_fn_threw = true;
                        direct_fn_exception_obj = 0;
                    }

                    // Build DispatchResult for SEH propagation.
                    {
                        DispatchResult dret;
                        if (direct_fn_threw) {
                            dret.threw_exception = true;
                            dret.exception_value = InterpreterValue::from_obj(
                                reinterpret_cast<void*>(direct_fn_exception_obj));
                        } else if (ret_tag != static_cast<uint8_t>(ValueTag::Void)) {
                            dret.has_value = true;
                            switch (static_cast<ValueTag>(ret_tag)) {
                            case ValueTag::Int32:
                                dret.value = InterpreterValue::from_i32(
                                    static_cast<int32_t>(raw_ret & 0xFFFFFFFFu));
                                break;
                            case ValueTag::Int64:
                                dret.value = InterpreterValue::from_i64(
                                    static_cast<int64_t>(raw_ret));
                                break;
                            case ValueTag::Float32: {
                                float f;
                                std::memcpy(&f, &raw_ret, sizeof(float));
                                dret.value = InterpreterValue::from_f32(f);
                                break;
                            }
                            case ValueTag::Float64: {
                                double d;
                                std::memcpy(&d, &raw_ret, sizeof(double));
                                dret.value = InterpreterValue::from_f64(d);
                                break;
                            }
                            default:
                                dret.value = InterpreterValue::from_obj(
                                    reinterpret_cast<void*>(raw_ret));
                                break;
                            }
                        }
                        const DispatchAction da = handleDispatchResult(dret);
                        if (da == DispatchAction::Return) return result;
                        if (da == DispatchAction::Continue) continue;
                    }
                    break;
                }

                const CHAOS_IL2CPP_SIZE arg_count = static_cast<CHAOS_IL2CPP_SIZE>(instruction.arg_count);

                if (frame->dispatch_fn != nullptr) {
                    // Inline dispatch via callback (Phase 4+).
                    // Stack-allocate for small arg counts (common case).
                    InterpreterValue local_buf[kMaxCallArgs];
                    auto* arg_buf = (arg_count <= kMaxCallArgs)
                        ? local_buf
                        : static_cast<InterpreterValue*>(CHAOS_IL2CPP_MALLOC(
                            sizeof(InterpreterValue) * arg_count));

                    for (CHAOS_IL2CPP_SIZE ai = arg_count; ai > 0u; --ai) {
                        arg_buf[ai - 1u] = Pop(&frame->stack);
                    }

                    DispatchResult dret = frame->dispatch_fn(
                        instruction.call_target, arg_buf,
                        static_cast<CHAOS_IL2CPP_UINT32>(arg_count),
                        instruction.is_instance_call,
                        frame->dispatch_context);

                    if (arg_count > kMaxCallArgs) CHAOS_IL2CPP_FREE(arg_buf);

                    const DispatchAction da = handleDispatchResult(dret);
                    if (da == DispatchAction::Return) return result;
                    if (da == DispatchAction::Continue) continue;
                    break;
                }

                // Fallback: existing external-dispatch behavior.
                result.needs_external_dispatch = true;
                result.call_target = instruction.call_target;
                result.call_args.resize(arg_count);
                for (CHAOS_IL2CPP_SIZE ai = arg_count; ai > 0u; --ai) {
                    result.call_args[ai - 1u] = Pop(&frame->stack);
                }
                return result;
            }
            case IROpCode::CallVirtConstrained: {
                // Constrained CallVirt: the .constrained. prefix indicates that
                // if 'this' is a value type, the call should be made directly
                // (without boxing). For reference types, normal virtual dispatch.
                const CHAOS_IL2CPP_SIZE cv_arg_count = static_cast<CHAOS_IL2CPP_SIZE>(instruction.arg_count);

                // Collect call args (needed by both paths).
                InterpreterValue local_buf[kMaxCallArgs];
                auto* cv_args = (cv_arg_count <= kMaxCallArgs)
                    ? local_buf
                    : static_cast<InterpreterValue*>(CHAOS_IL2CPP_MALLOC(
                        sizeof(InterpreterValue) * cv_arg_count));
                for (CHAOS_IL2CPP_SIZE ai = cv_arg_count; ai > 0u; --ai) {
                    cv_args[ai - 1u] = Pop(&frame->stack);
                }

                // Resolve call target (vtable or direct).
                void* resolved_target = instruction.call_target;
                if (cv_arg_count > 0u) {
                    const InterpreterValue& this_val = cv_args[0u];
                    if (this_val.tag != ValueTag::Struct) {
                        // Reference or boxed value — normal virtual dispatch.
                        CHAOS_IL2CPP_UINT32 inst_type_token = 0u;
                        if (this_val.tag == ValueTag::ObjectRef && this_val.obj != nullptr) {
                            inst_type_token = static_cast<InterpreterObject*>(this_val.obj)->type_token;
                        }
                        const CHAOS_IL2CPP_UINT32 decl_method_token =
                            static_cast<CHAOS_IL2CPP_UINT32>(instruction.secondary_index);
                        if (inst_type_token != 0u && decl_method_token != 0u) {
                            void* vtable_resolved = chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(
                                inst_type_token, decl_method_token);
                            if (vtable_resolved != nullptr) {
                                resolved_target = vtable_resolved;
                            }
                        }
                    }
                }

                if (frame->dispatch_fn != nullptr) {
                    DispatchResult dret = frame->dispatch_fn(
                        resolved_target, cv_args,
                        static_cast<CHAOS_IL2CPP_UINT32>(cv_arg_count),
                        true, /* CallVirtConstrained is always instance */
                        frame->dispatch_context);

                    if (cv_arg_count > kMaxCallArgs) CHAOS_IL2CPP_FREE(cv_args);

                    const DispatchAction da_cv = handleDispatchResult(dret);
                    if (da_cv == DispatchAction::Return) return result;
                    if (da_cv == DispatchAction::Continue) continue;
                    break;
                }

                // Fallback: existing external-dispatch behavior.
                // Copy args BEFORE freeing the heap buffer (if any).
                result.call_args.resize(cv_arg_count);
                for (CHAOS_IL2CPP_SIZE ai = 0u; ai < cv_arg_count; ++ai) {
                    result.call_args[ai] = cv_args[ai];
                }
                if (cv_arg_count > kMaxCallArgs) CHAOS_IL2CPP_FREE(cv_args);
                result.call_target = resolved_target;
                result.needs_external_dispatch = true;
                return result;
            }
            case IROpCode::CallVirt: {
                const CHAOS_IL2CPP_SIZE arg_count_v = static_cast<CHAOS_IL2CPP_SIZE>(instruction.arg_count);

                // Collect call args.
                InterpreterValue local_buf_v[kMaxCallArgs];
                auto* cv_args_v = (arg_count_v <= kMaxCallArgs)
                    ? local_buf_v
                    : static_cast<InterpreterValue*>(CHAOS_IL2CPP_MALLOC(
                        sizeof(InterpreterValue) * arg_count_v));
                for (CHAOS_IL2CPP_SIZE ai = arg_count_v; ai > 0u; --ai) {
                    cv_args_v[ai - 1u] = Pop(&frame->stack);
                }

                // Resolve virtual dispatch.
                void* resolved_target_v = instruction.call_target;
                if (arg_count_v > 0u) {
                    const InterpreterValue& this_val = cv_args_v[0u];
                    CHAOS_IL2CPP_UINT32 instance_type_token = 0u;
                    if (this_val.tag == ValueTag::ObjectRef && this_val.obj != nullptr) {
                        instance_type_token = static_cast<InterpreterObject*>(this_val.obj)->type_token;
                    }

                    const CHAOS_IL2CPP_UINT32 declared_method_token =
                        static_cast<CHAOS_IL2CPP_UINT32>(instruction.secondary_index);

                    if (instance_type_token != 0u) {
                        void* vtable_resolved = chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(
                            instance_type_token, declared_method_token);
                        if (vtable_resolved != nullptr) {
                            resolved_target_v = vtable_resolved;
                        }
                    }
                }

                if (frame->dispatch_fn != nullptr) {
                    DispatchResult dret_v = frame->dispatch_fn(
                        resolved_target_v, cv_args_v,
                        static_cast<CHAOS_IL2CPP_UINT32>(arg_count_v),
                        true, /* CallVirt is always instance */
                        frame->dispatch_context);

                    if (arg_count_v > kMaxCallArgs) CHAOS_IL2CPP_FREE(cv_args_v);

                    const DispatchAction da_v = handleDispatchResult(dret_v);
                    if (da_v == DispatchAction::Return) return result;
                    if (da_v == DispatchAction::Continue) continue;
                    break;
                }

                // Fallback: existing external-dispatch behavior.
                // Copy args BEFORE freeing the heap buffer (if any).
                result.call_args.resize(arg_count_v);
                for (CHAOS_IL2CPP_SIZE ai = 0u; ai < arg_count_v; ++ai) {
                    result.call_args[ai] = cv_args_v[ai];
                }
                if (arg_count_v > kMaxCallArgs) CHAOS_IL2CPP_FREE(cv_args_v);
                result.call_target = resolved_target_v;
                result.needs_external_dispatch = true;
                return result;
            }
            case IROpCode::Conv_I4:
                frame->stack.push_back(InterpreterValue::from_i32(ReadInt32(Pop(&frame->stack))));
                break;
            case IROpCode::Conv_I8:
                frame->stack.push_back(InterpreterValue::from_i64(ReadInt64(Pop(&frame->stack))));
                break;
            case IROpCode::Conv_R4:
                frame->stack.push_back(InterpreterValue::from_f32(ReadFloat32(Pop(&frame->stack))));
                break;
            case IROpCode::Conv_R8:
                frame->stack.push_back(InterpreterValue::from_f64(ReadFloat64(Pop(&frame->stack))));
                break;
            case IROpCode::Throw:
            case IROpCode::Rethrow: {
                // Pop the exception object.
                exception_obj = Pop(&frame->stack);

                // Phase 1: Search for a matching catch handler (innermost first).
                if (dispatchException(/*start_idx_excl=*/-1)) {
                    continue;
                }
                // No catch handler — propagate to caller.
                result.threw_exception = true;
                result.exception_value = exception_obj;
                return result;
            }
            case IROpCode::Leave: {
                // Check if leaving a try block that has a finally handler.
                const int finally_idx = FindEnclosingFinally(method, instruction_index);
                if (finally_idx >= 0) {
                    const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(finally_idx)];
                    pending_leave = true;
                    pending_leave_target = instruction.branch_target;
                    in_handler = true;
                    active_handler_clause = finally_idx;
                    instruction_index = clause.handler_start_idx;
                    continue;
                }
                // No finally — normal branch.
                instruction_index = GetBranchTarget(method, instruction.branch_target);
                continue;
            }
            case IROpCode::EndFilter: {
                // EndFilter pops the filter result (i32) and decides whether to
                // commit to the filter's handler or resume searching.
                //
                // Per ECMA-335, the filter expression must end with EndFilter,
                // which consumes a single i32 from the stack:
                //   non-zero (1) → exception caught; transfer control to handler
                //   zero (0)     → filter rejects; continue searching outward
                if (!filter_evaluating) {
                    // Defensive: stray EndFilter outside any in-flight filter.
                    // Drop the result if present and continue sequentially.
                    if (!frame->stack.empty()) frame->stack.pop_back();
                    in_handler = false;
                    active_handler_clause = -1;
                    break;
                }

                // Pop filter result; treat as i32.
                const InterpreterValue r = Pop(&frame->stack);
                int32_t filter_result = 0;
                if (r.tag == ValueTag::Int32) {
                    filter_result = r.i32;
                } else if (r.tag == ValueTag::Int64) {
                    filter_result = static_cast<int32_t>(r.i64 != 0 ? 1 : 0);
                } else if (r.tag == ValueTag::ObjectRef) {
                    filter_result = (r.obj != nullptr) ? 1 : 0;
                }

                const int active = filter_active_clause;
                const int resume_excl = filter_search_resume_idx;
                const CHAOS_IL2CPP_SIZE throw_ip = filter_throw_ip;
                filter_evaluating = false;
                filter_active_clause = -1;

                if (filter_result != 0) {
                    // Filter accepts: enter handler for the active clause.
                    // Do not re-run findCatchHandler; commit directly.
                    const auto& clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(active)];
                    // Restore instruction_index to the original throw ip so
                    // setupFinallyUnwind selects the correct nested clauses.
                    instruction_index = throw_ip;
                    setupFinallyUnwind(active, throw_ip);
                    if (unwind_finally_count > 0) {
                        exception_in_flight = true;
                        unwind_catch_clause = active;
                        unwind_finally_current = 0;
                        instruction_index = method.seh_clauses[
                            static_cast<CHAOS_IL2CPP_SIZE>(unwind_finally_list[0])].handler_start_idx;
                        in_handler = true;
                        active_handler_clause = unwind_finally_list[0];
                    } else {
                        instruction_index = clause.handler_start_idx;
                        in_handler = true;
                        active_handler_clause = active;
                        frame->stack.push_back(exception_obj);
                    }
                    continue;
                }

                // Filter rejects: resume search excluding the rejected clause.
                instruction_index = throw_ip;
                if (dispatchException(/*start_idx_excl=*/resume_excl)) {
                    continue;
                }
                // No further handler — propagate.
                result.threw_exception = true;
                result.exception_value = exception_obj;
                return result;
            }
            case IROpCode::EndFinally: {
                if (exception_in_flight) {
                    // Phase 2 unwind: advance to the next finally/fault handler.
                    unwind_finally_current++;
                    if (unwind_finally_current < unwind_finally_count) {
                        const int next_finally = unwind_finally_list[unwind_finally_current];
                        instruction_index = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(next_finally)].handler_start_idx;
                        active_handler_clause = next_finally;
                        continue;
                    } else {
                        // All finally/fault handlers done — transfer to catch.
                        const auto& catch_clause = method.seh_clauses[static_cast<CHAOS_IL2CPP_SIZE>(unwind_catch_clause)];
                        instruction_index = catch_clause.handler_start_idx;
                        in_handler = true;
                        active_handler_clause = unwind_catch_clause;
                        frame->stack.push_back(exception_obj);
                        exception_in_flight = false;
                        continue;
                    }
                } else if (pending_leave) {
                    // Resume from the pending leave branch target.
                    instruction_index = GetBranchTarget(method, pending_leave_target);
                    pending_leave = false;
                    in_handler = false;
                    active_handler_clause = -1;
                    continue;
                }
                // Normal endfinally — continue sequentially.
                in_handler = false;
                active_handler_clause = -1;
                break;
            }
            // ── Phase A: new opcodes ──────────────────────────────────────
            case IROpCode::Dup: {
                if (frame->stack.empty()) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("dup on empty stack");
                }
                frame->stack.push_back(frame->stack.back());
                break;
            }
            case IROpCode::DivUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(left / right)));
                break;
            }
            case IROpCode::RemUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(left % right)));
                break;
            }
            case IROpCode::And: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left  = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left & right));
                break;
            }
            case IROpCode::Or: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left  = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left | right));
                break;
            }
            case IROpCode::Xor: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left  = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(left ^ right));
                break;
            }
            case IROpCode::Not: {
                const CHAOS_IL2CPP_INT32 value = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(~value));
                break;
            }
            case IROpCode::Shl: {
                const CHAOS_IL2CPP_INT32 amount = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 value  = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(value << (amount & 0x1F)));
                break;
            }
            case IROpCode::Shr: {
                const CHAOS_IL2CPP_INT32 amount = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 value  = ReadInt32(Pop(&frame->stack));
                frame->stack.push_back(InterpreterValue::from_i32(value >> (amount & 0x1F)));
                break;
            }
            case IROpCode::ShrUn: {
                const CHAOS_IL2CPP_INT32 amount = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_UINT32 value = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(value >> (amount & 0x1F))));
                break;
            }
            case IROpCode::ConvRUn: {
                const CHAOS_IL2CPP_UINT32 value = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                frame->stack.push_back(InterpreterValue::from_f64(static_cast<double>(value)));
                break;
            }
            case IROpCode::ConvI:
                frame->stack.push_back(InterpreterValue::from_i32(ReadInt32(Pop(&frame->stack))));
                break;
            case IROpCode::ConvU: {
                const CHAOS_IL2CPP_UINT32 value = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(value)));
                break;
            }
            case IROpCode::LdInd: {
                const InterpreterValue addr_val = Pop(&frame->stack);
                // ldind.* dereferences a pointer. In the stack-based interpreter
                // model, we reinterpret the value according to the type discriminator.
                switch (instruction.immediate_i4) {
                    case 0: case 1: case 2: case 3:  // i1, u1, i2, u2
                    case 4: case 5:  // i4, u4
                    default:
                        frame->stack.push_back(InterpreterValue::from_i32(ReadInt32(addr_val)));
                        break;
                    case 6:  // i8
                        frame->stack.push_back(InterpreterValue::from_i64(ReadInt64(addr_val)));
                        break;
                    case 7:  // i (native int)
                        frame->stack.push_back(InterpreterValue::from_i32(ReadInt32(addr_val)));
                        break;
                    case 8:  // r4
                        frame->stack.push_back(InterpreterValue::from_f32(ReadFloat32(addr_val)));
                        break;
                    case 9:  // r8
                        frame->stack.push_back(InterpreterValue::from_f64(ReadFloat64(addr_val)));
                        break;
                    case 10: // ref
                        frame->stack.push_back(addr_val);
                        break;
                }
                break;
            }
            case IROpCode::StInd: {
                // stind.*: pop value then address; write value bytes to address.
                const InterpreterValue stind_val = Pop(&frame->stack);
                const InterpreterValue stind_addr = Pop(&frame->stack);
                // Determine write size from type discriminator (immediate_i4).
                CHAOS_IL2CPP_SIZE stind_write_size = sizeof(void*);
                switch (instruction.immediate_i4) {
                    case 0: case 1: stind_write_size = 1; break;  // i1, u1
                    case 2: case 3: stind_write_size = 2; break;  // i2, u2
                    case 4: case 5: stind_write_size = 4; break;  // i4, u4
                    case 6:          stind_write_size = 8; break;  // i8
                    case 7:          stind_write_size = sizeof(void*); break; // i
                    case 8:          stind_write_size = 4; break;  // r4
                    case 9:          stind_write_size = 8; break;  // r8
                    case 10:         stind_write_size = sizeof(void*); break; // ref
                    default:         stind_write_size = sizeof(void*); break;
                }
                void* stind_dst = (stind_addr.tag == ValueTag::Struct || stind_addr.tag == ValueTag::ObjectRef)
                    ? stind_addr.obj : nullptr;
                if (stind_dst != nullptr) {
                    CHAOS_IL2CPP_INT64 stind_raw = 0;
                    if (stind_val.tag == ValueTag::Int32 || stind_val.tag == ValueTag::Float32) {
                        stind_raw = stind_val.i32;
                    } else if (stind_val.tag == ValueTag::Int64 || stind_val.tag == ValueTag::Float64) {
                        stind_raw = stind_val.i64;
                    } else {
                        stind_raw = reinterpret_cast<CHAOS_IL2CPP_INT64>(stind_val.obj);
                    }
                    // Write barrier for reference stores (immediate_i4 == 10)
                    if (instruction.immediate_i4 == 10) {
                        using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
                        BgcSatbPreWriteBarrier(reinterpret_cast<void**>(stind_dst));
                    }
                    std::memcpy(stind_dst, &stind_raw, stind_write_size);
                    if (instruction.immediate_i4 == 10) {
                        chaos_gc_dirty_card(stind_dst);
                    }
                }
                break;
            }
            case IROpCode::Switch: {
                const CHAOS_IL2CPP_INT32 index = ReadInt32(Pop(&frame->stack));
                // Check if index is within valid range of case targets.
                if (index >= 0 && instruction.switch_targets != nullptr &&
                    static_cast<CHAOS_IL2CPP_UINT32>(index) < instruction.switch_target_count) {
                    instruction_index = instruction.switch_targets[index];
                    continue;
                }
                // Out of range: use default target (branch_target), if set.
                if (instruction.branch_target != static_cast<CHAOS_IL2CPP_SIZE>(-1)) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                // Fall through if no default target.
                break;
            }
            case IROpCode::LdToken: {
                // Push the raw metadata token as a placeholder RuntimeHandle.
                frame->stack.push_back(InterpreterValue::from_i32(instruction.immediate_i4));
                break;
            }
            case IROpCode::InitObj: {
                const InterpreterValue addr = Pop(&frame->stack);
                if (addr.tag == ValueTag::Struct && addr.obj != nullptr) {
                    std::memset(addr.obj, 0, addr.struct_size);
                }
                // For value types on stack, initobj is a no-op (already default).
                break;
            }
            case IROpCode::SizeOf:
                frame->stack.push_back(InterpreterValue::from_i32(instruction.immediate_i4));
                break;
            case IROpCode::LdFtn:
                frame->stack.push_back(InterpreterValue::from_obj(instruction.call_target));
                break;
            case IROpCode::LdVirtFtn: {
                // Pop the object, then push the resolved virtual function pointer.
                const InterpreterValue ldvirtftn_obj = Pop(&frame->stack);
                if (ldvirtftn_obj.tag != ValueTag::ObjectRef || ldvirtftn_obj.obj == nullptr) {
                    frame->stack.push_back(InterpreterValue::null_val());
                    break;
                }
                const CHAOS_IL2CPP_UINT32 inst_type_token =
                    static_cast<InterpreterObject*>(ldvirtftn_obj.obj)->type_token;
                const CHAOS_IL2CPP_UINT32 decl_method_token =
                    static_cast<CHAOS_IL2CPP_UINT32>(instruction.secondary_index);
                void* resolved = nullptr;
                if (inst_type_token != 0u && decl_method_token != 0u) {
                    resolved = chaos::il2cpp::vtable_registry::ResolveVirtualMethodPointer(
                        inst_type_token, decl_method_token);
                }
                if (resolved != nullptr) {
                    frame->stack.push_back(InterpreterValue::from_obj(resolved));
                } else {
                    // Fallback: push the declared method's call_target
                    frame->stack.push_back(InterpreterValue::from_obj(instruction.call_target));
                }
                break;
            }
            case IROpCode::LdArgA: {
                if (instruction.operand_index < 0 ||
                    static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index) >= frame->arguments.size()) {
                    throw CHAOS_IL2CPP_OUT_OF_RANGE("argument");
                }
                // Push ADDRESS of argument slot (managed pointer / by-ref lvalue).
                // Dereferenced by LdObj/StObj.
                {
                    InterpreterValue mp;
                    mp.tag = ValueTag::ManagedPtr;
                    mp.obj = &frame->arguments[static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index)];
                    frame->stack.push_back(mp);
                }
                break;
            }
            case IROpCode::LdLocA: {
                EnsureLocal(&frame->locals, static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index));
                // Push ADDRESS of local slot (managed pointer / by-ref lvalue).
                // Dereferenced by LdObj/StObj.
                {
                    InterpreterValue mp;
                    mp.tag = ValueTag::ManagedPtr;
                    mp.obj = &frame->locals[static_cast<CHAOS_IL2CPP_SIZE>(instruction.operand_index)];
                    frame->stack.push_back(mp);
                }
                break;
            }
            case IROpCode::LocAlloc: {
                const CHAOS_IL2CPP_SIZE size = static_cast<CHAOS_IL2CPP_SIZE>(ReadInt32(Pop(&frame->stack)));
                void* buf = CHAOS_IL2CPP_MALLOC(size > 0u ? size : 1u);
                if (buf != nullptr) {
                    std::memset(buf, 0, size > 0u ? size : 1u);
                    // Register for automatic cleanup on frame exit.
                    frame->localloc_blocks.push_back(buf);
                }
                frame->stack.push_back(InterpreterValue::from_obj(buf));
                break;
            }
            [[likely]]
            case IROpCode::Ret:
                if (!frame->stack.empty()) {
                    result.has_return_value = true;
                    result.return_value = Pop(&frame->stack);
                    if (result.return_value.tag == ValueTag::Int32) {
                        result.int32_value = result.return_value.i32;
                    }
                }

                return result;

            // ── Phase A+: Full coverage expansion ─────────────────────────
            case IROpCode::Break:
                // Break (debug opcode) — treat as NOP.
                break;
            case IROpCode::BneUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                if (left != right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::BgeUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                if (left >= right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::BgtUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                if (left > right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::BleUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                if (left <= right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::BltUn: {
                const CHAOS_IL2CPP_UINT32 right = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_UINT32 left  = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                if (left < right) {
                    instruction_index = GetBranchTarget(method, instruction.branch_target);
                    continue;
                }
                break;
            }
            case IROpCode::AddOvf: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left  = ReadInt32(Pop(&frame->stack));
                // Promote to 64-bit to detect signed overflow.
                const CHAOS_IL2CPP_INT64 result_64 = static_cast<CHAOS_IL2CPP_INT64>(left) +
                    static_cast<CHAOS_IL2CPP_INT64>(right);
                if (result_64 > static_cast<CHAOS_IL2CPP_INT64>(INT32_MAX) ||
                    result_64 < static_cast<CHAOS_IL2CPP_INT64>(INT32_MIN)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: arithmetic overflow in add.ovf");
                }
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(result_64)));
                break;
            }
            case IROpCode::SubOvf: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left  = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT64 result_64 = static_cast<CHAOS_IL2CPP_INT64>(left) -
                    static_cast<CHAOS_IL2CPP_INT64>(right);
                if (result_64 > static_cast<CHAOS_IL2CPP_INT64>(INT32_MAX) ||
                    result_64 < static_cast<CHAOS_IL2CPP_INT64>(INT32_MIN)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: arithmetic overflow in sub.ovf");
                }
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(result_64)));
                break;
            }
            case IROpCode::MulOvf: {
                const CHAOS_IL2CPP_INT32 right = ReadInt32(Pop(&frame->stack));
                const CHAOS_IL2CPP_INT32 left  = ReadInt32(Pop(&frame->stack));
                // Use compiler builtin for overflow-checked multiplication.
                CHAOS_IL2CPP_INT32 mul_result = 0;
#if defined(__GNUC__) || defined(__clang__)
                if (__builtin_mul_overflow(left, right, &mul_result)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: arithmetic overflow in mul.ovf");
                }
#else
                // MSVC fallback: promote to 64-bit and check range.
                const CHAOS_IL2CPP_INT64 mul_64 = static_cast<CHAOS_IL2CPP_INT64>(left) *
                    static_cast<CHAOS_IL2CPP_INT64>(right);
                if (mul_64 > static_cast<CHAOS_IL2CPP_INT64>(INT32_MAX) ||
                    mul_64 < static_cast<CHAOS_IL2CPP_INT64>(INT32_MIN)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: arithmetic overflow in mul.ovf");
                }
                mul_result = static_cast<CHAOS_IL2CPP_INT32>(mul_64);
#endif
                frame->stack.push_back(InterpreterValue::from_i32(mul_result));
                break;
            }
            case IROpCode::ConvOvfI: {
                const CHAOS_IL2CPP_INT64 val = ReadInt64(Pop(&frame->stack));
                if (val > static_cast<CHAOS_IL2CPP_INT64>(INT32_MAX) ||
                    val < static_cast<CHAOS_IL2CPP_INT64>(INT32_MIN)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: conv.ovf.i");
                }
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(val)));
                break;
            }
            case IROpCode::ConvOvfI4: {
                const CHAOS_IL2CPP_INT64 val = ReadInt64(Pop(&frame->stack));
                if (val > static_cast<CHAOS_IL2CPP_INT64>(INT32_MAX) ||
                    val < static_cast<CHAOS_IL2CPP_INT64>(INT32_MIN)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: conv.ovf.i4");
                }
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(val)));
                break;
            }
            case IROpCode::ConvOvfI8:
                frame->stack.push_back(InterpreterValue::from_i64(ReadInt64(Pop(&frame->stack))));
                break;
            case IROpCode::ConvOvfU: {
                const CHAOS_IL2CPP_INT64 val = ReadInt64(Pop(&frame->stack));
                if (val < 0 || val > static_cast<CHAOS_IL2CPP_INT64>(UINT32_MAX)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: conv.ovf.u");
                }
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(val)));
                break;
            }
            case IROpCode::ConvOvfU4: {
                const CHAOS_IL2CPP_INT64 val = ReadInt64(Pop(&frame->stack));
                if (val < 0 || val > static_cast<CHAOS_IL2CPP_INT64>(UINT32_MAX)) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: conv.ovf.u4");
                }
                frame->stack.push_back(InterpreterValue::from_i32(static_cast<CHAOS_IL2CPP_INT32>(val)));
                break;
            }
            case IROpCode::ConvOvfU8: {
                const CHAOS_IL2CPP_INT64 val = ReadInt64(Pop(&frame->stack));
                if (val < 0) {
                    throw CHAOS_IL2CPP_RUNTIME_ERROR("OverflowException: conv.ovf.u8");
                }
                frame->stack.push_back(InterpreterValue::from_i64(val));
                break;
            }
            case IROpCode::LdObj: {
                // Pop address, push value at address.
                const InterpreterValue addr_val = Pop(&frame->stack);
                if (addr_val.tag == ValueTag::ManagedPtr && addr_val.obj != nullptr) {
                    // Managed pointer (from LdArgA/LdLocA): dereference InterpreterValue.
                    frame->stack.push_back(*static_cast<InterpreterValue*>(addr_val.obj));
                } else {
                    // Raw pointer fallback: read as int32.
                    frame->stack.push_back(InterpreterValue::from_i32(ReadInt32(addr_val)));
                }
                break;
            }
            case IROpCode::StObj: {
                // StObj: pop value then address; write value bytes to address.
                const InterpreterValue stobj_val = Pop(&frame->stack);
                const InterpreterValue stobj_addr = Pop(&frame->stack);
                // Managed pointer (from LdArgA/LdLocA): write through to InterpreterValue slot.
                if (stobj_addr.tag == ValueTag::ManagedPtr && stobj_addr.obj != nullptr) {
                    auto* iv_slot = static_cast<InterpreterValue*>(stobj_addr.obj);
                    using chaos::il2cpp::runtime_core::BgcSatbPreWriteBarrier;
                    BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&iv_slot->obj));
                    *iv_slot = stobj_val;
                    chaos_gc_dirty_card(stobj_addr.obj);
                } else {
                    void* stobj_dst = (stobj_addr.tag == ValueTag::Struct || stobj_addr.tag == ValueTag::ObjectRef)
                        ? stobj_addr.obj : nullptr;
                    if (stobj_dst != nullptr) {
                        CHAOS_IL2CPP_SIZE stobj_write_size = sizeof(void*);
                        const void* stobj_src = nullptr;
                        CHAOS_IL2CPP_INT64 stobj_raw = 0;
                        if (stobj_val.tag == ValueTag::Struct && stobj_val.obj != nullptr) {
                            stobj_src = stobj_val.obj;
                            stobj_write_size = stobj_val.struct_size;
                        } else if (stobj_val.tag == ValueTag::Int32 || stobj_val.tag == ValueTag::Float32) {
                            stobj_raw = stobj_val.i32;
                            stobj_src = &stobj_raw;
                            stobj_write_size = 4;
                        } else if (stobj_val.tag == ValueTag::Int64 || stobj_val.tag == ValueTag::Float64) {
                            stobj_raw = stobj_val.i64;
                            stobj_src = &stobj_raw;
                            stobj_write_size = 8;
                        } else {
                            stobj_raw = reinterpret_cast<CHAOS_IL2CPP_INT64>(stobj_val.obj);
                            stobj_src = &stobj_raw;
                            stobj_write_size = sizeof(void*);
                        }
                        std::memcpy(stobj_dst, stobj_src, stobj_write_size);
                    }
                }
                break;
            }
            case IROpCode::LdElemA: {
                // Load element address — in our stack model, push element value.
                const CHAOS_IL2CPP_SIZE index = static_cast<CHAOS_IL2CPP_SIZE>(ReadInt32(Pop(&frame->stack)));
                auto* array = RequireArray(Pop(&frame->stack));
                if (index >= array->elements.size()) {
                    throw CHAOS_IL2CPP_OUT_OF_RANGE("array_index");
                }
                frame->stack.push_back(array->elements[index]);
                break;
            }
            case IROpCode::Cpblk: {
                const CHAOS_IL2CPP_UINT32 cp_size = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const InterpreterValue src_val = Pop(&frame->stack);
                const InterpreterValue dst_val = Pop(&frame->stack);
                void* dst_ptr = (dst_val.tag == ValueTag::Struct || dst_val.tag == ValueTag::ObjectRef) ? dst_val.obj : nullptr;
                void* src_ptr = (src_val.tag == ValueTag::Struct || src_val.tag == ValueTag::ObjectRef) ? src_val.obj : nullptr;
                if (dst_ptr != nullptr && src_ptr != nullptr) {
                    std::memcpy(dst_ptr, src_ptr, cp_size);
                }
                break;
            }
            case IROpCode::InitBlk: {
                const CHAOS_IL2CPP_UINT32 init_size = static_cast<CHAOS_IL2CPP_UINT32>(ReadInt32(Pop(&frame->stack)));
                const CHAOS_IL2CPP_INT32 init_value = ReadInt32(Pop(&frame->stack));
                const InterpreterValue addr_val = Pop(&frame->stack);
                void* ptr = (addr_val.tag == ValueTag::Struct || addr_val.tag == ValueTag::ObjectRef) ? addr_val.obj : nullptr;
                if (ptr != nullptr) {
                    std::memset(ptr, init_value, init_size);
                }
                break;
            }
            default:
                throw CHAOS_IL2CPP_RUNTIME_ERROR("unsupported opcode");
        }

        ++instruction_index;
    }

    return result;
}

}  // namespace chaos::il2cpp::interpreter
