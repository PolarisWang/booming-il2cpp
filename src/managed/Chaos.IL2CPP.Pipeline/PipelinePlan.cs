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
        var loadedAssembly = _loader.Load(request);
        var semanticWorld = _semanticWorld.Build(loadedAssembly);
        var linkedWorld = _linker.Link(semanticWorld);
        var metadataWriterOutput = _metadataWriter.Write(linkedWorld);

        return _codeGen.Generate(request, linkedWorld, metadataWriterOutput);
    }
}
