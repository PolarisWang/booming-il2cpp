using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStringBuilderManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Text.StringBuilder::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonStringBuilderManagedInvokeCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("string-builder-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStringBuilderManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStringBuilderManagedInvokeStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }
        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static bool TryBuildAssemblyBoundStringBuilderManagedInvokeStub(
        string assemblyName, string subjectId, MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId, string stubName, out string stub) =>
        TryBuildAssemblyBoundGenericManagedInvokeStub(
            assemblyName, subjectId, metadataRegistration, methodsBySubjectId, stubName,
            "string-builder-managed-invoke-v1",
            IsStringBuilderDirectReferenceReturnType,
            out stub);
}
