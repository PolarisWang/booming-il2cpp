using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonTracingManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Diagnostics.Tracing.", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonTracingManagedInvokeCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("tracing-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonTracingManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundTracingManagedInvokeStub(
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

    private static bool TryBuildAssemblyBoundTracingManagedInvokeStub(
        string assemblyName, string subjectId, MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId, string stubName, out string stub) =>
        TryBuildAssemblyBoundGenericManagedInvokeStub(
            assemblyName, subjectId, metadataRegistration, methodsBySubjectId, stubName,
            "tracing-managed-invoke-v1",
            IsTracingDirectReferenceReturnType,
            out stub);
}
