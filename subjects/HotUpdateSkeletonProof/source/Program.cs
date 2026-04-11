using System.Text.Json;
using Chaos.IL2CPP.HotUpdate;

internal static class Helper
{
    public static int GetValue()
    {
        return 1;
    }
}

internal static class Program
{
    private const string CurrentAotVersion = "1.0.0";
    private const string HotUpdateSubjectId = "HotUpdateSkeletonProof/Helper::GetValue()";

    private static int Main(string[] args)
    {
        var workspace = Path.Combine(Path.GetTempPath(), $"hot-update-skeleton-proof-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var runtimeManager = new RuntimeManager();
            System.Console.WriteLine("HotUpdateSkeletonProof entry reached.");
            System.Console.WriteLine($"args={args.Length}");
            System.Console.WriteLine($"before-load={runtimeManager.DispatchInt32(HotUpdateSubjectId, Helper.GetValue)}");

            var validPackageRoot = CreatePackageRoot(workspace, "valid", corruptAssemblyHash: false);
            var validPackage = PackageReader.ReadFromDirectory(validPackageRoot);
            runtimeManager.LoadPackage(
                validPackage,
                CurrentAotVersion,
                new Dictionary<string, int>(StringComparer.Ordinal)
                {
                    [HotUpdateSubjectId] = 42,
                });

            System.Console.WriteLine($"after-load={runtimeManager.DispatchInt32(HotUpdateSubjectId, Helper.GetValue)}");
            runtimeManager.UnloadPackage();
            System.Console.WriteLine($"after-unload={runtimeManager.DispatchInt32(HotUpdateSubjectId, Helper.GetValue)}");

            try
            {
                var corruptPackageRoot = CreatePackageRoot(workspace, "corrupt", corruptAssemblyHash: true);
                PackageReader.ReadFromDirectory(corruptPackageRoot);
                System.Console.WriteLine("corruption=unexpected-pass");
                return 1;
            }
            catch (InvalidDataException)
            {
                System.Console.WriteLine("corruption=rejected");
            }

            return 0;
        }
        finally
        {
            if (Directory.Exists(workspace))
            {
                Directory.Delete(workspace, recursive: true);
            }
        }
    }

    private static string CreatePackageRoot(string workspaceRoot, string packageIdSuffix, bool corruptAssemblyHash)
    {
        var packageRoot = Path.Combine(workspaceRoot, packageIdSuffix);
        Directory.CreateDirectory(packageRoot);

        var assemblyBytes = new byte[]
        {
            0x48,
            0x4F,
            0x54,
            0x34,
            0x32,
        };
        File.WriteAllBytes(Path.Combine(packageRoot, "HotPatch.dll"), assemblyBytes);
        File.WriteAllBytes(Path.Combine(packageRoot, "metadata-supplement.bin"), new byte[] { 0x01, 0x02, 0x03 });

        var correctHash = PackageReader.ComputeFileHash(assemblyBytes);
        var manifest = new HotUpdatePackage
        {
            PackageId = $"com.example.hotupdate.{packageIdSuffix}",
            TargetAotVersion = CurrentAotVersion,
            Assemblies = new[]
            {
                new HotUpdateAssemblyEntry
                {
                    Name = "HotPatch.dll",
                    Hash = corruptAssemblyHash ? "sha256:deadbeef" : correctHash,
                    Size = assemblyBytes.Length,
                    EntryPoint = "HotPatch/Patch::Apply()",
                },
            },
            SupplementalMetadata = "metadata-supplement.bin",
            Signature = "signed-proof",
        };

        File.WriteAllText(
            Path.Combine(packageRoot, PackageReader.ManifestFileName),
            JsonSerializer.Serialize(manifest, new JsonSerializerOptions { WriteIndented = true }));
        return packageRoot;
    }
}
