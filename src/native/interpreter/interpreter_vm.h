#ifndef CHAOS_IL2CPP_INTERPRETER_VM_H_
#define CHAOS_IL2CPP_INTERPRETER_VM_H_

#include <chaos/native_types.h>

#include <cstddef>
#include <cstdint>
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
};

enum class ValueTag : uint8_t {
    Void = 0,
    Int32 = 1,
    Int64 = 2,
    Float32 = 3,
    Float64 = 4,
    ObjectRef = 5,
    Null = 6,
};

struct InterpreterValue {
    ValueTag tag = ValueTag::Void;
    union {
        int32_t i32;
        int64_t i64;
        float f32;
        double f64;
        void* obj;
    };

    InterpreterValue() noexcept : tag(ValueTag::Void), i64(0) {}
    InterpreterValue(int32_t value) noexcept : tag(ValueTag::Int32), i32(value) {}

    static InterpreterValue from_i32(int32_t value);
    static InterpreterValue from_i64(int64_t value);
    static InterpreterValue from_f32(float value);
    static InterpreterValue from_f64(double value);
    static InterpreterValue from_obj(void* value);
    static InterpreterValue null_val();
};

struct IRInstruction {
    IROpCode op_code = IROpCode::Ret;
    int32_t operand_index = 0;
    int32_t immediate_i4 = 0;
    size_t branch_target = 0;
    int64_t immediate_i8 = 0;
    double immediate_r8 = 0.0;
    const char* string_operand = nullptr;
    size_t field_offset = 0;
    size_t secondary_index = 0;
};

struct IRMethod {
    CHAOS_IL2CPP_VECTOR(IRInstruction) instructions = {};
};

struct ExecutionFrame {
    CHAOS_IL2CPP_VECTOR(InterpreterValue) arguments = {};
    CHAOS_IL2CPP_VECTOR(InterpreterValue) locals = {};
    CHAOS_IL2CPP_VECTOR(InterpreterValue) stack = {};
};

struct ExecutionResult {
    bool has_return_value = false;
    int32_t int32_value = 0;
    InterpreterValue return_value = {};
};

class InterpreterVM {
public:
    ExecutionResult Execute(const IRMethod& method, ExecutionFrame* frame) const;

private:
    static size_t GetBranchTarget(const IRMethod& method, size_t target);
    static void EnsureLocal(CHAOS_IL2CPP_VECTOR(InterpreterValue)* locals, size_t index);
    static InterpreterValue Pop(CHAOS_IL2CPP_VECTOR(InterpreterValue)* stack);
};

}  // namespace chaos::il2cpp::interpreter

#endif  // CHAOS_IL2CPP_INTERPRETER_VM_H_
