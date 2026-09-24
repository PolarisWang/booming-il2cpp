using System.Text.Json;
using System.Text.Json.Serialization;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Generator;
using Chaos.IL2CPP.Pipeline;

namespace Chaos.IL2CPP.Driver;

/// <summary>
/// Orchestrates IL2CPP pipeline execution and artifact persistence.
/// Separates pipeline logic from CLI routing (DriverEntry).
/// </summary>
public sealed class PipelineOrchestrator
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
        NumberHandling = JsonNumberHandling.AllowNamedFloatingPointLiterals,
    };

    public int RunPipeline(ManagedClosureRequest request)
    {
        var result = RunPipelineCore(request);
        return result is not null ? 0 : 1;
    }

    /// <summary>
    /// Run the pipeline and return the result object for callers that need
    /// access to individual artifacts (e.g., RunEmitPatchDataFull).
    /// Returns null on failure.
    /// </summary>
    public ManagedClosureResult? RunPipelineAndGetResult(ManagedClosureRequest request)
    {
        return RunPipelineCore(request);
    }

    private ManagedClosureResult? RunPipelineCore(ManagedClosureRequest request)
    {
        var pipeline = new PipelinePlan();
        var pipelineResult = pipeline.Execute(request);
        if (pipelineResult.IsFailure)
        {
            Console.Error.WriteLine($"Pipeline failed: [{pipelineResult.Error!.Code}] {pipelineResult.Error.Message}");
            return null;
        }
        var result = pipelineResult.Value!;

        Directory.CreateDirectory(result.OutputRootPath);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.TypedIlIr), result.TypedIlIr);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.AotCoreIr), result.AotCoreIr);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.AotManifest), result.AotManifest);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.MetadataRegistration), result.MetadataRegistration);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.SupplementalMetadataTemplate), result.SupplementalMetadataTemplate);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.CodeRegistration), result.CodeRegistration);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.GenericInstantiationDemandGraph), result.GenericInstantiationDemandGraph);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.GenericCapabilityMatrix), result.GenericCapabilityMatrix);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.OptimizationFacts), result.OptimizationFacts);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.PreserveDescriptor), result.PreserveDescriptor);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.NativeReferenceLoweringPlan), result.NativeReferenceLoweringPlan);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.NativeAotLoweringPlan), result.NativeAotLoweringPlan);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.ClosureManifest), result.ClosureManifest);

        return result;
    }

    public int RunNativeReference(NativeReferenceProofRequest request)
    {
        var emitter = new NativeReferenceProofEmitter();
        var result = emitter.Generate(request);

        Directory.CreateDirectory(result.OutputRootPath);
        foreach (var generatedSource in result.GeneratedSources)
        {
            var targetPath = Path.Combine(result.OutputRootPath, generatedSource.RelativePath.Replace('/', Path.DirectorySeparatorChar));
            Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
            File.WriteAllText(targetPath, NormalizeGeneratedLineEndings(generatedSource.Contents));
        }

        WriteJson(Path.Combine(result.OutputRootPath, NativeReferenceArtifactNames.LoweringPlan), result.LoweringPlan);
        WriteJson(Path.Combine(result.OutputRootPath, NativeReferenceArtifactNames.Manifest), result.Manifest);
        WriteJson(Path.Combine(result.OutputRootPath, NativeReferenceArtifactNames.CodegenMetrics), result.CodegenMetrics);
        return 0;
    }

    public int RunNativeAot(NativeAotRequest request)
    {
        var emitter = new NativeAotEmitter();
        var result = emitter.Generate(request);

        Directory.CreateDirectory(result.OutputRootPath);
        foreach (var generatedSource in result.GeneratedSources)
        {
            var targetPath = Path.Combine(result.OutputRootPath, generatedSource.RelativePath.Replace('/', Path.DirectorySeparatorChar));
            Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
            File.WriteAllText(targetPath, NormalizeGeneratedLineEndings(generatedSource.ContentsBuilder?.ToString() ?? generatedSource.Contents));
        }

        WriteJson(Path.Combine(result.OutputRootPath, NativeAotArtifactNames.LoweringPlan), result.LoweringPlan);
        WriteJson(Path.Combine(result.OutputRootPath, NativeAotArtifactNames.Manifest), result.Manifest);
        WriteJson(Path.Combine(result.OutputRootPath, NativeAotArtifactNames.CodegenMetrics), result.CodegenMetrics);
        return 0;
    }

    /// <summary>
    /// Normalize generated C++ to LF line endings before it lands on disk.
    ///
    /// The emitters build source with <see cref="StringBuilder.AppendLine"/>-style
    /// calls, whose separator is <see cref="Environment.NewLine"/> — i.e. CRLF on
    /// Windows, LF on Linux.  Layers compound: template-rendered fragments
    /// already carry the renderer's newlines, the builder appends its own, and a
    /// second pass over content that already contains CR produces CR CR LF, then
    /// CR CR CR LF.  Measured on the Windows agent: 45,780 CRs across 14,636
    /// lines (~3.1 CR per line) while the identical generator run on Linux
    /// emitted pure LF.
    ///
    /// MSVC treats a stray CR as a token break, so the CRLF file does not
    /// compile at all:
    ///   native-aot.generated.cpp(2074): error C2182 / C2365 / C2065
    ///   native-aot.generated.cpp(2078): error C2143
    ///
    /// Emitting LF unconditionally removes the platform dependence entirely:
    /// MSVC compiles LF sources fine, the Linux output is unchanged, and the
    /// generated bytes become cross-platform identical — which the nightly's
    /// cross-platform diff implicitly assumes.
    /// </summary>
    private static string NormalizeGeneratedLineEndings(string contents)
    {
        // Every CR in the generated source is line-ending residue — C++ source
        // never contains a bare CR otherwise — so removing them all is exact:
        // \r\r\r\n collapses to \n, a clean \r\n collapses to \n, and pure-LF
        // content is returned unchanged (fast path above).
        return contents.Replace("\r", "");
    }

    private static void WriteJson<T>(string path, T value)
    {
        var directoryPath = Path.GetDirectoryName(path);
        if (!string.IsNullOrEmpty(directoryPath))
        {
            Directory.CreateDirectory(directoryPath);
        }

        var json = JsonSerializer.Serialize(value, JsonOptions);
        File.WriteAllText(path, json + Environment.NewLine);
    }
}
