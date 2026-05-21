using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record RuntimeSkeletonUnsafeManagedInvokeArgShape(
    string FieldDeclaration,
    string ArgReferenceExpression);

internal sealed record RuntimeSkeletonUnsafeManagedInvokeReturnShape(
    string ManagedType,
    string FieldDeclarations,
    string ValidationStatement,
    string ReturnValueDeclaration,
    string ReturnValueExpression,
    string ReturnValueSizeExpression,
    string ReturnValueIsIndirectExpression);

internal sealed record RuntimeSkeletonUnsafeManagedInvokeAbi(
    string ContractId,
    IReadOnlyList<RuntimeSkeletonUnsafeManagedInvokeArgShape> ArgumentShapes,
    RuntimeSkeletonUnsafeManagedInvokeReturnShape ReturnShape)
{
    public const string HotUpdateStableContractId = "unsafe-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonUnsafeManagedInvokeAbi abi) =>
        RuntimeSkeletonUnsafeManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonUnsafeManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonUnsafeManagedInvokeAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("/System.Runtime.CompilerServices.Unsafe::", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !IsSupportedMethodRole(method.MethodRole))
        {
            return false;
        }

        var argumentShapes = new List<RuntimeSkeletonUnsafeManagedInvokeArgShape>(method.Parameters.Count);
        for (var parameterIndex = 0; parameterIndex < method.Parameters.Count; parameterIndex++)
        {
            argumentShapes.Add(new RuntimeSkeletonUnsafeManagedInvokeArgShape(
                $"void* arg{parameterIndex};",
                $"request->arg{parameterIndex}"));
        }

        if (!TryCreateReturnShape(GetMethodReturnType(method.SubjectId), out var returnShape))
        {
            return false;
        }

        abi = new RuntimeSkeletonUnsafeManagedInvokeAbi(
            RuntimeSkeletonUnsafeManagedInvokeAbi.HotUpdateStableContractId,
            argumentShapes,
            returnShape);
        return true;
    }

    private static bool IsSupportedMethodRole(string methodRole)
    {
        return string.Equals(methodRole, "static-method", StringComparison.Ordinal) ||
               string.Equals(methodRole, "static-forwarder", StringComparison.Ordinal);
    }

    private static bool TryCreateReturnShape(
        string managedType,
        out RuntimeSkeletonUnsafeManagedInvokeReturnShape shape)
    {
        if (string.Equals(managedType, "System.Void", StringComparison.Ordinal))
        {
            shape = new RuntimeSkeletonUnsafeManagedInvokeReturnShape(
                managedType,
                string.Empty,
                string.Empty,
                string.Empty,
                "nullptr",
                "0u",
                "false");
            return true;
        }

        shape = new RuntimeSkeletonUnsafeManagedInvokeReturnShape(
            managedType,
            "void* return_value;\n    CHAOS_IL2CPP_SIZE return_value_size;\n    bool return_value_is_indirect;",
            "if (request->return_value == nullptr || request->return_value_size == 0u) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }",
            "void* return_value_ptr = request->return_value;",
            "request->return_value_is_indirect ? static_cast<void*>(&return_value_ptr) : request->return_value",
            "request->return_value_size",
            "request->return_value_is_indirect");
        return true;
    }

    private static string GetMethodReturnType(string subjectId)
    {
        var parameterStart = subjectId.IndexOf('(', StringComparison.Ordinal);
        if (parameterStart < 0)
        {
            return string.Empty;
        }

        var returnStart = subjectId.LastIndexOf(':', parameterStart - 1);
        if (returnStart < 0 || parameterStart <= returnStart + 1)
        {
            return string.Empty;
        }

        return subjectId[(returnStart + 1)..parameterStart];
    }
}
