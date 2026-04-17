using System.Globalization;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Linker;

public sealed partial class LinkerStage
{
    private static string? TryResolveDevirtualizedCallTarget(
        IReadOnlyDictionary<string, ManagedMethodModel> methodMap,
        IReadOnlyList<ManagedInstructionModel> instructions,
        int instructionIndex,
        string declaredTargetSubjectId)
    {
        if (instructionIndex <= 0 ||
            !methodMap.TryGetValue(declaredTargetSubjectId, out var declaredMethod))
        {
            return null;
        }

        var constructorTypeSubjectId = TryResolveReceiverConstructorTypeSubjectId(
            instructions,
            instructionIndex,
            declaredMethod.Parameters.Count);
        if (string.IsNullOrWhiteSpace(constructorTypeSubjectId))
        {
            return null;
        }
        var declaredTypeSubjectId = GetDeclaringTypeSubjectId(declaredTargetSubjectId);
        if (string.Equals(constructorTypeSubjectId, declaredTypeSubjectId, StringComparison.Ordinal))
        {
            return declaredTargetSubjectId;
        }

        var exactMatch = methodMap.Values.FirstOrDefault(candidate =>
            string.Equals(candidate.DeclaringTypeSubjectId, constructorTypeSubjectId, StringComparison.Ordinal) &&
            string.Equals(candidate.Name, declaredMethod.Name, StringComparison.Ordinal) &&
            candidate.Body.Blocks.Any(block => block.Instructions.Count > 0) &&
            candidate.Parameters.Select(parameter => parameter.Type).SequenceEqual(
                declaredMethod.Parameters.Select(parameter => parameter.Type),
                StringComparer.Ordinal))
            ?.SubjectId;
        if (!string.IsNullOrWhiteSpace(exactMatch))
        {
            return exactMatch;
        }

        return methodMap.Values.FirstOrDefault(candidate =>
            string.Equals(candidate.DeclaringTypeSubjectId, constructorTypeSubjectId, StringComparison.Ordinal) &&
            string.Equals(candidate.Name, declaredMethod.Name, StringComparison.Ordinal) &&
            candidate.Body.Blocks.Any(block => block.Instructions.Count > 0))
            ?.SubjectId;
    }

    private static string? TryResolveReceiverConstructorTypeSubjectId(
        IReadOnlyList<ManagedInstructionModel> instructions,
        int instructionIndex,
        int parameterCount)
    {
        var receiverInstructionIndex = instructionIndex - parameterCount - 1;
        if (receiverInstructionIndex < 0)
        {
            return null;
        }

        var receiverInstruction = instructions[receiverInstructionIndex];
        if (string.Equals(receiverInstruction.Op, "newobj", StringComparison.Ordinal) &&
            !string.IsNullOrWhiteSpace(receiverInstruction.Callee))
        {
            return GetDeclaringTypeSubjectId(receiverInstruction.Callee);
        }

        if (!string.Equals(receiverInstruction.Op, "ldloc", StringComparison.Ordinal) ||
            !TryGetLocalIndex(receiverInstruction.Operand, out var localIndex))
        {
            return null;
        }

        for (var instructionCursor = receiverInstructionIndex - 1; instructionCursor > 0; instructionCursor--)
        {
            var storeInstruction = instructions[instructionCursor];
            if (!string.Equals(storeInstruction.Op, "stloc", StringComparison.Ordinal) ||
                !TryGetLocalIndex(storeInstruction.Operand, out var storedLocalIndex) ||
                storedLocalIndex != localIndex)
            {
                continue;
            }

            var valueProducer = instructions[instructionCursor - 1];
            if (string.Equals(valueProducer.Op, "newobj", StringComparison.Ordinal) &&
                !string.IsNullOrWhiteSpace(valueProducer.Callee))
            {
                return GetDeclaringTypeSubjectId(valueProducer.Callee);
            }

            return null;
        }

        return null;
    }

    private static bool TryGetLocalIndex(object? operand, out int localIndex)
    {
        switch (operand)
        {
            case int intValue:
                localIndex = intValue;
                return true;
            case long longValue when longValue is >= int.MinValue and <= int.MaxValue:
                localIndex = (int)longValue;
                return true;
            case string stringValue when int.TryParse(stringValue, NumberStyles.Integer, CultureInfo.InvariantCulture, out var parsedLocalIndex):
                localIndex = parsedLocalIndex;
                return true;
            default:
                localIndex = default;
                return false;
        }
    }

    private static bool MatchesVirtualDispatchSignature(
        ManagedMethodModel candidateMethod,
        ManagedMethodModel slotMethod)
    {
        return !candidateMethod.IsStatic &&
               string.Equals(candidateMethod.Name, slotMethod.Name, StringComparison.Ordinal) &&
               string.Equals(candidateMethod.ReturnType, slotMethod.ReturnType, StringComparison.Ordinal) &&
               candidateMethod.Parameters.Select(parameter => parameter.Type).SequenceEqual(
                   slotMethod.Parameters.Select(parameter => parameter.Type),
                   StringComparer.Ordinal);
    }

    private static bool ImplementsInterface(
        ManagedTypeModel candidateType,
        ManagedTypeModel interfaceType,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap)
    {
        return candidateType.ImplementedInterfaceSubjectIds is not null &&
               candidateType.ImplementedInterfaceSubjectIds.Any(candidateInterfaceSubjectId =>
                   MatchesInterfaceSubjectId(candidateInterfaceSubjectId, interfaceType, typeMap));
    }

    private static bool MatchesInterfaceSubjectId(
        string candidateInterfaceSubjectId,
        ManagedTypeModel interfaceType,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap)
    {
        if (string.Equals(candidateInterfaceSubjectId, interfaceType.SubjectId, StringComparison.Ordinal) ||
            string.Equals(candidateInterfaceSubjectId, interfaceType.DefinitionSubjectId, StringComparison.Ordinal))
        {
            return true;
        }

        return typeMap.TryGetValue(candidateInterfaceSubjectId, out var candidateInterfaceType) &&
               (string.Equals(candidateInterfaceType.SubjectId, interfaceType.SubjectId, StringComparison.Ordinal) ||
                string.Equals(candidateInterfaceType.DefinitionSubjectId, interfaceType.DefinitionSubjectId, StringComparison.Ordinal));
    }

    private static bool IsCompatibleVirtualDispatchTargetType(
        ManagedTypeModel candidateType,
        ManagedTypeModel slotType,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap)
    {
        return slotType.IsInterface
            ? ImplementsInterface(candidateType, slotType, typeMap)
            : DerivesFromOrMatchesType(candidateType, slotType, typeMap);
    }

    private static bool DerivesFromOrMatchesType(
        ManagedTypeModel candidateType,
        ManagedTypeModel slotType,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap)
    {
        var currentSubjectId = candidateType.SubjectId;
        var currentDefinitionSubjectId = candidateType.DefinitionSubjectId;

        while (!string.IsNullOrWhiteSpace(currentSubjectId))
        {
            if (MatchesTypeSubjectId(currentSubjectId, currentDefinitionSubjectId, slotType))
            {
                return true;
            }

            var baseTypeSubjectId = TryResolveBaseTypeSubjectId(currentSubjectId, currentDefinitionSubjectId, typeMap);
            if (string.IsNullOrWhiteSpace(baseTypeSubjectId))
            {
                return false;
            }

            if (typeMap.TryGetValue(baseTypeSubjectId, out var baseType))
            {
                currentSubjectId = baseType.SubjectId;
                currentDefinitionSubjectId = baseType.DefinitionSubjectId;
                continue;
            }

            currentSubjectId = baseTypeSubjectId;
            currentDefinitionSubjectId = baseTypeSubjectId;
        }

        return false;
    }

    private static string? TryResolveBaseTypeSubjectId(
        string currentSubjectId,
        string currentDefinitionSubjectId,
        IReadOnlyDictionary<string, ManagedTypeModel> typeMap)
    {
        if (typeMap.TryGetValue(currentSubjectId, out var currentType) &&
            !string.IsNullOrWhiteSpace(currentType.BaseTypeSubjectId))
        {
            return currentType.BaseTypeSubjectId;
        }

        if (!string.IsNullOrWhiteSpace(currentDefinitionSubjectId) &&
            typeMap.TryGetValue(currentDefinitionSubjectId, out var currentDefinitionType) &&
            !string.IsNullOrWhiteSpace(currentDefinitionType.BaseTypeSubjectId))
        {
            return currentDefinitionType.BaseTypeSubjectId;
        }

        return null;
    }

    private static bool MatchesTypeSubjectId(
        string? candidateSubjectId,
        string? candidateDefinitionSubjectId,
        ManagedTypeModel slotType)
    {
        return (!string.IsNullOrWhiteSpace(candidateSubjectId) &&
                (string.Equals(candidateSubjectId, slotType.SubjectId, StringComparison.Ordinal) ||
                 string.Equals(candidateSubjectId, slotType.DefinitionSubjectId, StringComparison.Ordinal))) ||
               (!string.IsNullOrWhiteSpace(candidateDefinitionSubjectId) &&
                (string.Equals(candidateDefinitionSubjectId, slotType.SubjectId, StringComparison.Ordinal) ||
                 string.Equals(candidateDefinitionSubjectId, slotType.DefinitionSubjectId, StringComparison.Ordinal)));
    }

}
