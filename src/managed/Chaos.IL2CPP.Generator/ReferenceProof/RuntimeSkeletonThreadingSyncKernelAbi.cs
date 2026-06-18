using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record ThreadingSyncKernelArgShape(
    string FieldDeclaration,
    string ValidationStatement,
    string ArgReferenceExpression);

internal sealed record ThreadingSyncKernelReturnShape(
    string ManagedType,
    string FieldDeclarations,
    string ValidationStatement,
    string ReturnValueDeclaration,
    string ReturnValueArgument,
    string ReturnValueSizeExpression);

internal sealed record RuntimeSkeletonThreadingSyncKernelAbi(
    string ContractId,
    bool HasThisArgument,
    string ThisFieldDeclaration,
    IReadOnlyList<ThreadingSyncKernelArgShape> ArgumentShapes,
    ThreadingSyncKernelReturnShape ReturnShape,
    int FieldOffsetCount,
    bool NotSupported,
    string HelperCallExpression)
{
    public const string HotUpdateStableContractId = "threading-sync-kernel-v1";

    public const int ContractVersion = 1;

    public const string ContractVersionHex = "0x01";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public string ArgumentValidationStatements =>
        string.Join("\n    ", ArgumentShapes
            .Select(shape => shape.ValidationStatement)
            .Where(statement => !string.IsNullOrWhiteSpace(statement)));

    public string VersionAssertionExpression =>
        $"CHAOS_IL2CPP_STATIC_ASSERT(kContractVersion == {ContractVersion}, \"ThreadingSyncKernel contract version mismatch\");";

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonThreadingSyncKernelAbi abi) =>
        RuntimeSkeletonThreadingSyncKernelAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonThreadingSyncKernelAbiFactory
{
    private static readonly IReadOnlyDictionary<string, string> MonitorMethodToHelper = new Dictionary<string, string>(StringComparer.Ordinal)
    {
        ["Enter"] = "MonitorEnter",
        ["Exit"] = "MonitorExit",
        ["TryEnter"] = "MonitorTryEnter",
        ["IsEntered"] = "MonitorIsEntered",
    };

    private static readonly IReadOnlyDictionary<string, string> SpinLockMethodToHelper = new Dictionary<string, string>(StringComparer.Ordinal)
    {
        ["Exit"] = "MonitorExit",
        ["get_IsHeld"] = "SpinLockIsHeld",
    };

    private static readonly IReadOnlyDictionary<string, string> LockMethodToHelper = new Dictionary<string, string>(StringComparer.Ordinal)
    {
        ["Enter"] = "LockEnter",
        ["Exit"] = "LockExit",
    };

    private static readonly IReadOnlyDictionary<string, string> WaitHandleMethodToHelper = new Dictionary<string, string>(StringComparer.Ordinal)
    {
        ["Set"] = "WaitHandleSet",
        ["Reset"] = "WaitHandleReset",
    };

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonThreadingSyncKernelAbi abi)
    {
        abi = null!;
        if (!string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        var methodName = ExtractMethodName(method.SubjectId);
        if (string.IsNullOrEmpty(methodName))
        {
            return false;
        }

        if (!TryResolveHelper(method.SubjectId, methodName, out var helperName))
        {
            return false;
        }

        // Only handle methods where all parameters are void*-compatible (reference, byref, or pointer types).
        // Value-type parameters (int, bool, etc.) fall through to managed-invoke.
        if (!AreAllArgsVoidCompatible(method))
        {
            return false;
        }

        var hasThis = RuntimeSkeletonManagedInvokeAbiCore.IsInstanceLike(
            method.MethodRole,
            supportsConstructors: false,
            supportsInstanceFieldGetters: false);

        var argumentShapes = new List<ThreadingSyncKernelArgShape>(method.Parameters.Count);
        for (var i = 0; i < method.Parameters.Count; i++)
        {
            argumentShapes.Add(new ThreadingSyncKernelArgShape(
                $"void* arg{i};",
                $"if (request->arg{i} == nullptr) {{\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }}",
                $"request->arg{i}"));
        }

        if (!TryCreateReturnShape(out var returnShape))
        {
            return false;
        }

        var callArgs = new List<string>(argumentShapes.Count + 1);
        if (hasThis)
        {
            callArgs.Add("request->this_arg");
        }
        for (var i = 0; i < argumentShapes.Count; i++)
        {
            callArgs.Add($"request->arg{i}");
        }

        var callExpression = $"chaos_runtime_get_abi_v0()->{helperName}({string.Join(", ", callArgs)})";

        abi = new RuntimeSkeletonThreadingSyncKernelAbi(
            RuntimeSkeletonThreadingSyncKernelAbi.HotUpdateStableContractId,
            hasThis,
            hasThis ? "void* this_arg;" : string.Empty,
            argumentShapes,
            returnShape,
            0,
            false,
            callExpression);

        return true;
    }

    private static bool TryResolveHelper(string subjectId, string methodName, out string helperName)
    {
        helperName = string.Empty;

        if (subjectId.Contains("/System.Threading.Monitor::", StringComparison.Ordinal))
        {
            if (MonitorMethodToHelper.TryGetValue(methodName, out var resolvedHelperName))
            {
                helperName = resolvedHelperName;
                return true;
            }
            return false;
        }

        if (subjectId.Contains("/System.Threading.SpinLock::", StringComparison.Ordinal))
        {
            if (SpinLockMethodToHelper.TryGetValue(methodName, out var resolvedHelperName))
            {
                helperName = resolvedHelperName;
                return true;
            }
            return false;
        }

        if (subjectId.Contains("/System.Threading.Lock::", StringComparison.Ordinal))
        {
            if (LockMethodToHelper.TryGetValue(methodName, out var resolvedHelperName))
            {
                helperName = resolvedHelperName;
                return true;
            }
            return false;
        }

        if (subjectId.Contains("/System.Threading.WaitHandle::", StringComparison.Ordinal) ||
            subjectId.Contains("/System.Threading.EventWaitHandle::", StringComparison.Ordinal))
        {
            if (WaitHandleMethodToHelper.TryGetValue(methodName, out var resolvedHelperName))
            {
                helperName = resolvedHelperName;
                return true;
            }
            return false;
        }

        return false;
    }

    private static bool AreAllArgsVoidCompatible(TypedIlMethodArtifact method)
    {
        for (var i = 0; i < method.Parameters.Count; i++)
        {
            var paramType = method.Parameters[i].Type;
            if (!IsVoidCompatibleType(paramType))
            {
                return false;
            }
        }

        return true;
    }

    private static bool IsVoidCompatibleType(string managedType)
    {
        // Reference types and byref types map to void*.
        if (managedType.StartsWith("System.", StringComparison.Ordinal) &&
            !managedType.Contains("System.Int", StringComparison.Ordinal) &&
            !managedType.Contains("System.Boolean", StringComparison.Ordinal) &&
            !managedType.Contains("System.Single", StringComparison.Ordinal) &&
            !managedType.Contains("System.Double", StringComparison.Ordinal) &&
            !managedType.Contains("System.Char", StringComparison.Ordinal) &&
            !managedType.Contains("System.Void", StringComparison.Ordinal) &&
            !managedType.Contains("System.Decimal", StringComparison.Ordinal))
        {
            return true;
        }

        // Byref types (ending with &) are pointer-compatible.
        if (managedType.EndsWith("&", StringComparison.Ordinal))
        {
            return true;
        }

        // Generic type parameters and object references are void*-compatible.
        if (managedType.StartsWith("!", StringComparison.Ordinal) ||
            managedType.StartsWith("System.Object", StringComparison.Ordinal))
        {
            return true;
        }

        return false;
    }

    private static bool TryCreateReturnShape(out ThreadingSyncKernelReturnShape shape)
    {
        shape = new ThreadingSyncKernelReturnShape(
            "System.Boolean",
            string.Empty,
            string.Empty,
            string.Empty,
            "nullptr",
            "0u");
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
}
