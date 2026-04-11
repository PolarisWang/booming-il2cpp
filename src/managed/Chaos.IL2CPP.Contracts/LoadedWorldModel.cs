namespace Chaos.IL2CPP.Contracts;

public sealed record LoadedWorldModel
{
    public required string InputAssemblyPath { get; init; }

    public required AssemblyIdentityModel Assembly { get; init; }

    public required string EntryPointSubjectId { get; init; }

    public required IReadOnlyList<LoadedAssemblyModel> Assemblies { get; init; }

    public required IReadOnlyList<ManagedTypeModel> Types { get; init; }

    public required IReadOnlyList<ManagedFieldModel> Fields { get; init; }

    public required IReadOnlyList<ManagedPropertyModel> Properties { get; init; }

    public required IReadOnlyList<ManagedMethodModel> Methods { get; init; }
}
