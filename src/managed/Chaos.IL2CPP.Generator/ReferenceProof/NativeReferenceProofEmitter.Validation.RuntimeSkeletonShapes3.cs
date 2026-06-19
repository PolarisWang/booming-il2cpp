using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{



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

}
