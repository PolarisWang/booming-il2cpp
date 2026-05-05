using Chaos.IL2CPP.CodeGen;
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

    public ManagedClosureResult Execute(ManagedClosureRequest request)
    {
        var loadedWorld = _loader.LoadMultiple(request);
        var semanticWorld = _semanticWorld.Build(loadedWorld);
        var linkedWorld = _linker.Link(semanticWorld);
        var metadataWriterOutput = _metadataWriter.Write(linkedWorld);

        return _codeGen.Generate(request, linkedWorld, metadataWriterOutput);
    }

    /// <summary>
    /// Execute the pipeline for multiple input assemblies.
    /// All assemblies are loaded into a unified world, linked together,
    /// and produce per-assembly codegen results.
    /// </summary>
    public IReadOnlyList<ManagedClosureResult> ExecuteMulti(MultiAssemblyClosureRequest request)
    {
        var loadedWorld = _loader.LoadMultiple(new ManagedClosureRequest(
            request.InputAssemblyPaths[0],
            request.OutputRootPath,
            EntryPointSubjectIdOverride: request.EntryPointSubjectIdOverride,
            AdditionalAssemblyPaths: request.InputAssemblyPaths.Skip(1)
                .Concat(request.AdditionalAssemblyPaths ?? [])
                .ToList(),
            FullAssemblyClosure: string.IsNullOrWhiteSpace(request.EntryPointSubjectIdOverride)));

        var semanticWorld = _semanticWorld.Build(loadedWorld);
        var linkedWorld = _linker.Link(semanticWorld);
        var metadataWriterOutput = _metadataWriter.Write(linkedWorld);

        var fullResult = _codeGen.Generate(
            new ManagedClosureRequest(
                request.InputAssemblyPaths[0],
                request.OutputRootPath,
                EntryPointSubjectIdOverride: request.EntryPointSubjectIdOverride,
                AdditionalAssemblyPaths: request.InputAssemblyPaths.Skip(1)
                    .Concat(request.AdditionalAssemblyPaths ?? [])
                    .ToList(),
                FullAssemblyClosure: true),
            linkedWorld,
            metadataWriterOutput);

        return _codeGen.FilterResultPerAssembly(fullResult, request.InputAssemblyPaths);
    }
}
