using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIOTextReaderWriterManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.IO.TextReader::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.TextWriter::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.StreamReader::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.StreamWriter::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.TextReader+", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.TextWriter+", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.TextWriter+SyncTextWriter", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.TextWriter+NullTextWriter", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.TextReader+SyncTextReader", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.StreamReader+NullStreamReader", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.StringReader::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IO.StringWriter::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonIOTextReaderWriterManagedInvokeCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("io-text-reader-writer-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIOTextReaderWriterManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundIOTextReaderWriterManagedInvokeStub(
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

    private static bool TryBuildAssemblyBoundIOTextReaderWriterManagedInvokeStub(
        string assemblyName, string subjectId, MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId, string stubName, out string stub) =>
        TryBuildAssemblyBoundGenericManagedInvokeStub(
            assemblyName, subjectId, metadataRegistration, methodsBySubjectId, stubName,
            "io-text-reader-writer-managed-invoke-v1",
            IsIODirectReferenceReturnType,
            out stub);
}
