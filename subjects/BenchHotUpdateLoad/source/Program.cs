using System.Diagnostics;
using System.Text.Json;
using Chaos.IL2CPP.HotUpdate;

namespace BenchHotUpdateLoad;

internal static class Program
{
    private const string CurrentAotVersion = "1.0.0";
    private const string SubjectId = "BenchHotUpdateLoad/HotPatch::GetValue()";

    public static int Main(string[] args)
    {
        var workspace = Path.Combine(Path.GetTempPath(), $"bench-hot-update-load-{Guid.NewGuid():N}");
        Directory.CreateDirectory(workspace);

        try
        {
            var runtimeManager = new RuntimeManager();
            var packageRoot = CreatePackageRoot(workspace);

            var sw = Stopwatch.StartNew();
            var loaded = runtimeManager.LoadPackage(
                packageRoot,
                CurrentAotVersion,
                subjectIdToConstantInt32: new Dictionary<string, int>(StringComparer.Ordinal)
                {
                    [SubjectId] = 1,
                });
            sw.Stop();

            Console.WriteLine(
                JsonSerializer.Serialize(new
                {
                    elapsedMilliseconds = sw.Elapsed.TotalMilliseconds,
                    loadDurationMs = sw.Elapsed.TotalMilliseconds,
                    methodCount = 1,
                    loaded,
                }));
            return loaded ? 0 : 1;
        }
        finally
        {
            if (Directory.Exists(workspace))
            {
                Directory.Delete(workspace, recursive: true);
            }
        }
    }

    private static string CreatePackageRoot(string workspaceRoot)
    {
        var packageRoot = Path.Combine(workspaceRoot, "package");
        Directory.CreateDirectory(packageRoot);

        var assemblyBytes = new byte[] { 0x42, 0x48, 0x4C };
        File.WriteAllBytes(Path.Combine(packageRoot, "HotPatch.dll"), assemblyBytes);
        File.WriteAllText(Path.Combine(packageRoot, "metadata-supplement.bin"), "{}");

        var manifest = new HotUpdatePackage
        {
            PackageId = "bench.hotupdate.load",
            TargetAotVersion = CurrentAotVersion,
            Assemblies =
            [
                new HotUpdateAssemblyEntry
                {
                    Name = "HotPatch.dll",
                    Hash = PackageReader.ComputeFileHash(assemblyBytes),
                    Size = assemblyBytes.Length,
                    EntryPoint = SubjectId,
                },
            ],
            SupplementalMetadata = "metadata-supplement.bin",
            Signature = "bench-load",
        };

        File.WriteAllText(
            Path.Combine(packageRoot, PackageReader.ManifestFileName),
            JsonSerializer.Serialize(manifest, new JsonSerializerOptions { WriteIndented = true }));
        return packageRoot;
    }
}
