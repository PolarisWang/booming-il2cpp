using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record MathKernelArgShape(
    string CppType,
    string Name,
    string FieldDeclaration);

internal sealed record RuntimeSkeletonMathKernelReturnShape(
    string ManagedType,
    string CppType,
    string ReturnValueExpression);

internal sealed record RuntimeSkeletonMathKernelAbi(
    string ContractId,
    IReadOnlyList<MathKernelArgShape> ArgumentShapes,
    RuntimeSkeletonMathKernelReturnShape ReturnShape)
{
    public const string HotUpdateStableContractId = "math-kernel-v1";

    public bool HasReturn => !string.Equals(ReturnShape.ManagedType, "System.Void", StringComparison.Ordinal);

    public string KernelCallExpression { get; init; } = string.Empty;

    public string KernelFunctionName { get; init; } = string.Empty;

    public string CppReturnType => ReturnShape.CppType;

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonMathKernelAbi abi) =>
        RuntimeSkeletonMathKernelAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonMathKernelAbiFactory
{
    private static readonly IReadOnlyDictionary<string, string> ManagedToCppTypeMap = new Dictionary<string, string>(StringComparer.Ordinal)
    {
        ["System.Int32"] = "CHAOS_IL2CPP_INT32",
        ["System.Int64"] = "CHAOS_IL2CPP_INT64",
        ["System.UInt32"] = "CHAOS_IL2CPP_UINT32",
        ["System.UInt64"] = "CHAOS_IL2CPP_UINT64",
        ["System.Int16"] = "CHAOS_IL2CPP_INT16",
        ["System.UInt16"] = "CHAOS_IL2CPP_UINT16",
        ["System.Byte"] = "CHAOS_IL2CPP_UINT8",
        ["System.SByte"] = "CHAOS_IL2CPP_INT8",
        ["System.Double"] = "double",
        ["System.Single"] = "float",
        ["System.Boolean"] = "bool",
        ["System.IntPtr"] = "CHAOS_IL2CPP_INTPTR",
        ["System.UIntPtr"] = "CHAOS_IL2CPP_UINTPTR",
        ["System.Void"] = "void",
    };

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonMathKernelAbi abi)
    {
        abi = null!;
        if (!string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal))
        {
            return false;
        }

        // Build arg shapes
        var argShapes = new List<MathKernelArgShape>(method.Parameters.Count);
        foreach (var param in method.Parameters)
        {
            var managedType = param.Type ?? "System.Int32";
            if (!ManagedToCppTypeMap.TryGetValue(managedType, out var cppType))
            {
                return false;
            }

            var name = $"arg{argShapes.Count}";
            argShapes.Add(new MathKernelArgShape(
                cppType,
                name,
                $"{cppType} {name};"));
        }

        // Build return shape
        var returnManagedType = GetMethodReturnType(method.SubjectId);
        if (!ManagedToCppTypeMap.TryGetValue(returnManagedType, out var returnCppType))
        {
            return false;
        }

        var returnShape = new RuntimeSkeletonMathKernelReturnShape(
            returnManagedType,
            returnCppType,
            returnCppType != "void" ? "request->return_value" : string.Empty);

        abi = new RuntimeSkeletonMathKernelAbi(
            RuntimeSkeletonMathKernelAbi.HotUpdateStableContractId,
            argShapes,
            returnShape);

        // Build kernel function name and call expression from subject ID suffix
        if (!TryBuildKernelCall(method.SubjectId, abi, out var kernelFn, out var callExpr))
        {
            return false;
        }

        abi = abi with { KernelFunctionName = kernelFn, KernelCallExpression = callExpr };
        return true;
    }

    private static bool TryBuildKernelCall(
        string subjectId,
        RuntimeSkeletonMathKernelAbi abi,
        out string kernelFunctionName,
        out string callExpression)
    {
        kernelFunctionName = string.Empty;
        callExpression = string.Empty;

        var suffix = ExtractMethodSuffix(subjectId);
        if (string.IsNullOrEmpty(suffix))
        {
            return false;
        }

        // Map suffix to runtime_core helper name
        var helperName = suffix switch
        {
            "Abs(System.Int32)" => "MathAbs<CHAOS_IL2CPP_INT32>",
            "Abs(System.Int64)" => "MathAbs<CHAOS_IL2CPP_INT64>",
            "Abs(System.Single)" => "MathAbs<float>",
            "Abs(System.Double)" => "MathAbs<double>",
            "Abs(System.Int16)" => "MathAbs<CHAOS_IL2CPP_INT16>",
            "Abs(System.SByte)" => "MathAbs<CHAOS_IL2CPP_INT8>",
            "Abs(System.Decimal)" => null, // Decimal not supported in kernel
            "Ceiling(System.Double)" => "CHAOS_IL2CPP_CEIL",
            "Floor(System.Double)" => "CHAOS_IL2CPP_FLOOR",
            "Round(System.Double)" => "MathTruncate",
            "Round(System.Double,System.Int32)" => null, // Not a direct kernel
            "Sqrt(System.Double)" => "CHAOS_IL2CPP_SQRT",
            "Pow(System.Double,System.Double)" => "CHAOS_IL2CPP_POW",
            "Sin(System.Double)" => "CHAOS_IL2CPP_SIN",
            "Cos(System.Double)" => "CHAOS_IL2CPP_COS",
            "Tan(System.Double)" => "CHAOS_IL2CPP_TAN",
            "Max(System.Int32,System.Int32)" => "MathMax<CHAOS_IL2CPP_INT32>",
            "Max(System.Int64,System.Int64)" => "MathMax<CHAOS_IL2CPP_INT64>",
            "Max(System.Single,System.Single)" => "MathMax<float>",
            "Max(System.Double,System.Double)" => "MathMax<double>",
            "Max(System.Int16,System.Int16)" => "MathMax<CHAOS_IL2CPP_INT16>",
            "Max(System.Byte,System.Byte)" => "MathMax<CHAOS_IL2CPP_UINT8>",
            "Min(System.Int32,System.Int32)" => "MathMin<CHAOS_IL2CPP_INT32>",
            "Min(System.Int64,System.Int64)" => "MathMin<CHAOS_IL2CPP_INT64>",
            "Min(System.Single,System.Single)" => "MathMin<float>",
            "Min(System.Double,System.Double)" => "MathMin<double>",
            "Min(System.Int16,System.Int16)" => "MathMin<CHAOS_IL2CPP_INT16>",
            "Min(System.Byte,System.Byte)" => "MathMin<CHAOS_IL2CPP_UINT8>",
            "BigMul(System.Int32,System.Int32)" => "MathBigMul",
            "Log(System.Double)" => "CHAOS_IL2CPP_LOG",
            "Log(System.Double,System.Double)" => "MathLog",
            "Exp(System.Double)" => "CHAOS_IL2CPP_EXP",
            _ => null,
        };

        if (helperName == null)
        {
            return false;
        }

        kernelFunctionName = helperName;

        // Build call expression
        var argExprs = abi.ArgumentShapes.Select(a => $"request->{a.Name}").ToList();
        var qualifiedCall = $"chaos::il2cpp::runtime_core::{helperName}({string.Join(", ", argExprs)})";

        callExpression = qualifiedCall;
        return true;
    }

    private static string ExtractMethodSuffix(string subjectId)
    {
        // Extract method name and parameter types after the last ::
        // e.g. "System.Private.CoreLib/System.Math::Abs(System.Int32)" -> "Abs(System.Int32)"
        var methodStart = subjectId.LastIndexOf("::", StringComparison.Ordinal);
        if (methodStart < 0)
        {
            return string.Empty;
        }

        return subjectId[(methodStart + 2)..];
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
