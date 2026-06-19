using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{





    private static bool IsRuntimeSkeletonDirectPrimitiveValuePreservingReturn(
        string inputManagedType,
        string outputManagedType)
    {
        if (string.Equals(inputManagedType, outputManagedType, StringComparison.Ordinal))
        {
            return true;
        }

        if ((string.Equals(inputManagedType, "System.UInt16", StringComparison.Ordinal) &&
             string.Equals(outputManagedType, "System.Char", StringComparison.Ordinal)) ||
            (string.Equals(inputManagedType, "System.Char", StringComparison.Ordinal) &&
             string.Equals(outputManagedType, "System.UInt16", StringComparison.Ordinal)))
        {
            return true;
        }

        return string.Equals(inputManagedType, "System.Byte", StringComparison.Ordinal) &&
               (string.Equals(outputManagedType, "System.Char", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.UInt16", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Int16", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Int32", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.UInt32", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Int64", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.UInt64", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Single", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Double", StringComparison.Ordinal));
    }







    private static bool TryMatchRuntimeSkeletonPrimitiveConvRetInstructionShape(
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        string inputManagedType,
        string outputManagedType)
    {
        if (!TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out _) ||
            !TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(outputManagedType, out _))
        {
            return false;
        }

        if (instructions.Count == 3 &&
            string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[0]) == 0 &&
            string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
        {
            return IsExpectedPrimitiveConvOp(instructions[1].Op, inputManagedType, outputManagedType);
        }

        return instructions.Count == 4 &&
               string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
               GetRequiredOperandInt(instructions[0]) == 0 &&
               string.Equals(instructions[1].Op, "conv.r.un", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal) &&
               IsExpectedPrimitiveConvOp(instructions[2].Op, inputManagedType, outputManagedType) &&
               (string.Equals(inputManagedType, "System.UInt32", StringComparison.Ordinal) ||
                string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal)) &&
               (string.Equals(outputManagedType, "System.Single", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Double", StringComparison.Ordinal));
    }







    private static bool IsExpectedPrimitiveConvOp(
        string op,
        string inputManagedType,
        string outputManagedType)
    {
        if (string.Equals(inputManagedType, outputManagedType, StringComparison.Ordinal))
        {
            return false;
        }

        if (string.Equals(outputManagedType, "System.Byte", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.u1", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.SByte", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.i1", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.Int16", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.i2", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.UInt16", StringComparison.Ordinal) ||
            string.Equals(outputManagedType, "System.Char", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.u2", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.Int32", StringComparison.Ordinal) ||
            string.Equals(outputManagedType, "System.UInt32", StringComparison.Ordinal))
        {
            return false;
        }

        if (string.Equals(outputManagedType, "System.Int64", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.i8", StringComparison.Ordinal) ||
                   string.Equals(op, "conv.u8", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.UInt64", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.i8", StringComparison.Ordinal) ||
                   string.Equals(op, "conv.u8", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.Single", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.r4", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.Double", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.r8", StringComparison.Ordinal);
        }

        return false;
    }







    private static bool IsZeroLiteralOperand(TypedIlInstructionArtifact instruction)
    {
        return instruction.Operand switch
        {
            int value => value == 0,
            long value => value == 0L,
            float value => value == 0f,
            double value => value == 0d,
            JsonElement element when element.ValueKind == JsonValueKind.Number => element.GetDouble() == 0d,
            _ => false,
        };
    }







    private static bool TryCreateAsyncTaskFamilyShape(string taskReturnType, out AsyncTaskFamilyShape familyShape)
    {
        familyShape = null!;
        if (!TryParseClosedTaskResultType(taskReturnType, out var managedResultType) ||
            !TryResolveAsyncManagedTypeShape(managedResultType, out var resultTypeShape))
        {
            return false;
        }

        familyShape = new AsyncTaskFamilyShape(managedResultType, resultTypeShape);
        return true;
    }







    private static bool TryResolveAsyncTaskFactoryMethodShape(
        TypedIlMethodArtifact method,
        out AsyncTaskFactoryMethodShape methodShape)
    {
        methodShape = null!;
        if (method.Parameters.Count > 2 ||
            !TryCreateAsyncTaskFamilyShape(GetMethodReturnType(method.SubjectId), out var familyShape))
        {
            return false;
        }

        var parameterTypeShapes = new List<AsyncManagedTypeShape>(method.Parameters.Count);
        foreach (var parameter in method.Parameters)
        {
            if (!TryResolveAsyncManagedTypeShape(parameter.Type, out var parameterTypeShape))
            {
                return false;
            }

            parameterTypeShapes.Add(parameterTypeShape);
        }

        methodShape = new AsyncTaskFactoryMethodShape(familyShape, parameterTypeShapes);
        return true;
    }







    private static bool ResolveAsyncLiteralParameterShapes(
        IReadOnlyList<TypedIlParameterArtifact> producerParameters,
        out IReadOnlyList<AsyncManagedTypeShape> literalParameterShapes)
    {
        var resolvedShapes = new List<AsyncManagedTypeShape>(producerParameters.Count);
        foreach (var parameter in producerParameters)
        {
            if (!TryResolveAsyncManagedTypeShape(parameter.Type, out var parameterTypeShape) ||
                !parameterTypeShape.SupportsLiteralLoading)
            {
                literalParameterShapes = [];
                return false;
            }

            resolvedShapes.Add(parameterTypeShape);
        }

        literalParameterShapes = resolvedShapes;
        return true;
    }







    private static bool TryResolveAsyncGetResultWrapperShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        out AsyncGetResultWrapperShape wrapperShape)
    {
        wrapperShape = null!;
        if (!TryResolveAsyncManagedTypeShape(GetMethodReturnType(method.SubjectId), out var wrapperResultTypeShape))
        {
            return false;
        }

        for (var callInstructionIndex = 0; callInstructionIndex <= instructions.Count - 6; callInstructionIndex++)
        {
            if (!string.Equals(instructions[callInstructionIndex].Op, "call", StringComparison.Ordinal) ||
                string.IsNullOrWhiteSpace(instructions[callInstructionIndex].Callee))
            {
                continue;
            }

            var producedTaskMethod = instructions[callInstructionIndex].Callee!;
            if (string.Equals(producedTaskMethod, method.SubjectId, StringComparison.Ordinal) ||
                !methodsBySubjectId.TryGetValue(producedTaskMethod, out var producerMethod) ||
                !TryResolveAsyncTaskFactoryMethodShape(producerMethod, out var producerMethodShape) ||
                !ResolveAsyncLiteralParameterShapes(producerMethod.Parameters, out var expectedLiteralParameterShapes))
            {
                continue;
            }

            if (!string.Equals(producerMethodShape.Family.ManagedResultType, wrapperResultTypeShape.ManagedType, StringComparison.Ordinal) ||
                callInstructionIndex != expectedLiteralParameterShapes.Count ||
                instructions.Count != expectedLiteralParameterShapes.Count + 6)
            {
                continue;
            }

            var matchesLiteralPrefix = true;
            for (var instructionIndex = 0; instructionIndex < expectedLiteralParameterShapes.Count; instructionIndex++)
            {
                if (!string.Equals(
                        instructions[instructionIndex].Op,
                        expectedLiteralParameterShapes[instructionIndex].LiteralOp,
                        StringComparison.Ordinal))
                {
                    matchesLiteralPrefix = false;
                    break;
                }
            }

            if (!matchesLiteralPrefix)
            {
                continue;
            }

            wrapperShape = new AsyncGetResultWrapperShape(producerMethodShape.Family, expectedLiteralParameterShapes);
            return true;
        }

        return false;
    }







    private static void ValidateAsyncTaskFactoryShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        AsyncTaskFactoryMethodShape methodShape)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");

        var argc = methodShape.ParameterTypeShapes.Count;
        if (argc > 2)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async {methodShape.Family.DisplayName} factory '{method.SubjectId}' to take no more than two supported arguments");
        }

        var expectedInstructionCount = 14 + (argc * 3);
        RequireInstructionCount(method, instructions, expectedInstructionCount);
        RequireInstructionOp(instructions[0], "ldloca", method.SubjectId, 0);
        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to initialize local state machine slot 0");
        }

        RequireInstructionOp(instructions[1], "call", method.SubjectId, 1);
        RequireInstructionCallee(
            instructions[1],
            methodShape.Family.BuilderCreateCallee,
            method.SubjectId,
            1);
        RequireInstructionOp(instructions[2], "stfld", method.SubjectId, 2);

        var instructionIndex = 3;
        for (var argIndex = 0; argIndex < argc; argIndex++)
        {
            RequireInstructionOp(instructions[instructionIndex], "ldloca", method.SubjectId, instructionIndex);
            if (GetRequiredOperandInt(instructions[instructionIndex]) != 0)
            {
                throw new InvalidOperationException(
                    $"native-reference emitter expects '{method.SubjectId}' to reload local state machine slot 0");
            }

            RequireInstructionOp(instructions[instructionIndex + 1], "ldarg", method.SubjectId, instructionIndex + 1);
            if (GetRequiredOperandInt(instructions[instructionIndex + 1]) != argIndex)
            {
                throw new InvalidOperationException(
                    $"native-reference emitter expects '{method.SubjectId}' to store async {methodShape.Family.DisplayName} wrapper argument {argIndex}");
            }

            RequireInstructionOp(instructions[instructionIndex + 2], "stfld", method.SubjectId, instructionIndex + 2);
            instructionIndex += 3;
        }

        RequireInstructionOp(instructions[instructionIndex], "ldloca", method.SubjectId, instructionIndex);
        RequireInstructionOp(instructions[instructionIndex + 1], "ldc.i4", method.SubjectId, instructionIndex + 1);
        RequireInstructionOp(instructions[instructionIndex + 2], "stfld", method.SubjectId, instructionIndex + 2);
        if (GetRequiredOperandInt(instructions[instructionIndex]) != 0 ||
            GetRequiredOperandInt(instructions[instructionIndex + 1]) != -1)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to set async state to -1 on local slot 0");
        }

        RequireInstructionOp(instructions[instructionIndex + 3], "ldloca", method.SubjectId, instructionIndex + 3);
        RequireInstructionOp(instructions[instructionIndex + 4], "ldflda", method.SubjectId, instructionIndex + 4);
        RequireInstructionOp(instructions[instructionIndex + 5], "ldloca", method.SubjectId, instructionIndex + 5);
        RequireInstructionOp(instructions[instructionIndex + 6], "call", method.SubjectId, instructionIndex + 6);
        RequireInstructionOp(instructions[instructionIndex + 7], "ldloca", method.SubjectId, instructionIndex + 7);
        RequireInstructionOp(instructions[instructionIndex + 8], "ldflda", method.SubjectId, instructionIndex + 8);
        RequireInstructionOp(instructions[instructionIndex + 9], "call", method.SubjectId, instructionIndex + 9);
        RequireInstructionOp(instructions[instructionIndex + 10], "ret", method.SubjectId, instructionIndex + 10);

        if (GetRequiredOperandInt(instructions[instructionIndex + 3]) != 0 ||
            GetRequiredOperandInt(instructions[instructionIndex + 5]) != 0 ||
            GetRequiredOperandInt(instructions[instructionIndex + 7]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to use local async state machine slot 0");
        }

        RequireInstructionCalleePrefix(
            instructions[instructionIndex + 6],
            methodShape.Family.BuilderStartPrefix,
            method.SubjectId,
            instructionIndex + 6);
        RequireInstructionCallee(
            instructions[instructionIndex + 9],
            methodShape.Family.BuilderGetTaskCallee,
            method.SubjectId,
            instructionIndex + 9);
    }







    private static AsyncGetResultWrapperShape ValidateAsyncGetResultShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        if (method.Parameters.Count != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async get-result wrapper '{method.SubjectId}' to be parameterless");
        }

        if (!TryResolveAsyncGetResultWrapperShape(method, instructions, methodsBySubjectId, out var wrapperShape))
        {
            var literalExpectation = "supported literal arguments";
            if (instructions.Count >= 6 &&
                !string.IsNullOrWhiteSpace(instructions[0].Callee) &&
                methodsBySubjectId.TryGetValue(instructions[0].Callee!, out var producerMethod) &&
                ResolveAsyncLiteralParameterShapes(producerMethod.Parameters, out var literalParameterShapes))
            {
                literalExpectation = literalParameterShapes.Count == 0
                    ? "no literal arguments"
                    : string.Join(", ", literalParameterShapes.Select(shape => shape.LiteralExpectation));
            }

            throw new InvalidOperationException(
                $"native-reference emitter expects async get-result wrapper '{method.SubjectId}' to load {literalExpectation} before invoking the async producer");
        }

        var producerFamilyShape = wrapperShape.ProducerFamilyShape;
        var expectedLiteralParameterShapes = wrapperShape.ExpectedLiteralParameterShapes;
        var callInstructionIndex = expectedLiteralParameterShapes.Count;
        RequireInstructionOp(instructions[callInstructionIndex], "call", method.SubjectId, callInstructionIndex);
        RequireInstructionOp(instructions[callInstructionIndex + 1], "call", "callvirt", method.SubjectId, callInstructionIndex + 1);
        RequireInstructionOp(instructions[callInstructionIndex + 2], "stloc", method.SubjectId, callInstructionIndex + 2);
        RequireInstructionOp(instructions[callInstructionIndex + 3], "ldloca", method.SubjectId, callInstructionIndex + 3);
        RequireInstructionOp(instructions[callInstructionIndex + 4], "call", method.SubjectId, callInstructionIndex + 4);
        RequireInstructionOp(instructions[callInstructionIndex + 5], "ret", method.SubjectId, callInstructionIndex + 5);

        var producedTaskMethod = GetRequiredInstructionCallee(instructions[callInstructionIndex], method.SubjectId, callInstructionIndex);
        if (string.Equals(producedTaskMethod, method.SubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to consume an async producer, not recurse into itself");
        }

        RequireInstructionCallee(
            instructions[callInstructionIndex + 1],
            producerFamilyShape.TaskGetAwaiterCallee,
            method.SubjectId,
            callInstructionIndex + 1);
        RequireInstructionCallee(
            instructions[callInstructionIndex + 4],
            producerFamilyShape.AwaiterGetResultCallee,
            method.SubjectId,
            callInstructionIndex + 4);
        return wrapperShape;
    }







    private static void ValidateAsyncStateMachineMoveNextShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-method", "has-canonical-body");
        if (instructions.Count < 20)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to contain the canonical async body");
        }

        var callees = instructions
            .Where(instruction =>
                (string.Equals(instruction.Op, "call", StringComparison.Ordinal) ||
                 string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal)) &&
                !string.IsNullOrWhiteSpace(instruction.Callee))
            .Select(instruction => instruction.Callee!)
            .ToList();

        if (!callees.Contains("System.Private.CoreLib/System.Threading.Tasks.Task::Yield:System.Runtime.CompilerServices.YieldAwaitable()", StringComparer.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to call Task::Yield()");
        }

        if (!callees.Any(IsSupportedAsyncAwaitUnsafeOnCompletedCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to call AwaitUnsafeOnCompleted");
        }

        if (!callees.Any(IsSupportedAsyncSetResultCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to call SetResult");
        }

        if (!callees.Any(IsSupportedAsyncSetExceptionCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to call SetException");
        }

        if (!callees.Contains(
                "System.Private.CoreLib/System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter::GetResult:System.Void()",
                StringComparer.Ordinal) &&
            !callees.Any(IsSupportedAsyncTaskAwaiterGetResultCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to observe an awaiter GetResult call");
        }
    }







    private static void ValidateAsyncStateMachineSetStateMachineShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-method", "has-canonical-body");
        if (method.Parameters.Count != 1 ||
            !string.Equals(method.Parameters[0].Type, "System.Runtime.CompilerServices.IAsyncStateMachine", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine SetStateMachine '{method.SubjectId}' to take IAsyncStateMachine");
        }

        RequireInstructionCount(method, instructions, 5);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldflda", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ldarg", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "ret", method.SubjectId, 4);

        if (GetRequiredOperandInt(instructions[0]) != 0 ||
            GetRequiredOperandInt(instructions[2]) != 1)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to load ldarg 0 and ldarg 1");
        }

        var setStateMachineCallee = instructions[3].Callee;
        if (string.IsNullOrWhiteSpace(setStateMachineCallee) ||
            !IsSupportedAsyncSetStateMachineCallee(setStateMachineCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction 3 in '{method.SubjectId}' to call a supported AsyncTaskMethodBuilder<T>.SetStateMachine, but found '{setStateMachineCallee ?? "<null>"}'");
        }
    }







    private static void ValidateSingleArgumentForwarderShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-forwarder", "has-canonical-body");
        RequireInstructionCount(method, instructions, 2);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ret", method.SubjectId, 1);

        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to forward ldarg 0");
        }
    }







    private static void ValidateDelegateClosedTargetRelayEntryPointShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");

        if (!IsDelegateClosedTargetRelayEntryPointShape(instructions))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to follow delegate-closed-target-relay proof shape");
        }

        var writeLineInstructionIndex = GetDelegateClosedTargetRelayWriteLineInstructionIndex(instructions);
        if (!IsConsoleWriteLineStringTarget(GetRequiredInstructionCallee(instructions[writeLineInstructionIndex], method.SubjectId, writeLineInstructionIndex)))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to call Console.WriteLine(System.String)");
        }

        var returnValueInstructionIndex = GetDelegateClosedTargetRelayReturnValueInstructionIndex(instructions);
        if (GetRequiredOperandInt(instructions[returnValueInstructionIndex]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to return ldc.i4 0 before ret");
        }
    }







    private static bool IsDelegateClosedTargetRelayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "ldftn", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldsfld", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "brtrue", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "pop", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldnull", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldftn", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "stsfld", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ret", StringComparison.Ordinal);
    }







    private static int GetDelegateClosedTargetRelayStaticMethodInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 10
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }







    private static int GetDelegateClosedTargetRelayMessagePrefixInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 15
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }







    private static int GetDelegateClosedTargetRelayWriteLineInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 18
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }







    private static int GetDelegateClosedTargetRelayReturnValueInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 19
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }







    private static void ValidateDelegateClosedTargetRelayStaticTailShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        RequireInstructionCount(method, instructions, 4);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldstr", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "call", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "ret", method.SubjectId, 3);

        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to load ldarg 0 before appending a trailing literal");
        }

        var concatIcall = NormalizeStringConcatIcall(GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2));
        if (!string.Equals(concatIcall, StringConcatPairIcall, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects delegate static tail '{method.SubjectId}' to use pair string concat");
        }
    }







    private static string GetDelegateClosedTargetRelayTrailingLiteral(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        ValidateDelegateClosedTargetRelayStaticTailShape(method, instructions);
        return GetRequiredOperandString(instructions[1]);
    }







    private static void ValidateStaticExceptionThrowLiteralShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.Void", StringComparison.Ordinal) ||
            method.Parameters.Count != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects static exception throw literal '{method.SubjectId}' to be parameterless and return void");
        }

        RequireInstructionCount(method, instructions, 3);
        RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "newobj", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "throw", method.SubjectId, 2);
        RequireInstructionCallee(
            instructions[1],
            "System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)",
            method.SubjectId,
            1);
    }







    private static void ValidateStaticExceptionThrowStringProducerShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.Void", StringComparison.Ordinal) ||
            method.Parameters.Count != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects static exception throw string producer '{method.SubjectId}' to be parameterless and return void");
        }

        RequireInstructionCount(method, instructions, 3);
        RequireInstructionOp(instructions[0], "call", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "newobj", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "throw", method.SubjectId, 2);

        var exceptionConstructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (!exceptionConstructorSubjectId.EndsWith("::.ctor:System.Void(System.String)", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects static exception throw string producer '{method.SubjectId}' to construct a string-taking exception, but found '{exceptionConstructorSubjectId}'");
        }
    }







    private static void ValidateStaticExceptionCatchStringReturnShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal) ||
            method.Parameters.Count != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects static exception catch string return '{method.SubjectId}' to be parameterless and return string");
        }

        RequireInstructionCount(method, instructions, 10);
        RequireInstructionOp(instructions[0], "call", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldstr", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "stloc", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "leave", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "pop", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ldstr", method.SubjectId, 5);
        RequireInstructionOp(instructions[6], "stloc", method.SubjectId, 6);
        RequireInstructionOp(instructions[7], "leave", method.SubjectId, 7);
        RequireInstructionOp(instructions[8], "ldloc", method.SubjectId, 8);
        RequireInstructionOp(instructions[9], "ret", method.SubjectId, 9);
    }







    private static void ValidateFieldGetterShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-field-getter", "has-canonical-body");
        RequireInstructionCount(method, instructions, 3);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldfld", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ret", method.SubjectId, 2);

        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to start with ldarg 0");
        }
    }







    private static string GetCapturedFieldSubjectIdForSupportedStringInstanceMethod(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        try
        {
            ValidateFieldGetterShape(method, instructions);
            return GetRequiredOperandString(instructions[1]);
        }
        catch
        {
        }

        ValidateFieldBackedStringInstanceMethodShape(method, instructions);
        return GetRequiredOperandString(instructions[2]);
    }







    private static bool HasMethodContract(
        TypedIlMethodArtifact method,
        string expectedMethodRole,
        string expectedBodyAvailability)
    {
        return string.Equals(method.MethodRole, expectedMethodRole, StringComparison.Ordinal) &&
               string.Equals(method.BodyAvailability, expectedBodyAvailability, StringComparison.Ordinal);
    }







    private static bool IsRuntimeSkeletonNonExecutableDeclaration(TypedIlMethodArtifact method)
    {
        return string.Equals(method.BodyAvailability, "no-canonical-body", StringComparison.Ordinal) &&
               !string.Equals(method.MethodRole, "imported-method", StringComparison.Ordinal) &&
               method.Blocks.All(block => block.Instructions.Count == 0);
    }







    private static bool HasCapability(
        TypedIlMethodArtifact method,
        string requiredCapability)
    {
        return method.Capabilities.Contains(requiredCapability, StringComparer.Ordinal);
    }







    private static void RequireMethodContract(
        TypedIlMethodArtifact method,
        string expectedMethodRole,
        string expectedBodyAvailability)
    {
        if (!string.Equals(method.MethodRole, expectedMethodRole, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to have method role '{expectedMethodRole}', but found '{method.MethodRole}'");
        }

        if (!string.Equals(method.BodyAvailability, expectedBodyAvailability, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to have body availability '{expectedBodyAvailability}', but found '{method.BodyAvailability}'");
        }
    }







    private static void RequireCapability(
        TypedIlMethodArtifact method,
        string requiredCapability)
    {
        if (!method.Capabilities.Contains(requiredCapability, StringComparer.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to carry capability '{requiredCapability}'");
        }
    }







    private static void RequireInstructionCount(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        int expectedCount)
    {
        if (instructions.Count != expectedCount)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to have {expectedCount} instructions, but found {instructions.Count}");
        }
    }







    private static void RequireInstructionOp(
        TypedIlInstructionArtifact instruction,
        string expectedOp,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Op, expectedOp, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to be '{expectedOp}', but found '{instruction.Op}'");
        }
    }







    private static void RequireInstructionOp(
        TypedIlInstructionArtifact instruction,
        string expectedOp,
        string alternateExpectedOp,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Op, expectedOp, StringComparison.Ordinal) &&
            !string.Equals(instruction.Op, alternateExpectedOp, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to be '{expectedOp}' or '{alternateExpectedOp}', but found '{instruction.Op}'");
        }
    }







    private static void RequireInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string expectedCallee,
        string subjectId,
        int instructionIndex)
    {
        RequireInstructionCallee(instruction, expectedCallee, null, subjectId, instructionIndex);
    }







    private static void RequireInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string expectedCallee,
        string? alternateExpectedCallee,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Callee, expectedCallee, StringComparison.Ordinal) &&
            !string.Equals(instruction.Callee, alternateExpectedCallee, StringComparison.Ordinal))
        {
            var expectedDescription = alternateExpectedCallee is null
                ? $"'{expectedCallee}'"
                : $"'{expectedCallee}' or '{alternateExpectedCallee}'";
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to call {expectedDescription}, but found '{instruction.Callee ?? "<null>"}'");
        }
    }







    private static void RequireInstructionCalleePrefix(
        TypedIlInstructionArtifact instruction,
        string expectedPrefix,
        string subjectId,
        int instructionIndex)
    {
        if (string.IsNullOrWhiteSpace(instruction.Callee) ||
            !instruction.Callee.StartsWith(expectedPrefix, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to call a method starting with '{expectedPrefix}', but found '{instruction.Callee ?? "<null>"}'");
        }
    }

}
