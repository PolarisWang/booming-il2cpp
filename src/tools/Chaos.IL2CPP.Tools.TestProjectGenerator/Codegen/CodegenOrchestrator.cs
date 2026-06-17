using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.PortableExecutable;
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

            // ── BCL assembly auto-resolution ──
            // Scan the subjects DLL for references to BCL assemblies and add
            // their specific DLLs as --additional-assembly. This ensures BCL
            // methods (System.Linq, etc.) are AOT-compiled instead of routed
            // through ChaosExternalRuntimeFallback, without adding the entire
            // .NET runtime dir (182 DLLs / ~910K methods).
            if (assemblyPaths.Count > 0)
            {
                try
                {
                    var subjectsPath = Path.GetFullPath(assemblyPaths[0]);
                    var runtimeDir = FindNetCoreRuntimeDirectory();
                    if (runtimeDir != null)
                    {
                        foreach (var bclName in ScanAssemblyReferences(subjectsPath))
                        {
                            var bclPath = Path.Combine(runtimeDir, bclName + ".dll");
                            if (File.Exists(bclPath))
                            {
                                args.Add("--additional-assembly");
                                args.Add(bclPath);
                            }
                        }
                    }
                }
                catch { /* BCL resolution is best-effort */ }
            }

            args.Add("--sdk-out");
            args.Add(Path.GetFullPath(outputDir));
            args.Add("--full-closure");

            // Cross-assembly: add target DLL directory as --assembly-dir.
            // The subjects DLL references the target assembly (e.g. System.Linq.dll).
            // Adding its directory lets the codegen resolve cross-assembly methods
            // directly, rather than routing them through ChaosExternalRuntimeFallback.
            // IMPORTANT: only add the target DLL's own directory, NOT the full runtime
            // dir — that would pull in 174K+ methods from 182 DLLs.
            if (assemblyPaths.Count > 0)
            {
                try
                {
                    var targetDir = Path.GetDirectoryName(Path.GetFullPath(assemblyPaths[0]));
                    if (targetDir != null && Directory.Exists(targetDir))
                    {
                        args.Add("--assembly-dir");
                        args.Add(targetDir);
                    }
                }
                catch { }
            }

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

    /// <summary>
    /// Find the .NET Core runtime directory containing BCL assemblies.
    /// Resolves from the currently running .NET runtime.
    /// </summary>
    private static string? FindNetCoreRuntimeDirectory()
    {
        try
        {
            // Use System.Runtime.InteropServices to find the runtime path
            var runtimeDir = System.Runtime.InteropServices.RuntimeEnvironment.GetRuntimeDirectory();
            if (runtimeDir != null && Directory.Exists(runtimeDir))
                return runtimeDir;
        }
        catch { }

        // Fallback: search common install locations
        var candidates = new[]
        {
            Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles),
                "dotnet", "shared", "Microsoft.NETCore.App"),
            Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ProgramFilesX86),
                "dotnet", "shared", "Microsoft.NETCore.App"),
        };

        foreach (var baseDir in candidates)
        {
            if (!Directory.Exists(baseDir)) continue;
            // Pick the highest version directory
            var version = Directory.GetDirectories(baseDir)
                .Select(Path.GetFileName)
                .Where(v => Version.TryParse(v, out _))
                .OrderByDescending(v => Version.Parse(v))
                .FirstOrDefault();
            if (version != null)
                return Path.Combine(baseDir, version);
        }
        return null;
    }

    /// <summary>
    /// Scan a managed assembly's metadata for references to BCL assemblies.
    /// Returns assembly names (without .dll extension) that are known BCL assemblies.
    /// </summary>
    private static IEnumerable<string> ScanAssemblyReferences(string assemblyPath)
    {
        if (!File.Exists(assemblyPath)) yield break;

        using var stream = File.OpenRead(assemblyPath);
        using var peReader = new PEReader(stream);
        if (!peReader.HasMetadata) yield break;

        var metadataReader = peReader.GetMetadataReader();

        foreach (var assemblyRefHandle in metadataReader.AssemblyReferences)
        {
            var assemblyRef = metadataReader.GetAssemblyReference(assemblyRefHandle);
            var name = metadataReader.GetString(assemblyRef.Name);

            // Filter to known BCL assemblies that should be AOT-compiled.
            // This list covers assemblies commonly referenced by foundation tests.
            if (IsBclAssembly(name))
                yield return name;
        }
    }

    /// <summary>Check if an assembly name is a known BCL assembly.</summary>
    private static bool IsBclAssembly(string name)
    {
        return name switch
        {
            "System.Linq" or "System.Linq.Expressions" or "System.Collections" or
            "System.Collections.Immutable" or "System.Collections.Concurrent" or
            "System.ComponentModel" or "System.ComponentModel.TypeConverter" or
            "System.Data.Common" or "System.Diagnostics.DiagnosticSource" or
            "System.Formats.Asn1" or "System.IO.Compression" or "System.IO.Compression.Brotli" or
            "System.IO.Pipelines" or "System.Net.Http" or "System.Net.Sockets" or
            "System.ObjectModel" or "System.Reflection.Metadata" or
            "System.Runtime.InteropServices" or "System.Runtime.Serialization.Formatters" or
            "System.Security.Claims" or "System.Security.Cryptography" or
            "System.Security.Principal.Windows" or "System.Text.Json" or
            "System.Threading.Tasks.Parallel" or "System.Xml.ReaderWriter" or
            "System.Private.Xml" or "System.Runtime.Intrinsics" or
            "System.ComponentModel.Primitives" or "System.ComponentModel.DataAnnotations"
                => true,
            _ => false,
        };
    }
}
