using System.Security.Cryptography;
using System.Text.Json;

namespace Chaos.IL2CPP.HotUpdate;

public static class PackageReader
{
    public const string ManifestFileName = "package.manifest.json";

    public static LoadedHotUpdatePackage ReadFromDirectory(string rootPath)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(rootPath);

        var packageRoot = Path.GetFullPath(rootPath);
        if (!Directory.Exists(packageRoot))
        {
            throw new DirectoryNotFoundException($"hot update package root missing: {packageRoot}");
        }

        var manifestPath = Path.Combine(packageRoot, ManifestFileName);
        if (!File.Exists(manifestPath))
        {
            throw new FileNotFoundException($"hot update package manifest missing: {manifestPath}");
        }

        var manifest = JsonSerializer.Deserialize<HotUpdatePackage>(
            File.ReadAllText(manifestPath),
            new JsonSerializerOptions
            {
                PropertyNameCaseInsensitive = true,
            });
        if (manifest is null)
        {
            throw new InvalidDataException($"failed to deserialize hot update package manifest: {manifestPath}");
        }

        if (!string.Equals(manifest.FormatVersion, "v0", StringComparison.Ordinal))
        {
            throw new InvalidDataException($"unsupported hot update package format version: {manifest.FormatVersion}");
        }

        var loadedAssemblies = new Dictionary<string, LoadedAssemblyImage>(StringComparer.Ordinal);
        foreach (var assembly in manifest.Assemblies)
        {
            var assemblyPath = Path.Combine(packageRoot, assembly.Name);
            if (!File.Exists(assemblyPath))
            {
                throw new FileNotFoundException($"hot update assembly missing: {assemblyPath}");
            }

            var bytes = File.ReadAllBytes(assemblyPath);
            if (bytes.Length != assembly.Size)
            {
                throw new InvalidDataException(
                    $"hot update assembly size mismatch for '{assembly.Name}': expected {assembly.Size}, got {bytes.Length}");
            }

            var hash = ComputeFileHash(bytes);
            if (!string.Equals(hash, assembly.Hash, StringComparison.Ordinal))
            {
                throw new InvalidDataException(
                    $"hot update assembly hash mismatch for '{assembly.Name}': expected {assembly.Hash}, got {hash}");
            }

            loadedAssemblies[assembly.Name] = new LoadedAssemblyImage
            {
                Name = assembly.Name,
                Bytes = bytes,
                Hash = hash,
            };
        }

        var supplementalMetadataPath = Path.Combine(packageRoot, manifest.SupplementalMetadata);
        if (!File.Exists(supplementalMetadataPath))
        {
            throw new FileNotFoundException($"hot update supplemental metadata missing: {supplementalMetadataPath}");
        }

        return new LoadedHotUpdatePackage
        {
            RootPath = packageRoot,
            Manifest = manifest,
            LoadedAssemblies = loadedAssemblies,
        };
    }

    public static string ComputeFileHash(byte[] bytes)
    {
        ArgumentNullException.ThrowIfNull(bytes);
        return $"sha256:{Convert.ToHexString(SHA256.HashData(bytes)).ToLowerInvariant()}";
    }
}
