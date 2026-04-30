using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
{
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonNumericsManagedInvokeFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Numerics.Matrix4x4", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Numerics.Matrix3x2", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Numerics.Quaternion", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Numerics.Plane", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        var result = TryBuildRuntimeSkeletonNumericsManagedInvokeCore(buildContext);
        return result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match
            ? result
            : RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("numerics-managed-invoke-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonNumericsManagedInvokeCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundNumericsManagedInvokeStub(
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

    private static bool TryBuildAssemblyBoundNumericsManagedInvokeStub(
        string assemblyName, string subjectId, MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId, string stubName, out string stub) =>
        TryBuildAssemblyBoundGenericManagedInvokeStub(
            assemblyName, subjectId, metadataRegistration, methodsBySubjectId, stubName,
            "numerics-managed-invoke-v1",
            IsNumericsDirectReferenceReturnType,
            out stub);

    private static bool IsNumericsDirectReferenceReturnType(string managedType) =>
        managedType.StartsWith("System.Numerics.", StringComparison.Ordinal) ||
        managedType is "System.String" or "System.Object";
}
