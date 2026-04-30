using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record BufferMemoryKernelArgShape(
    string FieldDeclaration,
    string ValidationStatement,
    string ArgReferenceExpression);

internal sealed record RuntimeSkeletonBufferMemoryKernelReturnShape(
    string ManagedType,
    string FieldDeclarations,
    string ValidationStatement,
    string ReturnValueDeclaration,
    string ReturnValueArgument,
    string ReturnValueSizeExpression);

internal sealed record RuntimeSkeletonBufferMemoryKernelAbi(
    string ContractId,
    IReadOnlyList<BufferMemoryKernelArgShape> ArgumentShapes,
    RuntimeSkeletonBufferMemoryKernelReturnShape ReturnShape,
    int FieldOffsetCount,
    bool NotSupported)
{
    public const string HotUpdateStableContractId = "buffermemory-kernel-v1";

    public const int ContractVersion = 1;

    public const string ContractVersionHex = "0x01";

    public static readonly RuntimeSkeletonBufferMemoryKernelAbi NOT_SUPPORTED = new(
        HotUpdateStableContractId,
        Array.Empty<BufferMemoryKernelArgShape>(),
        new RuntimeSkeletonBufferMemoryKernelReturnShape("System.Void", string.Empty, string.Empty, string.Empty, "nullptr", "0u"),
        0,
        true);

    public int field_offset_count => FieldOffsetCount;

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public string VersionAssertionExpression =>
        $"CHAOS_IL2CPP_STATIC_ASSERT(kContractVersion == {ContractVersion}, \"BufferMemoryKernel contract version mismatch\");";

    public static string GetVersionAssertionExpression(int version) =>
        $"CHAOS_IL2CPP_STATIC_ASSERT(kContractVersion == {version}, \"BufferMemoryKernel contract version mismatch\");";

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonBufferMemoryKernelAbi abi) =>
        RuntimeSkeletonBufferMemoryKernelAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonBufferMemoryKernelAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonBufferMemoryKernelAbi abi)
    {
        abi = null!;
        if ((!method.SubjectId.Contains("/System.Buffer::", StringComparison.Ordinal) &&
             !method.SubjectId.Contains("/System.Memory::", StringComparison.Ordinal)) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal))
        {
            return false;
        }

        var argumentShapes = new List<BufferMemoryKernelArgShape>(method.Parameters.Count);
        for (var parameterIndex = 0; parameterIndex < method.Parameters.Count; parameterIndex++)
        {
            argumentShapes.Add(new BufferMemoryKernelArgShape(
                $"void* arg{parameterIndex};",
                $"if (request->arg{parameterIndex} == nullptr) {{\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }}",
                $"request->arg{parameterIndex}"));
        }

        if (!TryCreateReturnShape(GetMethodReturnType(method.SubjectId), out var returnShape))
        {
            return false;
        }

        abi = new RuntimeSkeletonBufferMemoryKernelAbi(
            RuntimeSkeletonBufferMemoryKernelAbi.HotUpdateStableContractId,
            argumentShapes,
            returnShape,
            0,
            false);
        return true;
    }

    private static bool TryCreateReturnShape(
        string managedType,
        out RuntimeSkeletonBufferMemoryKernelReturnShape shape)
    {
        if (string.Equals(managedType, "System.Void", StringComparison.Ordinal))
        {
            shape = new RuntimeSkeletonBufferMemoryKernelReturnShape(
                managedType,
                string.Empty,
                string.Empty,
                string.Empty,
                "nullptr",
                "0u");
            return true;
        }

        shape = new RuntimeSkeletonBufferMemoryKernelReturnShape(
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
