using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonArrayAndMemoryMarshalArgShape(
    string FieldDeclaration,
    string ValidationStatement,
    string ArgReferenceExpression);

internal sealed record RuntimeSkeletonArrayAndMemoryMarshalReturnShape(
    string ManagedType,
    string FieldDeclarations,
    string ValidationStatement,
    string ReturnValueDeclaration,
    string ReturnValueArgument,
    string ReturnValueSizeExpression,
    string ReturnValueIsIndirectExpression);

internal sealed record RuntimeSkeletonArrayAndMemoryMarshalAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonArrayAndMemoryMarshalArgShape> ArgumentShapes,
    RuntimeSkeletonArrayAndMemoryMarshalReturnShape ReturnShape)
{
    public const string HotUpdateStableContractId = "array-memorymarshal-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonArrayAndMemoryMarshalAbi abi) =>
        RuntimeSkeletonArrayAndMemoryMarshalAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonArrayAndMemoryMarshalAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonArrayAndMemoryMarshalAbi abi)
    {
        abi = null!;
        var isArray = method.SubjectId.Contains("/System.Array::", StringComparison.Ordinal);
        var isMemoryMarshal = method.SubjectId.Contains("/System.Runtime.InteropServices.MemoryMarshal::", StringComparison.Ordinal);
        if ((!isArray && !isMemoryMarshal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !IsSupportedMethodRole(method.MethodRole))
        {
            return false;
        }

        var argumentShapes = new List<RuntimeSkeletonArrayAndMemoryMarshalArgShape>(method.Parameters.Count);
        for (var parameterIndex = 0; parameterIndex < method.Parameters.Count; parameterIndex++)
        {
            argumentShapes.Add(new RuntimeSkeletonArrayAndMemoryMarshalArgShape(
                $"void* arg{parameterIndex};",
                $"if (request->arg{parameterIndex} == nullptr) {{\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }}",
                $"request->arg{parameterIndex}"));
        }

        if (!TryCreateReturnShape(GetMethodReturnType(method.SubjectId), out var returnShape))
        {
            return false;
        }

        var hasThisArgument = IsInstanceLike(method.MethodRole);
        abi = new RuntimeSkeletonArrayAndMemoryMarshalAbi(
            RuntimeSkeletonArrayAndMemoryMarshalAbi.HotUpdateStableContractId,
            hasThisArgument ? "void* this_arg;" : string.Empty,
            hasThisArgument
                ? "if (request->this_arg == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }"
                : string.Empty,
            hasThisArgument ? "request->this_arg" : "nullptr",
            argumentShapes,
            returnShape);
        return true;
    }

    private static bool IsSupportedMethodRole(string methodRole)
    {
        return string.Equals(methodRole, "static-method", StringComparison.Ordinal) ||
               string.Equals(methodRole, "static-forwarder", StringComparison.Ordinal) ||
               string.Equals(methodRole, "instance-method", StringComparison.Ordinal) ||
               string.Equals(methodRole, "constructor", StringComparison.Ordinal);
    }

    private static bool IsInstanceLike(string methodRole)
    {
        return string.Equals(methodRole, "instance-method", StringComparison.Ordinal) ||
               string.Equals(methodRole, "constructor", StringComparison.Ordinal);
    }

    private static bool TryCreateReturnShape(
        string managedType,
        out RuntimeSkeletonArrayAndMemoryMarshalReturnShape shape)
    {
        if (string.Equals(managedType, "System.Void", StringComparison.Ordinal))
        {
            shape = new RuntimeSkeletonArrayAndMemoryMarshalReturnShape(
                managedType,
                string.Empty,
                string.Empty,
                string.Empty,
                "nullptr",
                "0u",
                "false");
            return true;
        }

        shape = new RuntimeSkeletonArrayAndMemoryMarshalReturnShape(
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
