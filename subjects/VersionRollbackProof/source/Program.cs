using System.Text.Json;
using Chaos.IL2CPP.HotUpdate;

namespace VersionRollbackProof;

internal static class Program
{
    private const string CurrentAotVersion = "1.0.0";
    private const string SubjectId = "VersionRollbackProof/HotPatch::GetValue()";

    private static int Main(string[] args)
    {
        var workspace = Path.Combine(Path.GetTempPath(), $"version-rollback-proof-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var runtimeManager = new RuntimeManager();
            var v1Root = CreatePackageRoot(workspace, "v1", "1.0.0");
            var v2Root = CreatePackageRoot(workspace, "v2", "1.0.0");
            var incompatibleRoot = CreatePackageRoot(workspace, "v3", "2.0.0");

            runtimeManager.LoadPackage(
                v1Root,
                CurrentAotVersion,
                subjectIdToConstantInt32: new Dictionary<string, int>(StringComparer.Ordinal)
                {
                    [SubjectId] = 11,
                });
            Console.WriteLine($"version-rollback-v1={runtimeManager.DispatchInt32(SubjectId, GetAotFallback)}");

            runtimeManager.LoadPackage(
                v2Root,
                CurrentAotVersion,
                subjectIdToConstantInt32: new Dictionary<string, int>(StringComparer.Ordinal)
                {
                    [SubjectId] = 22,
                });
            Console.WriteLine($"version-rollback-v2={runtimeManager.DispatchInt32(SubjectId, GetAotFallback)}");

            runtimeManager.Rollback();
            Console.WriteLine($"version-rollback-back-v1={runtimeManager.DispatchInt32(SubjectId, GetAotFallback)}");

            runtimeManager.Rollback();
            Console.WriteLine($"version-rollback-back-aot={runtimeManager.DispatchInt32(SubjectId, GetAotFallback)}");

            var compatible = runtimeManager.LoadPackage(
                incompatibleRoot,
                CurrentAotVersion,
                subjectIdToConstantInt32: new Dictionary<string, int>(StringComparer.Ordinal)
                {
                    [SubjectId] = 99,
                });
            Console.WriteLine($"version-rollback-compatibility={(compatible ? "unexpected" : "rejected")}");
            return compatible ? 1 : 0;
        }
        finally
        {
            if (Directory.Exists(workspace))
            {
                Directory.Delete(workspace, recursive: true);
            }
        }
    }

    private static int GetAotFallback()
    {
        return 5;
    }

    private static string CreatePackageRoot(string workspaceRoot, string suffix, string targetAotVersion)
    {
        var packageRoot = Path.Combine(workspaceRoot, suffix);
        Directory.CreateDirectory(packageRoot);

        var assemblyBytes = new byte[] { 0x56, 0x52, 0x50, 0x31 };
        File.WriteAllBytes(Path.Combine(packageRoot, "HotPatch.dll"), assemblyBytes);
        File.WriteAllText(Path.Combine(packageRoot, "metadata-supplement.bin"), "{}");

        var manifest = new HotUpdatePackage
        {
            PackageId = $"com.example.rollback.{suffix}",
            TargetAotVersion = targetAotVersion,
            Assemblies =
            [
                new HotUpdateAssemblyEntry
                {
                    Name = "HotPatch.dll",
                    Hash = PackageReader.ComputeFileHash(assemblyBytes),
                    Size = assemblyBytes.Length,
                    EntryPoint = "VersionRollbackProof/HotPatch::Apply()",
                },
            ],
            SupplementalMetadata = "metadata-supplement.bin",
            Signature = "rollback-proof",
        };

        File.WriteAllText(
            Path.Combine(packageRoot, PackageReader.ManifestFileName),
            JsonSerializer.Serialize(manifest, new JsonSerializerOptions { WriteIndented = true }));
        return packageRoot;
    }
}
