using System.Text;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

/// <summary>
/// Partial class for generating per-DLL module registration code.
/// Emits a ModuleDescriptor with the assembly name and a call to
/// RegisterModule() via a static initializer in the anonymous namespace.
/// Tier 1 (type_flags/names/namespaces) and Tier 2 (image) metadata
/// are deferred — filled in when per-assembly metadata generation is active.
/// </summary>
public sealed partial class NativeAotLoweringPlanner
{
    internal string BuildModuleRegistration()
    {
        var assemblyName = _assemblyName;
        if (string.IsNullOrWhiteSpace(assemblyName))
        {
            return string.Empty;
        }

        var sb = new StringBuilder(8192);
        sb.AppendLine("// ── Module registration ──────────────────────────────────────");

        bool hasTypeData = _moduleTypeCount > 0 && _moduleTypeFlags.Count == _moduleTypeCount;

        if (hasTypeData)
        {
            int count = _moduleTypeCount;

            // ── type_flags array ──────────────────────────────────────────
            sb.Append("static constexpr uint32_t s_type_flags[").Append(count).AppendLine("] =");
            sb.AppendLine("{");
            for (int i = 0; i < count; i++)
            {
                sb.Append("    ").Append(_moduleTypeFlags[i]).Append("u,");
                if (i < count - 1) sb.AppendLine();
            }
            sb.AppendLine();
            sb.AppendLine("};");
            sb.AppendLine();

            // ── type_names array ──────────────────────────────────────────
            sb.Append("static constexpr const char* s_type_names[").Append(count).AppendLine("] =");
            sb.AppendLine("{");
            for (int i = 0; i < count; i++)
            {
                sb.Append("    \"").Append(EscapeCppStringLiteral(_moduleTypeNames[i])).AppendLine("\",");
            }
            sb.AppendLine("};");
            sb.AppendLine();

            // ── type_namespaces array ─────────────────────────────────────
            sb.Append("static constexpr const char* s_type_namespaces[").Append(count).AppendLine("] =");
            sb.AppendLine("{");
            for (int i = 0; i < count; i++)
            {
                sb.Append("    \"").Append(EscapeCppStringLiteral(_moduleTypeNamespaces[i])).AppendLine("\",");
            }
            sb.AppendLine("};");
            sb.AppendLine();

            // ── type_parent_tokens array ──────────────────────────────────
            sb.Append("static constexpr uint32_t s_type_parent_tokens[").Append(count).AppendLine("] =");
            sb.AppendLine("{");
            for (int i = 0; i < count; i++)
            {
                sb.Append("    ").Append(_moduleTypeParentTokens[i]).Append("u,");
                if (i < count - 1) sb.AppendLine();
            }
            sb.AppendLine();
            sb.AppendLine("};");
            sb.AppendLine();

            // ── type_info_ptrs array (NOT constexpr — addresses of inline variables) ──
            sb.Append("static const TypeInfo* const s_type_info_ptrs[").Append(count).AppendLine("] =");
            sb.AppendLine("{");
            for (int i = 0; i < count; i++)
            {
                string? symbol = _moduleTypeInfoSymbols[i];
                if (symbol != null)
                    sb.Append("    ").Append(symbol).AppendLine(",");
                else
                    sb.AppendLine("    nullptr,");
            }
            sb.AppendLine("};");
            sb.AppendLine();
        }

        sb.Append("static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {");
        sb.AppendLine();
        sb.Append("    /* .name_utf8         = */ \"").Append(EscapeCppStringLiteral(assemblyName)).AppendLine("\",");
        sb.AppendLine("    /* .image             = */ nullptr,  // Tier 2 metadata — deferred");
        if (hasTypeData)
        {
            sb.AppendLine("    /* .type_flags        = */ s_type_flags,");
            sb.AppendLine("    /* .type_names        = */ s_type_names,");
            sb.AppendLine("    /* .type_namespaces   = */ s_type_namespaces,");
            sb.AppendLine("    /* .type_parent_tokens= */ s_type_parent_tokens,");
            sb.AppendLine("    /* .type_info_ptrs    = */ s_type_info_ptrs,");
            sb.Append("    /* .type_count        = */ ").Append(_moduleTypeCount).AppendLine("u,");
        }
        else
        {
            sb.AppendLine("    /* .type_flags        = */ nullptr,  // Tier 1 — deferred");
            sb.AppendLine("    /* .type_names        = */ nullptr,");
            sb.AppendLine("    /* .type_namespaces   = */ nullptr,");
            sb.AppendLine("    /* .type_parent_tokens= */ nullptr,");
            sb.AppendLine("    /* .type_info_ptrs    = */ nullptr,");
            sb.AppendLine("    /* .type_count        = */ 0u,");
        }
        sb.AppendLine("    /* .abi_manifest      = */ s_abi_manifest,");
        sb.AppendLine("};");
        sb.AppendLine("static const uint32_t s_native_aot_module_id =");
        sb.Append("    ::chaos::il2cpp::runtime_core::RegisterModule(\"").Append(EscapeCppStringLiteral(assemblyName)).AppendLine("\", &s_native_aot_module);");
        return sb.ToString();
    }

    internal static string BuildAbiManifest(IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        if (reachableMethods.Count == 0)
        {
            return string.Empty;
        }

        var sb = new StringBuilder(4096);
        sb.AppendLine("// ── ABI manifest ──────────────────────────────────────────────");
        sb.AppendLine("// Single contiguous struct: header + entries + params in same object");
        sb.AppendLine("// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.");
        sb.AppendLine("// NOTE: reinterpret_cast is needed because MSVC rejects &anon_struct.header");

        int totalParams = reachableMethods.Sum(m => m.ParameterAbis.Count);
        uint checksum = ComputeAbiManifestChecksum(reachableMethods);

        sb.Append("static constexpr struct {").AppendLine();
        sb.Append("    ::ChaosAbiManifestV0 header;").AppendLine();
        sb.Append("    ::ChaosAbiMethodEntryV0 entries[").Append(reachableMethods.Count).AppendLine("];");
        sb.Append("    uint8_t params[").Append(totalParams).AppendLine("];");
        sb.Append("} s_abi_manifest_storage = {").AppendLine();
        sb.AppendLine("    {");
        sb.AppendLine("        CHAOS_ABI_MANIFEST_VERSION,");
        sb.Append("        ").Append(reachableMethods.Count).AppendLine("u,");
        sb.Append("        ").Append(totalParams).AppendLine("u,");
        sb.Append("        ").Append(checksum).AppendLine("u,  // FNV-1a over entries+params");
        sb.AppendLine("    },");
        sb.AppendLine("    {");
        for (int i = 0; i < reachableMethods.Count; i++)
        {
            var method = reachableMethods[i];
            sb.Append("        { ").Append((int)method.ReturnAbi.CarrierKindCode).Append("u, ")
                .Append(method.ParameterAbis.Count).Append("u },");
            sb.Append("  // ").Append(method.NativeSymbol);
            sb.AppendLine();
        }
        sb.AppendLine("    },");
        sb.AppendLine("    {");
        if (totalParams > 0)
        {
            int paramIndex = 0;
            foreach (var method in reachableMethods)
            {
                foreach (var abi in method.ParameterAbis)
                {
                    sb.Append("        ").Append((int)abi.CarrierKindCode).Append("u,");
                    paramIndex++;
                    if (paramIndex < totalParams)
                        sb.AppendLine();
                }
            }
            sb.AppendLine();
        }
        sb.AppendLine("    },");
        sb.AppendLine("};");
        sb.Append("static const ::ChaosAbiManifestV0* const s_abi_manifest =");
        sb.Append(" reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);");
        sb.AppendLine();

        return sb.ToString();
    }

    private static string EscapeCppStringLiteral(string value)
    {
        return value
            .Replace("\\", "\\\\")
            .Replace("\"", "\\\"")
            .Replace("\n", "\\n")
            .Replace("\r", "\\r")
            .Replace("\t", "\\t");
    }

    private static uint ComputeAbiManifestChecksum(IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        // FNV-1a hash over the entries+params payload layout that the native
        // ChaosAbiManifestValidate hashes (everything starting after the
        // checksum field): entries[] then parameters[].
        const uint fnvOffsetBasis = 2166136261u;
        const uint fnvPrime = 16777619u;

        uint hash = fnvOffsetBasis;

        // Hash entries: each ChaosAbiMethodEntryV0 = 2 bytes (return_carrier, parameter_count)
        foreach (var method in reachableMethods)
        {
            hash ^= (uint)method.ReturnAbi.CarrierKindCode;
            hash *= fnvPrime;
            hash ^= (uint)method.ParameterAbis.Count;
            hash *= fnvPrime;
        }

        // Hash parameters array
        foreach (var method in reachableMethods)
        {
            foreach (var abi in method.ParameterAbis)
            {
                hash ^= (uint)abi.CarrierKindCode;
                hash *= fnvPrime;
            }
        }

        return hash;
    }
}
