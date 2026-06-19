using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;


public sealed partial class NativeReferenceProofEmitter
{
    private static bool TryResolveRuntimeSkeletonPrimitiveConvertShape(
            string inputManagedType,
            string outputManagedType,
            out string inputCppType,
            out string outputCppType,
            out string convertedValueExpression)
    {
        inputCppType = string.Empty;
        outputCppType = string.Empty;
        convertedValueExpression = string.Empty;

        if (string.Equals(inputManagedType, "System.Boolean", StringComparison.Ordinal) &&
            string.Equals(outputManagedType, "System.Byte", StringComparison.Ordinal))
        {
            inputCppType = "bool";
            outputCppType = "CHAOS_IL2CPP_UINT8";
            convertedValueExpression = "request->value ? static_cast<CHAOS_IL2CPP_UINT8>(1) : static_cast<CHAOS_IL2CPP_UINT8>(0)";
            return true;
        }

        if (string.Equals(inputManagedType, "System.Byte", StringComparison.Ordinal) &&
            string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal))
        {
            inputCppType = "CHAOS_IL2CPP_UINT8";
            outputCppType = "bool";
            convertedValueExpression = "request->value != static_cast<CHAOS_IL2CPP_UINT8>(0)";
            return true;
        }

        if (string.Equals(inputManagedType, outputManagedType, StringComparison.Ordinal) &&
            TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType))
        {
            outputCppType = inputCppType;
            convertedValueExpression = "request->value";
            return true;
        }

        if (TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType) &&
            TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(outputManagedType, out outputCppType))
        {
            convertedValueExpression = "request->value";
            return true;
        }

        if (string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal) &&
            TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType))
        {
            outputCppType = "bool";
            convertedValueExpression = "request->value != 0";
            return true;
        }

        return false;
    }

    private static bool MatchesRuntimeSkeletonPrimitiveConvertInstructionShape(
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        string inputManagedType,
        string outputManagedType)
    {
        if (instructions.Count == 2)
        {
            return string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                   GetRequiredOperandInt(instructions[0]) == 0 &&
                   string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal) &&
                   TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out _) &&
                   TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(outputManagedType, out _) &&
                   IsRuntimeSkeletonDirectPrimitiveValuePreservingReturn(inputManagedType, outputManagedType);
        }

        if (TryMatchRuntimeSkeletonPrimitiveConvRetInstructionShape(instructions, inputManagedType, outputManagedType))
        {
            return true;
        }

        if (instructions.Count == 6)
        {
            return string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal) &&
                   (string.Equals(inputManagedType, "System.Single", StringComparison.Ordinal) ||
                    string.Equals(inputManagedType, "System.Double", StringComparison.Ordinal)) &&
                   string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                   GetRequiredOperandInt(instructions[0]) == 0 &&
                   string.Equals(instructions[1].Op, string.Equals(inputManagedType, "System.Single", StringComparison.Ordinal) ? "ldc.r4" : "ldc.r8", StringComparison.Ordinal) &&
                   IsZeroLiteralOperand(instructions[1]) &&
                   string.Equals(instructions[2].Op, "ceq", StringComparison.Ordinal) &&
                   string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
                   GetRequiredOperandInt(instructions[3]) == 0 &&
                   string.Equals(instructions[4].Op, "ceq", StringComparison.Ordinal) &&
                   string.Equals(instructions[5].Op, "ret", StringComparison.Ordinal);
        }

        if (instructions.Count < 4 || instructions.Count > 5)
        {
            return false;
        }

        if (!string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[0]) != 0 ||
            !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[1]) != 0)
        {
            return false;
        }

        var compareInstructionIndex = 2;
        if (instructions.Count == 5 &&
            string.Equals(instructions[2].Op, "conv.i8", StringComparison.Ordinal))
        {
            compareInstructionIndex = 3;
        }

        var compareOp = instructions[compareInstructionIndex].Op;
        if (!string.Equals(compareOp, "cgt", StringComparison.Ordinal) &&
            !string.Equals(compareOp, "cgt.un", StringComparison.Ordinal))
        {
            return false;
        }

        if (compareInstructionIndex == 3)
        {
            return string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal) &&
                   string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal) &&
                   (string.Equals(inputManagedType, "System.Int64", StringComparison.Ordinal) ||
                    string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal)) &&
                   string.Equals(compareOp, "cgt.un", StringComparison.Ordinal);
        }

        if (instructions.Count == 4)
        {
            return string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal) &&
                   string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal) &&
                   (string.Equals(compareOp, "cgt", StringComparison.Ordinal) ||
                    string.Equals(compareOp, "cgt.un", StringComparison.Ordinal));
        }

        return string.Equals(instructions[3].Op, "conv.u1", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal) &&
               string.Equals(inputManagedType, "System.Boolean", StringComparison.Ordinal) &&
               string.Equals(outputManagedType, "System.Byte", StringComparison.Ordinal) &&
               string.Equals(compareOp, "cgt.un", StringComparison.Ordinal);
    }

    private static bool TryResolveRuntimeSkeletonCheckedByteConvertShape(
        string subjectId,
        string inputManagedType,
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

        if (string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal))
        {
            if (instructions.Count != 8 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 255 ||
                !string.Equals(instructions[2].Op, "conv.i8", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ble.un", StringComparison.Ordinal) ||
                !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[5].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[5]) != 0 ||
                !string.Equals(instructions[6].Op, "conv.u1", StringComparison.Ordinal) ||
                !string.Equals(instructions[7].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            throwSubjectId = GetRequiredInstructionCallee(instructions[4], subjectId, 4);
            overflowConditionExpression = "request->value > static_cast<CHAOS_IL2CPP_UINT64>(255)";
            return true;
        }

        switch (inputManagedType)
        {
            case "System.SByte":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 0 ||
                    !string.Equals(instructions[2].Op, "bge", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<CHAOS_IL2CPP_INT8>(0)";
                return true;
            case "System.Int16":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 255 ||
                    !string.Equals(instructions[2].Op, "ble.un", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<CHAOS_IL2CPP_INT16>(0) || request->value > static_cast<CHAOS_IL2CPP_INT16>(255)";
                return true;
            case "System.UInt16":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 255 ||
                    !string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value > static_cast<CHAOS_IL2CPP_UINT16>(255)";
                return true;
            case "System.UInt32":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 255 ||
                    !string.Equals(instructions[2].Op, "ble.un", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value > static_cast<CHAOS_IL2CPP_UINT32>(255)";
                return true;
            case "System.Char":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 255 ||
                    !string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value > static_cast<CHAOS_IL2CPP_UINT16>(255)";
                return true;
            default:
                return false;
        }
    }

    private static bool TryResolveRuntimeSkeletonCheckedCharConvertShape(
        string subjectId,
        string inputManagedType,
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

        if (string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal))
        {
            if (instructions.Count != 8 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 65535 ||
                !string.Equals(instructions[2].Op, "conv.i8", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ble.un", StringComparison.Ordinal) ||
                !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[5].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[5]) != 0 ||
                !string.Equals(instructions[6].Op, "conv.u2", StringComparison.Ordinal) ||
                !string.Equals(instructions[7].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            throwSubjectId = GetRequiredInstructionCallee(instructions[4], subjectId, 4);
            overflowConditionExpression = "request->value > static_cast<CHAOS_IL2CPP_UINT64>(65535)";
            return true;
        }

        switch (inputManagedType)
        {
            case "System.SByte":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 0 ||
                    !string.Equals(instructions[2].Op, "bge", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u2", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<CHAOS_IL2CPP_INT8>(0)";
                return true;
            case "System.Int16":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 0 ||
                    !string.Equals(instructions[2].Op, "bge", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u2", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<CHAOS_IL2CPP_INT16>(0)";
                return true;
            case "System.UInt32":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 65535 ||
                    !string.Equals(instructions[2].Op, "ble.un", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u2", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value > static_cast<CHAOS_IL2CPP_UINT32>(65535)";
                return true;
            default:
                return false;
        }
    }
}
