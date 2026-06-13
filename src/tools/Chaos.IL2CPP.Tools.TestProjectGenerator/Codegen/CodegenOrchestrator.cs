using System.Text.Json;
using Chaos.IL2CPP.Driver;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator.Codegen;

// ── Layer boundary note ──
// This file lives in the TPG project (Layer 3) because TPG references Driver,
// and Driver references Generator. Moving this file into the Generator project
// would create a circular dependency: Driver → Generator → Driver.
//
// TPG is the correct adapter layer: it sits between the Python pipeline (Layer 4)
// and the Codegen (Layer 2), providing a stable orchestration boundary.
// The actual IL→C++ translation logic remains in Generator (Layer 2).
//
// See: wiki/01-项目总览/codegen-tpg-python-module-boundaries.md

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
            args.Add("--full-closure");

            // Cross-assembly: add SPC runtime dir as --assembly-dir (F15)
            try { var d = System.Runtime.InteropServices.RuntimeEnvironment.GetRuntimeDirectory(); if (d != null) { var p = System.IO.Path.Combine(d, "System.Private.CoreLib.dll"); if (System.IO.File.Exists(p)) { args.Add("--assembly-dir"); args.Add(d); } } } catch { }

            if (codegenMode == "jit")
            {
                args.Add("--mode");
                args.Add("jit");
            }

            // ── Subject method IDs (--subject-methods) ──
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

            // Run ConvertToCppHandler directly
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
            var generatedDirs = new List<string>();
            if (Directory.Exists(outputDir))
            {
                foreach (var subDir in Directory.GetDirectories(outputDir))
                {
                    var genDir = Path.Combine(subDir, "generated");
                    if (Directory.Exists(genDir) && Directory.GetFiles(genDir, "*.cpp").Length > 0)
                        generatedDirs.Add(genDir);
                }
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
