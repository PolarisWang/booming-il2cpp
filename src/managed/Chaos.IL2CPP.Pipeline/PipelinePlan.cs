using Chaos.IL2CPP.Generator;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Linker;
using Chaos.IL2CPP.Loader;
using Chaos.IL2CPP.MetadataWriter;
using Chaos.IL2CPP.SemanticWorld;

namespace Chaos.IL2CPP.Pipeline;

public sealed class PipelinePlan
{
    private readonly LoaderStage _loader = new();
    private readonly SemanticWorldStage _semanticWorld = new();
    private readonly LinkerStage _linker = new();
    private readonly MetadataWriterStage _metadataWriter = new();
    private readonly CodeGenStage _codeGen = new();

    public IReadOnlyList<string> Stages { get; } =
        [
            "Loader",
            "SemanticWorld",
            "Linker",
            "MetadataWriter",
            "CodeGen",
        ];

    public PipelineResult<ManagedClosureResult> Execute(ManagedClosureRequest request)
    {
        var loadedWorldResult = _loader.LoadMultiple(request);
        if (loadedWorldResult.IsFailure)
            return PipelineResult<ManagedClosureResult>.Fail(
                loadedWorldResult.Error!);

        var semanticWorldResult = _semanticWorld.Build(loadedWorldResult.Value!);
        if (semanticWorldResult.IsFailure)
            return PipelineResult<ManagedClosureResult>.Fail(
                semanticWorldResult.Error!);

        var linkedWorldResult = _linker.Link(semanticWorldResult.Value!);
        if (linkedWorldResult.IsFailure)
            return PipelineResult<ManagedClosureResult>.Fail(
                linkedWorldResult.Error!);

        var metadataWriterResult = _metadataWriter.Write(linkedWorldResult.Value!);
        if (metadataWriterResult.IsFailure)
            return PipelineResult<ManagedClosureResult>.Fail(
                metadataWriterResult.Error!);

        return _codeGen.Generate(request, linkedWorldResult.Value!, metadataWriterResult.Value!);
    }

    /// <summary>
    /// Execute the pipeline for multiple input assemblies.
    /// All assemblies are loaded into a unified world, linked together,
    /// and produce per-assembly codegen results.
    /// </summary>
    public PipelineResult<IReadOnlyList<ManagedClosureResult>> ExecuteMulti(MultiAssemblyClosureRequest request)
    {
        var loadRequest = new ManagedClosureRequest(
            request.InputAssemblyPaths[0],
            request.OutputRootPath,
            EntryPointSubjectIdOverride: request.EntryPointSubjectIdOverride,
            AdditionalAssemblyPaths: request.InputAssemblyPaths.Skip(1)
                .Concat(request.AdditionalAssemblyPaths ?? [])
                .ToList(),
            FullAssemblyClosure: string.IsNullOrWhiteSpace(request.EntryPointSubjectIdOverride));

        var loadedWorldResult = _loader.LoadMultiple(loadRequest);
        if (loadedWorldResult.IsFailure)
            return PipelineResult<IReadOnlyList<ManagedClosureResult>>.Fail(
                loadedWorldResult.Error!);

        var semanticWorldResult = _semanticWorld.Build(loadedWorldResult.Value!);
        if (semanticWorldResult.IsFailure)
            return PipelineResult<IReadOnlyList<ManagedClosureResult>>.Fail(
                semanticWorldResult.Error!);

        var linkedWorldResult = _linker.Link(semanticWorldResult.Value!);
        if (linkedWorldResult.IsFailure)
            return PipelineResult<IReadOnlyList<ManagedClosureResult>>.Fail(
                linkedWorldResult.Error!);

        var metadataWriterResult = _metadataWriter.Write(linkedWorldResult.Value!);
        if (metadataWriterResult.IsFailure)
            return PipelineResult<IReadOnlyList<ManagedClosureResult>>.Fail(
                metadataWriterResult.Error!);

        var generateRequest = new ManagedClosureRequest(
            request.InputAssemblyPaths[0],
            request.OutputRootPath,
            EntryPointSubjectIdOverride: request.EntryPointSubjectIdOverride,
            AdditionalAssemblyPaths: request.InputAssemblyPaths.Skip(1)
                .Concat(request.AdditionalAssemblyPaths ?? [])
                .ToList(),
            FullAssemblyClosure: true);

        var fullResult = _codeGen.Generate(generateRequest, linkedWorldResult.Value!, metadataWriterResult.Value!);
        if (fullResult.IsFailure)
            return PipelineResult<IReadOnlyList<ManagedClosureResult>>.Fail(
                fullResult.Error!);

        return PipelineResult<IReadOnlyList<ManagedClosureResult>>.Ok(
            _codeGen.FilterResultPerAssembly(fullResult.Value!, request.InputAssemblyPaths));
    }
}
