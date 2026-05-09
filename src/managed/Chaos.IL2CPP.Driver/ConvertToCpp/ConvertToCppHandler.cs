using System.Reflection.PortableExecutable;
using System.Text.Json;
using Chaos.IL2CPP.CodeGen;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Diagnostics;
using Chaos.IL2CPP.Pipeline;

namespace Chaos.IL2CPP.Driver;

/// <summary>
/// Handles the convert-to-cpp command. Supports multi-assembly translation
/// in Unity IL2CPP style: --assembly A.dll --assembly B.dll --output dir/
///
/// Each assembly gets its own per-assembly generated C++ files plus a
/// combined CMakeLists.txt for full program compilation.
/// </summary>
internal static class ConvertToCppHandler
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = System.Text.Json.Serialization.JsonIgnoreCondition.WhenWritingNull,
        NumberHandling = System.Text.Json.Serialization.JsonNumberHandling.AllowNamedFloatingPointLiterals,
    };

    public static int Run(string[] args)
    {
        var config = ConvertToCppConfig.Parse(args);
        if (config.AssemblyPaths.Count == 0)
            return 1;

        var outputRoot = Path.GetFullPath(config.OutputDir);
        Directory.CreateDirectory(outputRoot);

        Console.WriteLine($"chaos-il2cpp convert-to-cpp");
        Console.WriteLine($"  Assemblies: {config.AssemblyPaths.Count}");
        foreach (var asm in config.AssemblyPaths)
            Console.WriteLine($"    {asm}");
        Console.WriteLine($"  Output:   {outputRoot}");

        // ── Step 1: Collect managed dependency DLLs ────────────────────────
        var additionalPaths = new List<string>();
        var entryAssemblyNames = new HashSet<string>(config.AssemblyPaths.Select(Path.GetFileNameWithoutExtension), StringComparer.OrdinalIgnoreCase);
        foreach (var dir in config.AssemblyDirs)
        {
            if (Directory.Exists(dir))
            {
                foreach (var dll in Directory.GetFiles(dir, "*.dll"))
                {
                    try
                    {
                        using var peReader = new PEReader(File.OpenRead(dll));
                        if (peReader.HasMetadata)
                        {
                            // Skip assemblies already specified via --assembly
                            var name = Path.GetFileNameWithoutExtension(dll);
                            if (!entryAssemblyNames.Contains(name))
                                additionalPaths.Add(dll);
                        }
                    }
                    catch { }
                }
            }
            else if (File.Exists(dir))
            {
                additionalPaths.Add(dir);
            }
        }

        // ── Step 2: Run pipeline ───────────────────────────────────────────
        Console.Write($"  [1/3] Running IL2CPP pipeline...");
        ChaosTrace.Point("convert-to-cpp.pipeline", "codegen");

        var pipeline = new PipelinePlan();

        if (config.AssemblyPaths.Count == 1)
        {
            // Single assembly full-closure — entry is specified or auto-detected
            var request = new ManagedClosureRequest(
                config.AssemblyPaths[0],
                outputRoot,
                EntryPointSubjectIdOverride: config.EntryPoint,
                AdditionalAssemblyPaths: additionalPaths,
                FullAssemblyClosure: true);

            var closureResult = pipeline.Execute(request);
            Console.WriteLine($" {closureResult.AotCoreIr.Methods.Count} methods lowered");

            Console.Write("  [2/3] Writing closure artifacts...");
            WriteArtifacts(outputRoot, closureResult);
            Console.WriteLine(" done");

            Console.Write("  [3/3] Emitting C++ (NativeAot)...");
            var emitResult = EmitNativeAot(outputRoot);
            // EmitNativeAot writes .cpp to outputRoot/generated/ but CmakeGenerator
            // expects outputRoot/{AssemblyName}/generated/. Move files to match.
            var asmName = emitResult.Manifest.AssemblyName;
            if (!string.IsNullOrEmpty(asmName))
            {
                var asmOutputDir = Path.Combine(outputRoot, asmName);
                foreach (var source in emitResult.GeneratedSources)
                {
                    var relativePath = source.RelativePath.Replace('/', Path.DirectorySeparatorChar);
                    var srcPath = Path.Combine(outputRoot, relativePath);
                    var dstPath = Path.Combine(asmOutputDir, relativePath);
                    Directory.CreateDirectory(Path.GetDirectoryName(dstPath)!);
                    if (File.Exists(srcPath))
                        File.Move(srcPath, dstPath, overwrite: true);
                }
            }
            Console.WriteLine($" {emitResult.GeneratedSources.Count} files");

            // Generate CMakeLists.txt
            var repoRoot = ResolveRepoRoot();
            var nativeLibDir = Path.Combine(repoRoot, "build", "native");
            var cmakeGen = new Chaos.IL2CPP.CodeGen.BuildSystem.CmakeGenerator(repoRoot);
            var singleCmakeContent = cmakeGen.Generate(
                new[] { emitResult }.ToList(),
                nativeLibDir: nativeLibDir,
                extraSources: new List<string> { "runtime-entry.cpp" },
                targetName: "entry");
            File.WriteAllText(Path.Combine(outputRoot, "CMakeLists.txt"), singleCmakeContent);

            var runtimeEntryCpp = GenerateRuntimeEntryCpp(config.EntryPoint is not null ? "RunNativeAot" : null);
            File.WriteAllText(Path.Combine(outputRoot, "runtime-entry.cpp"), runtimeEntryCpp);

            Console.WriteLine($"Convert completed: {outputRoot}");
        }
        else
        {
            // Multi-assembly or explicit entry point — use unified pipeline
            var multiRequest = new MultiAssemblyClosureRequest(
                config.AssemblyPaths.ToList(),
                outputRoot,
                EntryPointSubjectIdOverride: config.EntryPoint,
                AdditionalAssemblyPaths: additionalPaths);

            var results = pipeline.ExecuteMulti(multiRequest);
            Console.WriteLine($" {results.Sum(r => r.AotCoreIr?.Methods.Count ?? 0)} methods across {results.Count} assemblies");

            // Write per-assembly artifacts and emit C++
            Console.Write("  [2/3] Writing closure artifacts...");
            foreach (var result in results)
            {
                var assemblyOutput = result.OutputRootPath;
                Directory.CreateDirectory(assemblyOutput);
                WriteArtifacts(assemblyOutput, result);
            }
            Console.WriteLine(" done");

            // Emit C++ per assembly
            Console.Write("  [3/3] Emitting C++ (NativeAot)...");
            int totalFiles = 0;
            foreach (var result in results)
            {
                var emitResult = EmitNativeAot(result.OutputRootPath);
                totalFiles += emitResult.GeneratedSources.Count;
            }

            // Write combined report
            WriteCombinedReport(outputRoot, config, results);

            // Generate CMakeLists.txt
            var repoRoot = ResolveRepoRoot();
            var nativeLibDir = Path.Combine(repoRoot, "build", "native");
            var cmakeGen = new Chaos.IL2CPP.CodeGen.BuildSystem.CmakeGenerator(repoRoot);
            var assemblyNames = results.Select(r => r.ClosureManifest?.AssemblyName ?? "unknown").ToList();
            var assemblyInfo = assemblyNames.Select(name => new
            {
                Name = name,
                Sources = new[] { $"{name}.cpp" },
            }).ToList();
            var cmakeContent = cmakeGen.Generate(
                results.Select(r => new NativeAotResult
                {
                    OutputRootPath = r.OutputRootPath,
                    LoweringPlan = new NativeAotLoweringPlanArtifact
                    {
                        PlanKind = "generic-managed-entry",
                        AssemblyName = r.ClosureManifest?.AssemblyName ?? "unknown",
                        EntrySubjectId = r.ClosureManifest?.EntrySubjectId ?? "",
                        EntrySymbol = "",
                        EntryMethodToken = "0u",
                        NativeEntryFunctionName = "RunNativeAot",
                        WorkloadAbi = "int(int32)",
                    },
                    Manifest = new NativeAotManifestArtifact
                    {
                        AssemblyName = r.ClosureManifest?.AssemblyName ?? "unknown",
                        EntrySubjectId = r.ClosureManifest?.EntrySubjectId ?? "",
                        ManagedClosureRootPath = outputRoot,
                        PlanArtifactPath = NativeAotArtifactNames.LoweringPlan,
                        GeneratedArtifacts = [],
                    },
                    CodegenMetrics = new NativeCodegenMetricsArtifact
                    {
                        CodegenKind = "native-aot",
                        PlanKind = "generic-managed-entry",
                        GeneratedSourcePaths = [],
                    },
                    GeneratedSources = r.AotCoreIr?.Methods.Select(m => new NativeAotGeneratedSource
                    {
                        RelativePath = m.SubjectId,
                        Contents = "",
                    }).ToList() ?? [],
                }).ToList(),
                nativeLibDir: nativeLibDir,
                extraSources: new List<string> { "runtime-entry.cpp" },
                targetName: "entry");
            File.WriteAllText(Path.Combine(outputRoot, "CMakeLists.txt"), cmakeContent);

            // Generate runtime entry point (simple text, no Scriban dependency)
            var runtimeEntryContent = GenerateRuntimeEntryCpp(config.EntryPoint is not null ? "RunNativeAot" : null);
            File.WriteAllText(Path.Combine(outputRoot, "runtime-entry.cpp"), runtimeEntryContent);

            Console.WriteLine($" {totalFiles} files across {results.Count} assemblies");
            Console.WriteLine($"  CMakeLists.txt — build with: cmake -S {outputRoot} -B {outputRoot}/build");
            Console.WriteLine($"Convert completed: {outputRoot}");
        }

        return 0;
    }

    private static void WriteArtifacts(string root, ManagedClosureResult result)
    {
        Directory.CreateDirectory(root);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.AotCoreIr), result.AotCoreIr);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.AotManifest), result.AotManifest);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.MetadataRegistration), result.MetadataRegistration);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.SupplementalMetadataTemplate), result.SupplementalMetadataTemplate);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.CodeRegistration), result.CodeRegistration);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.GenericInstantiationDemandGraph), result.GenericInstantiationDemandGraph);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.OptimizationFacts), result.OptimizationFacts);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.PreserveDescriptor), result.PreserveDescriptor);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.NativeReferenceLoweringPlan), result.NativeReferenceLoweringPlan);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.NativeAotLoweringPlan), result.NativeAotLoweringPlan);
        WriteJson(Path.Combine(root, ManagedClosureArtifactNames.ClosureManifest), result.ClosureManifest);
    }

    private static NativeAotResult EmitNativeAot(string outputRoot)
    {
        var request = new NativeAotRequest(outputRoot, outputRoot);
        var emitter = new NativeAotEmitter();
        var emitResult = emitter.Generate(request);

        foreach (var source in emitResult.GeneratedSources)
        {
            var targetPath = Path.Combine(outputRoot, source.RelativePath.Replace('/', Path.DirectorySeparatorChar));
            Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
            File.WriteAllText(targetPath, source.Contents);
        }

        WriteJson(Path.Combine(outputRoot, NativeAotArtifactNames.LoweringPlan), emitResult.LoweringPlan);
        WriteJson(Path.Combine(outputRoot, NativeAotArtifactNames.Manifest), emitResult.Manifest);
        WriteJson(Path.Combine(outputRoot, NativeAotArtifactNames.CodegenMetrics), emitResult.CodegenMetrics);

        return emitResult;
    }

    private static void WriteCombinedReport(string outputRoot, ConvertToCppConfig config, IReadOnlyList<ManagedClosureResult> results)
    {
        var report = new
        {
            assembly = string.Join(", ", config.AssemblyPaths.Select(Path.GetFileNameWithoutExtension)),
            totalAssemblies = config.AssemblyPaths.Count,
            assemblies = config.AssemblyPaths.Select(p => Path.GetFileNameWithoutExtension(p)).ToList(),
            results = results.Select(r => new
            {
                assembly = r.ClosureManifest?.AssemblyName ?? "unknown",
                methods = r.AotCoreIr?.Methods?.Count ?? 0,
                output = r.OutputRootPath,
            }).ToList(),
            timestamp = DateTime.UtcNow.ToString("o"),
        };
        WriteJson(Path.Combine(outputRoot, "convert-to-cpp-report.json"), report);
    }

    private static void WriteJson<T>(string path, T value)
    {
        var dir = Path.GetDirectoryName(path);
        if (!string.IsNullOrEmpty(dir))
            Directory.CreateDirectory(dir);
        File.WriteAllText(path, JsonSerializer.Serialize(value, JsonOptions) + Environment.NewLine);
    }

    private static string ResolveRepoRoot()
    {
        // Starting from the assembly output directory (e.g. Chaos.IL2CPP.Driver/bin/Debug/net8.0),
        // walk up to find the repo root (contains src/, build/, etc.)
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        while (dir != null)
        {
            if (Directory.Exists(Path.Combine(dir.FullName, "src")) &&
                Directory.Exists(Path.Combine(dir.FullName, "build")))
                return dir.FullName;
            dir = dir.Parent;
        }
        // Fallback: use current directory
        return Directory.GetCurrentDirectory();
    }

    private static string GenerateRuntimeEntryCpp(string? entryFunction)
    {
        if (string.IsNullOrEmpty(entryFunction))
        {
            return @"// Auto-generated runtime entry point for chaos-il2cpp full-assembly AOT output.
// No single entry point — all methods are available via dispatch/hotpatch tables.
#include ""codegen_bridge.h""
#include ""runtime_abi.h""

int main(int argc, char** argv) {
    auto* bridge = chaos_codegen_get_bridge_v0();
    if (!bridge) return -1;
    bridge->bootstrap_runtime();
    return 0;
}
";
        }

        return $@"// Auto-generated runtime entry point for chaos-il2cpp AOT output.
#include <cstdint>
#include <cstdlib>
#include ""codegen_bridge.h""
#include ""runtime_abi.h""

extern ""C"" std::int32_t {entryFunction}(std::int32_t);

int main(int argc, char** argv) {{
    auto* bridge = chaos_codegen_get_bridge_v0();
    if (!bridge) return -1;
    bridge->bootstrap_runtime();
    int result = {entryFunction}(argc > 1 ? std::atoi(argv[1]) : 0);
    return result;
}}
";
    }
}
