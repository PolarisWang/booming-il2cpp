using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record ConvertCharKernelArgShape(
    string CppType,
    string Name,
    string FieldDeclaration);

internal sealed record RuntimeSkeletonConvertCharKernelReturnShape(
    string ManagedType,
    string CppType);

internal sealed record RuntimeSkeletonConvertCharKernelAbi(
    string ContractId,
    IReadOnlyList<ConvertCharKernelArgShape> ArgumentShapes,
    RuntimeSkeletonConvertCharKernelReturnShape ReturnShape,
    string KernelFunctionName,
    string KernelCallExpression,
    string RangeCheckCode)
{
    public const string HotUpdateStableContractId = "convert-char-kernel-v1";

    public bool HasReturn => true;
    public bool HasRangeCheck => !string.IsNullOrEmpty(RangeCheckCode);
    public string CppReturnType => ReturnShape.CppType;

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonConvertCharKernelAbi abi) =>
        RuntimeSkeletonConvertCharKernelAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonConvertCharKernelAbiFactory
{
    private static readonly IReadOnlyDictionary<string, string> ManagedToCppTypeMap = new Dictionary<string, string>(StringComparer.Ordinal)
    {
        ["System.Boolean"] = "bool",
        ["System.Byte"] = "CHAOS_IL2CPP_UINT8",
        ["System.Char"] = "CHAOS_IL2CPP_UINT16",
        ["System.Double"] = "double",
        ["System.Int16"] = "CHAOS_IL2CPP_INT16",
        ["System.Int32"] = "CHAOS_IL2CPP_INT32",
        ["System.Int64"] = "CHAOS_IL2CPP_INT64",
        ["System.Object"] = "CHAOS_IL2CPP_INTPTR",
        ["System.SByte"] = "CHAOS_IL2CPP_INT8",
        ["System.Single"] = "float",
        ["System.UInt16"] = "CHAOS_IL2CPP_UINT16",
        ["System.UInt32"] = "CHAOS_IL2CPP_UINT32",
        ["System.UInt64"] = "CHAOS_IL2CPP_UINT64",
    };

    /// <summary>
    /// Methods not in this set fall through to the bridge handler.
    /// </summary>
    private static readonly IReadOnlySet<string> KernelEligibleParameterTypes = new HashSet<string>(StringComparer.Ordinal)
    {
        "System.Boolean",
        "System.Byte",
        "System.Char",
        "System.Double",
        "System.Int16",
        "System.Int32",
        "System.Int64",
        "System.Object",
        "System.SByte",
        "System.Single",
        "System.UInt16",
        "System.UInt32",
        "System.UInt64",
    };

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonConvertCharKernelAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("::ToChar:", StringComparison.Ordinal) ||
            !string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        // Must have exactly one parameter for kernel eligibility
        if (method.Parameters.Count != 1)
        {
            return false;
        }

        var paramManagedType = method.Parameters[0].Type ?? string.Empty;
        if (!KernelEligibleParameterTypes.Contains(paramManagedType))
        {
            return false;
        }

        if (!ManagedToCppTypeMap.TryGetValue(paramManagedType, out var paramCppType))
        {
            return false;
        }

        // Build arg shapes (single param)
        var argShapes = new List<ConvertCharKernelArgShape>(1)
        {
            new(paramCppType, "arg0", $"{paramCppType} arg0;"),
        };

        // Return is always System.Char → CHAOS_IL2CPP_UINT16
        var returnShape = new RuntimeSkeletonConvertCharKernelReturnShape("System.Char", "CHAOS_IL2CPP_UINT16");

        // Build kernel function name and call expression
        var sourceTypeSuffix = ExtractSourceTypeSuffix(method.SubjectId);
        if (string.IsNullOrEmpty(sourceTypeSuffix))
        {
            return false;
        }

        var helperName = sourceTypeSuffix switch
        {
            "System.Char" => null, // identity — no helper needed
            "System.UInt16" => null, // identity — no helper needed
            _ => $"ConvertToCharFrom{sourceTypeSuffix.Replace("System.", "")}",
        };

        string kernelCallExpression;
        if (helperName == null)
        {
            // Identity: arg0 is already the right type
            kernelCallExpression = "request->arg0";
        }
        else
        {
            kernelCallExpression = $"chaos_runtime_get_abi_v0()->{helperName}(request->arg0)";
        }

        // Build range check code
        string rangeCheckCode = paramManagedType switch
        {
            "System.Boolean" => string.Empty,
            "System.Byte" => string.Empty,
            "System.Char" => string.Empty,
            "System.UInt16" => string.Empty,
            "System.SByte" => "if (request->arg0 < 0) { return CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED; }",
            "System.Int16" => "if (request->arg0 < 0) { return CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED; }",
            "System.Int32" => "if (request->arg0 < 0 || request->arg0 > 0xFFFF) { return CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED; }",
            "System.Int64" => "if (request->arg0 < INT64_C(0) || request->arg0 > INT64_C(0xFFFF)) { return CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED; }",
            "System.UInt32" => "if (request->arg0 > UINT32_C(0xFFFF)) { return CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED; }",
            "System.UInt64" => "if (request->arg0 > UINT64_C(0xFFFF)) { return CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED; }",
            "System.Single" => "if (request->arg0 < 0.0f || request->arg0 > 65535.0f || !std::isfinite(request->arg0)) { return CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED; }",
            "System.Double" => "if (request->arg0 < 0.0 || request->arg0 > 65535.0 || !std::isfinite(request->arg0)) { return CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED; }",
            _ => string.Empty,
        };

        var kernelFunctionName = helperName ?? "identity";

        abi = new RuntimeSkeletonConvertCharKernelAbi(
            RuntimeSkeletonConvertCharKernelAbi.HotUpdateStableContractId,
            argShapes,
            returnShape,
            kernelFunctionName,
            kernelCallExpression,
            rangeCheckCode);
        return true;
    }

    private static string ExtractSourceTypeSuffix(string subjectId)
    {
        // Extract parameter type from ToChar method signature
        // e.g. "::ToChar:System.Char(System.Int32)" → "System.Int32"
        var paramStart = subjectId.IndexOf('(', StringComparison.Ordinal);
        var paramEnd = subjectId.IndexOf(')', StringComparison.Ordinal);
        if (paramStart < 0 || paramEnd < 0 || paramEnd <= paramStart + 1)
        {
            return string.Empty;
        }

        return subjectId[(paramStart + 1)..paramEnd];
    }
}
