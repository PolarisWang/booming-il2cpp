using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonValueTypeManagedInvokeDescriptor(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));
}

internal static class RuntimeSkeletonValueTypeManagedInvokeCore
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        string expectedFamilyId,
        string contractId,
        out RuntimeSkeletonValueTypeManagedInvokeDescriptor descriptor,
        Func<string, bool>? isDirectReferenceReturnType = null)
    {
        descriptor = null!;
        var semanticDescriptor = RuntimeSkeletonSemanticRegistry.TryResolveBySubjectId(method.SubjectId);
        if (semanticDescriptor is null ||
            !string.Equals(semanticDescriptor.FamilyId, expectedFamilyId, StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !RuntimeSkeletonManagedInvokeAbiCore.SupportsMethodRole(
                method.MethodRole,
                supportsStaticForwarder: semanticDescriptor.SupportsStaticForwarder,
                supportsInstanceMethods: semanticDescriptor.SupportsInstanceMethods,
                supportsConstructors: semanticDescriptor.SupportsConstructors,
                supportsInstanceFieldGetters: semanticDescriptor.SupportsInstanceFieldGetters))
        {
            return false;
        }

        if (RuntimeSkeletonBackendSelector.SelectBackend(method, semanticDescriptor) != RuntimeSkeletonExecutionBackendKind.ManagedInvoke)
        {
            return false;
        }

        var argumentShapes = RuntimeSkeletonManagedInvokeAbiCore.CreatePointerBackedArgumentShapes(method);
        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                RuntimeSkeletonManagedInvokeAbiCore.GetMethodReturnType(method.SubjectId),
                out var returnShape,
                isDirectReferenceReturnType,
                includeIndirectFlag: true))
        {
            return false;
        }

        var hasThisArgument = RuntimeSkeletonManagedInvokeAbiCore.IsInstanceLike(
            method.MethodRole,
            supportsConstructors: semanticDescriptor.SupportsConstructors,
            supportsInstanceFieldGetters: semanticDescriptor.SupportsInstanceFieldGetters);
        descriptor = new RuntimeSkeletonValueTypeManagedInvokeDescriptor(
            contractId,
            hasThisArgument ? "void* this_arg;" : string.Empty,
            hasThisArgument
                ? "if (request->this_arg == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }"
                : string.Empty,
            hasThisArgument ? "request->this_arg" : "nullptr",
            argumentShapes,
            returnShape);
        return true;
    }

    public static bool IsDefaultDirectReferenceReturnType(string managedType)
    {
        return managedType is "System.String" or "System.Object" ||
               managedType.EndsWith("&", StringComparison.Ordinal) ||
               managedType.EndsWith("*", StringComparison.Ordinal) ||
               managedType.EndsWith("[]", StringComparison.Ordinal);
    }
}
