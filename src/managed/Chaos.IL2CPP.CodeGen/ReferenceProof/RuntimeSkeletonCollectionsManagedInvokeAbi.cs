using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal sealed record RuntimeSkeletonCollectionsManagedInvokeArgShape(
    string FieldDeclaration,
    string ArgReferenceExpression);

internal sealed record RuntimeSkeletonCollectionsManagedInvokeAbi(
    string ContractId,
    string ThisFieldDeclaration,
    string ThisValidationStatement,
    string ThisArgumentExpression,
    IReadOnlyList<RuntimeSkeletonCollectionsManagedInvokeArgShape> ArgumentShapes,
    RuntimeSkeletonManagedInvokeReturnContract ReturnShape)
{
    public const string HotUpdateStableContractId = "collections-managed-invoke-v1";

    public int ArgumentCount => ArgumentShapes.Count;

    public int ArgumentStorageSize => Math.Max(1, ArgumentCount);

    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonCollectionsManagedInvokeAbi abi) =>
        RuntimeSkeletonCollectionsManagedInvokeAbiFactory.TryCreate(method, out abi);
}

internal static class RuntimeSkeletonCollectionsManagedInvokeAbiFactory
{
    public static bool TryCreate(
        TypedIlMethodArtifact method,
        out RuntimeSkeletonCollectionsManagedInvokeAbi abi)
    {
        abi = null!;
        if (!MatchesCollectionsPrefix(method.SubjectId) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !IsSupportedMethodRole(method.MethodRole))
        {
            return false;
        }

        var argumentShapes = new List<RuntimeSkeletonCollectionsManagedInvokeArgShape>(method.Parameters.Count);
        for (var parameterIndex = 0; parameterIndex < method.Parameters.Count; parameterIndex++)
        {
            CreateArgumentShape(method.Parameters[parameterIndex].Type, parameterIndex, out var argumentShape);
            argumentShapes.Add(argumentShape);
        }

        var hasThisArgument = IsInstanceLike(method.MethodRole);
        if (!RuntimeSkeletonManagedInvokeAbiCore.TryCreateStandardReturnContract(
                RuntimeSkeletonManagedInvokeAbiCore.GetMethodReturnType(method.SubjectId),
                out var returnShape,
                IsDirectOrOpaqueReturnType,
                includeIndirectFlag: true))
        {
            return false;
        }

        abi = new RuntimeSkeletonCollectionsManagedInvokeAbi(
            RuntimeSkeletonCollectionsManagedInvokeAbi.HotUpdateStableContractId,
            hasThisArgument ? "void* this_arg;" : string.Empty,
            hasThisArgument
                ? "if (request->this_arg == nullptr) {\n        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;\n    }"
                : string.Empty,
            hasThisArgument ? "request->this_arg" : "nullptr",
            argumentShapes,
            returnShape);
        return true;
    }

    private static bool MatchesCollectionsPrefix(string subjectId)
    {
        return subjectId.Contains("/System.Collections.", StringComparison.Ordinal) ||
               subjectId.Contains("Stack`1", StringComparison.Ordinal) ||
               subjectId.Contains("LinkedList`1", StringComparison.Ordinal) ||
               subjectId.Contains("SortedDictionary`2", StringComparison.Ordinal);
    }

    private static bool IsSupportedMethodRole(string methodRole)
    {
        return string.Equals(methodRole, "static-method", StringComparison.Ordinal) ||
               string.Equals(methodRole, "static-forwarder", StringComparison.Ordinal) ||
               string.Equals(methodRole, "instance-method", StringComparison.Ordinal) ||
               string.Equals(methodRole, "constructor", StringComparison.Ordinal) ||
               string.Equals(methodRole, "instance-field-getter", StringComparison.Ordinal);
    }

    private static bool IsInstanceLike(string methodRole)
    {
        return string.Equals(methodRole, "instance-method", StringComparison.Ordinal) ||
               string.Equals(methodRole, "constructor", StringComparison.Ordinal) ||
               string.Equals(methodRole, "instance-field-getter", StringComparison.Ordinal);
    }

    private static void CreateArgumentShape(
        string managedType,
        int parameterIndex,
        out RuntimeSkeletonCollectionsManagedInvokeArgShape shape)
    {
        if (TryResolveTypedValueCarrier(managedType, out var cppCarrier))
        {
            shape = new RuntimeSkeletonCollectionsManagedInvokeArgShape(
                $"{cppCarrier} arg{parameterIndex};",
                $"const_cast<void*>(reinterpret_cast<const void*>(&request->arg{parameterIndex}))");
            return;
        }

        shape = new RuntimeSkeletonCollectionsManagedInvokeArgShape(
            $"void* arg{parameterIndex};",
            $"request->arg{parameterIndex}");
    }

    private static bool IsDirectOrOpaqueReturnType(string managedType)
    {
        if (string.Equals(managedType, "System.Void", StringComparison.Ordinal))
            return false;

        if (managedType.StartsWith("!", StringComparison.Ordinal))
            return false;

        if (IsKnownPrimitiveType(managedType))
            return false;

        if (managedType.StartsWith("System.Collections.Generic.", StringComparison.Ordinal) &&
            (managedType.Contains("+Enumerator", StringComparison.Ordinal) ||
             managedType.Contains("+KeyCollection", StringComparison.Ordinal) ||
             managedType.Contains("+ValueCollection", StringComparison.Ordinal)))
        {
            return false;
        }

        if (managedType.StartsWith("System.Collections.Generic.KeyValuePair<", StringComparison.Ordinal) ||
            string.Equals(managedType, "System.Collections.Generic.KeyValuePair`2", StringComparison.Ordinal))
        {
            return false;
        }

        if (managedType.StartsWith("System.ValueTuple<", StringComparison.Ordinal) ||
            managedType.StartsWith("System.ValueTuple`", StringComparison.Ordinal))
        {
            return false;
        }

        if (managedType.StartsWith("System.Nullable<", StringComparison.Ordinal))
        {
            return false;
        }

        if (managedType.StartsWith("System.", StringComparison.Ordinal) &&
            (managedType.EndsWith("Comparison", StringComparison.Ordinal) ||
             managedType.EndsWith("Kind", StringComparison.Ordinal) ||
             managedType.EndsWith("Flags", StringComparison.Ordinal)))
        {
            return false;
        }

        return true;
    }

    private static bool IsKnownPrimitiveType(string managedType) => managedType switch
    {
        "System.Boolean" => true,
        "System.Byte" => true,
        "System.SByte" => true,
        "System.Int16" => true,
        "System.UInt16" => true,
        "System.Char" => true,
        "System.Int32" => true,
        "System.UInt32" => true,
        "System.Int64" => true,
        "System.UInt64" => true,
        "System.Single" => true,
        "System.Double" => true,
        "System.IntPtr" => true,
        "System.UIntPtr" => true,
        "System.StringComparison" => true,
        _ => false,
    };

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
            "System.IntPtr" => "CHAOS_IL2CPP_INTPTR",
            "System.UIntPtr" => "CHAOS_IL2CPP_UINTPTR",
            "System.StringComparison" => "CHAOS_IL2CPP_INT32",
            "System.Collections.Generic.InsertionBehavior" => "CHAOS_IL2CPP_INT32",
            _ => string.Empty,
        };

        return cppCarrier.Length > 0;
    }
}
