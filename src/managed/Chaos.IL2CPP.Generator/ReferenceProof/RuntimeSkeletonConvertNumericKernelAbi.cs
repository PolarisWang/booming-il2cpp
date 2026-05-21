using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed record ConvertNumericKernelArgShape(
    string CppType,
    string Name,
    string FieldDeclaration);

internal sealed record RuntimeSkeletonConvertNumericKernelReturnShape(
    string ManagedType,
    string CppType);

internal sealed record RuntimeSkeletonConvertNumericKernelAbi(
    string ContractId,
    IReadOnlyList<ConvertNumericKernelArgShape> ArgumentShapes,
    RuntimeSkeletonConvertNumericKernelReturnShape ReturnShape,
    string KernelFunctionName,
    string KernelCallExpression,
    string RangeCheckCode)
{
    public const string HotUpdateStableContractId = "convert-numeric-kernel-v1";

    public bool HasReturn => true;
    public bool HasRangeCheck => !string.IsNullOrEmpty(RangeCheckCode);
    public string CppReturnType => ReturnShape.CppType;

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonConvertNumericKernelAbi abi) =>
        RuntimeSkeletonConvertNumericKernelAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonConvertNumericKernelAbiFactory
{
    private static readonly IReadOnlyDictionary<string, string> ManagedToCppTypeMap = new Dictionary<string, string>(StringComparer.Ordinal)
    {
        ["System.Boolean"] = "bool",
        ["System.Byte"] = "CHAOS_IL2CPP_UINT8",
        ["System.SByte"] = "CHAOS_IL2CPP_INT8",
        ["System.Int16"] = "CHAOS_IL2CPP_INT16",
        ["System.UInt16"] = "CHAOS_IL2CPP_UINT16",
        ["System.Char"] = "CHAOS_IL2CPP_UINT16",
        ["System.Int32"] = "CHAOS_IL2CPP_INT32",
        ["System.UInt32"] = "CHAOS_IL2CPP_UINT32",
        ["System.Int64"] = "CHAOS_IL2CPP_INT64",
        ["System.UInt64"] = "CHAOS_IL2CPP_UINT64",
        ["System.Single"] = "float",
        ["System.Double"] = "double",
    };

    /// <summary>
    /// Methods not in this set fall through to the bridge handler.
    /// </summary>
    private static readonly IReadOnlySet<string> KernelEligibleParameterTypes = new HashSet<string>(StringComparer.Ordinal)
    {
        "System.Boolean",
        "System.Byte",
        "System.SByte",
        "System.Int16",
        "System.UInt16",
        "System.Char",
        "System.Int32",
        "System.UInt32",
        "System.Int64",
        "System.UInt64",
        "System.Single",
        "System.Double",
    };

    private static readonly IReadOnlySet<string> KernelEligibleReturnTypes = new HashSet<string>(StringComparer.Ordinal)
    {
        "System.Boolean",
        "System.Byte",
        "System.SByte",
        "System.Int16",
        "System.UInt16",
        "System.Char",
        "System.Int32",
        "System.UInt32",
        "System.Int64",
        "System.UInt64",
        "System.Single",
        "System.Double",
    };

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonConvertNumericKernelAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("::To", StringComparison.Ordinal) ||
            !string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        // Must have exactly one parameter
        if (method.Parameters.Count != 1)
        {
            return false;
        }

        var paramManagedType = method.Parameters[0].Type ?? string.Empty;
        if (!KernelEligibleParameterTypes.Contains(paramManagedType))
        {
            return false;
        }

        var returnManagedType = GetMethodReturnType(method.SubjectId);
        if (!KernelEligibleReturnTypes.Contains(returnManagedType))
        {
            return false;
        }

        // Don't handle identity conversions (same type in and out) — bridge is fine
        if (string.Equals(paramManagedType, returnManagedType, StringComparison.Ordinal))
        {
            return false;
        }

        if (!ManagedToCppTypeMap.TryGetValue(paramManagedType, out var paramCppType) ||
            !ManagedToCppTypeMap.TryGetValue(returnManagedType, out var returnCppType))
        {
            return false;
        }

        // Build arg shapes (single param)
        var argShapes = new List<ConvertNumericKernelArgShape>(1)
        {
            new(paramCppType, "arg0", $"{paramCppType} arg0;"),
        };

        var returnShape = new RuntimeSkeletonConvertNumericKernelReturnShape(returnManagedType, returnCppType);

        // Build kernel function name
        var sourceTypeSuffix = ExtractSourceTypeSuffix(method.SubjectId);
        var targetTypeName = returnManagedType.Replace("System.", "");
        var helperName = $"ConvertTo{targetTypeName}From{sourceTypeSuffix.Replace("System.", "")}";

        var kernelCallExpression = $"chaos::il2cpp::runtime_core::{helperName}(request->arg0)";

        // Build range check code based on the narrowing risk
        string rangeCheckCode = BuildRangeCheckCode(paramManagedType, returnManagedType);

        abi = new RuntimeSkeletonConvertNumericKernelAbi(
            RuntimeSkeletonConvertNumericKernelAbi.HotUpdateStableContractId,
            argShapes,
            returnShape,
            helperName,
            kernelCallExpression,
            rangeCheckCode);
        return true;
    }

    private static string BuildRangeCheckCode(string sourceType, string targetType)
    {
        // Source and target are both primitives. Range checks needed when narrowing.
        // Widening or same-size conversions need no range check (e.g. Int16 → Int32 is always safe).
        // Narrowing: Int32 → Int16, Double → Int32, Int64 → Int32, etc.

        var narrowingCategory = GetNarrowingCategory(sourceType, targetType);

        return narrowingCategory switch
        {
            NarrowingCategory.SameOrWidening => string.Empty,
            NarrowingCategory.SignedToUnsigned => $"if (request->arg0 < 0) {{ return CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED; }}",
            NarrowingCategory.UnsignedTruncation => $"if (request->arg0 > {GetTypeMax(targetType)}) {{ return CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED; }}",
            NarrowingCategory.SignedTruncation => $"if (request->arg0 < {GetTypeMin(targetType)} || request->arg0 > {GetTypeMax(targetType)}) {{ return CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED; }}",
            NarrowingCategory.FloatToInteger => $"if (request->arg0 < {GetTypeMin(targetType)} || request->arg0 > {GetTypeMax(targetType)} || !std::isfinite(request->arg0)) {{ return CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED; }}",
            NarrowingCategory.FloatToFloat => $"if (!std::isfinite(request->arg0)) {{ return CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED; }}",
            _ => string.Empty,
        };
    }

    private enum NarrowingCategory { SameOrWidening, SignedToUnsigned, UnsignedTruncation, SignedTruncation, FloatToInteger, FloatToFloat }

    private static NarrowingCategory GetNarrowingCategory(string sourceType, string targetType)
    {
        var sourceBits = GetTypeBits(sourceType);
        var targetBits = GetTypeBits(targetType);
        var sourceIsFloat = IsFloatType(sourceType);
        var targetIsFloat = IsFloatType(targetType);

        if (sourceIsFloat && targetIsFloat)
        {
            return sourceBits > targetBits ? NarrowingCategory.FloatToFloat : NarrowingCategory.SameOrWidening;
        }

        if (sourceIsFloat && !targetIsFloat)
        {
            return NarrowingCategory.FloatToInteger;
        }

        // Both are integers
        if (sourceBits < targetBits)
        {
            return NarrowingCategory.SameOrWidening;
        }

        if (sourceBits == targetBits)
        {
            // Same width: only narrowing if source is unsigned and target is signed
            // e.g., UInt32 → Int32: same bits but unsigned source can have values negative int can't represent
            if (IsUnsignedType(sourceType) && !IsUnsignedType(targetType))
            {
                return NarrowingCategory.UnsignedTruncation;
            }

            return NarrowingCategory.SameOrWidening;
        }

        // Source bits > target bits: narrowing
        if (IsUnsignedType(sourceType))
        {
            return NarrowingCategory.UnsignedTruncation;
        }

        var sourceIsSigned = !IsUnsignedType(sourceType);
        var targetIsSigned = !IsUnsignedType(targetType);

        if (sourceIsSigned && !targetIsSigned)
        {
            return NarrowingCategory.SignedToUnsigned;
        }

        return NarrowingCategory.SignedTruncation;
    }

    private static int GetTypeBits(string managedType) => managedType switch
    {
        "System.Boolean" => 1,
        "System.Byte" => 8,
        "System.SByte" => 8,
        "System.Int16" => 16,
        "System.UInt16" => 16,
        "System.Char" => 16,
        "System.Int32" => 32,
        "System.UInt32" => 32,
        "System.Single" => 32,
        "System.Int64" => 64,
        "System.UInt64" => 64,
        "System.Double" => 64,
        _ => 0,
    };

    private static bool IsFloatType(string managedType) =>
        string.Equals(managedType, "System.Single", StringComparison.Ordinal) ||
        string.Equals(managedType, "System.Double", StringComparison.Ordinal);

    private static bool IsUnsignedType(string managedType) => managedType switch
    {
        "System.Byte" => true,
        "System.UInt16" => true,
        "System.Char" => true,
        "System.UInt32" => true,
        "System.UInt64" => true,
        _ => false,
    };

    private static string GetTypeMin(string managedType) => managedType switch
    {
        "System.SByte" => "INT8_MIN",
        "System.Int16" => "INT16_MIN",
        "System.Int32" => "INT32_MIN",
        "System.Int64" => "INT64_MIN",
        _ => string.Empty,
    };

    private static string GetTypeMax(string managedType) => managedType switch
    {
        "System.Byte" => "UINT8_MAX",
        "System.SByte" => "INT8_MAX",
        "System.Int16" => "INT16_MAX",
        "System.UInt16" => "UINT16_MAX",
        "System.Char" => "UINT16_MAX",
        "System.Int32" => "INT32_MAX",
        "System.UInt32" => "UINT32_MAX",
        "System.Int64" => "INT64_MAX",
        "System.UInt64" => "UINT64_MAX",
        _ => string.Empty,
    };

    private static string GetMethodReturnType(string subjectId)
    {
        var parameterStart = subjectId.IndexOf('(', StringComparison.Ordinal);
        if (parameterStart < 0) return string.Empty;

        var returnStart = subjectId.LastIndexOf(':', parameterStart - 1);
        if (returnStart < 0 || parameterStart <= returnStart + 1) return string.Empty;

        return subjectId[(returnStart + 1)..parameterStart];
    }

    private static string ExtractSourceTypeSuffix(string subjectId)
    {
        var paramStart = subjectId.IndexOf('(', StringComparison.Ordinal);
        var paramEnd = subjectId.IndexOf(')', StringComparison.Ordinal);
        if (paramStart < 0 || paramEnd < 0 || paramEnd <= paramStart + 1) return string.Empty;

        return subjectId[(paramStart + 1)..paramEnd];
    }
}
