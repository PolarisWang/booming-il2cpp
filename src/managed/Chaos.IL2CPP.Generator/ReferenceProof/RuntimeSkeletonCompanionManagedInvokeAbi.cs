using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record RuntimeSkeletonCompanionManagedInvokeAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "companion-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonCompanionManagedInvokeAbi abi) =>
        RuntimeSkeletonCompanionManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonCompanionManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonCompanionManagedInvokeAbi abi)
    {
        abi = null!;
        var descriptor = RuntimeSkeletonSemanticRegistry.TryResolveBySubjectId(method.SubjectId);
        if (descriptor is null ||
            !string.Equals(descriptor.FamilyId, RuntimeSkeletonSemanticRegistry.CompanionManagedInvokeFamilyId, StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !RuntimeSkeletonManagedInvokeAbiCore.SupportsMethodRole(
                method.MethodRole,
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
                RuntimeSkeletonValueTypeManagedInvokeCore.IsDefaultDirectReferenceReturnType,
                includeIndirectFlag: true))
        {
            return false;
        }

        var hasThisArgument = RuntimeSkeletonManagedInvokeAbiCore.IsInstanceLike(
            method.MethodRole,
            supportsConstructors: descriptor.SupportsConstructors);
        abi = new RuntimeSkeletonCompanionManagedInvokeAbi(
            RuntimeSkeletonCompanionManagedInvokeAbi.HotUpdateStableContractId,
            hasThisArgument ? "void* this_arg;" : string.Empty,
            hasThisArgument
                ? "if (request->this_arg == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }"
                : string.Empty,
            hasThisArgument ? "request->this_arg" : "nullptr",
            argumentShapes,
            returnShape);
        return true;
    }
}
