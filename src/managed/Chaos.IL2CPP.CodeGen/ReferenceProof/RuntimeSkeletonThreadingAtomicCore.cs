using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

internal static class RuntimeSkeletonThreadingAtomicCore
{
    private const string AtomicContractId = "threading-atomic-v1";

    public static bool TryBuild(
        TypedIlMethodArtifact method,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!TryClassifyAtomicMethod(method.SubjectId, out var operation, out var managedType) ||
            !TryBuildAtomicStatements(operation, managedType, out var statements))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = AtomicContractId,
            ["atomic_statements"] = statements,
            ["return_managed_type"] = managedType ?? "System.Void",
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonThreadingAtomicStubTemplate(),
            model);
        return true;
    }

    private enum AtomicOperation
    {
        Increment,
        Decrement,
        Add,
        Exchange,
        CompareExchange,
        And,
        Or,
        Read,
        Write,
        MemoryBarrier,
    }

    private static bool TryClassifyAtomicMethod(string subjectId, out AtomicOperation operation, out string? managedType)
    {
        operation = default;
        managedType = null;

        if (subjectId.Contains("/System.Threading.Interlocked::Increment", StringComparison.Ordinal))
        {
            operation = AtomicOperation.Increment;
            managedType = ExtractReturnType(subjectId);
            return managedType is "System.Int32" or "System.Int64";
        }
        if (subjectId.Contains("/System.Threading.Interlocked::Decrement", StringComparison.Ordinal))
        {
            operation = AtomicOperation.Decrement;
            managedType = ExtractReturnType(subjectId);
            return managedType is "System.Int32" or "System.Int64";
        }
        if (subjectId.Contains("/System.Threading.Interlocked::Add", StringComparison.Ordinal))
        {
            operation = AtomicOperation.Add;
            managedType = ExtractReturnType(subjectId);
            return managedType is "System.Int32" or "System.Int64" or "System.UInt32" or "System.UInt64";
        }
        if (subjectId.Contains("/System.Threading.Interlocked::Exchange", StringComparison.Ordinal))
        {
            operation = AtomicOperation.Exchange;
            managedType = ExtractReturnType(subjectId);
            return true;
        }
        if (subjectId.Contains("/System.Threading.Interlocked::CompareExchange", StringComparison.Ordinal))
        {
            operation = AtomicOperation.CompareExchange;
            managedType = ExtractReturnType(subjectId);
            return true;
        }
        if (subjectId.Contains("/System.Threading.Interlocked::And", StringComparison.Ordinal))
        {
            operation = AtomicOperation.And;
            managedType = ExtractReturnType(subjectId);
            return managedType is "System.Int32" or "System.Int64" or "System.UInt32" or "System.UInt64";
        }
        if (subjectId.Contains("/System.Threading.Interlocked::Or", StringComparison.Ordinal))
        {
            operation = AtomicOperation.Or;
            managedType = ExtractReturnType(subjectId);
            return managedType is "System.Int32" or "System.Int64" or "System.UInt32" or "System.UInt64";
        }
        if (subjectId.Contains("/System.Threading.Interlocked::Read", StringComparison.Ordinal))
        {
            operation = AtomicOperation.Read;
            managedType = "System.Int64";
            return true;
        }
        if (subjectId.Contains("/System.Threading.Interlocked::MemoryBarrier", StringComparison.Ordinal))
        {
            operation = AtomicOperation.MemoryBarrier;
            managedType = "System.Void";
            return true;
        }
        if (subjectId.Contains("/System.Threading.Volatile::Read", StringComparison.Ordinal))
        {
            operation = AtomicOperation.Read;
            managedType = ExtractReturnType(subjectId);
            return true;
        }
        if (subjectId.Contains("/System.Threading.Volatile::Write", StringComparison.Ordinal))
        {
            operation = AtomicOperation.Write;
            managedType = ExtractReturnType(subjectId);
            return managedType is not null;
        }

        return false;
    }

    private static string? ExtractReturnType(string subjectId)
    {
        var paramStart = subjectId.IndexOf('(', StringComparison.Ordinal);
        if (paramStart < 0) return null;
        var returnStart = subjectId.LastIndexOf(':', paramStart - 1);
        if (returnStart < 0) return null;
        return subjectId[(returnStart + 1)..paramStart];
    }

    private static string ToCppType(string managedType)
    {
        return managedType switch
        {
            "System.Int32" => "int32_t",
            "System.UInt32" => "uint32_t",
            "System.Int64" => "int64_t",
            "System.UInt64" => "uint64_t",
            "System.IntPtr" => "intptr_t",
            "System.UIntPtr" => "uintptr_t",
            "System.Single" => "float",
            "System.Double" => "double",
            "System.Void" => "void",
            "System.Boolean" => "bool",
            "System.Object" => "void*",
            _ when managedType.EndsWith("&", StringComparison.Ordinal) => ToCppType(managedType[..^1]),
            _ when managedType.EndsWith("*", StringComparison.Ordinal) => ToCppType(managedType[..^1]),
            _ => "void*",
        };
    }

    private static bool TryBuildAtomicStatements(
        AtomicOperation operation,
        string? managedType,
        out string statements)
    {
        statements = string.Empty;

        switch (operation)
        {
            case AtomicOperation.MemoryBarrier:
                statements = "std::atomic_thread_fence(std::memory_order_seq_cst);";
                return true;

            case AtomicOperation.Increment:
            {
                var cppType = ToCppType(managedType!);
                if (managedType == "System.Int64" || managedType == "System.UInt64")
                {
                    statements = $"auto* ptr = static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->arg0);\n" +
                                $"    auto result = ptr->fetch_add(1, std::memory_order_seq_cst) + 1;\n" +
                                $"    *static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->return_value) = result;\n" +
                                $"    return CHAOS_BRIDGE_STATUS_OK;";
                }
                else
                {
                    statements = $"auto* ptr = static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->arg0);\n" +
                                $"    auto result = ptr->fetch_add(1, std::memory_order_seq_cst) + 1;\n" +
                                $"    *static_cast<{cppType}*>(request->return_value) = result;\n" +
                                $"    return CHAOS_BRIDGE_STATUS_OK;";
                }
                return true;
            }

            case AtomicOperation.Decrement:
            {
                var cppType = ToCppType(managedType!);
                if (managedType == "System.Int64" || managedType == "System.UInt64")
                {
                    statements = $"auto* ptr = static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->arg0);\n" +
                                $"    auto result = ptr->fetch_sub(1, std::memory_order_seq_cst) - 1;\n" +
                                $"    *static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->return_value) = result;\n" +
                                $"    return CHAOS_BRIDGE_STATUS_OK;";
                }
                else
                {
                    statements = $"auto* ptr = static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->arg0);\n" +
                                $"    auto result = ptr->fetch_sub(1, std::memory_order_seq_cst) - 1;\n" +
                                $"    *static_cast<{cppType}*>(request->return_value) = result;\n" +
                                $"    return CHAOS_BRIDGE_STATUS_OK;";
                }
                return true;
            }

            case AtomicOperation.Add:
            {
                var cppType = ToCppType(managedType!);
                if (managedType == "System.Int64" || managedType == "System.UInt64")
                {
                    statements = $"auto* ptr = static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->arg0);\n" +
                                $"    auto value = *static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->arg1);\n" +
                                $"    auto result = ptr->fetch_add(value, std::memory_order_seq_cst) + value;\n" +
                                $"    *static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->return_value) = result;\n" +
                                $"    return CHAOS_BRIDGE_STATUS_OK;";
                }
                else
                {
                    statements = $"auto* ptr = static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->arg0);\n" +
                                $"    auto value = *static_cast<{cppType}*>(request->arg1);\n" +
                                $"    auto result = ptr->fetch_add(value, std::memory_order_seq_cst) + value;\n" +
                                $"    *static_cast<{cppType}*>(request->return_value) = result;\n" +
                                $"    return CHAOS_BRIDGE_STATUS_OK;";
                }
                return true;
            }

            case AtomicOperation.Exchange:
            {
                var cppType = ToCppType(managedType!);
                statements = $"auto* ptr = static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->arg0);\n" +
                            $"    auto value = *static_cast<{cppType}*>(request->arg1);\n" +
                            $"    auto result = ptr->exchange(value, std::memory_order_seq_cst);\n" +
                            $"    *static_cast<{cppType}*>(request->return_value) = result;\n" +
                            $"    return CHAOS_BRIDGE_STATUS_OK;";
                return true;
            }

            case AtomicOperation.CompareExchange:
            {
                var cppType = ToCppType(managedType!);
                statements = $"auto* ptr = static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->arg0);\n" +
                            $"    auto expected = *static_cast<{cppType}*>(request->arg1);\n" +
                            $"    auto desired = *static_cast<{cppType}*>(request->arg2);\n" +
                            $"    auto result = ptr->compare_exchange_strong(expected, desired, std::memory_order_seq_cst);\n" +
                            $"    *static_cast<{cppType}*>(request->return_value) = result ? expected : ptr->load();\n" +
                            $"    return CHAOS_BRIDGE_STATUS_OK;";
                return true;
            }

            case AtomicOperation.And:
            {
                var cppType = ToCppType(managedType!);
                statements = $"auto* ptr = static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->arg0);\n" +
                            $"    auto value = *static_cast<{cppType}*>(request->arg1);\n" +
                            $"    auto result = ptr->fetch_and(value, std::memory_order_seq_cst);\n" +
                            $"    *static_cast<{cppType}*>(request->return_value) = result & value;\n" +
                            $"    return CHAOS_BRIDGE_STATUS_OK;";
                return true;
            }

            case AtomicOperation.Or:
            {
                var cppType = ToCppType(managedType!);
                statements = $"auto* ptr = static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->arg0);\n" +
                            $"    auto value = *static_cast<{cppType}*>(request->arg1);\n" +
                            $"    auto result = ptr->fetch_or(value, std::memory_order_seq_cst);\n" +
                            $"    *static_cast<{cppType}*>(request->return_value) = result | value;\n" +
                            $"    return CHAOS_BRIDGE_STATUS_OK;";
                return true;
            }

            case AtomicOperation.Read:
            {
                var cppType = ToCppType(managedType!);
                statements = $"auto* ptr = static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->arg0);\n" +
                            $"    auto result = ptr->load(std::memory_order_seq_cst);\n" +
                            $"    *static_cast<{cppType}*>(request->return_value) = result;\n" +
                            $"    return CHAOS_BRIDGE_STATUS_OK;";
                return true;
            }

            case AtomicOperation.Write:
            {
                var cppType = ToCppType(managedType!);
                statements = $"auto* ptr = static_cast<CHAOS_IL2CPP_ATOMIC({cppType})*>(request->arg0);\n" +
                            $"    auto value = *static_cast<{cppType}*>(request->arg1);\n" +
                            $"    ptr->store(value, std::memory_order_seq_cst);\n" +
                            $"    return CHAOS_BRIDGE_STATUS_OK;";
                return true;
            }

            default:
                return false;
        }
    }
}
