using System.Globalization;

namespace Chaos.IL2CPP.Interpreter;

public sealed class ManagedInterpreterExecutor
{
    private readonly Func<string, IReadOnlyList<object?>, object?>? _bridgeInvoker;
    private readonly Func<string, IRMethod>? _methodResolver;

    public ManagedInterpreterExecutor(
        Func<string, IReadOnlyList<object?>, object?>? bridgeInvoker = null,
        Func<string, IRMethod>? methodResolver = null)
    {
        _bridgeInvoker = bridgeInvoker;
        _methodResolver = methodResolver;
    }

    public Func<int, int> CreateInt32UnaryInvoker(IRMethod method)
    {
        ArgumentNullException.ThrowIfNull(method);
        return value => ExecuteInt32(method, new int[] { value });
    }

    public int ExecuteInt32(IRMethod method, IReadOnlyList<int> arguments)
    {
        ArgumentNullException.ThrowIfNull(arguments);
        return ExecuteInt32(method, arguments.Cast<object?>().ToArray());
    }

    public int ExecuteInt32(IRMethod method, IReadOnlyList<object?> arguments)
    {
        var result = Execute(method, arguments);
        return result switch
        {
            int int32 => int32,
            null => 0,
            _ => Convert.ToInt32(result, CultureInfo.InvariantCulture),
        };
    }

    public object? Execute(IRMethod method, IReadOnlyList<object?> arguments)
    {
        ArgumentNullException.ThrowIfNull(method);
        ArgumentNullException.ThrowIfNull(arguments);

        if (method.Blocks.Count == 0)
        {
            throw new InvalidOperationException("IR method does not contain any basic blocks.");
        }

        var values = new Dictionary<string, object?>(StringComparer.Ordinal);
        var blockOrder = method.Blocks
            .OrderBy(block => block.BlockId)
            .ToList();
        var blockOffsets = blockOrder
            .Select((block, index) => new { block.BlockId, Index = index })
            .ToDictionary(entry => entry.BlockId, entry => entry.Index);
        var blockOffset = 0;
        var pendingFinallyTargets = new Stack<int>();
        Exception? activeException = null;

        while (blockOffset < blockOrder.Count)
        {
            var block = blockOrder[blockOffset];
            var jumped = false;
            try
            {
                foreach (var instruction in block.Instructions)
                {
                    switch (instruction.OpCode)
                    {
                        case IROpCode.LdcI4:
                            WriteResult(values, instruction.Result, ReadInt32Literal(RequireOperand(instruction, 0)));
                            break;
                        case IROpCode.LdStr:
                            WriteResult(values, instruction.Result, RequireOperand(instruction, 0).StringValue);
                            break;
                        case IROpCode.LdNull:
                            WriteResult(values, instruction.Result, null);
                            break;
                        case IROpCode.Pop:
                            break;
                        case IROpCode.LdArg:
                            WriteResult(values, instruction.Result, ReadOperandValue(RequireOperand(instruction, 0), arguments, values));
                            break;
                        case IROpCode.LdLoc:
                            WriteResult(values, instruction.Result, ReadOperandValue(RequireOperand(instruction, 0), arguments, values));
                            break;
                        case IROpCode.StLoc:
                            values[RequireSymbol(RequireOperand(instruction, 1))] =
                                ReadOperandValue(RequireOperand(instruction, 0), arguments, values);
                            break;
                        case IROpCode.Add:
                            WriteBinary(values, instruction, arguments, values, static (left, right) => left + right);
                            break;
                        case IROpCode.Sub:
                            WriteBinary(values, instruction, arguments, values, static (left, right) => left - right);
                            break;
                        case IROpCode.Mul:
                            WriteBinary(values, instruction, arguments, values, static (left, right) => left * right);
                            break;
                        case IROpCode.Div:
                            WriteBinary(values, instruction, arguments, values, static (left, right) => left / right);
                            break;
                        case IROpCode.Rem:
                            WriteBinary(values, instruction, arguments, values, static (left, right) => left % right);
                            break;
                        case IROpCode.Ceq:
                            WriteBinary(values, instruction, arguments, values, static (left, right) => left == right ? 1 : 0);
                            break;
                        case IROpCode.Clt:
                            WriteBinary(values, instruction, arguments, values, static (left, right) => left < right ? 1 : 0);
                            break;
                        case IROpCode.Cgt:
                            WriteBinary(values, instruction, arguments, values, static (left, right) => left > right ? 1 : 0);
                            break;
                        case IROpCode.Br:
                            blockOffset = ResolveBlockOffset(RequireOperand(instruction, 0), blockOffsets);
                            jumped = true;
                            break;
                        case IROpCode.BrTrue:
                            if (ReadInt32Operand(RequireOperand(instruction, 0), arguments, values) != 0)
                            {
                                blockOffset = ResolveBlockOffset(RequireOperand(instruction, 1), blockOffsets);
                                jumped = true;
                            }
                            break;
                        case IROpCode.BrFalse:
                            if (ReadInt32Operand(RequireOperand(instruction, 0), arguments, values) == 0)
                            {
                                blockOffset = ResolveBlockOffset(RequireOperand(instruction, 1), blockOffsets);
                                jumped = true;
                            }
                            break;
                        case IROpCode.Blt:
                            if (ReadInt32Operand(RequireOperand(instruction, 0), arguments, values) <
                                ReadInt32Operand(RequireOperand(instruction, 1), arguments, values))
                            {
                                blockOffset = ResolveBlockOffset(RequireOperand(instruction, 2), blockOffsets);
                                jumped = true;
                            }
                            break;
                        case IROpCode.Bgt:
                            if (ReadInt32Operand(RequireOperand(instruction, 0), arguments, values) >
                                ReadInt32Operand(RequireOperand(instruction, 1), arguments, values))
                            {
                                blockOffset = ResolveBlockOffset(RequireOperand(instruction, 2), blockOffsets);
                                jumped = true;
                            }
                            break;
                        case IROpCode.Ble:
                            if (ReadInt32Operand(RequireOperand(instruction, 0), arguments, values) <=
                                ReadInt32Operand(RequireOperand(instruction, 1), arguments, values))
                            {
                                blockOffset = ResolveBlockOffset(RequireOperand(instruction, 2), blockOffsets);
                                jumped = true;
                            }
                            break;
                        case IROpCode.Bge:
                            if (ReadInt32Operand(RequireOperand(instruction, 0), arguments, values) >=
                                ReadInt32Operand(RequireOperand(instruction, 1), arguments, values))
                            {
                                blockOffset = ResolveBlockOffset(RequireOperand(instruction, 2), blockOffsets);
                                jumped = true;
                            }
                            break;
                        case IROpCode.CallBridge:
                            WriteResult(values, instruction.Result, InvokeBridge(instruction, arguments, values));
                            break;
                        case IROpCode.Call:
                            WriteResult(values, instruction.Result, InvokeMethod(instruction, arguments, values));
                            break;
                        case IROpCode.CallVirt:
                            WriteResult(values, instruction.Result, InvokeMethod(instruction, arguments, values));
                            break;
                        case IROpCode.Leave:
                            pendingFinallyTargets.Push(ResolveBlockOffset(RequireOperand(instruction, 0), blockOffsets));
                            blockOffset = ResolveBlockOffset(RequireOperand(instruction, 1), blockOffsets);
                            jumped = true;
                            break;
                        case IROpCode.EndFinally:
                            blockOffset = ResumePendingFinallyTarget(pendingFinallyTargets);
                            jumped = true;
                            break;
                        case IROpCode.EndFilter:
                            // EndFilter result is the filter outcome; handled by the catch-when flow
                            // Nothing to do here: the filter evaluation was managed by the caller
                            break;
                        case IROpCode.Throw:
                            ThrowManagedException(instruction);
                            break;
                        case IROpCode.Rethrow:
                            RethrowActiveException(activeException);
                            break;
                        case IROpCode.Ret:
                            return instruction.Operands.Count == 0
                                ? null
                                : ReadOperandValue(instruction.Operands[0], arguments, values);
                        default:
                            throw new NotSupportedException($"unsupported managed interpreter opcode: {instruction.OpCode}");
                    }

                    if (jumped)
                    {
                        break;
                    }
                }
            }
            catch (Exception exception) when (TryResolveCatchHandler(method, block.BlockId, blockOffsets, out var handlerOffset))
            {
                activeException = exception;
                blockOffset = handlerOffset;
                jumped = true;
            }

            if (!jumped)
            {
                blockOffset++;
            }
        }

        throw new InvalidOperationException($"IR method '{method.SubjectId}' completed without ret.");
    }

    private object? InvokeBridge(
        IRInstruction instruction,
        IReadOnlyList<object?> arguments,
        IReadOnlyDictionary<string, object?> values)
    {
        if (_bridgeInvoker is null)
        {
            throw new InvalidOperationException("bridgeInvoker is not configured for CallBridge.");
        }

        var bridgeId = RequireSymbol(RequireOperand(instruction, 0));
        var bridgeArguments = instruction.Operands
            .Skip(1)
            .Select(operand => ReadOperandValue(operand, arguments, values))
            .ToList();
        return _bridgeInvoker(bridgeId, bridgeArguments);
    }

    private object? InvokeMethod(
        IRInstruction instruction,
        IReadOnlyList<object?> arguments,
        IReadOnlyDictionary<string, object?> values)
    {
        if (_methodResolver is null)
        {
            throw new InvalidOperationException($"methodResolver is not configured for {instruction.OpCode}.");
        }

        if (instruction.OpCode == IROpCode.CallVirt && ReadOperandValue(RequireOperand(instruction, 1), arguments, values) is null)
        {
            throw new NullReferenceException("callvirt receiver is null.");
        }

        var calleeSubjectId = RequireSymbol(RequireOperand(instruction, 0));
        var calleeArguments = instruction.Operands
            .Skip(1)
            .Select(operand => ReadOperandValue(operand, arguments, values))
            .ToList();
        var callee = _methodResolver(calleeSubjectId);
        return Execute(callee, calleeArguments);
    }

    private static int ResumePendingFinallyTarget(Stack<int> pendingFinallyTargets)
    {
        if (pendingFinallyTargets.Count == 0)
        {
            throw new InvalidOperationException("EndFinally requires a pending leave target.");
        }

        return pendingFinallyTargets.Pop();
    }

    private static bool TryResolveCatchHandler(
        IRMethod method,
        int blockId,
        IReadOnlyDictionary<int, int> blockOffsets,
        out int handlerOffset)
    {
        // 1. Try Catch region (unconditional catch).
        var catchRegion = method.ExceptionRegions.FirstOrDefault(candidate =>
            candidate.Kind == IRExceptionRegionKind.Catch &&
            candidate.TryBlockIds.Contains(blockId));
        if (catchRegion is not null)
        {
            handlerOffset = ResolveBlockOffset(
                new IROperand
                {
                    Kind = IROperandKind.BasicBlock,
                    TypeTag = IRTypeTag.Void,
                    BasicBlockId = catchRegion.HandlerBlockId,
                },
                blockOffsets);
            return true;
        }

        // 2. Try Filter region: only enter handler if FilterBlockId resolves to nonzero.
        // For now we accept any filter as matching (conservative: filters always pass in interpreter).
        // Full filter evaluation would require executing the filter block in a nested context.
        var filterRegion = method.ExceptionRegions.FirstOrDefault(candidate =>
            candidate.Kind == IRExceptionRegionKind.Filter &&
            candidate.TryBlockIds.Contains(blockId));
        if (filterRegion is not null)
        {
            handlerOffset = ResolveBlockOffset(
                new IROperand
                {
                    Kind = IROperandKind.BasicBlock,
                    TypeTag = IRTypeTag.Void,
                    BasicBlockId = filterRegion.HandlerBlockId,
                },
                blockOffsets);
            return true;
        }

        // 3. Try Fault region: enter unconditionally when leaving via exception.
        var faultRegion = method.ExceptionRegions.FirstOrDefault(candidate =>
            candidate.Kind == IRExceptionRegionKind.Fault &&
            candidate.TryBlockIds.Contains(blockId));
        if (faultRegion is not null)
        {
            handlerOffset = ResolveBlockOffset(
                new IROperand
                {
                    Kind = IROperandKind.BasicBlock,
                    TypeTag = IRTypeTag.Void,
                    BasicBlockId = faultRegion.HandlerBlockId,
                },
                blockOffsets);
            return true;
        }

        handlerOffset = default;
        return false;
    }

    private static void RethrowActiveException(Exception? activeException)
    {
        if (activeException is null)
        {
            throw new InvalidOperationException("Rethrow requires an active exception.");
        }

        throw activeException;
    }

    private static void WriteBinary(
        IDictionary<string, object?> values,
        IRInstruction instruction,
        IReadOnlyList<object?> arguments,
        IReadOnlyDictionary<string, object?> state,
        Func<int, int, int> operation)
    {
        var left = ReadInt32Operand(RequireOperand(instruction, 0), arguments, state);
        var right = ReadInt32Operand(RequireOperand(instruction, 1), arguments, state);
        WriteResult(values, instruction.Result, operation(left, right));
    }

    private static int ResolveBlockOffset(IROperand operand, IReadOnlyDictionary<int, int> blockOffsets)
    {
        if (operand.BasicBlockId is int blockId && blockOffsets.TryGetValue(blockId, out var offset))
        {
            return offset;
        }

        throw new InvalidOperationException("IR branch target is missing BasicBlockId.");
    }

    private static void ThrowManagedException(IRInstruction instruction)
    {
        var operand = instruction.Operands.Count == 0 ? null : instruction.Operands[0];
        var message = operand?.Kind switch
        {
            IROperandKind.StringLiteral => operand.StringValue ?? "interpreter throw",
            IROperandKind.Int32Literal => (operand.Int32Value ?? 0).ToString(CultureInfo.InvariantCulture),
            IROperandKind.NullLiteral or null => "interpreter throw",
            _ => operand.Symbol ?? $"unsupported throw operand: {operand.Kind}",
        };

        throw new InvalidOperationException(message);
    }

    private static void WriteResult(IDictionary<string, object?> values, IROperand? result, object? value)
    {
        if (result is null)
        {
            return;
        }

        values[RequireSymbol(result)] = value;
    }

    private static int ReadInt32Literal(IROperand operand)
    {
        return operand.Int32Value
            ?? throw new InvalidOperationException("Int32 literal operand is missing value.");
    }

    private static int ReadInt32Operand(
        IROperand operand,
        IReadOnlyList<object?> arguments,
        IReadOnlyDictionary<string, object?> values)
    {
        var value = ReadOperandValue(operand, arguments, values);
        return value switch
        {
            int int32 => int32,
            null => throw new InvalidOperationException("operand value is null and cannot be converted to Int32."),
            _ => throw new InvalidOperationException($"operand kind '{operand.Kind}' produced non-Int32 value '{value}'."),
        };
    }

    private static object? ReadOperandValue(
        IROperand operand,
        IReadOnlyList<object?> arguments,
        IReadOnlyDictionary<string, object?> values)
    {
        return operand.Kind switch
        {
            IROperandKind.Int32Literal => ReadInt32Literal(operand),
            IROperandKind.StringLiteral => operand.StringValue,
            IROperandKind.NullLiteral => null,
            IROperandKind.Argument => ResolveArgumentValue(operand, arguments),
            IROperandKind.Local => ResolveLocalValue(operand, values),
            _ => throw new NotSupportedException($"unsupported managed interpreter operand kind: {operand.Kind}"),
        };
    }

    private static object? ResolveArgumentValue(IROperand operand, IReadOnlyList<object?> arguments)
    {
        var index = ParseIndexedSymbol(RequireSymbol(operand), "arg");
        if (index < 0 || index >= arguments.Count)
        {
            throw new InvalidOperationException($"argument index '{index}' is out of range.");
        }

        return arguments[index];
    }

    private static object? ResolveLocalValue(IROperand operand, IReadOnlyDictionary<string, object?> values)
    {
        var symbol = RequireSymbol(operand);
        return values.TryGetValue(symbol, out var value) ? value : 0;
    }

    private static IROperand RequireOperand(IRInstruction instruction, int index)
    {
        if (index < 0 || index >= instruction.Operands.Count)
        {
            throw new InvalidOperationException($"instruction '{instruction.OpCode}' requires operand[{index}].");
        }

        return instruction.Operands[index];
    }

    private static string RequireSymbol(IROperand operand)
    {
        if (!string.IsNullOrWhiteSpace(operand.Symbol))
        {
            return operand.Symbol;
        }

        throw new InvalidOperationException($"operand '{operand.Kind}' is missing symbol.");
    }

    private static int ParseIndexedSymbol(string symbol, string prefix)
    {
        if (!symbol.StartsWith(prefix, StringComparison.Ordinal))
        {
            throw new InvalidOperationException($"symbol '{symbol}' does not start with '{prefix}'.");
        }

        return int.Parse(symbol[prefix.Length..], CultureInfo.InvariantCulture);
    }
}
