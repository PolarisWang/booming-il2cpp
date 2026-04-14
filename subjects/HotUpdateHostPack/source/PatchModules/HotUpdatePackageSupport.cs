using System.Text.Json;
using Chaos.IL2CPP.HotUpdate;

namespace HotUpdateHostPack;

internal static class HotUpdatePackageSupport
{
    public const string CurrentAotVersion = "1.0.0";

    private const string EmptySupplementalMetadataPayload = """
        {
          "Types": [],
          "Methods": [],
          "GenericInstantiations": []
        }
        """;

    public static string CreatePackageRoot(
        string workspaceRoot,
        string packageIdSuffix,
        string targetAotVersion,
        string entryPoint,
        byte[] assemblyBytes,
        string signature,
        bool corruptAssemblyHash = false,
        string? metadataContent = null)
    {
        var packageRoot = Path.Combine(workspaceRoot, packageIdSuffix);
        Directory.CreateDirectory(packageRoot);

        File.WriteAllBytes(Path.Combine(packageRoot, "HotPatch.dll"), assemblyBytes);
        File.WriteAllText(
            Path.Combine(packageRoot, "metadata-supplement.bin"),
            metadataContent ?? EmptySupplementalMetadataPayload);

        var manifest = new HotUpdatePackage
        {
            PackageId = $"com.example.hotupdate.{packageIdSuffix}",
            TargetAotVersion = targetAotVersion,
            Assemblies =
            [
                new HotUpdateAssemblyEntry
                {
                    Name = "HotPatch.dll",
                    Hash = corruptAssemblyHash
                        ? "sha256:deadbeef"
                        : PackageReader.ComputeFileHash(assemblyBytes),
                    Size = assemblyBytes.Length,
                    EntryPoint = entryPoint,
                },
            ],
            SupplementalMetadata = "metadata-supplement.bin",
            Signature = signature,
        };

        File.WriteAllText(
            Path.Combine(packageRoot, PackageReader.ManifestFileName),
            JsonSerializer.Serialize(manifest, new JsonSerializerOptions { WriteIndented = true }));
        return packageRoot;
    }
}
