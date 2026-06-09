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
            File.WriteAllText(targetPath, generatedSource.Contents);
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
            File.WriteAllText(targetPath, generatedSource.ContentsBuilder?.ToString() ?? generatedSource.Contents);
        }

        WriteJson(Path.Combine(result.OutputRootPath, NativeAotArtifactNames.LoweringPlan), result.LoweringPlan);
        WriteJson(Path.Combine(result.OutputRootPath, NativeAotArtifactNames.Manifest), result.Manifest);
        WriteJson(Path.Combine(result.OutputRootPath, NativeAotArtifactNames.CodegenMetrics), result.CodegenMetrics);
        return 0;
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
