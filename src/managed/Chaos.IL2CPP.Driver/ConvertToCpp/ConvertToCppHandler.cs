using System.Reflection.PortableExecutable;
using System.Text.Json;
using Chaos.IL2CPP.CodeGen;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Diagnostics;
using Chaos.IL2CPP.Pipeline;

namespace Chaos.IL2CPP.Driver;

/// <summary>
/// Handles the convert-to-cpp command. Uses the existing PipelinePlan
/// for IL→C++ lowering and NativeAotEmitter for C++ generation.
///
/// Usage: chaos-il2cpp convert-to-cpp --assembly <path> --output <dir>
/// </summary>
internal static class ConvertToCppHandler
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = System.Text.Json.Serialization.JsonIgnoreCondition.WhenWritingNull,
    };

    public static int Run(string[] args)
    {
        var config = ConvertToCppConfig.Parse(args);
        if (string.IsNullOrWhiteSpace(config.AssemblyPath))
            return 1;

        var outputRoot = Path.GetFullPath(config.OutputDir);
        Directory.CreateDirectory(outputRoot);

        Console.WriteLine($"chaos-il2cpp convert-to-cpp");
        Console.WriteLine($"  Assembly: {config.AssemblyPath}");
        Console.WriteLine($"  Output:   {outputRoot}");

        // ── Step 1: Resolve assembly paths ──────────────────────────────
        // Collect all managed DLLs from search directories (skip native DLLs).
        // A managed DLL has "MZ" header AND CLI metadata header (DOS → PE → CLI).
        var assemblyDirs = config.AssemblyDirs;
        var additionalPaths = new List<string>();
        foreach (var dir in assemblyDirs)
        {
            if (Directory.Exists(dir))
            {
                foreach (var dll in Directory.GetFiles(dir, "*.dll"))
                {
                    try
                    {
                        using var peReader = new System.Reflection.PortableExecutable.PEReader(File.OpenRead(dll));
                        if (peReader.HasMetadata)
                        {
                            additionalPaths.Add(dll);
                        }
                    }
                    catch
                    {
                        // Skip non-managed DLLs
                    }
                }
            }
            else if (File.Exists(dir))
            {
                additionalPaths.Add(dir);
            }
        }

        // ── Step 2: Run full pipeline ────────────────────────────────────
        Console.Write("  [1/3] Running IL2CPP pipeline...");
        ChaosTrace.Point("convert-to-cpp.pipeline", "codegen");

        var request = new ManagedClosureRequest(
            config.AssemblyPath,
            outputRoot,
            EntryPointSubjectIdOverride: null,
            AdditionalAssemblyPaths: additionalPaths,
            FullAssemblyClosure: true);

        var pipeline = new PipelinePlan();
        var closureResult = pipeline.Execute(request);
        Console.WriteLine($" {closureResult.AotCoreIr.Methods.Count} methods lowered");

        // ── Step 2: Write closure artifacts ──────────────────────────────
        Console.Write("  [2/3] Writing closure artifacts...");
        ChaosTrace.Point("convert-to-cpp.write", "codegen");

        WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.AotCoreIr), closureResult.AotCoreIr);
        WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.TypedIlIr), closureResult.TypedIlIr);
        WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.AotManifest), closureResult.AotManifest);
        WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.MetadataRegistration), closureResult.MetadataRegistration);
        WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.SupplementalMetadataTemplate), closureResult.SupplementalMetadataTemplate);
        WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.CodeRegistration), closureResult.CodeRegistration);
        WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.GenericInstantiationDemandGraph), closureResult.GenericInstantiationDemandGraph);
        WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.GenericCapabilityMatrix), closureResult.GenericCapabilityMatrix);
        WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.OptimizationFacts), closureResult.OptimizationFacts);
        WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.PreserveDescriptor), closureResult.PreserveDescriptor);
        WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.NativeReferenceLoweringPlan), closureResult.NativeReferenceLoweringPlan);
        Console.WriteLine(" done");

        // ── Step 3: Patch artifacts for real C++ emission ──────────────
        // The pipeline produces audit-only artifacts when FullAssemblyClosure=true
        // with no entry point.  Patch them into "generic-managed-entry" mode for
        // NativeAotEmitter to generate real lowering (using first method as
        // synthetic entry point).
        var aotCoreIr = closureResult.AotCoreIr;
        var firstMethod = aotCoreIr.Methods.FirstOrDefault();
        if (firstMethod is not null)
        {
            var syntheticEntry = firstMethod.SubjectId;
            var original = closureResult.NativeAotLoweringPlan;

            // Patch closure manifest with the synthetic entry
            var patchedManifest = closureResult.ClosureManifest with
            {
                EntrySubjectId = syntheticEntry,
            };
            WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.ClosureManifest), patchedManifest);

            // Patch lowering plan with proper entry-based configuration
            var patchedPlan = new NativeAotLoweringPlanArtifact
            {
                PlanKind = "generic-managed-entry",
                AssemblyName = original.AssemblyName,
                EntrySubjectId = syntheticEntry,
                EntrySymbol = firstMethod.NativeSymbol,
                EntryMethodToken = firstMethod.SubjectId,
                NativeEntryFunctionName = "RunNativeAot",
                WorkloadAbi = "int(int32)",
                TranslationUnitPageSize = original.TranslationUnitPageSize,
                TranslationUnitPageCount = original.TranslationUnitPageCount,
                TranslationUnitPages = original.TranslationUnitPages,
                TranslationUnitMode = original.TranslationUnitMode,
                TranslationUnitMethodSubjectIds = original.TranslationUnitMethodSubjectIds,
            };
            WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.NativeAotLoweringPlan), patchedPlan);
        }
        else
        {
            WriteJson(Path.Combine(outputRoot, ManagedClosureArtifactNames.NativeAotLoweringPlan), closureResult.NativeAotLoweringPlan);
        }

        // ── Step 4: Emit C++ ─────────────────────────────────────────────
        Console.Write("  [3/3] Emitting C++ (NativeAot)...");
        ChaosTrace.Point("convert-to-cpp.emit", "codegen");

        var emitRequest = new NativeAotRequest(outputRoot, outputRoot);
        var emitter = new NativeAotEmitter();
        var emitResult = emitter.Generate(emitRequest);

        // Write generated sources
        foreach (var source in emitResult.GeneratedSources)
        {
            var targetPath = Path.Combine(outputRoot, source.RelativePath.Replace('/', Path.DirectorySeparatorChar));
            Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
            File.WriteAllText(targetPath, source.Contents);
        }

        WriteJson(Path.Combine(outputRoot, NativeAotArtifactNames.LoweringPlan), emitResult.LoweringPlan);
        WriteJson(Path.Combine(outputRoot, NativeAotArtifactNames.Manifest), emitResult.Manifest);
        WriteJson(Path.Combine(outputRoot, NativeAotArtifactNames.CodegenMetrics), emitResult.CodegenMetrics);

        Console.WriteLine($" {emitResult.GeneratedSources.Count} files");
        ChaosTrace.Point("convert-to-cpp.complete", "codegen",
            new Dictionary<string, object?>
            {
                ["methods"] = closureResult.AotCoreIr.Methods.Count,
                ["files"] = emitResult.GeneratedSources.Count,
            });

        Console.WriteLine($"Convert completed: {outputRoot}");
        return 0;
    }

    private static void WriteJson<T>(string path, T value)
    {
        var dir = Path.GetDirectoryName(path);
        if (!string.IsNullOrEmpty(dir))
            Directory.CreateDirectory(dir);
        File.WriteAllText(path, JsonSerializer.Serialize(value, JsonOptions) + Environment.NewLine);
    }
}
