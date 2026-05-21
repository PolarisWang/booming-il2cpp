using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record RuntimeSkeletonMemoryExtensionsManagedInvokeArgShape(
    string FieldDeclaration,
    string ValidationStatement,
    string ArgReferenceExpression);

internal sealed record RuntimeSkeletonMemoryExtensionsManagedInvokeReturnShape(
    string ManagedType,
    string FieldDeclarations,
    string ValidationStatement,
    string ReturnValueDeclaration,
    string ReturnValueArgument,
    string ReturnValueSizeExpression);

internal sealed record RuntimeSkeletonMemoryExtensionsManagedInvokeAbi(
    string ContractId,
    IReadOnlyList<RuntimeSkeletonMemoryExtensionsManagedInvokeArgShape> ArgumentShapes,
    RuntimeSkeletonMemoryExtensionsManagedInvokeReturnShape ReturnShape)
{
    public const string HotUpdateStableContractId = "memory-extensions-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonMemoryExtensionsManagedInvokeAbi abi) =>
        RuntimeSkeletonMemoryExtensionsManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonMemoryExtensionsManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonMemoryExtensionsManagedInvokeAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("/System.MemoryExtensions::", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal))
        {
            return false;
        }

        var argumentShapes = new List<RuntimeSkeletonMemoryExtensionsManagedInvokeArgShape>(method.Parameters.Count);
        for (var parameterIndex = 0; parameterIndex < method.Parameters.Count; parameterIndex++)
        {
            argumentShapes.Add(new RuntimeSkeletonMemoryExtensionsManagedInvokeArgShape(
                $"void* arg{parameterIndex};",
                $"if (request->arg{parameterIndex} == nullptr) {{\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }}",
                $"request->arg{parameterIndex}"));
        }

        if (!TryCreateReturnShape(GetMethodReturnType(method.SubjectId), out var returnShape))
        {
            return false;
        }

        abi = new RuntimeSkeletonMemoryExtensionsManagedInvokeAbi(
            RuntimeSkeletonMemoryExtensionsManagedInvokeAbi.HotUpdateStableContractId,
            argumentShapes,
            returnShape);
        return true;
    }

    private static bool TryCreateReturnShape(
        string managedType,
        out RuntimeSkeletonMemoryExtensionsManagedInvokeReturnShape shape)
    {
        if (string.Equals(managedType, "System.Void", StringComparison.Ordinal))
        {
            shape = new RuntimeSkeletonMemoryExtensionsManagedInvokeReturnShape(
                managedType,
                string.Empty,
                string.Empty,
                string.Empty,
                "nullptr",
                "0u");
            return true;
        }

        if (IsDirectReferenceReturnType(managedType))
        {
            shape = new RuntimeSkeletonMemoryExtensionsManagedInvokeReturnShape(
                managedType,
                "void** return_value;",
                "if (request->return_value == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }",
                string.Empty,
                "request->return_value",
                "sizeof(void*)");
            return true;
        }

        shape = new RuntimeSkeletonMemoryExtensionsManagedInvokeReturnShape(
            managedType,
            "void* return_value;\n    CHAOS_IL2CPP_SIZE return_value_size;",
            "if (request->return_value == nullptr || request->return_value_size == 0u) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }",
            "void* return_value_ptr = request->return_value;",
            "&return_value_ptr",
            "request->return_value_size");
        return true;
    }

    private static bool IsDirectReferenceReturnType(string managedType)
    {
        if (managedType.EndsWith("[]", StringComparison.Ordinal))
        {
            return true;
        }

        if (managedType.StartsWith("System.Collections.Generic.IEnumerator<", StringComparison.Ordinal) ||
            managedType.StartsWith("System.Buffers.MemoryManager<", StringComparison.Ordinal) ||
            managedType.StartsWith("System.Threading.Tasks.Sources.IValueTaskSource<", StringComparison.Ordinal))
        {
            return true;
        }

        return managedType is
            "System.String" or
            "System.Object" or
            "System.Array" or
            "System.MulticastDelegate" or
            "System.ComAwareWeakReference" or
            "System.RuntimeType+RuntimeTypeCache" or
            "System.Threading.IThreadPoolWorkItem" or
            "System.Threading.Tasks.Sources.IValueTaskSource" or
            "System.Threading.Tasks.Task";
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
