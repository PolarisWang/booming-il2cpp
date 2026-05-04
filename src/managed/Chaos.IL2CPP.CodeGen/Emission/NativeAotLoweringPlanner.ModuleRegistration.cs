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
        sb.AppendLine("    /* .name_utf8         = */ \"").Append(EscapeCppStringLiteral(assemblyName)).AppendLine("\",");
        sb.AppendLine("    /* .image             = */ nullptr,  // Tier 2 metadata — deferred");
        sb.AppendLine("    /* .type_flags        = */ nullptr,  // Tier 1 — deferred");
        sb.AppendLine("    /* .type_names        = */ nullptr,");
        sb.AppendLine("    /* .type_namespaces   = */ nullptr,");
        sb.AppendLine("    /* .type_parent_tokens= */ nullptr,");
        sb.AppendLine("    /* .type_count        = */ 0u");
        sb.AppendLine("};");
        sb.AppendLine("static const uint32_t s_native_aot_module_id =");
        sb.Append("    ::chaos::il2cpp::runtime_core::RegisterModule(\"").Append(EscapeCppStringLiteral(assemblyName)).AppendLine("\", &s_native_aot_module);");
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
}
