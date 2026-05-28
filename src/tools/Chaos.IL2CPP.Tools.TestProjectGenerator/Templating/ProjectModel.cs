using Scriban.Runtime;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Metadata;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator.Templating;

/// <summary>
/// Builds the ScriptObject model passed to Scriban templates for project generation.
/// </summary>
internal sealed class ProjectModelBuilder
{
    public ScriptObject Build(
        IReadOnlyList<SubjectModel> subjects,
        bool isJit,
        string configTier,
        bool isWindows,
        bool hasPatchData,
        int patchDataSize,
        string patchDataHostClass,
        string projectName,
        DateTime? generatedAt = null)
    {
        var model = new ScriptObject();

        // ── General settings ──
        model["is_jit"] = isJit;
        model["config_tier"] = configTier;
        model["is_windows"] = isWindows;
        model["project_name"] = projectName;
        model["has_patch_data"] = hasPatchData;
        model["patch_data_size"] = patchDataSize;
        model["patch_data_host_class"] = patchDataHostClass;
        model["sdk_dir"] = "${CMAKE_CURRENT_SOURCE_DIR}/chaos-sdk";
        model["generated_at"] = (generatedAt ?? DateTime.UtcNow).ToString("O");

        // ── Subject metadata ──
        var subjectCount = subjects.Count;
        model["subject_count"] = subjectCount;
        model["has_subjects"] = subjectCount > 0;
        model["has_hotupdate"] = subjects.Any(s => s.Kind == SubjectKind.HotUpdate);
        model["has_benchmark"] = subjects.Any(s => s.Kind == SubjectKind.Benchmark);

        // Fact / benchmark / hotupdate counts
        model["fact_count"] = subjects.Count(s => s.Kind == SubjectKind.Fact);
        model["benchmark_count"] = subjects.Count(s => s.Kind == SubjectKind.Benchmark);
        model["hotupdate_count"] = subjects.Count(s => s.Kind == SubjectKind.HotUpdate);

        // Subject list for --list mode
        var subjectArray = new ScriptArray();
        foreach (var s in subjects)
        {
            var entry = new ScriptObject
            {
                ["index"] = subjectArray.Count,
                ["subject_id"] = EscapeCppString(s.SubjectId),
                ["assembly_name"] = EscapeCppString(s.AssemblyName),
                ["type_name"] = EscapeCppString(s.FullTypeName),
                ["method_name"] = EscapeCppString(s.MethodName),
                ["is_static"] = s.IsStatic,
                ["kind"] = s.Kind.ToString().ToLowerInvariant(),
            };
            subjectArray.Add(entry);
        }
        model["subjects"] = subjectArray;

        // ── Config tier defines ──
        var defines = new ScriptObject();
        switch (configTier.ToLowerInvariant())
        {
            case "profile":
                defines["config_tier_value"] = "CHAOS_IL2CPP_CONFIG_TIER_PROFILE";
                defines["config_tier_macro"] = "CHAOS_IL2CPP_CONFIG_PROFILE";
                defines["log_level"] = 2;
                break;
            case "ship":
                defines["config_tier_value"] = "CHAOS_IL2CPP_CONFIG_TIER_SHIP";
                defines["config_tier_macro"] = "CHAOS_IL2CPP_CONFIG_SHIP";
                defines["log_level"] = 0;
                break;
            default: // "debug" / "check"
                defines["config_tier_value"] = "CHAOS_IL2CPP_CONFIG_TIER_CHECK";
                defines["config_tier_macro"] = "CHAOS_IL2CPP_CONFIG_CHECK";
                defines["log_level"] = 3;
                break;
        }
        model["defines"] = defines;

        return model;
    }

    private static string EscapeCppString(string value)
    {
        return value
            .Replace("\\", "\\\\")
            .Replace("\"", "\\\"")
            .Replace("\n", "\\n")
            .Replace("\r", "\\r")
            .Replace("\t", "\\t");
    }
}
