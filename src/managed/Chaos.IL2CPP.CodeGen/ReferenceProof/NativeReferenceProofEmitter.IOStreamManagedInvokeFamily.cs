using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIOStreamManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.IO.Stream::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.MemoryStream::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.FileStream::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.BufferedStream::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.UnmanagedMemoryStream::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.Stream+", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.Stream+NullStream", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.Stream+SyncStream", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.Stream+ReadWriteTask", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.Strategies.BufferedFileStreamStrategy::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.Strategies.OSFileStreamStrategy::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.Strategies.DerivedFileStreamStrategy::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.Strategies.FileStreamStrategy::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.Strategies.AsyncWindowsFileStreamStrategy::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.Strategies.FileStreamHelpers::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.UnmanagedMemoryStreamWrapper::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.PinnedBufferMemoryStream::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.UnmanagedMemoryAccessor::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonIOStreamManagedInvokeCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("io-stream-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIOStreamManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundIOStreamManagedInvokeStub(
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

    private static bool TryBuildAssemblyBoundIOStreamManagedInvokeStub(
        string assemblyName, string subjectId, MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId, string stubName, out string stub) =>
        TryBuildAssemblyBoundGenericManagedInvokeStub(
            assemblyName, subjectId, metadataRegistration, methodsBySubjectId, stubName,
            "io-stream-managed-invoke-v1",
            IsIODirectReferenceReturnType,
            out stub);
}
