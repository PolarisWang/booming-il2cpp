using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonStringManagedInvokeArgShape(
    string FieldDeclaration,
    string ArgReferenceExpression);

internal sealed record RuntimeSkeletonStringManagedInvokeReturnShape(
    string ManagedType,
    string FieldDeclarations,
    string ValidationStatement,
    string BufferDeclaration,
    string ReturnValueArgument,
    string ReturnValueSizeExpression,
    string AssignmentStatement);

internal sealed record RuntimeSkeletonStringManagedInvokeAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonStringManagedInvokeArgShape> ArgumentShapes,
    RuntimeSkeletonStringManagedInvokeReturnShape ReturnShape)
{
    public const string HotUpdateStableContractId = "string-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public bool HasThisArgument => !string.IsNullOrWhiteSpace(ThisFieldDeclaration);

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonStringManagedInvokeAbi abi) =>
        RuntimeSkeletonStringManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonStringManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonStringManagedInvokeAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("/System.String::", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !IsSupportedMethodRole(method.MethodRole))
        {
            return false;
        }

        var argumentShapes = new List<RuntimeSkeletonStringManagedInvokeArgShape>(method.Parameters.Count);
        for (var parameterIndex = 0; parameterIndex < method.Parameters.Count; parameterIndex++)
        {
            if (!TryCreateArgumentShape(method.Parameters[parameterIndex].Type, parameterIndex, out var argumentShape))
            {
                return false;
            }

            argumentShapes.Add(argumentShape);
        }

        var hasThisArgument = IsInstanceLike(method.MethodRole);
        if (!TryCreateReturnShape(GetMethodReturnType(method.SubjectId), out var returnShape))
        {
            return false;
        }

        abi = new RuntimeSkeletonStringManagedInvokeAbi(
            RuntimeSkeletonStringManagedInvokeAbi.HotUpdateStableContractId,
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
               string.Equals(methodRole, "instance-field-getter", StringComparison.Ordinal);
    }

    private static bool IsInstanceLike(string methodRole)
    {
        return string.Equals(methodRole, "instance-method", StringComparison.Ordinal) ||
               string.Equals(methodRole, "instance-field-getter", StringComparison.Ordinal);
    }

    private static bool TryCreateArgumentShape(
        string managedType,
        int parameterIndex,
        out RuntimeSkeletonStringManagedInvokeArgShape shape)
    {
        shape = null!;
        if (TryResolveTypedValueCarrier(managedType, out var cppCarrier))
        {
            shape = new RuntimeSkeletonStringManagedInvokeArgShape(
                $"{cppCarrier} arg{parameterIndex};",
                $"const_cast<void*>(reinterpret_cast<const void*>(&request->arg{parameterIndex}))");
            return true;
        }

        if (IsReferenceCarrierType(managedType))
        {
            shape = new RuntimeSkeletonStringManagedInvokeArgShape(
                $"void* arg{parameterIndex};",
                $"const_cast<void*>(reinterpret_cast<const void*>(&request->arg{parameterIndex}))");
            return true;
        }

        if (IsPassThroughCarrierType(managedType))
        {
            shape = new RuntimeSkeletonStringManagedInvokeArgShape(
                $"void* arg{parameterIndex};",
                $"request->arg{parameterIndex}");
            return true;
        }

        return false;
    }

    private static bool TryCreateReturnShape(
        string managedType,
        out RuntimeSkeletonStringManagedInvokeReturnShape shape)
    {
        if (string.Equals(managedType, "System.Void", StringComparison.Ordinal))
        {
            shape = new RuntimeSkeletonStringManagedInvokeReturnShape(
                managedType,
                string.Empty,
                string.Empty,
                string.Empty,
                "nullptr",
                "0u",
                string.Empty);
            return true;
        }

        if (TryResolveTypedValueCarrier(managedType, out var cppCarrier))
        {
            shape = new RuntimeSkeletonStringManagedInvokeReturnShape(
                managedType,
                $"{cppCarrier}* return_value;",
                "if (request->return_value == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }",
                $"{cppCarrier} return_buffer = {{}};\n    void* return_value_ptr = &return_buffer;",
                "&return_value_ptr",
                "sizeof(return_buffer)",
                "*request->return_value = return_buffer;");
            return true;
        }

        if (IsReferenceReturnType(managedType))
        {
            shape = new RuntimeSkeletonStringManagedInvokeReturnShape(
                managedType,
                "void** return_value;",
                "if (request->return_value == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }",
                "void* return_value = nullptr;",
                "&return_value",
                "sizeof(return_value)",
                "*request->return_value = return_value;");
            return true;
        }

        if (IsOpaqueReturnType(managedType))
        {
            shape = new RuntimeSkeletonStringManagedInvokeReturnShape(
                managedType,
                "void* return_value;\n    CHAOS_IL2CPP_SIZE return_value_size;",
                "if (request->return_value == nullptr || request->return_value_size == 0u) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }",
                "void* return_value_ptr = request->return_value;",
                "&return_value_ptr",
                "request->return_value_size",
                string.Empty);
            return true;
        }

        shape = null!;
        return false;
    }

    private static bool TryResolveTypedValueCarrier(string managedType, out string cppCarrier)
    {
        cppCarrier = managedType switch
        {
            "System.Boolean" => "bool",
            "System.Byte" => "CHAOS_IL2CPP_UINT8",
            "System.SByte" => "CHAOS_IL2CPP_INT8",
            "System.Int16" => "CHAOS_IL2CPP_INT16",
            "System.UInt16" => "CHAOS_IL2CPP_UINT16",
            "System.Char" => "CHAOS_IL2CPP_UINT16",
            "System.Int32" => "CHAOS_IL2CPP_INT32",
            "System.UInt32" => "CHAOS_IL2CPP_UINT32",
            "System.Int64" => "CHAOS_IL2CPP_INT64",
            "System.UInt64" => "CHAOS_IL2CPP_UINT64",
            "System.Single" => "float",
            "System.Double" => "double",
            "System.Decimal" => "RuntimeSkeletonStringDecimalCarrier",
            "System.DateTime" => "CHAOS_IL2CPP_UINT64",
            "System.IntPtr" => "CHAOS_IL2CPP_INTPTR",
            "System.StringComparison" => "CHAOS_IL2CPP_INT32",
            "System.StringSplitOptions" => "CHAOS_IL2CPP_INT32",
            "System.Globalization.CompareOptions" => "CHAOS_IL2CPP_INT32",
            "System.Text.NormalizationForm" => "CHAOS_IL2CPP_INT32",
            "System.Text.TrimType" => "CHAOS_IL2CPP_INT32",
            "System.TypeCode" => "CHAOS_IL2CPP_INT32",
            _ when managedType.StartsWith("System.ReadOnlySpan<", StringComparison.Ordinal) => "RuntimeSkeletonStringSpanCarrier",
            _ when managedType.StartsWith("System.Span<", StringComparison.Ordinal) => "RuntimeSkeletonStringSpanCarrier",
            _ => string.Empty,
        };

        return cppCarrier.Length > 0;
    }

    private static bool IsReferenceCarrierType(string managedType)
    {
        if (managedType.EndsWith("[]", StringComparison.Ordinal))
        {
            return true;
        }

        if (managedType.StartsWith("System.Collections.Generic.IEnumerable<", StringComparison.Ordinal) ||
            managedType.StartsWith("System.Collections.Generic.IEnumerator<", StringComparison.Ordinal))
        {
            return true;
        }

        return managedType is
            "System.String" or
            "System.Object" or
            "System.IFormatProvider" or
            "System.Globalization.CultureInfo" or
            "System.Globalization.CompareInfo" or
            "System.Text.Encoding" or
            "System.Type" or
            "System.Text.CompositeFormat" or
            "System.Collections.IEnumerator" or
            "System.Buffers.SpanAction<System.Char,!!0>" or
            "System.Buffers.SpanAction<System.Char,System.IntPtr>" or
            "System.Buffers.SpanAction<System.Char,System.DateOnly>" or
            "System.Buffers.SpanAction<System.Char,System.TimeOnly>" or
            "System.Buffers.SpanAction<System.Char,System.ValueTuple<System.IntPtr,System.HexConverter+Casing>>" or
            "System.Buffers.SpanAction<System.Char,System.IO.Path+JoinInternalState>";
    }

    private static bool IsPassThroughCarrierType(string managedType)
    {
        if (managedType.StartsWith("!!", StringComparison.Ordinal) ||
            managedType.StartsWith("!", StringComparison.Ordinal))
        {
            return true;
        }

        if (managedType.EndsWith("&", StringComparison.Ordinal) ||
            managedType.EndsWith("*", StringComparison.Ordinal))
        {
            return true;
        }

        return managedType is
            "System.Runtime.CompilerServices.DefaultInterpolatedStringHandler&" or
            "System.DateOnly" or
            "System.TimeOnly" or
            "System.ValueTuple<System.IntPtr,System.HexConverter+Casing>" or
            "System.IO.Path+JoinInternalState";
    }

    private static bool IsReferenceReturnType(string managedType)
    {
        if (managedType.EndsWith("&", StringComparison.Ordinal) ||
            managedType.EndsWith("[]", StringComparison.Ordinal))
        {
            return true;
        }

        if (managedType.StartsWith("System.Collections.Generic.IEnumerator<", StringComparison.Ordinal))
        {
            return true;
        }

        return managedType is
            "System.String" or
            "System.Object" or
            "System.Collections.IEnumerator";
    }

    private static bool IsOpaqueReturnType(string managedType)
    {
        return managedType is
            "System.CharEnumerator" or
            "System.Text.StringRuneEnumerator";
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
