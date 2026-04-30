using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record CollectionsKernelArgShape(
    string FieldDeclaration,
    string ValidationStatement,
    string ArgReferenceExpression);

internal sealed record CollectionsKernelReturnShape(
    string ManagedType,
    string FieldDeclarations,
    string ValidationStatement,
    string ReturnValueDeclaration,
    string ReturnValueArgument,
    string ReturnValueSizeExpression);

internal sealed record RuntimeSkeletonCollectionsKernelAbi(
    string ContractId,
    IReadOnlyList<CollectionsKernelArgShape> ArgumentShapes,
    CollectionsKernelReturnShape ReturnShape,
    int FieldOffsetCount,
    bool NotSupported)
{
    public const string HotUpdateStableContractId = "collections-kernel-v1";

    public const int ContractVersion = 1;

    public const string CollectionsKernelContractVersionHex = "0x01";

    public static readonly RuntimeSkeletonCollectionsKernelAbi NOT_SUPPORTED = new(
        HotUpdateStableContractId,
        Array.Empty<CollectionsKernelArgShape>(),
        new CollectionsKernelReturnShape("System.Void", string.Empty, string.Empty, string.Empty, "nullptr", "0u"),
        0,
        true);

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public int field_offset_count => FieldOffsetCount;

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public string HelperCallExpression { get; init; } = string.Empty;

    public string VersionAssertionExpression =>
        $"CHAOS_IL2CPP_STATIC_ASSERT(kContractVersion == {ContractVersion}, \"CollectionsKernel contract version mismatch\");";

    public static string GetVersionAssertionExpression(int version) =>
        $"CHAOS_IL2CPP_STATIC_ASSERT(kContractVersion == {version}, \"CollectionsKernel contract version mismatch\");";

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonCollectionsKernelAbi abi)
    {
        if (TryCreateStackAbi(method, out abi))
        {
            return true;
        }

        return RuntimeSkeletonCollectionsKernelAbiFactory.TryCreate(method, out abi);
    }

    internal static bool TryCreateStackAbi(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonCollectionsKernelAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("Stack", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        return RuntimeSkeletonCollectionsKernelAbiFactory.TryCreateStackAbi(method, out abi);
    }
}

internal static class RuntimeSkeletonCollectionsKernelAbiFactory
{
    private static readonly IReadOnlyDictionary<string, string> StackMethodToHelperMap = new Dictionary<string, string>(StringComparer.Ordinal)
    {
        ["get_Count"] = "CollectionStackGetCount",
        ["Push"] = "CollectionStackPush",
        ["Pop"] = "CollectionStackPop",
        ["Peek"] = "CollectionStackPeek",
        ["TryPop"] = "CollectionStackTryPop",
        ["TryPeek"] = "CollectionStackTryPeek",
    };

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonCollectionsKernelAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("/System.Collections.", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal))
        {
            return false;
        }

        var argumentShapes = new List<CollectionsKernelArgShape>(method.Parameters.Count);
        for (var parameterIndex = 0; parameterIndex < method.Parameters.Count; parameterIndex++)
        {
            argumentShapes.Add(new CollectionsKernelArgShape(
                $"void* arg{parameterIndex};",
                $"if (request->arg{parameterIndex} == nullptr) {{\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }}",
                $"request->arg{parameterIndex}"));
        }

        if (!TryCreateReturnShape(GetMethodReturnType(method.SubjectId), out var returnShape))
        {
            return false;
        }

        abi = new RuntimeSkeletonCollectionsKernelAbi(
            RuntimeSkeletonCollectionsKernelAbi.HotUpdateStableContractId,
            argumentShapes,
            returnShape,
            0,
            false);
        return true;
    }

    internal static bool TryCreateStackAbi(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonCollectionsKernelAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("/System.Collections.Generic.Stack`1::", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        var methodName = ExtractMethodName(method.SubjectId);
        if (string.IsNullOrEmpty(methodName) ||
            !StackMethodToHelperMap.TryGetValue(methodName, out var helperName))
        {
            return false;
        }

        var argumentShapes = new List<CollectionsKernelArgShape>(method.Parameters.Count);
        for (var parameterIndex = 0; parameterIndex < method.Parameters.Count; parameterIndex++)
        {
            argumentShapes.Add(new CollectionsKernelArgShape(
                $"void* arg{parameterIndex};",
                $"if (request->arg{parameterIndex} == nullptr) {{\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }}",
                $"request->arg{parameterIndex}"));
        }

        if (!TryCreateReturnShape(GetMethodReturnType(method.SubjectId), out var returnShape))
        {
            return false;
        }

        var argExprs = new List<string>(argumentShapes.Count);
        for (var i = 0; i < argumentShapes.Count; i++)
        {
            argExprs.Add($"request->arg{i}");
        }

        var callExpression = $"chaos::il2cpp::runtime_core::{helperName}({string.Join(", ", argExprs)})";

        abi = new RuntimeSkeletonCollectionsKernelAbi(
            RuntimeSkeletonCollectionsKernelAbi.HotUpdateStableContractId,
            argumentShapes,
            returnShape,
            0,
            false)
        {
            HelperCallExpression = callExpression,
        };

        return true;
    }

    private static string ExtractMethodName(string subjectId)
    {
        var doubleColon = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (doubleColon < 0) return string.Empty;

        var parenStart = subjectId.IndexOf('(', doubleColon);
        if (parenStart < 0) return subjectId[(doubleColon + 2)..];

        return subjectId[(doubleColon + 2)..parenStart];
    }

    private static bool TryCreateReturnShape(
        string managedType,
        out CollectionsKernelReturnShape shape)
    {
        if (string.Equals(managedType, "System.Void", StringComparison.Ordinal))
        {
            shape = new CollectionsKernelReturnShape(
                managedType,
                string.Empty,
                string.Empty,
                string.Empty,
                "nullptr",
                "0u");
            return true;
        }

        shape = new CollectionsKernelReturnShape(
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
