using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Interpreter;

public sealed class ILToIRLowering
{
    public InterpreterIR Lower(IReadOnlyList<ManagedMethodModel> methods)
    {
        var internalAssemblyNames = methods
            .Select(method => method.AssemblyName)
            .ToHashSet(StringComparer.Ordinal);
        var methodsBySubjectId = methods
            .ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        return new InterpreterIR
        {
            Methods = methods.Select(method => Lower(method, internalAssemblyNames, methodsBySubjectId)).ToList(),
        };
    }

    public IRMethod Lower(ManagedMethodModel method)
    {
        return Lower(
            method,
            new HashSet<string>(StringComparer.Ordinal)
            {
                method.AssemblyName,
            },
            new Dictionary<string, ManagedMethodModel>(StringComparer.Ordinal)
            {
                [method.SubjectId] = method,
            });
    }

    private IRMethod Lower(
        ManagedMethodModel method,
        IReadOnlySet<string> internalAssemblyNames,
        IReadOnlyDictionary<string, ManagedMethodModel> methodsBySubjectId)
    {
        if (TryBuildOffsetBlocks(method, out var offsetBlocks, out var offsetToBlockIndex, out var blockIds))
        {
            var locals = new Dictionary<int, IRTypeTag>();
            var stack = new Stack<IROperand>();
            var temporaryIndex = 0;
            var blocks = offsetBlocks
                .Select((block, index) => LowerBlock(
                    method,
                    block.Instructions,
                    index,
                    blockIds,
                    offsetToBlockIndex,
                    internalAssemblyNames,
                    methodsBySubjectId,
                    locals,
                    stack,
                    ref temporaryIndex))
                .ToList();

            return new IRMethod
            {
                MethodId = ManagedNaming.CreateMethodId(method),
                SubjectId = method.SubjectId,
                Identity = ManagedMethodIdentityResolver.Create(method),
                BodyAvailabilityCode = BodyAvailabilityResolver.Resolve(method),
                Blocks = blocks,
                ExceptionRegions = LowerExceptionRegions(method, offsetBlocks, offsetToBlockIndex),
            };
        }

        var fallbackBlockIds = method.Body.Blocks
            .Select((block, index) => new { block.BlockId, Index = index })
            .ToDictionary(entry => entry.BlockId, entry => entry.Index, StringComparer.Ordinal);
        var fallbackLocals = new Dictionary<int, IRTypeTag>();
        var fallbackStack = new Stack<IROperand>();
        var fallbackTemporaryIndex = 0;
        var fallbackBlocks = method.Body.Blocks
            .Select((block, index) => LowerBlock(
                method,
                block.Instructions,
                index,
                fallbackBlockIds,
                new Dictionary<int, int>(),
                internalAssemblyNames,
                methodsBySubjectId,
                fallbackLocals,
                fallbackStack,
                ref fallbackTemporaryIndex))
            .ToList();

        return new IRMethod
        {
            MethodId = ManagedNaming.CreateMethodId(method),
            SubjectId = method.SubjectId,
            Identity = ManagedMethodIdentityResolver.Create(method),
            BodyAvailabilityCode = BodyAvailabilityResolver.Resolve(method),
            Blocks = fallbackBlocks,
        };
    }

    private static bool TryBuildOffsetBlocks(
        ManagedMethodModel method,
        out IReadOnlyList<OffsetBlock> blocks,
        out IReadOnlyDictionary<int, int> offsetToBlockIndex,
        out IReadOnlyDictionary<string, int> blockIds)
    {
        var instructions = method.Body.Blocks
            .SelectMany(static block => block.Instructions)
            .ToList();
        if (instructions.Count == 0 || instructions.Any(static instruction => instruction.IlOffset is null))
        {
            blocks = [];
            offsetToBlockIndex = new Dictionary<int, int>();
            blockIds = new Dictionary<string, int>(StringComparer.Ordinal);
            return false;
        }

        var orderedInstructions = instructions
            .Select(static instruction => new OffsetInstruction(GetRequiredIlOffset(instruction), instruction))
            .OrderBy(static entry => entry.Offset)
            .ToList();
        var instructionOffsets = orderedInstructions.Select(static entry => entry.Offset).ToHashSet();
        var blockStartOffsets = new SortedSet<int>
        {
            orderedInstructions[0].Offset,
        };

        for (var index = 0; index < orderedInstructions.Count; index++)
        {
            var instruction = orderedInstructions[index].Instruction;
            if (instruction.Operand is int targetOffset && IsBlockTargetingInstruction(instruction.Op))
            {
                blockStartOffsets.Add(targetOffset);
            }

            if (RequiresFallthroughBoundary(instruction.Op) && index + 1 < orderedInstructions.Count)
            {
                blockStartOffsets.Add(orderedInstructions[index + 1].Offset);
            }
        }

        foreach (var region in method.Body.ExceptionRegions)
        {
            blockStartOffsets.Add(region.TryOffset);
            blockStartOffsets.Add(region.HandlerOffset);

            var tryEndOffset = region.TryOffset + region.TryLength;
            if (instructionOffsets.Contains(tryEndOffset))
            {
                blockStartOffsets.Add(tryEndOffset);
            }

            var handlerEndOffset = region.HandlerOffset + region.HandlerLength;
            if (instructionOffsets.Contains(handlerEndOffset))
            {
                blockStartOffsets.Add(handlerEndOffset);
            }
        }

        var loweredBlocks = new List<OffsetBlock>();
        var currentInstructions = new List<ManagedInstructionModel>();
        var currentStartOffset = orderedInstructions[0].Offset;

        foreach (var entry in orderedInstructions)
        {
            if (currentInstructions.Count > 0 && blockStartOffsets.Contains(entry.Offset))
            {
                loweredBlocks.Add(new OffsetBlock(CreateIlBlockId(currentStartOffset), currentStartOffset, currentInstructions.ToList()));
                currentInstructions.Clear();
                currentStartOffset = entry.Offset;
            }

            currentInstructions.Add(entry.Instruction);
        }

        if (currentInstructions.Count > 0)
        {
            loweredBlocks.Add(new OffsetBlock(CreateIlBlockId(currentStartOffset), currentStartOffset, currentInstructions.ToList()));
        }

        blocks = loweredBlocks;
        offsetToBlockIndex = loweredBlocks
            .Select((block, index) => new { block.StartOffset, Index = index })
            .ToDictionary(entry => entry.StartOffset, entry => entry.Index);
        blockIds = loweredBlocks
            .Select((block, index) => new { block.BlockId, Index = index })
            .ToDictionary(entry => entry.BlockId, entry => entry.Index, StringComparer.Ordinal);
        return true;
    }

    private static bool IsBlockTargetingInstruction(string op)
    {
        return op switch
        {
            "br" or "brtrue" or "brfalse" or "blt" or "bgt" or "ble" or "bge" or "leave" => true,
            _ => false,
        };
    }

    private static bool RequiresFallthroughBoundary(string op)
    {
        return op switch
        {
            "brtrue" or "brfalse" or "blt" or "bgt" or "ble" or "bge" => true,
            _ => false,
        };
    }

    private static string CreateIlBlockId(int offset)
    {
        return $"IL_{offset:X4}";
    }

    private static IReadOnlyList<IRExceptionRegion> LowerExceptionRegions(
        ManagedMethodModel method,
        IReadOnlyList<OffsetBlock> blocks,
        IReadOnlyDictionary<int, int> offsetToBlockIndex)
    {
        var regions = new List<IRExceptionRegion>();

        foreach (var region in method.Body.ExceptionRegions)
        {
            if (!TryMapExceptionRegionKind(region.HandlingKind, out var kind))
            {
                continue;
            }

            if (!offsetToBlockIndex.TryGetValue(region.HandlerOffset, out var handlerBlockId))
            {
                continue;
            }

            var tryBlockIds = blocks
                .Where(block => block.StartOffset >= region.TryOffset && block.StartOffset < region.TryOffset + region.TryLength)
                .Select(block => offsetToBlockIndex[block.StartOffset])
                .ToList();
            if (tryBlockIds.Count == 0)
            {
                continue;
            }

            regions.Add(new IRExceptionRegion
            {
                Kind = kind,
                TryBlockIds = tryBlockIds,
                HandlerBlockId = handlerBlockId,
            });
        }

        return regions;
    }

    private static bool TryMapExceptionRegionKind(string handlingKind, out IRExceptionRegionKind kind)
    {
        switch (handlingKind)
        {
            case "catch":
                kind = IRExceptionRegionKind.Catch;
                return true;
            case "finally":
                kind = IRExceptionRegionKind.Finally;
                return true;
            default:
                kind = default;
                return false;
        }
    }

    private static IRBasicBlock LowerBlock(
        ManagedMethodModel method,
        IReadOnlyList<ManagedInstructionModel> instructions,
        int blockIndex,
        IReadOnlyDictionary<string, int> blockIds,
        IReadOnlyDictionary<int, int> offsetToBlockIndex,
        IReadOnlySet<string> internalAssemblyNames,
        IReadOnlyDictionary<string, ManagedMethodModel> methodsBySubjectId,
        IDictionary<int, IRTypeTag> locals,
        Stack<IROperand> stack,
        ref int temporaryIndex)
    {
        stack.Clear();
        var loweredInstructions = new List<IRInstruction>(instructions.Count);

        foreach (var instruction in instructions)
        {
            loweredInstructions.Add(LowerInstruction(
                method,
                instruction,
                blockIds,
                offsetToBlockIndex,
                internalAssemblyNames,
                methodsBySubjectId,
                method.Body.ExceptionRegions,
                locals,
                stack,
                ref temporaryIndex));
        }

        return new IRBasicBlock
        {
            BlockId = blockIndex,
            Instructions = loweredInstructions,
        };
    }

    private static IRInstruction LowerInstruction(
        ManagedMethodModel method,
        ManagedInstructionModel instruction,
        IReadOnlyDictionary<string, int> blockIds,
        IReadOnlyDictionary<int, int> offsetToBlockIndex,
        IReadOnlySet<string> internalAssemblyNames,
        IReadOnlyDictionary<string, ManagedMethodModel> methodsBySubjectId,
        IReadOnlyList<ManagedExceptionRegionModel> exceptionRegions,
        IDictionary<int, IRTypeTag> locals,
        Stack<IROperand> stack,
        ref int temporaryIndex)
    {
        return instruction.Op switch
        {
            "ldarg" => LowerLdarg(method, instruction, stack, ref temporaryIndex),
            "ldloc" => LowerLdloc(instruction, locals, stack, ref temporaryIndex),
            "stloc" => LowerStloc(instruction, locals, stack),
            "ldc.i4" => LowerLdcI4(instruction, stack, ref temporaryIndex),
            "ldstr" => LowerLdstr(instruction, stack, ref temporaryIndex),
            "ldnull" => LowerLdnull(stack, ref temporaryIndex),
            "pop" => new IRInstruction { OpCode = IROpCode.Pop },
            "add" => LowerBinaryNumeric(IROpCode.Add, instruction, stack, ref temporaryIndex),
            "sub" => LowerBinaryNumeric(IROpCode.Sub, instruction, stack, ref temporaryIndex),
            "mul" => LowerBinaryNumeric(IROpCode.Mul, instruction, stack, ref temporaryIndex),
            "div" => LowerBinaryNumeric(IROpCode.Div, instruction, stack, ref temporaryIndex),
            "rem" => LowerBinaryNumeric(IROpCode.Rem, instruction, stack, ref temporaryIndex),
            "call" => LowerMethodCall(method, instruction, internalAssemblyNames, methodsBySubjectId, stack, ref temporaryIndex),
            "callvirt" => LowerMethodCall(method, instruction, internalAssemblyNames, methodsBySubjectId, stack, ref temporaryIndex),
            "ceq" => LowerBinaryNumeric(IROpCode.Ceq, instruction, stack, ref temporaryIndex),
            "clt" => LowerBinaryNumeric(IROpCode.Clt, instruction, stack, ref temporaryIndex),
            "cgt" => LowerBinaryNumeric(IROpCode.Cgt, instruction, stack, ref temporaryIndex),
            "br" => LowerBranch(IROpCode.Br, instruction, blockIds, offsetToBlockIndex),
            "brtrue" => LowerConditionalBranch(IROpCode.BrTrue, instruction, blockIds, offsetToBlockIndex, stack),
            "brfalse" => LowerConditionalBranch(IROpCode.BrFalse, instruction, blockIds, offsetToBlockIndex, stack),
            "blt" => LowerRelationalBranch(IROpCode.Blt, instruction, blockIds, offsetToBlockIndex, stack),
            "bgt" => LowerRelationalBranch(IROpCode.Bgt, instruction, blockIds, offsetToBlockIndex, stack),
            "ble" => LowerRelationalBranch(IROpCode.Ble, instruction, blockIds, offsetToBlockIndex, stack),
            "bge" => LowerRelationalBranch(IROpCode.Bge, instruction, blockIds, offsetToBlockIndex, stack),
            "leave" => LowerLeave(instruction, blockIds, offsetToBlockIndex, exceptionRegions),
            "endfinally" => new IRInstruction { OpCode = IROpCode.EndFinally },
            "rethrow" => new IRInstruction { OpCode = IROpCode.Rethrow },
            "ret" => LowerRet(method, stack),
            _ => throw new NotSupportedException($"unsupported lowering opcode: {instruction.Op}"),
        };
    }

    private static IRInstruction LowerLdarg(
        ManagedMethodModel method,
        ManagedInstructionModel instruction,
        Stack<IROperand> stack,
        ref int temporaryIndex)
    {
        var index = GetRequiredInt32Operand(instruction);
        var sourceType = ResolveArgumentType(method, index);
        var source = CreateArgumentOperand(index, sourceType);
        var result = CreateTemporaryOperand(ref temporaryIndex, sourceType);
        stack.Push(result);

        return new IRInstruction
        {
            OpCode = IROpCode.LdArg,
            Operands = [source],
            Result = result,
        };
    }

    private static IRInstruction LowerLdloc(
        ManagedInstructionModel instruction,
        IDictionary<int, IRTypeTag> locals,
        Stack<IROperand> stack,
        ref int temporaryIndex)
    {
        var index = GetRequiredInt32Operand(instruction);
        var sourceType = locals.TryGetValue(index, out var existingType)
            ? existingType
            : MapTypeTag(instruction.ResultType);
        var source = CreateLocalOperand(index, sourceType);
        var result = CreateTemporaryOperand(ref temporaryIndex, sourceType);
        stack.Push(result);

        return new IRInstruction
        {
            OpCode = IROpCode.LdLoc,
            Operands = [source],
            Result = result,
        };
    }

    private static IRInstruction LowerStloc(
        ManagedInstructionModel instruction,
        IDictionary<int, IRTypeTag> locals,
        Stack<IROperand> stack)
    {
        var index = GetRequiredInt32Operand(instruction);
        var value = Pop(stack, instruction.Op);
        locals[index] = value.TypeTag;
        var target = CreateLocalOperand(index, value.TypeTag);

        return new IRInstruction
        {
            OpCode = IROpCode.StLoc,
            Operands = [value, target],
        };
    }

    private static IRInstruction LowerLdcI4(
        ManagedInstructionModel instruction,
        Stack<IROperand> stack,
        ref int temporaryIndex)
    {
        var constant = new IROperand
        {
            Kind = IROperandKind.Int32Literal,
            TypeTag = IRTypeTag.Int32,
            Int32Value = GetRequiredInt32Operand(instruction),
        };
        var result = CreateTemporaryOperand(ref temporaryIndex, IRTypeTag.Int32);
        stack.Push(result);

        return new IRInstruction
        {
            OpCode = IROpCode.LdcI4,
            Operands = [constant],
            Result = result,
        };
    }

    private static IRInstruction LowerLdstr(
        ManagedInstructionModel instruction,
        Stack<IROperand> stack,
        ref int temporaryIndex)
    {
        var constant = new IROperand
        {
            Kind = IROperandKind.StringLiteral,
            TypeTag = IRTypeTag.String,
            StringValue = instruction.Operand?.ToString(),
        };
        var result = CreateTemporaryOperand(ref temporaryIndex, IRTypeTag.String);
        stack.Push(result);

        return new IRInstruction
        {
            OpCode = IROpCode.LdStr,
            Operands = [constant],
            Result = result,
        };
    }

    private static IRInstruction LowerLdnull(
        Stack<IROperand> stack,
        ref int temporaryIndex)
    {
        var constant = new IROperand
        {
            Kind = IROperandKind.NullLiteral,
            TypeTag = IRTypeTag.Null,
        };
        var result = CreateTemporaryOperand(ref temporaryIndex, IRTypeTag.Null);
        stack.Push(result);

        return new IRInstruction
        {
            OpCode = IROpCode.LdNull,
            Operands = [constant],
            Result = result,
        };
    }

    private static IRInstruction LowerBinaryNumeric(
        IROpCode opCode,
        ManagedInstructionModel instruction,
        Stack<IROperand> stack,
        ref int temporaryIndex)
    {
        var right = Pop(stack, instruction.Op);
        var left = Pop(stack, instruction.Op);
        var resultType = MapTypeTag(instruction.ResultType);
        var result = CreateTemporaryOperand(ref temporaryIndex, resultType);
        stack.Push(result);

        return new IRInstruction
        {
            OpCode = opCode,
            Operands = [left, right],
            Result = result,
        };
    }

    private static IRInstruction LowerMethodCall(
        ManagedMethodModel method,
        ManagedInstructionModel instruction,
        IReadOnlySet<string> internalAssemblyNames,
        IReadOnlyDictionary<string, ManagedMethodModel> methodsBySubjectId,
        Stack<IROperand> stack,
        ref int temporaryIndex)
    {
        var calleeSubjectId = instruction.Callee
            ?? instruction.Reference?.SubjectId
            ?? throw new InvalidOperationException($"instruction {instruction.Op} is missing callee subject id.");
        var inputCount = GetCallInputCount(instruction);
        var arguments = new IROperand[inputCount];
        for (var index = inputCount - 1; index >= 0; index--)
        {
            arguments[index] = Pop(stack, instruction.Op);
        }

        var dispatchKindCode = HybridDispatchResolver.ResolveInstruction(
            method.AssemblyName,
            internalAssemblyNames,
            instruction,
            methodsBySubjectId);
        var opCode = ResolveCallOpCode(instruction, dispatchKindCode);
        var resultType = MapTypeTag(instruction.ResultType);
        var operands = new List<IROperand>(capacity: arguments.Length + 1)
        {
            new()
            {
                Kind = IROperandKind.Method,
                TypeTag = resultType,
                Symbol = calleeSubjectId,
            },
        };
        operands.AddRange(arguments);

        IROperand? result = null;
        if (!string.Equals(instruction.ResultType, "System.Void", StringComparison.Ordinal))
        {
            result = CreateTemporaryOperand(ref temporaryIndex, resultType);
            stack.Push(result);
        }

        return new IRInstruction
        {
            OpCode = opCode,
            DispatchKindCode = dispatchKindCode,
            Operands = operands,
            Result = result,
        };
    }

    private static IRInstruction LowerBranch(
        IROpCode opCode,
        ManagedInstructionModel instruction,
        IReadOnlyDictionary<string, int> blockIds,
        IReadOnlyDictionary<int, int> offsetToBlockIndex)
    {
        return new IRInstruction
        {
            OpCode = opCode,
            Operands = [CreateBranchTargetOperand(instruction, blockIds, offsetToBlockIndex)],
        };
    }

    private static IRInstruction LowerConditionalBranch(
        IROpCode opCode,
        ManagedInstructionModel instruction,
        IReadOnlyDictionary<string, int> blockIds,
        IReadOnlyDictionary<int, int> offsetToBlockIndex,
        Stack<IROperand> stack)
    {
        var condition = Pop(stack, instruction.Op);
        var target = CreateBranchTargetOperand(instruction, blockIds, offsetToBlockIndex);

        return new IRInstruction
        {
            OpCode = opCode,
            Operands = [condition, target],
        };
    }

    private static IRInstruction LowerRelationalBranch(
        IROpCode opCode,
        ManagedInstructionModel instruction,
        IReadOnlyDictionary<string, int> blockIds,
        IReadOnlyDictionary<int, int> offsetToBlockIndex,
        Stack<IROperand> stack)
    {
        var right = Pop(stack, instruction.Op);
        var left = Pop(stack, instruction.Op);
        var target = CreateBranchTargetOperand(instruction, blockIds, offsetToBlockIndex);

        return new IRInstruction
        {
            OpCode = opCode,
            Operands = [left, right, target],
        };
    }

    private static IRInstruction LowerLeave(
        ManagedInstructionModel instruction,
        IReadOnlyDictionary<string, int> blockIds,
        IReadOnlyDictionary<int, int> offsetToBlockIndex,
        IReadOnlyList<ManagedExceptionRegionModel> exceptionRegions)
    {
        var target = CreateBranchTargetOperand(instruction, blockIds, offsetToBlockIndex);
        var instructionOffset = GetRequiredIlOffset(instruction);
        var targetOffset = GetRequiredInt32Operand(instruction);
        var finallyRegion = exceptionRegions
            .Where(region =>
                string.Equals(region.HandlingKind, "finally", StringComparison.Ordinal) &&
                RegionContainsOffset(region.TryOffset, region.TryLength, instructionOffset) &&
                !RegionContainsOffset(region.TryOffset, region.TryLength, targetOffset))
            .OrderBy(region => region.TryLength)
            .FirstOrDefault();

        if (finallyRegion is null)
        {
            return new IRInstruction
            {
                OpCode = IROpCode.Br,
                Operands = [target],
            };
        }

        return new IRInstruction
        {
            OpCode = IROpCode.Leave,
            Operands =
            [
                target,
                CreateBranchTargetOperand(finallyRegion.HandlerOffset, offsetToBlockIndex),
            ],
        };
    }

    private static bool RegionContainsOffset(int startOffset, int length, int offset)
    {
        return offset >= startOffset && offset < startOffset + length;
    }

    private static IRInstruction LowerRet(ManagedMethodModel method, Stack<IROperand> stack)
    {
        if (string.Equals(method.ReturnType, "System.Void", StringComparison.Ordinal))
        {
            return new IRInstruction
            {
                OpCode = IROpCode.Ret,
            };
        }

        return new IRInstruction
        {
            OpCode = IROpCode.Ret,
            Operands = [Pop(stack, "ret")],
        };
    }

    private static IROpCode ResolveCallOpCode(
        ManagedInstructionModel instruction,
        HybridDispatchKind? dispatchKindCode)
    {
        if (dispatchKindCode is HybridDispatchKind.Bridge or HybridDispatchKind.ExternalRuntime)
        {
            return IROpCode.CallBridge;
        }

        if (dispatchKindCode == HybridDispatchKind.Virtual ||
            string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal))
        {
            return IROpCode.CallVirt;
        }

        return IROpCode.Call;
    }

    private static int GetCallInputCount(ManagedInstructionModel instruction)
    {
        var calleeSubjectId = instruction.Callee
            ?? instruction.Reference?.SubjectId
            ?? throw new InvalidOperationException($"instruction {instruction.Op} is missing callee subject id.");
        var parameterCount = GetMethodParameterCount(calleeSubjectId);
        return string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal)
            ? parameterCount + 1
            : parameterCount;
    }

    private static int GetMethodParameterCount(string subjectId)
    {
        return ManagedMethodIdentityResolver.ResolveParameterCount(identity: null, fallbackSubjectId: subjectId);
    }

    private static IROperand CreateArgumentOperand(int index, IRTypeTag typeTag)
    {
        return new IROperand
        {
            Kind = IROperandKind.Argument,
            TypeTag = typeTag,
            Symbol = $"arg{index}",
        };
    }

    private static IROperand CreateLocalOperand(int index, IRTypeTag typeTag)
    {
        return new IROperand
        {
            Kind = IROperandKind.Local,
            TypeTag = typeTag,
            Symbol = $"local{index}",
        };
    }

    private static IROperand CreateTemporaryOperand(ref int temporaryIndex, IRTypeTag typeTag)
    {
        var symbol = $"stack{temporaryIndex++}";
        return new IROperand
        {
            Kind = IROperandKind.Local,
            TypeTag = typeTag,
            Symbol = symbol,
        };
    }

    private static IROperand CreateBranchTargetOperand(
        ManagedInstructionModel instruction,
        IReadOnlyDictionary<string, int> blockIds,
        IReadOnlyDictionary<int, int> offsetToBlockIndex)
    {
        if (instruction.Operand is string blockId && blockIds.TryGetValue(blockId, out var blockIndex))
        {
            return new IROperand
            {
                Kind = IROperandKind.BasicBlock,
                TypeTag = IRTypeTag.Void,
                Symbol = blockId,
                BasicBlockId = blockIndex,
            };
        }

        if (instruction.Operand is int targetOffset && offsetToBlockIndex.TryGetValue(targetOffset, out var targetBlockIndex))
        {
            return new IROperand
            {
                Kind = IROperandKind.BasicBlock,
                TypeTag = IRTypeTag.Void,
                Symbol = CreateIlBlockId(targetOffset),
                BasicBlockId = targetBlockIndex,
            };
        }

        return new IROperand
        {
            Kind = IROperandKind.BasicBlock,
            TypeTag = IRTypeTag.Void,
            Symbol = instruction.Operand?.ToString(),
            BasicBlockId = instruction.Operand as int? ?? -1,
        };
    }

    private static IROperand CreateBranchTargetOperand(
        int targetOffset,
        IReadOnlyDictionary<int, int> offsetToBlockIndex)
    {
        if (offsetToBlockIndex.TryGetValue(targetOffset, out var targetBlockIndex))
        {
            return new IROperand
            {
                Kind = IROperandKind.BasicBlock,
                TypeTag = IRTypeTag.Void,
                Symbol = CreateIlBlockId(targetOffset),
                BasicBlockId = targetBlockIndex,
            };
        }

        throw new InvalidOperationException($"missing block mapping for IL offset {targetOffset}.");
    }

    private static IROperand Pop(Stack<IROperand> stack, string op)
    {
        if (stack.Count == 0)
        {
            throw new InvalidOperationException($"evaluation stack underflow while lowering {op}");
        }

        return stack.Pop();
    }

    private static int GetRequiredInt32Operand(ManagedInstructionModel instruction)
    {
        return instruction.Operand switch
        {
            int value => value,
            sbyte value => value,
            byte value => value,
            short value => value,
            ushort value => value,
            _ => throw new InvalidOperationException($"instruction {instruction.Op} requires Int32 operand"),
        };
    }

    private static int GetRequiredIlOffset(ManagedInstructionModel instruction)
    {
        return instruction.IlOffset
            ?? throw new InvalidOperationException($"instruction {instruction.Op} is missing IlOffset.");
    }

    private static IRTypeTag ResolveArgumentType(ManagedMethodModel method, int index)
    {
        if (!method.IsStatic && index == 0)
        {
            return IRTypeTag.Object;
        }

        var parameterIndex = method.IsStatic ? index : index - 1;
        if (parameterIndex < 0 || parameterIndex >= method.Parameters.Count)
        {
            throw new InvalidOperationException($"invalid ldarg index {index} for method {method.SubjectId}");
        }

        return MapTypeTag(method.Parameters[parameterIndex].Type);
    }

    private static IRTypeTag MapTypeTag(string? value)
    {
        return value switch
        {
            null or "" => IRTypeTag.Void,
            "System.Void" => IRTypeTag.Void,
            "System.Int32" => IRTypeTag.Int32,
            "System.String" => IRTypeTag.String,
            "System.Boolean" => IRTypeTag.Boolean,
            "System.Object" => IRTypeTag.Object,
            _ when value.EndsWith("[]", StringComparison.Ordinal) => IRTypeTag.Array,
            _ when value.StartsWith("System.IntPtr", StringComparison.Ordinal) => IRTypeTag.NativeInt,
            _ => IRTypeTag.Object,
        };
    }

    private sealed record OffsetInstruction(int Offset, ManagedInstructionModel Instruction);

    private sealed record OffsetBlock(string BlockId, int StartOffset, IReadOnlyList<ManagedInstructionModel> Instructions);
}
