using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{

    private static void ValidateRuntimeSkeletonBoxedIConvertibleCharEntryShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        RequireInstructionCount(method, instructions, 5);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "box", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ldnull", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "callvirt", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "ret", method.SubjectId, 4);
        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects boxed iconvertible char entry '{method.SubjectId}' to load ldarg 0");
        }

        RequireInstructionCallee(
            instructions[3],
            "System.Private.CoreLib/System.IConvertible::ToChar:System.Char(System.IFormatProvider)",
            method.SubjectId,
            3);
    }



    private static void ValidateRuntimeSkeletonBoxedIConvertibleCharInvalidCastTargetShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-method", "has-canonical-body");
        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.Char", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(method.Parameters[0].Type, "System.IFormatProvider", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects boxed iconvertible char invalid-cast target '{method.SubjectId}' to be an instance method returning char with a single IFormatProvider parameter");
        }

        RequireInstructionCount(method, instructions, 6);
        RequireInstructionOp(instructions[0], "call", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldstr", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ldstr", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "newobj", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "throw", method.SubjectId, 5);
        RequireInstructionCallee(
            instructions[0],
            "System.Private.CoreLib/System.SR::get_InvalidCast_FromTo:System.String()",
            method.SubjectId,
            0);
        RequireInstructionCallee(
            instructions[3],
            "System.Private.CoreLib/System.SR::Format:System.String(System.String,System.Object,System.Object)",
            method.SubjectId,
            3);

        var exceptionConstructorSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        if (!string.Equals(
                exceptionConstructorSubjectId,
                "System.Private.CoreLib/System.InvalidCastException::.ctor:System.Void(System.String)",
                StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects boxed iconvertible char invalid-cast target '{method.SubjectId}' to construct InvalidCastException(string), but found '{exceptionConstructorSubjectId}'");
        }
    }



    private static bool LooksLikeInterfaceMethodSubjectId(string subjectId)
    {
        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        var typeName = declaringTypeSubjectId[(declaringTypeSubjectId.LastIndexOfAny(['/', '.']) + 1)..];
        return typeName.StartsWith("I", StringComparison.Ordinal) &&
               typeName.Length > 1 &&
               char.IsUpper(typeName[1]);
    }



    private static string GetMethodName(string subjectId)
    {
        var methodSeparatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        var returnTypeSeparatorIndex = subjectId.LastIndexOf(':');
        var parameterSeparatorIndex = subjectId.IndexOf('(', methodSeparatorIndex + 2);
        if (methodSeparatorIndex <= 0 || parameterSeparatorIndex <= methodSeparatorIndex + 2)
        {
            throw new InvalidOperationException($"failed to extract method name from subject id '{subjectId}'");
        }

        var methodEndIndex = returnTypeSeparatorIndex > methodSeparatorIndex
            && returnTypeSeparatorIndex < parameterSeparatorIndex
            ? returnTypeSeparatorIndex
            : parameterSeparatorIndex;
        if (methodEndIndex <= methodSeparatorIndex + 2)
        {
            throw new InvalidOperationException($"failed to extract method name from subject id '{subjectId}'");
        }

        return subjectId[(methodSeparatorIndex + 2)..methodEndIndex];
    }



    private static IReadOnlyList<string> GetMethodParameterTypesFromSubjectId(string subjectId)
    {
        if (s_refProofParamTypeCache.TryGetValue(subjectId, out var cached))
            return cached;
        var result = GetMethodParameterTypesFromSubjectIdImpl(subjectId);
        s_refProofParamTypeCache[subjectId] = result;
        return result;
    }



    private static IReadOnlyList<string> GetMethodParameterTypesFromSubjectIdImpl(string subjectId)
    {
        var startIndex = subjectId.IndexOf('(', StringComparison.Ordinal);
        var endIndex = subjectId.LastIndexOf(')');
        if (startIndex < 0 || endIndex < startIndex)
        {
            throw new InvalidOperationException($"failed to extract parameter types from subject id '{subjectId}'");
        }

        var parameterList = subjectId[(startIndex + 1)..endIndex];
        if (string.IsNullOrWhiteSpace(parameterList))
        {
            return Array.Empty<string>();
        }

        var parameters = new List<string>();
        var segmentStart = 0;
        var genericDepth = 0;
        for (var index = 0; index < parameterList.Length; index++)
        {
            switch (parameterList[index])
            {
                case '<':
                    genericDepth++;
                    break;
                case '>':
                    genericDepth--;
                    break;
                case ',' when genericDepth == 0:
                    parameters.Add(parameterList[segmentStart..index].Trim());
                    segmentStart = index + 1;
                    break;
            }
        }

        parameters.Add(parameterList[segmentStart..].Trim());
        return parameters;
    }



    private static string GetMethodReturnType(string subjectId)
    {
        var returnTypeSeparatorIndex = subjectId.LastIndexOf(':');
        var parameterSeparatorIndex = subjectId.IndexOf('(', StringComparison.Ordinal);
        if (returnTypeSeparatorIndex <= 0 || parameterSeparatorIndex <= returnTypeSeparatorIndex + 1)
        {
            throw new InvalidOperationException($"failed to extract return type from subject id '{subjectId}'");
        }

        return subjectId[(returnTypeSeparatorIndex + 1)..parameterSeparatorIndex];
    }



    private static bool IsCompilerGeneratedAsyncStateMachineMethodSubjectId(string subjectId)
    {
        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        return declaringTypeSubjectId.Contains("+<", StringComparison.Ordinal) &&
               declaringTypeSubjectId.Contains(">d__", StringComparison.Ordinal);
    }



    private static bool IsSupportedAsyncAwaitUnsafeOnCompletedCallee(string callee)
    {
        return IsSupportedAsyncBuilderCallee(callee, "::AwaitUnsafeOnCompleted<");
    }



    private static bool IsSupportedAsyncSetResultCallee(string callee)
    {
        return IsSupportedAsyncBuilderCallee(callee, "::SetResult:System.Void(");
    }



    private static bool IsSupportedAsyncSetExceptionCallee(string callee)
    {
        return IsSupportedAsyncBuilderCallee(callee, "::SetException:System.Void(System.Exception)");
    }



    private static bool IsSupportedAsyncSetStateMachineCallee(string callee)
    {
        return IsSupportedAsyncBuilderCallee(callee, "::SetStateMachine:System.Void(System.Runtime.CompilerServices.IAsyncStateMachine)");
    }



    private static bool IsSupportedAsyncTaskAwaiterGetResultCallee(string callee)
    {
        return IsSupportedAsyncAwaiterCallee(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter<") ||
               IsSupportedAsyncAwaiterCallee(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.ValueTaskAwaiter<");
    }



    private static bool IsSupportedAsyncBuilderCallee(string callee, string marker)
    {
        return (callee.StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<", StringComparison.Ordinal) ||
                callee.StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder<", StringComparison.Ordinal)) &&
               callee.Contains(marker, StringComparison.Ordinal);
    }



    private static bool IsSupportedAsyncAwaiterCallee(string callee, string prefix)
    {
        return callee.StartsWith(prefix, StringComparison.Ordinal) &&
               callee.Contains("::GetResult:", StringComparison.Ordinal) &&
               callee.EndsWith("()", StringComparison.Ordinal);
    }



    private static bool TryParseClosedTaskResultType(string taskReturnType, out string managedResultType)
    {
        const string taskPrefix = "System.Threading.Tasks.Task<";
        managedResultType = string.Empty;
        if (!taskReturnType.StartsWith(taskPrefix, StringComparison.Ordinal) ||
            !taskReturnType.EndsWith(">", StringComparison.Ordinal) ||
            taskReturnType.Length <= taskPrefix.Length + 1)
        {
            return false;
        }

        managedResultType = taskReturnType[taskPrefix.Length..^1];
        return !string.IsNullOrWhiteSpace(managedResultType);
    }



    private static bool TryResolveAsyncManagedTypeShape(string managedType, out AsyncManagedTypeShape typeShape)
    {
        switch (managedType)
        {
            case "System.Boolean":
            case "System.Byte":
            case "System.SByte":
            case "System.Int16":
            case "System.UInt16":
            case "System.Int32":
            case "System.UInt32":
            case "System.Char":
                typeShape = new AsyncManagedTypeShape(managedType, "CHAOS_IL2CPP_INT32", "0", "ldc.i4", "integer-like literals");
                return true;
            case "System.Int64":
            case "System.UInt64":
                typeShape = new AsyncManagedTypeShape(managedType, "CHAOS_IL2CPP_INT64", "0", "ldc.i8", "int64 literals");
                return true;
            case "System.Single":
                typeShape = new AsyncManagedTypeShape(managedType, "float", "0.0f", "ldc.r4", "float literals");
                return true;
            case "System.Double":
                typeShape = new AsyncManagedTypeShape(managedType, "double", "0.0", "ldc.r8", "double literals");
                return true;
            case "System.String":
                typeShape = new AsyncManagedTypeShape(managedType, "void*", "nullptr", "ldstr", "string literals");
                return true;
            case "System.Object":
                typeShape = new AsyncManagedTypeShape(managedType, "void*", "nullptr", "ldnull", "null literals");
                return true;
            default:
                typeShape = null!;
                return false;
        }
    }

}
