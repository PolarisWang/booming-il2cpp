using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator.BuildSystem;

/// <summary>
/// Generates CMake build files for per-assembly IL2CPP translation outputs.
/// Each assembly becomes a STATIC library, and a combined entry.exe links them
/// with the pre-built native runtime libs.
/// </summary>
public sealed class CmakeGenerator
{
    private readonly string _repoRoot;

    public CmakeGenerator(string repoRoot)
    {
        _repoRoot = repoRoot;
    }

    /// <summary>
    /// Generate a CMakeLists.txt for the given assembly codegen results.
    /// </summary>
    public string Generate(
        IReadOnlyList<NativeAotResult> assemblyResults,
        string nativeLibDir,
        string buildConfig = "RelWithDebInfo",
        IReadOnlyList<string>? extraSources = null,
        string targetName = "entry_exe")
    {
        var repoRootNormalized = _repoRoot.Replace('\\', '/');
        var nativeLibDirNormalized = nativeLibDir.Replace('\\', '/');

        // Native .lib paths (relative to NATIVE_LIB_DIR)
        var nativeLibs = new[]
        {
            "src/native/runtime-core/${CFG}/chaos_runtime_core.lib",
            "src/native/bootstrap/${CFG}/chaos_bootstrap.lib",
            "src/native/common/${CFG}/chaos_common.lib",
            "src/native/support/${CFG}/chaos_support.lib",
            "src/native/interpreter/${CFG}/chaos_interpreter.lib",
            "src/native/hot-update/${CFG}/chaos_hot_update.lib",
            "src/native/codegen/${CFG}/chaos_codegen.lib",
            "src/native/diagnostics/debugger/${CFG}/chaos_debugger.lib",
            "src/native/diagnostics/eventpipe/${CFG}/chaos_eventpipe.lib",
            "fmt_build/${CFG}/chaos_fmt.lib",
        };

        // Include dirs (relative to REPO_ROOT)
        var includeDirs = new[]
        {
            "contracts/native/v0",
            "src/native/common",
            "src/native/runtime-core",
            "src/native/runtime-core/gc",
            "src/native/runtime-core/runtime_stubs",
            "src/native/bootstrap",
            "src/native/interpreter",
            "src/native/codegen",
            "src/native/support",
            "src/native/hot-update",
            "src/native",
            "third_party/fmt/include",
            "third_party/unordered_dense/include",
        };

        // Build per-assembly static library entries
        var assemblies = new List<object>();
        var staticTargets = new List<string>();
        foreach (var result in assemblyResults)
        {
            var assemblyName = SanitizeTargetName(result.Manifest.AssemblyName);
            var sourceFiles = result.GeneratedSources
                .Select(s => $"${{CHAOS_GEN_DIR}}/{s.RelativePath.Replace('\\', '/')}")
                .Where(p => p.EndsWith(".cpp", StringComparison.OrdinalIgnoreCase))
                .ToList();

            if (sourceFiles.Count == 0)
                continue;

            var target = $"chaos_gen_{assemblyName}";
            staticTargets.Add(target);

            assemblies.Add(new
            {
                name = result.Manifest.AssemblyName,
                target,
                sources = sourceFiles,
            });
        }

        var extraSrcs = (extraSources ?? [])
            .Select(s => $"${{CHAOS_GEN_DIR}}/{s.Replace('\\', '/')}")
            .ToList();

        var model = new ScriptObject
        {
            ["repo_root"] = repoRootNormalized,
            ["native_lib_dir"] = nativeLibDirNormalized,
            ["build_config"] = buildConfig,
            ["native_libs"] = nativeLibs,
            ["include_dirs"] = includeDirs,
            ["compile_options"] = "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>",
            ["assemblies"] = assemblies,
            ["static_targets"] = staticTargets,
            ["target_name"] = targetName,
            ["extra_sources"] = extraSrcs,
        };

        return ScribanTemplateRenderer.RenderTemplate(
            BuildSystemTemplateCatalog.GetCMakeListsTemplate(), model);
    }

    private static string SanitizeTargetName(string name)
    {
        var sb = new StringBuilder(name.Length);
        foreach (var c in name)
        {
            if (char.IsLetterOrDigit(c))
                sb.Append(c);
            else
                sb.Append('_');
        }
        return sb.ToString();
    }
}