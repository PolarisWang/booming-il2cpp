using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonDebugViewManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Diagnostics.DebugView", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Collections.Generic.DebugView", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonDebugViewManagedInvokeCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("debugview-managed-invoke-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonDebugViewManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundDebugViewManagedInvokeStub(
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

    private static bool TryBuildAssemblyBoundDebugViewManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundDebugViewManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundDebugViewManagedInvokeCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !RuntimeSkeletonGenericManagedInvokeAbiFactory.TryCreate(
                method,
                "debugview-managed-invoke-v1",
                out var abi))
        {
            return false;
        }

        return TryRenderValueTypeManagedInvokeStub(
            NativeReferenceProofCatalog.GetRuntimeSkeletonIdentityStructManagedInvokeStubTemplate(),
            assemblyName,
            subjectId,
            metadataRegistration,
            stubName,
            abi.ContractId,
            abi.ThisFieldDeclaration,
            abi.ThisValidationStatement,
            abi.ThisArgumentExpression,
            abi.ArgumentShapes,
            abi.ArgumentValidationStatements,
            abi.ArgumentCount,
            abi.ArgumentStorageSize,
            abi.ReturnShape,
            out stub);
    }
}
