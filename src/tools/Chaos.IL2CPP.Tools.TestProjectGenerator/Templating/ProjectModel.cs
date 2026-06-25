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
        string nativeDir,
        DateTime? generatedAt = null,
        string? projectRoot = null,
        string? codegenDir = null,
        string? sdkDir = null,
        List<string>? codegenAssemblyNames = null,
        bool verificationEnabled = true,
        bool hotupdateActive = false,
        string hostNamespace = "",
        List<string>? hostTypeNames = null,
        List<string>? hostMethodNames = null)
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
        model["sdk_dir"] = NormalizePath(
            sdkDir is not null ? GetRelativePath(nativeDir, sdkDir) : "${CMAKE_CURRENT_SOURCE_DIR}/chaos-sdk");
        model["project_root"] = NormalizePath(GetRelativePath(nativeDir, projectRoot ?? ""));
        model["codegen_dir"] = NormalizePath(
            codegenDir is not null ? GetRelativePath(nativeDir, codegenDir) : "${CMAKE_CURRENT_SOURCE_DIR}/codegen");
        // Prepend CMAKE_CURRENT_SOURCE_DIR so all relative paths resolve
        // correctly from cmake's working directory (build/) instead of nativeDir.
        if (model["project_root"] is string pr && !pr.StartsWith("${", StringComparison.Ordinal) && !pr.StartsWith("/", StringComparison.Ordinal) && pr.IndexOf(':') < 0)
            model["project_root"] = "${CMAKE_CURRENT_SOURCE_DIR}/" + pr;
        if (model["sdk_dir"] is string sd && !sd.StartsWith("${", StringComparison.Ordinal) && !sd.StartsWith("/", StringComparison.Ordinal) && sd.IndexOf(':') < 0)
            model["sdk_dir"] = "${CMAKE_CURRENT_SOURCE_DIR}/" + sd;
        if (model["codegen_dir"] is string cd && !cd.StartsWith("${", StringComparison.Ordinal) && !cd.StartsWith("/", StringComparison.Ordinal) && cd.IndexOf(':') < 0)
            model["codegen_dir"] = "${CMAKE_CURRENT_SOURCE_DIR}/" + cd;
        model["generated_at"] = (generatedAt ?? DateTime.UtcNow).ToString("O");

        // ── Subject metadata ──
        var subjectCount = subjects.Count;
        model["subject_count"] = subjectCount;
        model["has_subjects"] = subjectCount > 0;
        model["has_hotupdate"] = subjects.Any(s => s.Kind == SubjectKind.HotUpdate);
        model["has_benchmark"] = subjects.Any(s => s.Kind == SubjectKind.Benchmark);

        // Fact / benchmark / hotupdate counts
        model["fact_count"] = subjects.Count(s => s.Kind == SubjectKind.Fact && s.GeneratedMethodId is not null);
        model["benchmark_count"] = subjects.Count(s => s.Kind == SubjectKind.Benchmark);
        model["hotupdate_count"] = subjects.Count(s => s.Kind == SubjectKind.HotUpdate);

        // Subject list for --list mode
        var subjectArray = new ScriptArray();
        foreach (var s in subjects)
        {
            if (s is null)
            {
                Console.Error.WriteLine("  [WARN] Skipping null subject entry in model builder");
                continue;
            }

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
                ["function_symbol"] = "CombinedSubjects_AutoGenerated_"
                    + s.FullTypeName.Replace('.', '_').Replace('+', '_').Replace('`', '_')
                        .Replace('<', '_').Replace('>', '_')
                    + "_" + s.MethodName,
                ["has_generated_method_id"] = s.GeneratedMethodId is not null,
            };
            subjectArray.Add(entry);
        }
        model["subjects"] = subjectArray;

        // Generate kSubjectEntries C++ initializer in C# to avoid Scriban for-loop
        // iteration bug where s.index produces "null object" for valid ScriptArray entries.
        var entriesCode = new System.Text.StringBuilder();
        for (int si = 0; si < subjectArray.Count; si++)
        {
            if (subjectArray[si] is ScriptObject so)
            {
                entriesCode.Append("    { ");
                entriesCode.Append(so["index"]);
                entriesCode.Append(", \"");
                entriesCode.Append(so["subject_id"]);
                entriesCode.Append("\", \"");
                entriesCode.Append(so["assembly_name"]);
                entriesCode.Append("\", \"");
                entriesCode.Append(so["type_name"]);
                entriesCode.Append("\", \"");
                entriesCode.Append(so["method_name"]);
                entriesCode.Append("\", ");
                entriesCode.Append(so["is_static"]);
                entriesCode.Append(", \"");
                entriesCode.Append(so["kind"]);
                entriesCode.Append("\", \"");
                entriesCode.Append(so["function_symbol"]);
                entriesCode.Append("\" },\n");
            }
        }
        model["subject_entries_code"] = entriesCode.ToString();

        // Generate metadata JSON entries (avoids Scriban for-loop bug with ScriptObject arrays)
        {
            var metaEntries = new System.Text.StringBuilder();
            metaEntries.AppendLine("[");
            for (int si = 0; si < subjectArray.Count; si++)
            {
                if (subjectArray[si] is ScriptObject so)
                {
                    if (si > 0) metaEntries.AppendLine(",");
                    metaEntries.Append("      { \"index\": ");
                    metaEntries.Append(so["index"]);
                    metaEntries.Append(", \"kind\": \"");
                    metaEntries.Append(so["kind"]);
                    metaEntries.Append("\", \"subjectId\": \"");
                    metaEntries.Append(so["subject_id"]);
                    metaEntries.Append("\" }");
                }
            }
            metaEntries.AppendLine();
            metaEntries.Append("    ]");
            model["subject_metadata_json_code"] = metaEntries.ToString();
        }

        // Generate CMake include directories (deduplicated assembly names)
        {
            var cmakeDirs = new System.Text.StringBuilder();
            var seen = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            for (int si = 0; si < subjectArray.Count; si++)
            {
                if (subjectArray[si] is ScriptObject so)
                {
                    var asmName = so["assembly_name"]?.ToString() ?? "";
                    if (seen.Add(asmName))
                    {
                        cmakeDirs.Append("    \"${CHAOS_CODEGEN_DIR}/");
                        cmakeDirs.Append(asmName);
                        cmakeDirs.AppendLine("/generated\"");
                    }
                }
            }
            model["subject_cmake_include_dirs_code"] = cmakeDirs.ToString();
        }

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

        model["verification_enabled"] = verificationEnabled ? "1" : "0";

        // ── Hotupdate host arrays for patch-host-arrays.cpp ──
        model["hotupdate_active"] = hotupdateActive;
        model["host_namespace"] = hostNamespace;
        if (hostTypeNames is not null)
        {
            var typeArray = new ScriptArray();
            foreach (var n in hostTypeNames)
                typeArray.Add(n);
            model["host_type_names"] = typeArray;
        }
        else
        {
            model["host_type_names"] = new ScriptArray();
        }
        if (hostMethodNames is not null)
        {
            var methodArray = new ScriptArray();
            foreach (var n in hostMethodNames)
                methodArray.Add(n);
            model["host_method_names"] = methodArray;
        }
        else
        {
            model["host_method_names"] = new ScriptArray();
        }

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
    private static string GetRelativePath(string fromPath, string toPath)
    {
        if (string.IsNullOrEmpty(toPath))
            return "";
        try
        {
            return Path.GetRelativePath(fromPath, toPath).Replace('\\', '/');
        }
        catch
        {
            return toPath.Replace('\\', '/');
        }
    }

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
