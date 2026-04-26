using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonManagedInvokePointerArgShape(
    string FieldDeclaration,
    string ValidationStatement,
    string ArgReferenceExpression);

internal sealed record RuntimeSkeletonManagedInvokeReturnContract(
    string ManagedType,
    string FieldDeclarations,
    string ValidationStatement,
    string ReturnValueDeclaration,
    string ReturnValueArgument,
    string ReturnValueSizeExpression,
    string ReturnValueIsIndirectExpression);

internal static class RuntimeSkeletonManagedInvokeAbiCore
{
    public static IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> CreatePointerBackedArgumentShapes(
        TypedIlMethodArtifact method,
        bool validateNonNullArguments = true)
    {
        var argumentShapes = new List<RuntimeSkeletonManagedInvokePointerArgShape>(method.Parameters.Count);
        for (var parameterIndex = 0; parameterIndex < method.Parameters.Count; parameterIndex++)
        {
            argumentShapes.Add(new RuntimeSkeletonManagedInvokePointerArgShape(
                $"void* arg{parameterIndex};",
                validateNonNullArguments
                    ? $"if (request->arg{parameterIndex} == nullptr) {{\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }}"
                    : string.Empty,
                $"request->arg{parameterIndex}"));
        }

        return argumentShapes;
    }

    public static bool SupportsMethodRole(
        string methodRole,
        bool supportsStaticForwarder = false,
        bool supportsInstanceMethods = false,
        bool supportsConstructors = false,
        bool supportsInstanceFieldGetters = false)
    {
        return string.Equals(methodRole, "static-method", StringComparison.Ordinal) ||
               (supportsStaticForwarder && string.Equals(methodRole, "static-forwarder", StringComparison.Ordinal)) ||
               (supportsInstanceMethods && string.Equals(methodRole, "instance-method", StringComparison.Ordinal)) ||
               (supportsConstructors && string.Equals(methodRole, "constructor", StringComparison.Ordinal)) ||
               (supportsInstanceFieldGetters && string.Equals(methodRole, "instance-field-getter", StringComparison.Ordinal));
    }

    public static bool IsInstanceLike(
        string methodRole,
        bool supportsConstructors = false,
        bool supportsInstanceFieldGetters = false)
    {
        return string.Equals(methodRole, "instance-method", StringComparison.Ordinal) ||
               (supportsConstructors && string.Equals(methodRole, "constructor", StringComparison.Ordinal)) ||
               (supportsInstanceFieldGetters && string.Equals(methodRole, "instance-field-getter", StringComparison.Ordinal));
    }

    public static bool TryCreateStandardReturnContract(
        string managedType,
        out RuntimeSkeletonManagedInvokeReturnContract contract,
        Func<string, bool>? isDirectReferenceReturnType = null,
        bool includeIndirectFlag = true)
    {
        if (string.Equals(managedType, "System.Void", StringComparison.Ordinal))
        {
            contract = new RuntimeSkeletonManagedInvokeReturnContract(
                managedType,
                string.Empty,
                string.Empty,
                string.Empty,
                "nullptr",
                "0u",
                "false");
            return true;
        }

        if (isDirectReferenceReturnType?.Invoke(managedType) == true)
        {
            contract = new RuntimeSkeletonManagedInvokeReturnContract(
                managedType,
                "void** return_value;",
                "if (request->return_value == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }",
                string.Empty,
                "request->return_value",
                "sizeof(void*)",
                "false");
            return true;
        }

        contract = new RuntimeSkeletonManagedInvokeReturnContract(
            managedType,
            includeIndirectFlag
                ? "void* return_value;\n    std::size_t return_value_size;\n    bool return_value_is_indirect;"
                : "void* return_value;\n    std::size_t return_value_size;",
            "if (request->return_value == nullptr || request->return_value_size == 0u) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }",
            "void* return_value_ptr = request->return_value;",
            includeIndirectFlag
                ? "request->return_value_is_indirect ? static_cast<void*>(&return_value_ptr) : request->return_value"
                : "&return_value_ptr",
            "request->return_value_size",
            includeIndirectFlag ? "request->return_value_is_indirect" : "false");
        return true;
    }

    public static string GetMethodReturnType(string subjectId)
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
