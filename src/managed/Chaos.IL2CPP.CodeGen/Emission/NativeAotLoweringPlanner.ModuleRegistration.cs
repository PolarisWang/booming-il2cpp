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
    internal static string BuildModuleRegistration(NativeAotLoweringPlanArtifact loweringPlan)
    {
        var assemblyName = loweringPlan.AssemblyName;
        if (string.IsNullOrWhiteSpace(assemblyName))
        {
            return string.Empty;
        }

        var sb = new StringBuilder(512);
        sb.AppendLine("// ── Module registration ──────────────────────────────────────");
        sb.AppendLine("static constexpr ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {");
        sb.Append("    /* .name_utf8         = */ \"").Append(EscapeCppStringLiteral(assemblyName)).AppendLine("\",");
        sb.AppendLine("    /* .image             = */ nullptr,  // Tier 2 metadata — deferred");
        sb.AppendLine("    /* .type_flags        = */ nullptr,  // Tier 1 — deferred");
        sb.AppendLine("    /* .type_names        = */ nullptr,");
        sb.AppendLine("    /* .type_namespaces   = */ nullptr,");
        sb.AppendLine("    /* .type_parent_tokens= */ nullptr,");
        sb.AppendLine("    /* .type_count        = */ 0u,");
        sb.AppendLine("    /* .abi_manifest      = */ &s_abi_manifest,");
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

        // ── Parameter carriers array (flat sequence of uint8_t values) ──
        int totalParams = reachableMethods.Sum(m => m.ParameterAbis.Count);
        sb.Append("static constexpr uint8_t s_abi_manifest_params[").Append(totalParams).AppendLine("] =");
        sb.AppendLine("{");
        if (totalParams > 0)
        {
            int paramIndex = 0;
            foreach (var method in reachableMethods)
            {
                foreach (var abi in method.ParameterAbis)
                {
                    sb.Append("    ").Append((int)abi.CarrierKindCode).Append("u,");
                    paramIndex++;
                    if (paramIndex < totalParams)
                        sb.AppendLine();
                }
            }
            sb.AppendLine();
        }
        sb.AppendLine("};");
        sb.AppendLine();

        // ── Method entries array ─────────────────────────────────────────
        sb.Append("static constexpr ::ChaosAbiMethodEntryV0 s_abi_manifest_entries[")
            .Append(reachableMethods.Count)
            .AppendLine("] =");
        sb.AppendLine("{");
        for (int i = 0; i < reachableMethods.Count; i++)
        {
            var method = reachableMethods[i];
            sb.Append("    { ").Append((int)method.ReturnAbi.CarrierKindCode).Append("u, ")
                .Append(method.ParameterAbis.Count).Append("u },");
            if (i < reachableMethods.Count - 1)
            {
                sb.Append("  // ").Append(method.NativeSymbol);
            }
            sb.AppendLine();
        }
        sb.AppendLine("};");
        sb.AppendLine();

        // ── Manifest struct ──────────────────────────────────────────────
        uint checksum = ComputeAbiManifestChecksum(reachableMethods);
        sb.Append("static constexpr ::ChaosAbiManifestV0 s_abi_manifest =").AppendLine();
        sb.AppendLine("{");
        sb.AppendLine("    CHAOS_ABI_MANIFEST_VERSION,");
        sb.Append("    ").Append(reachableMethods.Count).AppendLine("u,");
        sb.Append("    static_cast<uint32_t>(sizeof(s_abi_manifest_params)),");
        sb.Append("    ").Append(checksum).AppendLine("u,  // FNV-1a over entries+params");
        sb.AppendLine("};");

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
