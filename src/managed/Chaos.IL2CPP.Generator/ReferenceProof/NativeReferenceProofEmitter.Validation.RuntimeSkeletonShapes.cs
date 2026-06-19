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

    private static bool TryResolveRuntimeSkeletonStringCharProviderShape(
        TypedIlMethodArtifact method,
        out string targetMethodSubjectId)
    {
        targetMethodSubjectId = string.Empty;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Char", StringComparison.Ordinal))
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

        if (method.Parameters.Count == 1 &&
            string.Equals(method.Parameters[0].Type, "System.String", StringComparison.Ordinal) &&
            instructions.Count == 4 &&
            string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[0]) == 0 &&
            string.Equals(instructions[1].Op, "ldnull", StringComparison.Ordinal) &&
            string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) &&
            string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
        {
            targetMethodSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
            if (!string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.Char", StringComparison.Ordinal))
            {
                return false;
            }

            var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
            return targetParameterTypes.Count == 2 &&
                   string.Equals(targetParameterTypes[0], "System.String", StringComparison.Ordinal) &&
                   string.Equals(targetParameterTypes[1], "System.IFormatProvider", StringComparison.Ordinal) &&
                   (string.Equals(targetMethodSubjectId, RuntimeSkeletonConvertStringCharProviderCoreLibSubjectId, StringComparison.Ordinal) ||
                    !string.IsNullOrWhiteSpace(targetMethodSubjectId));
        }

        if (method.Parameters.Count == 2 &&
            string.Equals(method.Parameters[0].Type, "System.String", StringComparison.Ordinal) &&
            string.Equals(method.Parameters[1].Type, "System.IFormatProvider", StringComparison.Ordinal) &&
            instructions.Count == 14 &&
            string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[0]) == 0 &&
            string.Equals(instructions[1].Op, "ldstr", StringComparison.Ordinal) &&
            string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) &&
            string.Equals(instructions[3].Op, "ldarg", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[3]) == 0 &&
            string.Equals(instructions[4].Op, "callvirt", StringComparison.Ordinal) &&
            string.Equals(instructions[5].Op, "ldc.i4", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[5]) == 1 &&
            string.Equals(instructions[6].Op, "beq", StringComparison.Ordinal) &&
            (string.Equals(instructions[7].Op, "call", StringComparison.Ordinal) ||
             string.Equals(instructions[7].Op, "ldstr", StringComparison.Ordinal)) &&
            string.Equals(instructions[8].Op, "newobj", StringComparison.Ordinal) &&
            string.Equals(instructions[9].Op, "throw", StringComparison.Ordinal) &&
            string.Equals(instructions[10].Op, "ldarg", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[10]) == 0 &&
            string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[11]) == 0 &&
            string.Equals(instructions[12].Op, "callvirt", StringComparison.Ordinal) &&
            string.Equals(instructions[13].Op, "ret", StringComparison.Ordinal))
        {
            RequireInstructionCallee(
                instructions[2],
                "System.Private.CoreLib/System.ArgumentNullException::ThrowIfNull:System.Void(System.Object,System.String)",
                method.SubjectId,
                2);
            RequireInstructionCallee(
                instructions[4],
                "System.Private.CoreLib/System.String::get_Length:System.Int32()",
                method.SubjectId,
                4);
            if (string.Equals(instructions[7].Op, "call", StringComparison.Ordinal))
            {
                RequireInstructionCallee(
                    instructions[7],
                    "System.Private.CoreLib/System.SR::get_Format_NeedSingleChar:System.String()",
                    method.SubjectId,
                    7);
            }
            RequireInstructionCallee(
                instructions[8],
                "System.Private.CoreLib/System.FormatException::.ctor:System.Void(System.String)",
                method.SubjectId,
                8);
            RequireInstructionCallee(
                instructions[12],
                "System.Private.CoreLib/System.String::get_Chars:System.Char(System.Int32)",
                method.SubjectId,
                12);
            targetMethodSubjectId = method.SubjectId;
            return true;
        }

        return false;
    }

    private static bool TryResolveRuntimeSkeletonStringProviderPassthroughShape(
        TypedIlMethodArtifact method)
    {
        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal) ||
            method.Parameters.Count != 2 ||
            !string.Equals(method.Parameters[0].Type, "System.String", StringComparison.Ordinal) ||
            !string.Equals(method.Parameters[1].Type, "System.IFormatProvider", StringComparison.Ordinal))
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

        return instructions.Count == 2 &&
               string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
               GetRequiredOperandInt(instructions[0]) == 0 &&
               string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal) &&
               (string.Equals(method.SubjectId, RuntimeSkeletonConvertStringProviderPassthroughCoreLibSubjectId, StringComparison.Ordinal) ||
                !string.IsNullOrWhiteSpace(method.SubjectId));
    }

    private static bool TryResolveRuntimeSkeletonObjectCharProviderShape(
        TypedIlMethodArtifact method,
        out string targetMethodSubjectId)
    {
        targetMethodSubjectId = string.Empty;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Char", StringComparison.Ordinal))
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

        if (method.Parameters.Count == 1 &&
            string.Equals(method.Parameters[0].Type, "System.Object", StringComparison.Ordinal))
        {
            if (instructions.Count == 4 &&
                string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[0]) == 0 &&
                string.Equals(instructions[1].Op, "ldnull", StringComparison.Ordinal) &&
                string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) &&
                string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                targetMethodSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
                var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
                return string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.Char", StringComparison.Ordinal) &&
                       targetParameterTypes.Count == 2 &&
                       string.Equals(targetParameterTypes[0], "System.Object", StringComparison.Ordinal) &&
                       string.Equals(targetParameterTypes[1], "System.IFormatProvider", StringComparison.Ordinal);
            }

            if (instructions.Count == 9 &&
                string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[0]) == 0 &&
                string.Equals(instructions[1].Op, "brfalse", StringComparison.Ordinal) &&
                string.Equals(instructions[2].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[2]) == 0 &&
                string.Equals(instructions[3].Op, "castclass", StringComparison.Ordinal) &&
                string.Equals(instructions[4].Op, "ldnull", StringComparison.Ordinal) &&
                string.Equals(instructions[5].Op, "callvirt", StringComparison.Ordinal) &&
                string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal) &&
                string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[7]) == 0 &&
                string.Equals(instructions[8].Op, "ret", StringComparison.Ordinal))
            {
                targetMethodSubjectId = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
                var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
                return string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.Char", StringComparison.Ordinal) &&
                       targetParameterTypes.Count == 1 &&
                       string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal);
            }

            if (instructions.Count == 9 &&
                string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[0]) == 0 &&
                string.Equals(instructions[1].Op, "brtrue", StringComparison.Ordinal) &&
                string.Equals(instructions[2].Op, "ldc.i4", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[2]) == 0 &&
                string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal) &&
                string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[4]) == 0 &&
                string.Equals(instructions[5].Op, "castclass", StringComparison.Ordinal) &&
                string.Equals(instructions[6].Op, "ldnull", StringComparison.Ordinal) &&
                string.Equals(instructions[7].Op, "callvirt", StringComparison.Ordinal) &&
                string.Equals(instructions[8].Op, "ret", StringComparison.Ordinal))
            {
                targetMethodSubjectId = GetRequiredInstructionCallee(instructions[7], method.SubjectId, 7);
                var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
                return string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.Char", StringComparison.Ordinal) &&
                       targetParameterTypes.Count == 1 &&
                       string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal);
            }
        }

        if (method.Parameters.Count == 2 &&
            string.Equals(method.Parameters[0].Type, "System.Object", StringComparison.Ordinal) &&
            string.Equals(method.Parameters[1].Type, "System.IFormatProvider", StringComparison.Ordinal) &&
            instructions.Count == 9 &&
            string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[0]) == 0)
        {
            if (string.Equals(instructions[1].Op, "brfalse", StringComparison.Ordinal) &&
                string.Equals(instructions[2].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[2]) == 0 &&
                string.Equals(instructions[3].Op, "castclass", StringComparison.Ordinal) &&
                string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[4]) == 1 &&
                string.Equals(instructions[5].Op, "callvirt", StringComparison.Ordinal) &&
                string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal) &&
                string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[7]) == 0 &&
                string.Equals(instructions[8].Op, "ret", StringComparison.Ordinal))
            {
                targetMethodSubjectId = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
                var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
                return string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.Char", StringComparison.Ordinal) &&
                       targetParameterTypes.Count == 1 &&
                       string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal);
            }

            if (string.Equals(instructions[1].Op, "brtrue", StringComparison.Ordinal) &&
                string.Equals(instructions[2].Op, "ldc.i4", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[2]) == 0 &&
                string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal) &&
                string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[4]) == 0 &&
                string.Equals(instructions[5].Op, "castclass", StringComparison.Ordinal) &&
                string.Equals(instructions[6].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[6]) == 1 &&
                string.Equals(instructions[7].Op, "callvirt", StringComparison.Ordinal) &&
                string.Equals(instructions[8].Op, "ret", StringComparison.Ordinal))
            {
                targetMethodSubjectId = GetRequiredInstructionCallee(instructions[7], method.SubjectId, 7);
                var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
                return string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.Char", StringComparison.Ordinal) &&
                       targetParameterTypes.Count == 1 &&
                       string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal);
            }
        }

        return false;
    }

    private static bool TryResolveRuntimeSkeletonBoxedValueTypeStringInstanceCallShape(
        TypedIlMethodArtifact method,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        out string inputCppType,
        out string boxedValueTypeSubjectId,
        out string targetMethodSubjectId,
        out string targetAssemblyName,
        out int inputSize,
        out int argCount)
    {
        inputCppType = string.Empty;
        boxedValueTypeSubjectId = string.Empty;
        targetMethodSubjectId = string.Empty;
        targetAssemblyName = string.Empty;
        inputSize = 0;
        argCount = 0;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal) ||
            method.Parameters.Count < 1 ||
            method.Parameters.Count > 2 ||
            !TryResolveRuntimeSkeletonValueTypeByValueInputCppType(method.Parameters[0].Type, out inputCppType, out inputSize))
        {
            return false;
        }

        if (method.Parameters.Count == 2 &&
            !string.Equals(method.Parameters[1].Type, "System.IFormatProvider", StringComparison.Ordinal))
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

        if (method.Parameters.Count == 1)
        {
            if (instructions.Count != 3 ||
                !string.Equals(instructions[0].Op, "ldarga", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            argCount = 0;
        }
        else
        {
            if (instructions.Count != 4 ||
                !string.Equals(instructions[0].Op, "ldarga", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 1 ||
                !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            argCount = 1;
        }

        boxedValueTypeSubjectId = $"System.Private.CoreLib/{method.Parameters[0].Type}";
        targetMethodSubjectId = GetRequiredInstructionCallee(
            instructions[argCount == 0 ? 1 : 2],
            method.SubjectId,
            argCount == 0 ? 1 : 2);
        targetAssemblyName = GetAssemblyNameFromSubjectId(targetMethodSubjectId);

        if (!string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetDeclaringTypeSubjectId(targetMethodSubjectId), boxedValueTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
        if (argCount == 0)
        {
            if (targetParameterTypes.Count != 0)
            {
                return false;
            }
        }
        else if (targetParameterTypes.Count != 1 ||
                 !string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal))
        {
            return false;
        }

        if (methodsBySubjectId.TryGetValue(targetMethodSubjectId, out var targetMethod))
        {
            return string.Equals(targetMethod.MethodRole, "instance-method", StringComparison.Ordinal) &&
                   string.Equals(targetMethod.BodyAvailability, "has-canonical-body", StringComparison.Ordinal);
        }

        return true;
    }

    private static bool TryResolveRuntimeSkeletonBoxedValueTypeInstanceCallShape(
        TypedIlMethodArtifact method,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string outputManagedType,
        out string inputCppType,
        out string outputCppType,
        out string boxedValueTypeSubjectId,
        out string targetMethodSubjectId,
        out string targetAssemblyName,
        out int inputSize,
        out int argCount)
    {
        inputCppType = string.Empty;
        outputCppType = string.Empty;
        boxedValueTypeSubjectId = string.Empty;
        targetMethodSubjectId = string.Empty;
        targetAssemblyName = string.Empty;
        inputSize = 0;
        argCount = 0;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), outputManagedType, StringComparison.Ordinal) ||
            method.Parameters.Count < 1 ||
            method.Parameters.Count > 2 ||
            !TryResolveRuntimeSkeletonValueTypeByValueInputCppType(method.Parameters[0].Type, out inputCppType, out inputSize) ||
            !TryResolveRuntimeSkeletonValueTypeByValueInputCppType(outputManagedType, out outputCppType, out _))
        {
            return false;
        }

        if (method.Parameters.Count == 2 &&
            !string.Equals(method.Parameters[1].Type, "System.IFormatProvider", StringComparison.Ordinal))
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

        int callInstructionIndex;
        if (method.Parameters.Count == 1)
        {
            if (instructions.Count == 3 &&
                string.Equals(instructions[0].Op, "ldarga", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[0]) == 0 &&
                (string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
                 string.Equals(instructions[1].Op, "callvirt", StringComparison.Ordinal)) &&
                string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
            {
                callInstructionIndex = 1;
            }
            else if (instructions.Count == 4 &&
                string.Equals(instructions[0].Op, "ldarga", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[0]) == 0 &&
                string.Equals(instructions[1].Op, "ldnull", StringComparison.Ordinal) &&
                (string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
                 string.Equals(instructions[2].Op, "callvirt", StringComparison.Ordinal)) &&
                string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                callInstructionIndex = 2;
            }
            else
            {
                return false;
            }

            argCount = 0;
        }
        else
        {
            if (instructions.Count != 4 ||
                !string.Equals(instructions[0].Op, "ldarga", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 1 ||
                !(string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
                  string.Equals(instructions[2].Op, "callvirt", StringComparison.Ordinal)) ||
                !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            callInstructionIndex = 2;
            argCount = 1;
        }

        boxedValueTypeSubjectId = $"System.Private.CoreLib/{method.Parameters[0].Type}";
        targetMethodSubjectId = GetRequiredInstructionCallee(
            instructions[callInstructionIndex],
            method.SubjectId,
            callInstructionIndex);
        targetAssemblyName = GetAssemblyNameFromSubjectId(targetMethodSubjectId);

        if (!string.Equals(GetMethodReturnType(targetMethodSubjectId), outputManagedType, StringComparison.Ordinal))
        {
            return false;
        }

        var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
        if (argCount == 0)
        {
            if (targetParameterTypes.Count != 0 &&
                (targetParameterTypes.Count != 1 ||
                 !string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal)))
            {
                return false;
            }
        }
        else if (targetParameterTypes.Count != 1 ||
                 !string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal))
        {
            return false;
        }

        if (methodsBySubjectId.TryGetValue(targetMethodSubjectId, out var targetMethod) &&
            !string.Equals(targetMethod.MethodRole, "instance-method", StringComparison.Ordinal))
        {
            return false;
        }

        return true;
    }

    private static bool TryResolveRuntimeSkeletonStaticValueTypeCallShape(
        TypedIlMethodArtifact method,
        string outputManagedType,
        out string targetMethodSubjectId,
        out string targetAssemblyName,
        out List<string> argFieldDeclarations,
        out List<string> argRefs,
        out int argCount)
    {
        targetMethodSubjectId = string.Empty;
        targetAssemblyName = string.Empty;
        argFieldDeclarations = new List<string>();
        argRefs = new List<string>();
        argCount = 0;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), outputManagedType, StringComparison.Ordinal) ||
            method.Parameters.Count < 1 ||
            method.Parameters.Count > 2)
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

        int callIndex;
        if (method.Parameters.Count == 1)
        {
            if (instructions.Count != 3 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            callIndex = 1;
        }
        else
        {
            if (instructions.Count != 4 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 1 ||
                !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            callIndex = 2;
        }

        targetMethodSubjectId = GetRequiredInstructionCallee(
            instructions[callIndex], method.SubjectId, callIndex);
        if (string.Equals(targetMethodSubjectId, method.SubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(targetMethodSubjectId), outputManagedType, StringComparison.Ordinal))
        {
            return false;
        }

        targetAssemblyName = GetAssemblyNameFromSubjectId(targetMethodSubjectId);

        // Build arg field declarations and refs for each parameter
        for (var i = 0; i < method.Parameters.Count; i++)
        {
            var parameter = method.Parameters[i];
            if (TryResolveRuntimeSkeletonValueTypeByValueInputCppType(parameter.Type, out var cppType, out _))
            {
                argFieldDeclarations.Add($"{cppType} arg{i + 1};");
            }
            else
            {
                argFieldDeclarations.Add($"void* arg{i + 1};");
            }

            argRefs.Add($"const_cast<void*>(reinterpret_cast<const void*>(&request->arg{i + 1}))");
        }

        argCount = method.Parameters.Count;
        return true;
    }

    private static bool TryResolveRuntimeSkeletonInstanceValueTypeCallShape(
        TypedIlMethodArtifact method,
        string outputManagedType,
        out string targetMethodSubjectId,
        out string targetAssemblyName,
        out bool hasNullArg,
        out List<string> argFieldDeclarations,
        out List<string> argRefs,
        out int argCount)
    {
        targetMethodSubjectId = string.Empty;
        targetAssemblyName = string.Empty;
        hasNullArg = false;
        argFieldDeclarations = new List<string>();
        argRefs = new List<string>();
        argCount = 0;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), outputManagedType, StringComparison.Ordinal) ||
            method.Parameters.Count < 1 ||
            method.Parameters.Count > 2)
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

        // Pattern: ldarg.0, [ldnull or ldarg.1], callvirt, ret
        if (method.Parameters.Count == 1)
        {
            // ToDateTime(object value) → ((IConvertible)value).ToDateTime(null)
            // IL: ldarg.0, ldnull, callvirt IConvertible::ToDateTime, ret
            if (instructions.Count != 4 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldnull", StringComparison.Ordinal) ||
                !string.Equals(instructions[2].Op, "callvirt", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            hasNullArg = true;
            argFieldDeclarations.Add("void* cdecl_this;");
            argRefs.Add("reinterpret_cast<void*>(nullptr)");
            argCount = 1;
            targetMethodSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        }
        else
        {
            // ToDateTime(object value, IFormatProvider provider) → ((IConvertible)value).ToDateTime(provider)
            // IL: ldarg.0, ldarg.1, callvirt IConvertible::ToDateTime, ret
            if (instructions.Count != 4 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 1 ||
                !string.Equals(instructions[2].Op, "callvirt", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            hasNullArg = false;
            argFieldDeclarations.Add("void* arg1;");
            argFieldDeclarations.Add("void* cdecl_this;");
            argRefs.Add("const_cast<void*>(reinterpret_cast<const void*>(&request->arg1))");
            argCount = 1;
            targetMethodSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        }

        if (string.Equals(targetMethodSubjectId, method.SubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(targetMethodSubjectId), outputManagedType, StringComparison.Ordinal))
        {
            return false;
        }

        targetAssemblyName = GetAssemblyNameFromSubjectId(targetMethodSubjectId);
        return true;
    }

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
