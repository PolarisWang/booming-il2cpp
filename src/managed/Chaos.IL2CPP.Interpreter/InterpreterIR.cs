namespace Chaos.IL2CPP.Interpreter;

public sealed record InterpreterIR
{
    public IReadOnlyList<IRMethod> Methods { get; init; } = [];
}

public sealed record IRMethod
{
    public required string MethodId { get; init; }

    public required string SubjectId { get; init; }

    public IReadOnlyList<IRBasicBlock> Blocks { get; init; } = [];

    public IReadOnlyList<IRExceptionRegion> ExceptionRegions { get; init; } = [];
}

public sealed record IRBasicBlock
{
    public required int BlockId { get; init; }

    public IReadOnlyList<IRInstruction> Instructions { get; init; } = [];
}

public sealed record IRInstruction
{
    public required IROpCode OpCode { get; init; }

    public IReadOnlyList<IROperand> Operands { get; init; } = [];

    public IROperand? Result { get; init; }
}

public sealed record IROperand
{
    public required IROperandKind Kind { get; init; }

    public required IRTypeTag TypeTag { get; init; }

    public string? Symbol { get; init; }

    public int? Int32Value { get; init; }

    public long? Int64Value { get; init; }

    public float? Float32Value { get; init; }

    public double? Float64Value { get; init; }

    public string? StringValue { get; init; }

    public int? BasicBlockId { get; init; }
}

public sealed record IRExceptionRegion
{
    public required IRExceptionRegionKind Kind { get; init; }

    public IReadOnlyList<int> TryBlockIds { get; init; } = [];

    public required int HandlerBlockId { get; init; }

    /// For Filter regions: the block that evaluates the filter condition.
    /// The filter block ends with EndFilter; HandlerBlockId is the catch handler.
    public int? FilterBlockId { get; init; }
}

public enum IROperandKind
{
    Int32Literal,
    StringLiteral,
    NullLiteral,
    Local,
    Argument,
    Field,
    Method,
    BasicBlock,
}

public enum IRTypeTag
{
    Void,
    Int32,
    Int64,
    Float32,
    Float64,
    String,
    Null,
    Boolean,
    Object,
    Array,
    NativeInt,
}

public enum IRExceptionRegionKind
{
    Catch,
    Finally,
    Filter,  // when(expr) filter — run filter block, enter catch if result != 0
    Fault,   // fault block — run when leaving try via exception (does not suppress)
}

public enum IROpCode
{
    LdcI4,
    LdcI8,
    LdcR4,
    LdcR8,
    LdStr,
    LdNull,
    LdLoc,
    StLoc,
    LdArg,
    StArg,
    LdFld,
    StFld,
    LdSFld,
    StSFld,
    Call,
    CallVirt,
    CallBridge,
    Br,
    BrTrue,
    BrFalse,
    Beq,
    Blt,
    Bgt,
    Ble,
    Bge,
    Add,
    Sub,
    Mul,
    Div,
    Rem,
    Neg,
    Ceq,
    Clt,
    Cgt,
    NewObj,
    Box,
    Unbox,
    CastClass,
    IsInst,
    Conv_I4,
    Conv_I8,
    Conv_R4,
    Conv_R8,
    NewArr,
    LdElem,
    StElem,
    LdLen,
    Pop,
    Throw,
    Rethrow,
    Leave,
    EndFinally,
    EndFilter,  // marks end of a filter block; operand is the filter result (0=reject, 1=accept)
    Ret,
}
