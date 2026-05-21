using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIOPathManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.IO.Path::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.Directory::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.File::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.DirectoryInfo::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.FileInfo::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.DriveInfo::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.PathHelper::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.PathInternal::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonIOPathManagedInvokeCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("io-path-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIOPathManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundIOPathManagedInvokeStub(
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

    private static bool TryBuildAssemblyBoundIOPathManagedInvokeStub(
        string assemblyName, string subjectId, MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId, string stubName, out string stub) =>
        TryBuildAssemblyBoundGenericManagedInvokeStub(
            assemblyName, subjectId, metadataRegistration, methodsBySubjectId, stubName,
            "io-path-managed-invoke-v1",
            IsIODirectReferenceReturnType,
            out stub);
}
