using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIOCommonManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.IO.IOException::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.EndOfStreamException::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.FileNotFoundException::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.DirectoryNotFoundException::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.InvalidDataException::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.FileLoadException::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.FileSystem::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.FileSystemInfo::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.RandomAccess::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.Enumeration.", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.EnumerationOptions::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.DriveInfoInternal::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.FileStreamOptions::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.Win32Marshal::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.ReadLinesIterator::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.EncodingCache::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.DisableMediaInsertionPrompt::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonIOCommonManagedInvokeCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("io-common-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIOCommonManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundIOCommonManagedInvokeStub(
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

    private static bool TryBuildAssemblyBoundIOCommonManagedInvokeStub(
        string assemblyName, string subjectId, MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId, string stubName, out string stub) =>
        TryBuildAssemblyBoundGenericManagedInvokeStub(
            assemblyName, subjectId, metadataRegistration, methodsBySubjectId, stubName,
            "io-common-managed-invoke-v1",
            IsIODirectReferenceReturnType,
            out stub);
}
