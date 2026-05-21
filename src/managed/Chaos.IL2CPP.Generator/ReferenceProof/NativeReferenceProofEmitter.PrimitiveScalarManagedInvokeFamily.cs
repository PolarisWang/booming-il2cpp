using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonPrimitiveScalarManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Boolean::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Byte::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.SByte::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Int16::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.UInt16::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Int32::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.UInt32::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Int64::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.UInt64::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.IntPtr::", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.UIntPtr::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonPrimitiveScalarManagedInvokeHandler(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("primitive-scalar-managed-invoke-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonPrimitiveScalarManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundPrimitiveScalarManagedInvokeStub(
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

    private static bool TryBuildAssemblyBoundPrimitiveScalarManagedInvokeCore(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !RuntimeSkeletonPrimitiveScalarManagedInvokeAbi.TryCreate(method, out var abi))
        {
            return false;
        }

        return TryRenderValueTypeManagedInvokeStub(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonPrimitiveScalarManagedInvokeStubTemplateRelativePath),
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
