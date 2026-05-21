using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record RuntimeSkeletonNumberManagedInvokeAbi(
    string ContractId,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "number-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonNumberManagedInvokeAbi abi) =>
        RuntimeSkeletonNumberManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonNumberManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonNumberManagedInvokeAbi abi)
    {
        abi = null!;
        var descriptor = RuntimeSkeletonSemanticRegistry.TryResolveBySubjectId(method.SubjectId);
        if (descriptor is null ||
            !string.Equals(descriptor.FamilyId, RuntimeSkeletonSemanticRegistry.NumberFormattingFamilyId, StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !RuntimeSkeletonManagedInvokeAbiCore.SupportsMethodRole(
                method.MethodRole,
                supportsStaticForwarder: descriptor.SupportsStaticForwarder,
                supportsInstanceMethods: descriptor.SupportsInstanceMethods,
                supportsConstructors: descriptor.SupportsConstructors))
        {
            return false;
        }

        if (RuntimeSkeletonBackendSelector.SelectBackend(method, descriptor) != RuntimeSkeletonExecutionBackendKind.ManagedInvoke)
        {
            return false;
        }

        var argumentShapes = RuntimeSkeletonManagedInvokeAbiCore.CreatePointerBackedArgumentShapes(method);
        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                RuntimeSkeletonManagedInvokeAbiCore.GetMethodReturnType(method.SubjectId),
                out var returnShape,
                IsDirectReferenceReturnType,
                includeIndirectFlag: false))
        {
            return false;
        }

        abi = new RuntimeSkeletonNumberManagedInvokeAbi(
            RuntimeSkeletonNumberManagedInvokeAbi.HotUpdateStableContractId,
            argumentShapes,
            returnShape);
        return true;
    }

    private static bool IsDirectReferenceReturnType(string managedType)
    {
        return managedType is "System.String" ||
               managedType.EndsWith("*", StringComparison.Ordinal);
    }
}
