namespace Chaos.IL2CPP.Pipeline;

public sealed class PipelinePlan
{
    public IReadOnlyList<string> Stages { get; } =
        [
            "Loader",
            "SemanticWorld",
            "Linker",
            "MetadataWriter",
            "CodeGen",
        ];
}
