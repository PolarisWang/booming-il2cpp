using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonSpanHelpersKernelArgShape(
    string FieldDeclaration,
    string ValidationStatement,
    string ArgReferenceExpression);

internal sealed record RuntimeSkeletonSpanHelpersKernelReturnShape(
    string ManagedType,
    string FieldDeclarations,
    string ValidationStatement,
    string ReturnValueDeclaration,
    string ReturnValueArgument,
    string ReturnValueSizeExpression);

internal sealed record RuntimeSkeletonSpanHelpersKernelAbi(
    string ContractId,
    IReadOnlyList<RuntimeSkeletonSpanHelpersKernelArgShape> ArgumentShapes,
    RuntimeSkeletonSpanHelpersKernelReturnShape ReturnShape)
{
    public const string HotUpdateStableContractId = "spanhelpers-kernel-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonSpanHelpersKernelAbi abi) =>
        RuntimeSkeletonSpanHelpersKernelAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonSpanHelpersKernelAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonSpanHelpersKernelAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("/System.SpanHelpers::", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal))
        {
            return false;
        }

        var argumentShapes = new List<RuntimeSkeletonSpanHelpersKernelArgShape>(method.Parameters.Count);
        for (var parameterIndex = 0; parameterIndex < method.Parameters.Count; parameterIndex++)
        {
            argumentShapes.Add(new RuntimeSkeletonSpanHelpersKernelArgShape(
                $"void* arg{parameterIndex};",
                $"if (request->arg{parameterIndex} == nullptr) {{\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }}",
                $"request->arg{parameterIndex}"));
        }

        if (!TryCreateReturnShape(GetMethodReturnType(method.SubjectId), out var returnShape))
        {
            return false;
        }

        abi = new RuntimeSkeletonSpanHelpersKernelAbi(
            RuntimeSkeletonSpanHelpersKernelAbi.HotUpdateStableContractId,
            argumentShapes,
            returnShape);
        return true;
    }

    private static bool TryCreateReturnShape(
        string managedType,
        out RuntimeSkeletonSpanHelpersKernelReturnShape shape)
    {
        if (string.Equals(managedType, "System.Void", StringComparison.Ordinal))
        {
            shape = new RuntimeSkeletonSpanHelpersKernelReturnShape(
                managedType,
                string.Empty,
                string.Empty,
                string.Empty,
                "nullptr",
                "0u");
            return true;
        }

        shape = new RuntimeSkeletonSpanHelpersKernelReturnShape(
            managedType,
            "void* return_value;\n    CHAOS_IL2CPP_SIZE return_value_size;",
            "if (request->return_value == nullptr || request->return_value_size == 0u) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }",
            "void* return_value_ptr = request->return_value;",
            "&return_value_ptr",
            "request->return_value_size");
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
