namespace Chaos.IL2CPP.ProjectGraph;

public sealed record ProjectGraphModel
{
    public required ProjectGraphProjectModel EntryProject { get; init; }

    public required IReadOnlyList<ProjectGraphProjectModel> Projects { get; init; }

    public required IReadOnlyList<ProjectGraphDependencyModel> Dependencies { get; init; }
}

public sealed record ProjectGraphProjectModel
{
    public required string AssemblyName { get; init; }

    public required string ProjectPath { get; init; }

    public required string OutputAssemblyPath { get; init; }
}

public sealed record ProjectGraphDependencyModel
{
    public required string DependencyKind { get; init; }

    public required string FromAssemblyName { get; init; }

    public string? ToAssemblyName { get; init; }

    public string? PackageId { get; init; }

    public string? PackageVersion { get; init; }

    public string? AssemblyPath { get; init; }
}
