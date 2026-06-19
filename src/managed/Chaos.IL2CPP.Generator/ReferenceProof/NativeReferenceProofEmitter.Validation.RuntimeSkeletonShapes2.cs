using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{

    private static bool TryResolveRuntimeSkeletonCheckedPrimitiveConvertShape(
        string subjectId,
        string inputManagedType,
        string outputManagedType,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        out string inputCppType,
        out string overflowConditionExpression,
        out string throwSubjectId)
    {
        inputCppType = string.Empty;
        overflowConditionExpression = string.Empty;
        throwSubjectId = string.Empty;

        if (!TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType))
        {
            return false;
        }

        if (string.Equals(outputManagedType, "System.Byte", StringComparison.Ordinal))
        {
            return TryResolveRuntimeSkeletonCheckedByteConvertShape(
                subjectId, inputManagedType, instructions,
                out inputCppType, out overflowConditionExpression, out throwSubjectId);
        }

        if (string.Equals(outputManagedType, "System.Char", StringComparison.Ordinal))
        {
            return TryResolveRuntimeSkeletonCheckedCharConvertShape(
                subjectId, inputManagedType, instructions,
                out inputCppType, out overflowConditionExpression, out throwSubjectId);
        }

        // For remaining output types, determine the type-specific constants
        string convOp;
        int maxValue;
        bool isSigned;
        if (string.Equals(outputManagedType, "System.SByte", StringComparison.Ordinal))
        {
            convOp = "conv.i1";
            maxValue = 127;
            isSigned = true;
        }
        else if (string.Equals(outputManagedType, "System.Int16", StringComparison.Ordinal))
        {
            convOp = "conv.i2";
            maxValue = 32767;
            isSigned = true;
        }
        else if (string.Equals(outputManagedType, "System.Int32", StringComparison.Ordinal))
        {
            convOp = "conv.i4";
            maxValue = 2147483647;
            isSigned = true;
        }
        else if (string.Equals(outputManagedType, "System.UInt32", StringComparison.Ordinal))
        {
            convOp = "conv.u4";
            maxValue = -1; // 4294967295 not representable as signed int, use uint expression
            isSigned = false;
        }
        else
        {
            return false;
        }

        // General pattern matching for checked primitive conversions
        string maxCppExpr;
        string maxBranchOp;
        if (string.Equals(outputManagedType, "System.UInt32", StringComparison.Ordinal))
        {
            maxCppExpr = "static_cast<CHAOS_IL2CPP_UINT64>(4294967295)";
            maxBranchOp = "ble.un";
        }
        else
        {
            maxCppExpr = $"static_cast<CHAOS_IL2CPP_INT32>({maxValue})";
            maxBranchOp = isSigned ? "ble" : "ble.un";
        }

        if (string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal))
        {
            // UInt64: 8-instruction pattern with conv.i8
            if (instructions.Count != 8 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                !string.Equals(instructions[2].Op, "conv.i8", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ble.un", StringComparison.Ordinal) ||
                !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[5].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[5]) != 0 ||
                !string.Equals(instructions[7].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            // Verify conv opcode at position 6
            if (!string.Equals(instructions[6].Op, convOp, StringComparison.Ordinal))
            {
                return false;
            }

            throwSubjectId = GetRequiredInstructionCallee(instructions[4], subjectId, 4);
            overflowConditionExpression = $"request->value > static_cast<CHAOS_IL2CPP_UINT64>({maxValue})";
            return true;
        }

        // 7-instruction patterns for non-UInt64 inputs
        if (instructions.Count != 7)
        {
            return false;
        }

        // Check common prefix: ldarg.0, ldc.i4, branch[, call, ldarg.0, conv.*, ret]
        if (!string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[0]) != 0 ||
            !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[4]) != 0 ||
            !string.Equals(instructions[5].Op, convOp, StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        // Signed input types that need a dual (lower bound) check for signed output types
        // Use a sign check pattern via bge/bge.un with 0
        if (isSigned &&
            (string.Equals(inputManagedType, "System.Int16", StringComparison.Ordinal) ||
             string.Equals(inputManagedType, "System.Int32", StringComparison.Ordinal)))
        {
            // First check lower bound: ldarg.0, ldc.i4 <min>, ble/bge, call_throw
            // Then upper bound check: ... (if needed)
            // For Int16→SByte/Int32→SByte/Int32→Int16, the compiler generates the upper check pattern

            // For Int16→SByte: dual check (value < -128 || value > 127)
            // The IL pattern for this is: ble.un maxCheck which handles both directions
            // Actually the C# compiler uses a single ble.un for unsigned comparison
            // For Int16→SByte the comparison is actually against 127 with ble.un:
            //   ldarg.0, ldc.i4 127, ble.un, call_throw, ldarg.0, conv.i1, ret
            // This catches both < -128 and > 127 because negative Int16 wraps to > 65535 in unsigned

            // Check for single-comparison pattern: ldarg.0, ldc.i4 max, ble.un, call, ldarg.0, conv.*, ret
            if (GetRequiredOperandInt(instructions[1]) == maxValue &&
                string.Equals(instructions[2].Op, maxBranchOp, StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = $"request->value > {maxCppExpr}";
                // For signed inputs, also check lower bound
                int signedMin = 0;
                if (string.Equals(outputManagedType, "System.SByte", StringComparison.Ordinal))
                    signedMin = -128;
                else if (string.Equals(outputManagedType, "System.Int16", StringComparison.Ordinal))
                    signedMin = -32768;
                else if (string.Equals(outputManagedType, "System.Int32", StringComparison.Ordinal))
                    signedMin = -2147483648;
                overflowConditionExpression = $"request->value < static_cast<CHAOS_IL2CPP_INT32>({signedMin}) || {overflowConditionExpression}";
                return true;
            }

            return false;
        }

        if (string.Equals(inputManagedType, "System.SByte", StringComparison.Ordinal))
        {
            // SByte: negative check first
            if (GetRequiredOperandInt(instructions[1]) == 0 &&
                string.Equals(instructions[2].Op, "bge", StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<CHAOS_IL2CPP_INT8>(0)";
                return true;
            }
            return false;
        }

        if (string.Equals(inputManagedType, "System.Int16", StringComparison.Ordinal))
        {
            // For unsigned output types with unsigned comparison: upper check only
            if (!isSigned && GetRequiredOperandInt(instructions[1]) == maxValue &&
                string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = $"request->value > {maxCppExpr}";
                return true;
            }
            // For signed output: already handled above
            return false;
        }

        if (string.Equals(inputManagedType, "System.UInt16", StringComparison.Ordinal))
        {
            if (GetRequiredOperandInt(instructions[1]) == maxValue &&
                string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = $"request->value > {maxCppExpr}";
                return true;
            }
            return false;
        }

        if (string.Equals(inputManagedType, "System.UInt32", StringComparison.Ordinal))
        {
            if (GetRequiredOperandInt(instructions[1]) == maxValue &&
                string.Equals(instructions[2].Op, maxBranchOp, StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = $"request->value > {maxCppExpr}";
                return true;
            }
            return false;
        }

        if (string.Equals(inputManagedType, "System.Char", StringComparison.Ordinal))
        {
            if (GetRequiredOperandInt(instructions[1]) == maxValue &&
                string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = $"request->value > {maxCppExpr}";
                return true;
            }
            return false;
        }

        if (string.Equals(inputManagedType, "System.Byte", StringComparison.Ordinal))
        {
            if (GetRequiredOperandInt(instructions[1]) == maxValue &&
                string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = $"request->value > {maxCppExpr}";
                return true;
            }
            return false;
        }

        return false;
    }




    private static bool TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(
        string inputManagedType,
        out string inputCppType)
    {
        inputCppType = string.Empty;
        switch (inputManagedType)
        {
            case "System.Boolean":
                inputCppType = "bool";
                return true;
            case "System.Byte":
                inputCppType = "CHAOS_IL2CPP_UINT8";
                return true;
            case "System.SByte":
                inputCppType = "CHAOS_IL2CPP_INT8";
                return true;
            case "System.Int16":
                inputCppType = "CHAOS_IL2CPP_INT16";
                return true;
            case "System.UInt16":
                inputCppType = "CHAOS_IL2CPP_UINT16";
                return true;
            case "System.Int32":
                inputCppType = "CHAOS_IL2CPP_INT32";
                return true;
            case "System.UInt32":
                inputCppType = "CHAOS_IL2CPP_UINT32";
                return true;
            case "System.Int64":
                inputCppType = "CHAOS_IL2CPP_INT64";
                return true;
            case "System.UInt64":
                inputCppType = "CHAOS_IL2CPP_UINT64";
                return true;
            case "System.Char":
                inputCppType = "CHAOS_IL2CPP_UINT16";
                return true;
            case "System.Single":
                inputCppType = "float";
                return true;
            case "System.Double":
                inputCppType = "double";
                return true;
            default:
                return false;
        }
    }



    private static bool TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(
        string outputManagedType,
        out string outputCppType)
    {
        return TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(outputManagedType, out outputCppType);
    }



    private static bool TryResolveRuntimeSkeletonBoxedIConvertibleCharInvalidCastShape(
        TypedIlMethodArtifact method,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        out string inputCppType,
        out string outputCppType,
        out string exceptionTypeSubjectId,
        out string sourceTypeName,
        out string targetTypeName)
    {
        inputCppType = string.Empty;
        outputCppType = string.Empty;
        exceptionTypeSubjectId = string.Empty;
        sourceTypeName = string.Empty;
        targetTypeName = string.Empty;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Char", StringComparison.Ordinal) ||
            !TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(method.Parameters[0].Type, out inputCppType) ||
            !TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType("System.Char", out outputCppType))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateRuntimeSkeletonBoxedIConvertibleCharEntryShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var boxedTypeSubjectId = GetRequiredOperandString(instructions[1]);
        var declaredTargetSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var resolvedTargetSubjectId = TryResolveBoxedInterfaceCallTarget(
            methodsBySubjectId.Values.ToArray(),
            instructions,
            1,
            declaredTargetSubjectId);
        if (string.IsNullOrWhiteSpace(resolvedTargetSubjectId) ||
            !methodsBySubjectId.TryGetValue(resolvedTargetSubjectId, out var targetMethod))
        {
            return TryResolveRuntimeSkeletonBoxedIConvertibleCharInvalidCastFallback(
                method,
                boxedTypeSubjectId,
                out exceptionTypeSubjectId,
                out sourceTypeName,
                out targetTypeName);
        }

        IReadOnlyList<TypedIlInstructionArtifact> targetInstructions;
        try
        {
            targetInstructions = GetSingleBlockInstructions(targetMethod);
            ValidateRuntimeSkeletonBoxedIConvertibleCharInvalidCastTargetShape(targetMethod, targetInstructions);
        }
        catch
        {
            return false;
        }

        if (!string.Equals(GetDeclaringTypeSubjectId(targetMethod.SubjectId), boxedTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        sourceTypeName = GetRequiredOperandString(targetInstructions[1]);
        targetTypeName = GetRequiredOperandString(targetInstructions[2]);
        if (!string.Equals(targetTypeName, "Char", StringComparison.Ordinal))
        {
            return false;
        }

        var exceptionConstructorSubjectId = GetRequiredInstructionCallee(targetInstructions[4], targetMethod.SubjectId, 4);
        exceptionTypeSubjectId = GetDeclaringTypeSubjectId(exceptionConstructorSubjectId);
        return string.Equals(exceptionTypeSubjectId, "System.Private.CoreLib/System.InvalidCastException", StringComparison.Ordinal);
    }

}
