using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{





    private static bool TryResolveRuntimeSkeletonBoxedIConvertibleCharInvalidCastFallback(
        TypedIlMethodArtifact method,
        string boxedTypeSubjectId,
        out string exceptionTypeSubjectId,
        out string sourceTypeName,
        out string targetTypeName)
    {
        exceptionTypeSubjectId = string.Empty;
        sourceTypeName = string.Empty;
        targetTypeName = string.Empty;

        if (!TryResolveRuntimeSkeletonBoxedIConvertibleCharFallbackSourceTypeName(
                method.Parameters[0].Type,
                boxedTypeSubjectId,
                out sourceTypeName))
        {
            return false;
        }

        exceptionTypeSubjectId = "System.Private.CoreLib/System.InvalidCastException";
        targetTypeName = "Char";
        return true;
    }







    private static bool TryResolveRuntimeSkeletonBoxedIConvertibleCharFallbackSourceTypeName(
        string methodParameterType,
        string boxedTypeSubjectId,
        out string sourceTypeName)
    {
        sourceTypeName = string.Empty;
        if (!string.Equals(methodParameterType, "System.Boolean", StringComparison.Ordinal) &&
            !string.Equals(methodParameterType, "System.Single", StringComparison.Ordinal) &&
            !string.Equals(methodParameterType, "System.Double", StringComparison.Ordinal))
        {
            return false;
        }

        var expectedBoxedTypeSubjectId = $"System.Private.CoreLib/{methodParameterType}";
        if (!string.Equals(boxedTypeSubjectId, expectedBoxedTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        sourceTypeName = GetTypeDisplayName(methodParameterType);
        return true;
    }







    private static bool TryResolveRuntimeSkeletonBoxedValueTypeCharInvalidCastShape(
        TypedIlMethodArtifact method,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        out string inputCppType,
        out string outputCppType,
        out string boxedValueTypeSubjectId,
        out string exceptionTypeSubjectId,
        out string sourceTypeName,
        out string targetTypeName,
        out int inputSize)
    {
        inputCppType = string.Empty;
        outputCppType = string.Empty;
        boxedValueTypeSubjectId = string.Empty;
        exceptionTypeSubjectId = string.Empty;
        sourceTypeName = string.Empty;
        targetTypeName = string.Empty;
        inputSize = 0;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Char", StringComparison.Ordinal) ||
            !TryResolveRuntimeSkeletonValueTypeByValueInputCppType(method.Parameters[0].Type, out inputCppType, out inputSize) ||
            !TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType("System.Char", out outputCppType))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
        }
        catch
        {
            return false;
        }

        if (instructions.Count != 5 ||
            !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[0]) != 0 ||
            !string.Equals(instructions[1].Op, "box", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "ldnull", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "callvirt", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        boxedValueTypeSubjectId = GetRequiredOperandString(instructions[1]);
        var declaredTargetSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var resolvedTargetSubjectId = TryResolveBoxedInterfaceCallTarget(
            methodsBySubjectId.Values.ToArray(),
            instructions,
            1,
            declaredTargetSubjectId);
        if (string.IsNullOrWhiteSpace(resolvedTargetSubjectId) ||
            !methodsBySubjectId.TryGetValue(resolvedTargetSubjectId, out var targetMethod))
        {
            return TryResolveRuntimeSkeletonBoxedValueTypeCharInvalidCastFallback(
                method,
                boxedValueTypeSubjectId,
                out exceptionTypeSubjectId,
                out sourceTypeName,
                out targetTypeName);
        }

        IReadOnlyList<TypedIlInstructionArtifact> targetInstructions;
        try
        {
            targetInstructions = GetSingleBlockInstructions(targetMethod);
        }
        catch
        {
            return false;
        }

        return TryResolveRuntimeSkeletonBoxedValueTypeCharInvalidCastTargetShape(
            targetMethod,
            targetInstructions,
            out exceptionTypeSubjectId,
            out sourceTypeName,
            out targetTypeName);
    }







    private static bool TryResolveRuntimeSkeletonBoxedValueTypeCharInvalidCastFallback(
        TypedIlMethodArtifact method,
        string boxedValueTypeSubjectId,
        out string exceptionTypeSubjectId,
        out string sourceTypeName,
        out string targetTypeName)
    {
        exceptionTypeSubjectId = string.Empty;
        sourceTypeName = string.Empty;
        targetTypeName = string.Empty;

        if (!TryResolveRuntimeSkeletonBoxedValueTypeCharFallbackSourceTypeName(
                method.Parameters[0].Type,
                boxedValueTypeSubjectId,
                out sourceTypeName))
        {
            return false;
        }

        exceptionTypeSubjectId = "System.Private.CoreLib/System.InvalidCastException";
        targetTypeName = "Char";
        return true;
    }







    private static bool TryResolveRuntimeSkeletonBoxedValueTypeCharFallbackSourceTypeName(
        string methodParameterType,
        string boxedValueTypeSubjectId,
        out string sourceTypeName)
    {
        sourceTypeName = string.Empty;
        if (!string.Equals(methodParameterType, "System.Decimal", StringComparison.Ordinal) &&
            !string.Equals(methodParameterType, "System.DateTime", StringComparison.Ordinal))
        {
            return false;
        }

        var expectedBoxedTypeSubjectId = $"System.Private.CoreLib/{methodParameterType}";
        if (!string.Equals(boxedValueTypeSubjectId, expectedBoxedTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        sourceTypeName = GetTypeDisplayName(methodParameterType);
        return true;
    }







    private static bool TryResolveRuntimeSkeletonBoxedValueTypeCharInvalidCastTargetShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        out string exceptionTypeSubjectId,
        out string sourceTypeName,
        out string targetTypeName)
    {
        exceptionTypeSubjectId = string.Empty;
        sourceTypeName = string.Empty;
        targetTypeName = string.Empty;

        if (!string.Equals(method.MethodRole, "instance-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Char", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(method.Parameters[0].Type, "System.IFormatProvider", StringComparison.Ordinal))
        {
            return false;
        }

        if (instructions.Count == 6 &&
            string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) &&
            string.Equals(instructions[1].Op, "ldstr", StringComparison.Ordinal) &&
            string.Equals(instructions[2].Op, "ldstr", StringComparison.Ordinal) &&
            string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
            string.Equals(instructions[4].Op, "newobj", StringComparison.Ordinal) &&
            string.Equals(instructions[5].Op, "throw", StringComparison.Ordinal))
        {
            sourceTypeName = GetRequiredOperandString(instructions[1]);
            targetTypeName = GetRequiredOperandString(instructions[2]);
            var exceptionConstructorSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
            exceptionTypeSubjectId = GetDeclaringTypeSubjectId(exceptionConstructorSubjectId);
            return string.Equals(exceptionTypeSubjectId, "System.Private.CoreLib/System.InvalidCastException", StringComparison.Ordinal) &&
                   string.Equals(targetTypeName, "Char", StringComparison.Ordinal);
        }

        if (instructions.Count == 3 &&
            string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
            string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) &&
            string.Equals(instructions[2].Op, "throw", StringComparison.Ordinal))
        {
            targetTypeName = GetRequiredOperandString(instructions[0]);
            sourceTypeName = GetTypeDisplayName(GetDeclaringTypeSubjectId(method.SubjectId));
            exceptionTypeSubjectId = "System.Private.CoreLib/System.InvalidCastException";
            return string.Equals(
                       GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1),
                       "System.Private.CoreLib/System.DateTime::InvalidCast:System.InvalidCastException(System.String)",
                       StringComparison.Ordinal) &&
                   string.Equals(targetTypeName, "Char", StringComparison.Ordinal);
        }

        return false;
    }







    private static bool TryResolveRuntimeSkeletonValueTypeByValueInputCppType(
        string inputManagedType,
        out string inputCppType,
        out int inputSize)
    {
        inputCppType = string.Empty;
        inputSize = 0;
        switch (inputManagedType)
        {
            case "System.Boolean":
                inputCppType = "bool";
                inputSize = 1;
                return true;
            case "System.Byte":
                inputCppType = "CHAOS_IL2CPP_UINT8";
                inputSize = 1;
                return true;
            case "System.SByte":
                inputCppType = "CHAOS_IL2CPP_INT8";
                inputSize = 1;
                return true;
            case "System.Int16":
                inputCppType = "CHAOS_IL2CPP_INT16";
                inputSize = 2;
                return true;
            case "System.UInt16":
            case "System.Char":
                inputCppType = "CHAOS_IL2CPP_UINT16";
                inputSize = 2;
                return true;
            case "System.Int32":
                inputCppType = "CHAOS_IL2CPP_INT32";
                inputSize = 4;
                return true;
            case "System.UInt32":
                inputCppType = "CHAOS_IL2CPP_UINT32";
                inputSize = 4;
                return true;
            case "System.Int64":
                inputCppType = "CHAOS_IL2CPP_INT64";
                inputSize = 8;
                return true;
            case "System.UInt64":
                inputCppType = "CHAOS_IL2CPP_UINT64";
                inputSize = 8;
                return true;
            case "System.Single":
                inputCppType = "float";
                inputSize = 4;
                return true;
            case "System.Double":
                inputCppType = "double";
                inputSize = 8;
                return true;
            case "System.Decimal":
                inputCppType = "struct { CHAOS_IL2CPP_UINT32 flags; CHAOS_IL2CPP_UINT64 lo64; CHAOS_IL2CPP_UINT32 hi32; }";
                inputSize = 16;
                return true;
            case "System.DateTime":
                inputCppType = "CHAOS_IL2CPP_UINT64";
                inputSize = 8;
                return true;
            default:
                return false;
        }
    }







    private static string GetTypeDisplayName(string managedTypeOrSubjectId)
    {
        var separatorIndex = managedTypeOrSubjectId.LastIndexOfAny(['/', '.']);
        return separatorIndex >= 0 && separatorIndex < managedTypeOrSubjectId.Length - 1
            ? managedTypeOrSubjectId[(separatorIndex + 1)..]
            : managedTypeOrSubjectId;
    }

}
