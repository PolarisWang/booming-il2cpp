#ifndef CHAOS_IL2CPP_INTERPRETER_VM_H_
#define CHAOS_IL2CPP_INTERPRETER_VM_H_

#include <chaos/native_types.h>
#include <vector>

#include <ir_opcodes.h>

namespace chaos::il2cpp::interpreter {

// Use the generated IROpCode from ir_opcodes.h (auto-generated from schemas/ir_opcodes.yaml).
// Verify at compile time that the generated enum is available.
static_assert(static_cast<int>(IROpCode::LdcI4) == 0, "IROpCode::LdcI4 must be 0");
static_assert(static_cast<int>(IROpCode::CallVirtConstrained) == 98, "IROpCode::CallVirtConstrained must be 98");

enum class ValueTag : uint8_t {
    Void = 0,
    Int32 = 1,
    Int64 = 2,
    Float32 = 3,
    Float64 = 4,
    ObjectRef = 5,
    Null = 6,
    Struct = 7,   // Value type (struct) — data stored via heap pointer
    ManagedPtr = 8, // Managed pointer (address of arg/local slot) — obj points to InterpreterValue
};

struct InterpreterValue {
    ValueTag tag = ValueTag::Void;
    CHAOS_IL2CPP_UINT32 struct_size = 0u;  // valid only when tag == Struct
    union {
        CHAOS_IL2CPP_INT32 i32;
        CHAOS_IL2CPP_INT64 i64;
        float f32;
        double f64;
        void* obj;          // ObjectRef or Struct data (heap-allocated)
    };

    InterpreterValue() noexcept : tag(ValueTag::Void), i64(0) {}
    InterpreterValue(CHAOS_IL2CPP_INT32 value) noexcept : tag(ValueTag::Int32), i32(value) {}

    /// Deep-copy constructor — performs a deep copy of struct data.
    InterpreterValue(const InterpreterValue& other);
    /// Deep-copy assignment.
    InterpreterValue& operator=(const InterpreterValue& other);
    /// Destructor — frees struct data if present.
    ~InterpreterValue();

    static InterpreterValue from_i32(CHAOS_IL2CPP_INT32 value);
    static InterpreterValue from_i64(CHAOS_IL2CPP_INT64 value);
    static InterpreterValue from_f32(float value);
    static InterpreterValue from_f64(double value);
    static InterpreterValue from_obj(void* value);
    static InterpreterValue from_struct(const void* data, CHAOS_IL2CPP_UINT32 size);
    static InterpreterValue null_val();

    /// Free struct data if tag == Struct.  Called by destructor and assignment.
    void FreeStruct();
};

struct IRInstruction {
    IROpCode op_code = IROpCode::Ret;
    CHAOS_IL2CPP_INT32 operand_index = 0;
    CHAOS_IL2CPP_INT32 immediate_i4 = 0;
    CHAOS_IL2CPP_SIZE branch_target = 0;
    CHAOS_IL2CPP_INT64 immediate_i8 = 0;
    double immediate_r8 = 0.0;
    const char* string_operand = nullptr;
    CHAOS_IL2CPP_SIZE field_offset = 0;
    CHAOS_IL2CPP_SIZE secondary_index = 0;
    void* call_target = nullptr;   // MethodInfoHandle or bridge target for Call/CallVirt/CallBridge
    CHAOS_IL2CPP_UINT32 arg_count = 0u;  // Number of arguments for call instructions
    bool is_instance_call = false;  // true = call_args[0] is 'this' pointer (CallVirt/CallVirtConstrained, or instance Call)

    // Switch instruction: branch targets for each case, populated by the IR builder.
    // The default target is stored in branch_target (or -1 if absent).
    // secondary_index stores the number of case targets (when used for Switch).
    const CHAOS_IL2CPP_SIZE* switch_targets = nullptr;
    CHAOS_IL2CPP_SIZE       switch_target_count = 0u;
};

// ── SEH (Structured Exception Handling) ──────────────────────────────────

/// Exception clause flags (ECMA 335 Partition II, 25.4.6).
enum class SEHFlags : uint32_t {
    None      = 0x0000,
    Exception = 0x0000,    // Typed catch clause
    Filter    = 0x0001,    // Filter-based catch
    Finally   = 0x0002,    // Finally handler
    Fault     = 0x0004,    // Fault handler (always executed on exception)
    Typed     = 0x0008,    // Typed catch (has class_token)
};

/// A single exception-handling clause, converted from IL metadata.
struct SEHClause {
    SEHFlags              flags = SEHFlags::None;
    CHAOS_IL2CPP_SIZE     try_start_idx  = 0u;  // instruction index of try block start
    CHAOS_IL2CPP_SIZE     try_end_idx    = 0u;  // instruction index of try block end (exclusive)
    CHAOS_IL2CPP_SIZE     handler_start_idx = 0u;  // instruction index of handler start
    CHAOS_IL2CPP_SIZE     handler_end_idx   = 0u;  // instruction index of handler end (exclusive)
    CHAOS_IL2CPP_UINT32   class_token    = 0u;  // type token for typed catch
};

struct IRMethod {
    CHAOS_IL2CPP_VECTOR(IRInstruction) instructions = {};
    CHAOS_IL2CPP_VECTOR(SEHClause)     seh_clauses  = {};
};

/// Result of an external dispatch operation (DispatchCallback).
/// Normal return: has_value=true, value=return_value.
/// Exception:     threw_exception=true, exception_value=the thrown object.
/// This avoids C++ exception propagation through the callback function pointer.
struct DispatchResult {
    bool has_value = false;
    InterpreterValue value = {};
    bool threw_exception = false;
    InterpreterValue exception_value = {};
};

/// Callback for dispatching call instructions synchronously within Execute.
/// Implemented by the bridge layer (runtime_instantiation.cpp).
/// Returns DispatchResult to communicate both normal returns and exceptions.
using DispatchCallback = auto (*)(
    void*                               call_target,       // MethodInfoHandle
    const InterpreterValue*             call_args,         // arguments array
    CHAOS_IL2CPP_UINT32                 arg_count,         // number of args
    bool                                is_instance_call,  // true = call_args[0] is 'this'
    void*                               dispatch_context   // per-execution context
    ) -> DispatchResult;

struct ExecutionFrame {
    CHAOS_IL2CPP_VECTOR(InterpreterValue) arguments = {};
    CHAOS_IL2CPP_VECTOR(InterpreterValue) locals = {};
    CHAOS_IL2CPP_VECTOR(InterpreterValue) stack = {};

    // Optional dispatch callback for inline call resolution.
    // When non-null, Call/CallVirt/CallBridge invoke this synchronously
    // instead of returning needs_external_dispatch=true.
    DispatchCallback dispatch_fn = nullptr;
    void*            dispatch_context = nullptr;

    // Tracks localloc allocations for automatic cleanup on frame exit.
    CHAOS_IL2CPP_VECTOR(void*) localloc_blocks = {};

    ~ExecutionFrame();
    ExecutionFrame() = default;
    ExecutionFrame(const ExecutionFrame&) = delete;
    ExecutionFrame& operator=(const ExecutionFrame&) = delete;
};

// Lightweight object storage for the interpreter sandbox.
// Fields are stored as a flat vector; type_token enables virtual dispatch.
struct InterpreterObject {
    CHAOS_IL2CPP_VECTOR(InterpreterValue) fields = {};
    CHAOS_IL2CPP_UINT32 type_token = 0u;
};

// Lightweight array storage for NewArr / LdElem / StElem.
struct ArrayStorage {
    CHAOS_IL2CPP_VECTOR(InterpreterValue) elements = {};
};

// Boxed value type for Box/Unbox.
struct BoxedValue {
    InterpreterValue value = {};
};

struct ExecutionResult {
    bool has_return_value = false;
    CHAOS_IL2CPP_INT32 int32_value = 0;
    InterpreterValue return_value = {};

    // External dispatch support
    bool needs_external_dispatch = false;
    void* call_target = nullptr;  // MethodInfoHandle for external dispatch
    CHAOS_IL2CPP_VECTOR(InterpreterValue) call_args = {};

    // Exception propagation (cross-frame)
    bool threw_exception = false;
    InterpreterValue exception_value = {};  // exception object when threw_exception == true
};

class InterpreterVM {
public:
    ExecutionResult Execute(const IRMethod& method, ExecutionFrame* frame) const;

private:
    static CHAOS_IL2CPP_SIZE GetBranchTarget(const IRMethod& method, CHAOS_IL2CPP_SIZE target);
    static void EnsureLocal(CHAOS_IL2CPP_VECTOR(InterpreterValue)* locals, CHAOS_IL2CPP_SIZE index);
    static InterpreterValue Pop(CHAOS_IL2CPP_VECTOR(InterpreterValue)* stack);
};

}  // namespace chaos::il2cpp::interpreter

#endif  // CHAOS_IL2CPP_INTERPRETER_VM_H_
