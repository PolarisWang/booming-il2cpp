using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonConvertRuntimeHelperAbi(
    string ContractId,
    string ReturnManagedType,
    string ReturnFieldDeclaration,
    string ReturnValueValidationStatement,
    string ReturnBufferDeclaration,
    string ReturnValueArgument,
    string ReturnValueSizeExpression,
    string ReturnAssignmentStatement,
    IReadOnlyList<string> ArgumentFieldDeclarations,
    IReadOnlyList<string> ArgumentReferences)
{
    public const string HotUpdateStableContractId = "convert-runtime-helper-v1";

    public int ArgumentCount => ArgumentFieldDeclarations.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonConvertRuntimeHelperAbi abi) =>
        RuntimeSkeletonConvertRuntimeHelperAbiFactory.TryCreate(method, out abi);

}

internal static class RuntimeSkeletonConvertRuntimeHelperAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonConvertRuntimeHelperAbi abi)
    {
        abi = null!;
        if (!method.SubjectId.Contains("/System.Convert::", StringComparison.Ordinal) ||
            (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) &&
             !string.Equals(method.MethodRole, "static-forwarder", StringComparison.Ordinal)) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        var argumentFieldDeclarations = new List<string>(method.Parameters.Count);
        var argumentReferences = new List<string>(method.Parameters.Count);
        for (var parameterIndex = 0; parameterIndex < method.Parameters.Count; parameterIndex++)
        {
            if (!TryResolveCppCarrier(method.Parameters[parameterIndex].Type, out var cppCarrier))
            {
                return false;
            }

            argumentFieldDeclarations.Add($"{cppCarrier} arg{parameterIndex};");
            argumentReferences.Add($"const_cast<void*>(reinterpret_cast<const void*>(&request->arg{parameterIndex}))");
        }

        var returnManagedType = GetMethodReturnType(method.SubjectId);
        if (!TryCreateReturnAbi(
                returnManagedType,
                out var returnFieldDeclaration,
                out var returnValueValidationStatement,
                out var returnBufferDeclaration,
                out var returnValueArgument,
                out var returnValueSizeExpression,
                out var returnAssignmentStatement))
        {
            return false;
        }

        abi = new RuntimeSkeletonConvertRuntimeHelperAbi(
            RuntimeSkeletonConvertRuntimeHelperAbi.HotUpdateStableContractId,
            returnManagedType,
            returnFieldDeclaration,
            returnValueValidationStatement,
            returnBufferDeclaration,
            returnValueArgument,
            returnValueSizeExpression,
            returnAssignmentStatement,
            argumentFieldDeclarations,
            argumentReferences);
        return true;
    }

    private static bool TryCreateReturnAbi(
        string managedType,
        out string fieldDeclaration,
        out string validationStatement,
        out string bufferDeclaration,
        out string valueArgument,
        out string sizeExpression,
        out string assignmentStatement)
    {
        fieldDeclaration = string.Empty;
        validationStatement = string.Empty;
        bufferDeclaration = string.Empty;
        valueArgument = "nullptr";
        sizeExpression = "0u";
        assignmentStatement = string.Empty;

        if (string.Equals(managedType, "System.Void", StringComparison.Ordinal))
        {
            return true;
        }

        if (!TryResolveCppCarrier(managedType, out var cppCarrier))
        {
            return false;
        }

        fieldDeclaration = $"{cppCarrier}* return_value;";
        validationStatement = "if (request->return_value == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }";
        bufferDeclaration = $"{cppCarrier} return_buffer = {{}};";
        valueArgument = "&return_buffer";
        sizeExpression = "sizeof(return_buffer)";
        assignmentStatement = "*request->return_value = return_buffer;";
        return true;
    }

    private static bool TryResolveCppCarrier(string managedType, out string cppCarrier)
    {
        cppCarrier = managedType switch
        {
            "System.Boolean" => "bool",
            "System.Byte" => "std::uint8_t",
            "System.SByte" => "std::int8_t",
            "System.Int16" => "std::int16_t",
            "System.UInt16" => "std::uint16_t",
            "System.Char" => "std::uint16_t",
            "System.Int32" => "std::int32_t",
            "System.UInt32" => "std::uint32_t",
            "System.Int64" => "std::int64_t",
            "System.UInt64" => "std::uint64_t",
            "System.Single" => "float",
            "System.Double" => "double",
            "System.Decimal" => "RuntimeSkeletonConvertDecimalCarrier",
            "System.DateTime" => "std::uint64_t",
            "System.TypeCode" => "std::int32_t",
            "System.Base64FormattingOptions" => "std::int32_t",
            "System.String" => "void*",
            "System.Object" => "void*",
            "System.Type" => "void*",
            "System.IConvertible" => "void*",
            "System.IFormatProvider" => "void*",
            "System.Byte[]" => "void*",
            "System.Char[]" => "void*",
            "System.Byte*" => "void*",
            "System.Char*" => "void*",
            "System.Byte&" => "void*",
            "System.Char&" => "void*",
            "System.SByte&" => "void*",
            "System.Int32&" => "void*",
            "System.Span<System.Byte>" => "RuntimeSkeletonConvertSpanCarrier",
            "System.Span<System.Char>" => "RuntimeSkeletonConvertSpanCarrier",
            "System.ReadOnlySpan<System.Byte>" => "RuntimeSkeletonConvertSpanCarrier",
            "System.ReadOnlySpan<System.Char>" => "RuntimeSkeletonConvertSpanCarrier",
            "System.ReadOnlySpan<System.SByte>" => "RuntimeSkeletonConvertSpanCarrier",
            _ => string.Empty,
        };
        return cppCarrier.Length > 0;
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
