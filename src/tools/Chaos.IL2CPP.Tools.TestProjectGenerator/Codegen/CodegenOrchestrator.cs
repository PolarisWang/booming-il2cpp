using System.Text.Json;
using Chaos.IL2CPP.Driver;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator.Codegen;

public sealed class CodegenResult
{
    public bool Success { get; init; }
    public string OutputDir { get; init; } = "";
    public List<string> GeneratedDirs { get; init; } = [];
    public string? Error { get; init; }
}

public sealed class CodegenOrchestrator
{
    /// <summary>
    /// Run IL2CPP codegen on the given assemblies.
    /// </summary>
    /// <param name="assemblyPaths">Paths to managed DLLs to compile.</param>
    /// <param name="outputDir">Output directory for codegen artifacts (--sdk-out).</param>
    /// <param name="codegenMode">Codegen mode: "aot" or "jit".</param>
    /// <param name="subjectMethodIds">Optional list of managed SubjectIds to identify as subject entries.</param>
    /// <param name="assemblyDirs">Optional list of assembly search directories.
    /// Passed as --assembly-dir to ConvertToCppHandler for --full-closure resolution.
    /// When null or empty, only the subjects DLL itself is loaded (D5 default).
    /// Use for game engine scenarios where non-runtime DLLs are needed.</param>
    public CodegenResult Run(IReadOnlyList<string> assemblyPaths, string outputDir,
        string codegenMode = "aot",
        IReadOnlyList<string>? subjectMethodIds = null,
        IReadOnlyList<string>? assemblyDirs = null)
    {
        try
        {
            Directory.CreateDirectory(outputDir);

            // Build args for ConvertToCppHandler
            var args = new List<string>();
            foreach (var dll in assemblyPaths)
            {
                args.Add("--assembly");
                args.Add(Path.GetFullPath(dll));
            }

            // NOTE: We do NOT add runtime assemblies as --assembly here. The Loader
            // uses --full-closure to resolve cross-assembly type references, which
            // works correctly with only the subjects DLL loaded via --assembly.
            // Adding runtime assemblies directly causes closure validation errors
            // (missing entry methods). BRIDGE-AOT independently resolves callee
            // assemblies via the runtime directory for cross-assembly compilation.
            // See closure-precision-roadmap D5 for details.

            // Pass explicitly-configured assembly directories (from pipeline-config.yaml).
            // These are used by --full-closure to find cross-assembly type definitions.
            // By default (assemblyDirs null/empty), only the subjects DLL is loaded —
            // this prevents NuGet transitive dependencies (FSharp.Core, xunit) from
            // bloating the AOT IR. For game engine scenarios with 200+ DLLs, list
            // game engine directories here.
            if (assemblyDirs is { Count: > 0 })
            {
                foreach (var dir in assemblyDirs)
                {
                    if (string.IsNullOrWhiteSpace(dir)) continue;
                    var fullPath = Path.GetFullPath(dir);
                    if (Directory.Exists(fullPath))
                    {
                        args.Add("--assembly-dir");
                        args.Add(fullPath);
                    }
                }
            }

            args.Add("--sdk-out");
            args.Add(Path.GetFullPath(outputDir));
            // NOTE: --full-closure was previously disabled because the old family-based flow
            // generated 49044 methods per family with incompatible function pointer types
            // (C2440 in MSVC).  With the new namespace-chunk pipeline, each chunk contains
            // ≤500 subject wrapper methods plus their transitive closure (~2000-3000 total
            // methods). At this scale the typed dispatch table type incompatibility is
            // manageable — the subject wrappers and the framework methods they call share
            // compatible function pointer types.
            args.Add("--full-closure");

            if (codegenMode == "jit")
            {
                args.Add("--mode");
                args.Add("jit");
            }

            // ── Subject method IDs (--subject-methods) ──
            // Pass the list of subject method SubjectIds to the Driver so that
            // the generated dispatch entry code includes these in kSubjectSlotMap[].
            // The Driver matches these against AotCoreIrMethodArtifact.SubjectId
            // to identify which compiled methods are subject entries.
            string? subjectMethodsPath = null;
            if (subjectMethodIds is { Count: > 0 })
            {
                subjectMethodsPath = Path.Combine(Path.GetTempPath(),
                    $"chaos_subject_methods_{Guid.NewGuid():N}.json");
                var smJson = JsonSerializer.Serialize(new
                {
                    subjectMethods = subjectMethodIds
                });
                File.WriteAllText(subjectMethodsPath, smJson);
                args.Add("--subject-methods");
                args.Add(subjectMethodsPath);
            }

            // Run ConvertToCppHandler directly — output goes to Console
            var exitCode = ConvertToCppHandler.Run([.. args]);

            // Clean up temp subject methods file
            if (subjectMethodsPath != null)
            {
                try { File.Delete(subjectMethodsPath); } catch { }
            }

            if (exitCode != 0)
            {
                return new CodegenResult
                {
                    Success = false,
                    OutputDir = outputDir,
                    Error = $"convert-to-cpp failed with exit code {exitCode}",
                };
            }

            // Collect generated directories
            // Driver writes to sdkRoot/<assembly-name>/generated/ when --sdk-out is used
            var generatedDirs = new List<string>();
            if (Directory.Exists(outputDir))
            {
                // Per-assembly layout: <sdk-out>/<assembly>/generated/
                foreach (var subDir in Directory.GetDirectories(outputDir))
                {
                    var genDir = Path.Combine(subDir, "generated");
                    if (Directory.Exists(genDir) && Directory.GetFiles(genDir, "*.cpp").Length > 0)
                        generatedDirs.Add(genDir);
                }
                // Flat layout fallback: <sdk-out>/generated/
                var flatGen = Path.Combine(outputDir, "generated");
                if (generatedDirs.Count == 0 && Directory.Exists(flatGen))
                    generatedDirs.Add(flatGen);
            }

            return new CodegenResult
            {
                Success = true,
                OutputDir = outputDir,
                GeneratedDirs = generatedDirs,
            };
        }
        catch (Exception ex)
        {
            return new CodegenResult
            {
                Success = false,
                OutputDir = outputDir,
                Error = ex.Message,
            };
        }
    }
}
