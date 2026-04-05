using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.SemanticWorld;

public sealed class SemanticWorldStage
{
    public string Name => "SemanticWorld";

    public SemanticWorldModel Build(LoadedAssemblyModel loadedAssembly)
    {
        return new SemanticWorldModel
        {
            InputAssemblyPath = loadedAssembly.InputAssemblyPath,
            Assembly = loadedAssembly.Assembly,
            EntryPointSubjectId = loadedAssembly.EntryPointSubjectId,
            Types = loadedAssembly.Types,
            Fields = loadedAssembly.Fields,
            Methods = loadedAssembly.Methods,
        };
    }
}
