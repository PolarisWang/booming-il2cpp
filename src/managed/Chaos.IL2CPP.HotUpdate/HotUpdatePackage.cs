namespace Chaos.IL2CPP.HotUpdate;

public sealed record HotUpdateAssemblyEntry
{
    public required string Name { get; init; }

    public required string Hash { get; init; }

    public required int Size { get; init; }

    public required string EntryPoint { get; init; }
}

public sealed record HotUpdatePackage
{
    public string FormatVersion { get; init; } = "v0";

    public required string PackageId { get; init; }

    public required string TargetAotVersion { get; init; }

    public required IReadOnlyList<HotUpdateAssemblyEntry> Assemblies { get; init; }

    public required string SupplementalMetadata { get; init; }

    public required string Signature { get; init; }
}

public sealed record LoadedAssemblyImage
{
    public required string Name { get; init; }

    public required byte[] Bytes { get; init; }

    public required string Hash { get; init; }
}

public sealed record LoadedHotUpdatePackage
{
    public required string RootPath { get; init; }

    public required HotUpdatePackage Manifest { get; init; }

    public required IReadOnlyDictionary<string, LoadedAssemblyImage> LoadedAssemblies { get; init; }

    public required LoadedSupplementalMetadata SupplementalMetadata { get; init; }
}
