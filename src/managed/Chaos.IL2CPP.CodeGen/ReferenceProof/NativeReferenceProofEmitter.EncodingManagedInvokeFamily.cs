using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonEncodingManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Text.Encoding::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.Encoding+", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.Decoder::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.Encoder::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.ASCIIEncoding::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.ASCIIEncoding+", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.Latin1Encoding::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.Latin1Encoding+", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.UTF8Encoding::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.UTF8Encoding+", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.UnicodeEncoding::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.UnicodeEncoding+", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.UTF32Encoding::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.UTF32Encoding+", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.UTF7Encoding::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.UTF7Encoding+", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.DecoderNLS::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.DecoderExceptionFallback::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.DecoderFallback::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.DecoderFallbackBuffer::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.DecoderFallbackException::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.DecoderReplacementFallback::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.DecoderReplacementFallbackBuffer::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.EncoderNLS::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.EncoderExceptionFallback::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.EncoderFallback::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.EncoderFallbackBuffer::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.EncoderFallbackException::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.EncoderLatin1BestFitFallback::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.EncoderLatin1BestFitFallbackBuffer::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.EncoderReplacementFallback::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.EncoderReplacementFallbackBuffer::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.EncodingInfo::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.EncodingProvider::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Text.EncodingTable::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonEncodingManagedInvokeCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("encoding-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonEncodingManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundEncodingManagedInvokeStub(
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

    private static bool TryBuildAssemblyBoundEncodingManagedInvokeStub(
        string assemblyName, string subjectId, MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId, string stubName, out string stub) =>
        TryBuildAssemblyBoundGenericManagedInvokeStub(
            assemblyName, subjectId, metadataRegistration, methodsBySubjectId, stubName,
            "encoding-managed-invoke-v1",
            IsEncodingDirectReferenceReturnType,
            out stub);
}
