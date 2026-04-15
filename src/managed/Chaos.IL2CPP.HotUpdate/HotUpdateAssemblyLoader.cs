namespace Chaos.IL2CPP.HotUpdate;

public sealed class HotUpdateAssemblyLoader
{
    private readonly SupplementalMetadataLoader _supplementalMetadataLoader = new();

    public LoadedHotUpdatePackage LoadFromDirectory(string rootPath)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(rootPath);

        var packageRoot = Path.GetFullPath(rootPath);
        var manifestPath = Path.Combine(packageRoot, PackageReader.ManifestFileName);
        if (!File.Exists(manifestPath))
        {
            throw new FileNotFoundException($"hot update package manifest missing: {manifestPath}");
        }

        var package = PackageReader.ReadFromDirectory(packageRoot);
        var supplementalMetadataPath = Path.Combine(package.RootPath, package.Manifest.SupplementalMetadata);
        if (!File.Exists(supplementalMetadataPath))
        {
            throw new FileNotFoundException($"hot update supplemental metadata missing: {supplementalMetadataPath}");
        }

        var supplementalMetadata = _supplementalMetadataLoader.LoadFromBytes(
            File.ReadAllBytes(supplementalMetadataPath));

        return package with
        {
            SupplementalMetadata = supplementalMetadata,
        };
    }
}
