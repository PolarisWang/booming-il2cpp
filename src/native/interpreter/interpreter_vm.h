#ifndef CHAOS_IL2CPP_INTERPRETER_VM_H_
#define CHAOS_IL2CPP_INTERPRETER_VM_H_

#include <chaos/native_types.h>
#include <vector>

namespace chaos::il2cpp::interpreter {

enum class IROpCode {
    LdcI4 = 0,
    LdcI8 = 1,
    LdcR4 = 2,
    LdcR8 = 3,
    LdStr = 4,
    LdNull = 5,
    LdArg = 6,
    LdLoc = 7,
    StLoc = 8,
    StArg = 9,
    LdFld = 10,
    StFld = 11,
    LdSFld = 12,
    StSFld = 13,
    Call = 14,
    CallVirt = 15,
    CallBridge = 16,
    Br = 17,
    BrTrue = 18,
    BrFalse = 19,
    Beq = 20,
    Blt = 21,
    Bgt = 22,
    Ble = 23,
    Bge = 24,
    Add = 25,
    Sub = 26,
    Mul = 27,
    Div = 28,
    Rem = 29,
    Neg = 30,
    Ceq = 31,
    Clt = 32,
    Cgt = 33,
    NewObj = 34,
    Box = 35,
    Unbox = 36,
    CastClass = 37,
    IsInst = 38,
    Conv_I4 = 39,
    Conv_I8 = 40,
    Conv_R4 = 41,
    Conv_R8 = 42,
    NewArr = 43,
    LdElem = 44,
    StElem = 45,
    LdLen = 46,
    Pop = 47,
    Throw = 48,
    Rethrow = 49,
    Leave = 50,
    EndFinally = 51,
    EndFilter = 52,
    Ret = 53,

    // -- Opcode coverage expansion (Phase A) --
    Dup = 54,
    DivUn = 55,
    RemUn = 56,
    And = 57,
    Or = 58,
    Xor = 59,
    Not = 60,
    Shl = 61,
    Shr = 62,
    ShrUn = 63,
    ConvRUn = 64,
    ConvI = 65,     // native int
    ConvU = 66,     // native unsigned
    LdInd = 67,     // ldind.* -- type info via immediate_i4
    StInd = 68,     // stind.* -- type info via immediate_i4
    Switch = 69,
    LdToken = 70,
    InitObj = 71,
    SizeOf = 72,
    LdFtn = 73,
    LdVirtFtn = 74,
    LdArgA = 75,
    LdLocA = 76,
    LocAlloc = 77,
};

enum class ValueTag : uint8_t {
    Void = 0,
    Int32 = 1,
    Int64 = 2,
    Float32 = 3,
    Float64 = 4,
    ObjectRef = 5,
    Null = 6,
    Struct = 7,   // Value type (struct) — data stored via heap pointer
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

struct ExecutionFrame {
    CHAOS_IL2CPP_VECTOR(InterpreterValue) arguments = {};
    CHAOS_IL2CPP_VECTOR(InterpreterValue) locals = {};
    CHAOS_IL2CPP_VECTOR(InterpreterValue) stack = {};

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
