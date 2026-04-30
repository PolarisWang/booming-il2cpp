using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonBuffersManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Buffers.", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonBuffersManagedInvokeCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("buffers-managed-invoke-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonBuffersManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundBuffersManagedInvokeStub(
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

    private static bool TryBuildAssemblyBoundBuffersManagedInvokeStub(
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
                "buffers-managed-invoke-v1",
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
