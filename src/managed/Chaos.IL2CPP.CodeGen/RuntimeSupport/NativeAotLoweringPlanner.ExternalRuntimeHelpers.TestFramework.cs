using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    private const string LegacyTestFrameworkIntEqualMethodSubjectId =
        "Chaos.TestFramework/Assert::Equal(System.Int32,System.Int32,System.String)";

    private const string LegacyTestFrameworkAssertStateExitCodeFieldSubjectId =
        "Chaos.TestFramework/ChaosAssertState::ExitCode";

    private const string SdkTestFrameworkIntEqualMethodSubjectId =
        "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Equal(System.Int32,System.Int32,System.String)";

    private const string SdkTestFrameworkAssertStateExitCodeFieldSubjectId =
        "Chaos.TestFramework.Sdk/Chaos.TestFramework.ChaosAssertState::ExitCode";

    private bool TryCreateTestFrameworkRuntimeHelperDefinition(
        string callee,
        out ExternalRuntimeHelperDefinition? helperDefinition)
    {
        helperDefinition = null;
        if (!TryGetTestFrameworkAssertStateFieldSubjectId(callee, out var assertStateFieldSubjectId) ||
            string.IsNullOrWhiteSpace(assertStateFieldSubjectId))
        {
            return false;
        }

        helperDefinition = new ExternalRuntimeHelperDefinition(
            callee,
            GetExternalRuntimeHelperSymbol(callee),
            $"extern \"C\" void {GetExternalRuntimeHelperSymbol(callee)}(std::int32_t chaos_arg_0, std::int32_t chaos_arg_1, std::intptr_t chaos_arg_2)\n" +
            "{\n" +
            "    (void)chaos_arg_2;\n" +
            "    if (chaos_arg_0 == chaos_arg_1)\n" +
            "    {\n" +
            "        return;\n" +
            "    }\n" +
            "\n" +
            $"    {GetNativeStaticFieldSymbol(assertStateFieldSubjectId)} = static_cast<std::intptr_t>(1);\n" +
            "}",
            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateInt32AbiSlot(),
                    CreateInt32AbiSlot(),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                }),
            new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
                TypeShape = (AotCoreIrTypeShapeKind)0,
            },
            EmptyRawArgumentIndices);
        return true;
    }

    private static bool TryGetTestFrameworkAssertStateFieldSubjectId(
        string? subjectId,
        out string? fieldSubjectId)
    {
        fieldSubjectId = subjectId switch
        {
            LegacyTestFrameworkIntEqualMethodSubjectId => LegacyTestFrameworkAssertStateExitCodeFieldSubjectId,
            SdkTestFrameworkIntEqualMethodSubjectId => SdkTestFrameworkAssertStateExitCodeFieldSubjectId,
            _ => null,
        };

        return !string.IsNullOrWhiteSpace(fieldSubjectId);
    }
}
