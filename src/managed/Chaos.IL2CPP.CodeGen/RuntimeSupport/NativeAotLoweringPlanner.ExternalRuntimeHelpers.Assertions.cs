using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    private const string KnownEqualityAssertionContractNamespace = "Chaos.TestFramework";
    private const string KnownEqualityAssertionContractTypeName = "Assert";
    private const string KnownEqualityAssertionFailureStateTypeName = "ChaosAssertState";
    private const string KnownEqualityAssertionMethodName = "Equal";
    private const string KnownEqualityAssertionFailureStateExitCodeFieldName = "ExitCode";

    internal static IReadOnlyList<string> CreateEqualityAssertionRuntimeHelperBodyLines(
        AotCoreIrAbiSlotArtifact comparedAbi,
        IReadOnlyList<string> failureBodyLines)
    {
        if (string.Equals(comparedAbi.TypeSubjectId, StringTypeSubjectId, StringComparison.Ordinal))
        {
            return
            [
                "    (void)chaos_arg_2;",
                "    bool chaos_equals = false;",
                "    if (chaos_arg_0 == chaos_arg_1)",
                "    {",
                "        chaos_equals = true;",
                "    }",
                "    else if (chaos_arg_0 != static_cast<CHAOS_IL2CPP_INTPTR>(0) && chaos_arg_1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))",
                "    {",
                "        auto* chaos_left_string = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0);",
                "        auto* chaos_right_string = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_1);",
                "        if (chaos_left_string->length == chaos_right_string->length)",
                "        {",
                "            if (chaos_left_string->utf8_data == nullptr || chaos_right_string->utf8_data == nullptr)",
                "            {",
                "                chaos_equals = chaos_left_string->utf8_data == chaos_right_string->utf8_data;",
                "            }",
                "            else",
                "            {",
                "                chaos_equals = CHAOS_IL2CPP_STRCMP(chaos_left_string->utf8_data, chaos_right_string->utf8_data) == 0;",
                "            }",
                "        }",
                "    }",
                string.Empty,
                "    if (chaos_equals)",
                "    {",
                "        return;",
                "    }",
                string.Empty,
                .. failureBodyLines,
            ];
        }

        return
        [
            "    (void)chaos_arg_2;",
            "    if (chaos_arg_0 == chaos_arg_1)",
            "    {",
            "        return;",
            "    }",
            string.Empty,
            .. failureBodyLines,
        ];
    }

    internal static IReadOnlyList<string> CreateFailureStateWriteBodyLines(string failureStateFieldSymbol)
    {
        return
        [
            $"{failureStateFieldSymbol} = static_cast<CHAOS_IL2CPP_INTPTR>(1);",
        ];
    }

    internal static bool IsSupportedEqualityAssertionAbi(AotCoreIrAbiSlotArtifact comparedAbi)
    {
        if (string.Equals(comparedAbi.TypeSubjectId, StringTypeSubjectId, StringComparison.Ordinal))
        {
            return true;
        }

        return comparedAbi.CarrierKindCode is
            AotCoreIrAbiCarrierKind.Int8 or
            AotCoreIrAbiCarrierKind.UInt8 or
            AotCoreIrAbiCarrierKind.Int16 or
            AotCoreIrAbiCarrierKind.UInt16 or
            AotCoreIrAbiCarrierKind.Int32 or
            AotCoreIrAbiCarrierKind.NativeInt or
            AotCoreIrAbiCarrierKind.Float32 or
            AotCoreIrAbiCarrierKind.Float64 or
            AotCoreIrAbiCarrierKind.Int64 or
            AotCoreIrAbiCarrierKind.UInt64;
    }

    internal static bool TryParseKnownEqualityAssertionContract(
        string? subjectId,
        out EqualityAssertionRuntimeHelperSpec assertionSpec)
    {
        assertionSpec = default;
        if (string.IsNullOrEmpty(subjectId))
        {
            return false;
        }

        try
        {
            var declaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(subjectId);
            if (!string.Equals(
                    GetTypeDisplayName(declaringTypeSubjectId),
                    $"{KnownEqualityAssertionContractNamespace}.{KnownEqualityAssertionContractTypeName}",
                    StringComparison.Ordinal))
            {
                return false;
            }

            if (!TryReadKnownEqualityAssertionComparedType(subjectId, out var comparedTypeNameOrSubjectId) ||
                !TryCreateEqualityAssertionFailureStateFieldSubjectId(
                    GetAssemblyNameFromSubjectId(declaringTypeSubjectId),
                    out var failureStateFieldSubjectId))
            {
                return false;
            }

            assertionSpec = new EqualityAssertionRuntimeHelperSpec(
                comparedTypeNameOrSubjectId,
                failureStateFieldSubjectId);
            return true;
        }
        catch (InvalidOperationException)
        {
            return false;
        }
    }

    private static bool TryCreateEqualityAssertionFailureStateFieldSubjectId(
        string assemblyName,
        out string failureStateFieldSubjectId)
    {
        failureStateFieldSubjectId = string.Empty;
        if (string.IsNullOrEmpty(assemblyName))
        {
            return false;
        }

        var failureStateTypeSubjectId = ManagedNaming.CreateTypeSubjectId(
            assemblyName,
            KnownEqualityAssertionContractNamespace,
            KnownEqualityAssertionFailureStateTypeName);
        failureStateFieldSubjectId = ManagedNaming.CreateFieldSubjectId(
            failureStateTypeSubjectId,
            KnownEqualityAssertionFailureStateExitCodeFieldName);
        return true;
    }

    private static bool TryReadKnownEqualityAssertionComparedType(
        string subjectId,
        out string comparedTypeNameOrSubjectId)
    {
        comparedTypeNameOrSubjectId = string.Empty;
        var methodName = GetMethodName(subjectId);
        var parameterTypes = GetMethodParameterTypes(subjectId);
        if (string.Equals(methodName, KnownEqualityAssertionMethodName, StringComparison.Ordinal))
        {
            if (!MatchesEqualityAssertionParameterTypes(parameterTypes, "System.Int32"))
            {
                return false;
            }

            comparedTypeNameOrSubjectId = "System.Int32";
            return true;
        }

        return TryReadEqualityAssertionComparedType(methodName, out comparedTypeNameOrSubjectId) &&
               MatchesEqualityAssertionParameterTypes(parameterTypes, comparedTypeNameOrSubjectId);
    }

    private static bool TryReadEqualityAssertionComparedType(
        string methodName,
        out string comparedTypeNameOrSubjectId)
    {
        comparedTypeNameOrSubjectId = string.Empty;
        return TryReadSingleGenericTypeArgument(
                   methodName,
                   $"{KnownEqualityAssertionMethodName}<",
                   out comparedTypeNameOrSubjectId) &&
               !string.IsNullOrWhiteSpace(comparedTypeNameOrSubjectId);
    }

    private static bool MatchesEqualityAssertionParameterTypes(
        IReadOnlyList<string> parameterTypes,
        string comparedTypeNameOrSubjectId)
    {
        return parameterTypes.Count == 3 &&
               string.Equals(parameterTypes[0], comparedTypeNameOrSubjectId, StringComparison.Ordinal) &&
               string.Equals(parameterTypes[1], comparedTypeNameOrSubjectId, StringComparison.Ordinal) &&
               string.Equals(parameterTypes[2], "System.String", StringComparison.Ordinal);
    }

    internal readonly record struct EqualityAssertionRuntimeHelperSpec(
        string ComparedTypeNameOrSubjectId,
        string FailureStateFieldSubjectId);
}
