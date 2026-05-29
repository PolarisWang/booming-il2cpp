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
        DateTime? generatedAt = null,
        string? projectRoot = null,
        string? codegenDir = null,
        string? sdkDir = null,
        List<string>? codegenAssemblyNames = null)
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
        model["sdk_dir"] = NormalizePath(sdkDir ?? "${CMAKE_CURRENT_SOURCE_DIR}/chaos-sdk");
        model["project_root"] = NormalizePath(projectRoot ?? "");
        model["codegen_dir"] = NormalizePath(codegenDir ?? "${CMAKE_CURRENT_SOURCE_DIR}/codegen");
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
            // Resolve assembly_name to match codegen output directory.
            // The contract's assembly name (from methodSubjectIds) may not match
            // the actual codegen output directory (e.g. "NativeEntry" vs "Subjects").
            string resolvedAssemblyName;
            if (codegenAssemblyNames is { Count: > 0 })
            {
                // Try exact match first
                var exactMatch = codegenAssemblyNames.Find(
                    n => n.Equals(s.AssemblyName, StringComparison.OrdinalIgnoreCase));
                if (exactMatch is not null)
                {
                    resolvedAssemblyName = exactMatch;
                }
                else if (codegenAssemblyNames.Count == 1)
                {
                    // Single codegen assembly: use it directly
                    resolvedAssemblyName = codegenAssemblyNames[0];
                }
                else
                {
                    // Multiple: find best prefix match
                    var bestMatch = codegenAssemblyNames
                        .Select(n => new { Name = n, PrefixLen = CommonPrefixLength(n, s.AssemblyName) })
                        .OrderByDescending(x => x.PrefixLen)
                        .FirstOrDefault();
                    resolvedAssemblyName = bestMatch?.Name ?? s.AssemblyName;
                }
            }
            else
            {
                resolvedAssemblyName = s.AssemblyName;
            }

            var entry = new ScriptObject
            {
                ["index"] = subjectArray.Count,
                ["subject_id"] = EscapeCppString(s.SubjectId),
                ["assembly_name"] = EscapeCppString(resolvedAssemblyName),
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

    /// <summary>
    /// Normalize Windows backslashes to forward slashes for CMake compatibility.
    /// CMake interprets \ as an escape character in strings, so \a, \c, etc. are errors.
    /// </summary>
    private static string NormalizePath(string path)
    {
        return path.Replace('\\', '/');
    }

    /// <summary>
    /// Length of common prefix between two strings (case-insensitive).
    /// </summary>
    private static int CommonPrefixLength(string a, string b)
    {
        int minLen = Math.Min(a.Length, b.Length);
        int i = 0;
        for (; i < minLen; i++)
        {
            if (char.ToLowerInvariant(a[i]) != char.ToLowerInvariant(b[i]))
                break;
        }
        return i;
    }
}
