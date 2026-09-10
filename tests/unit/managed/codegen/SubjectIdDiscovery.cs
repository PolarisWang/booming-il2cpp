using System.IO;
using System.Linq;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Pipeline;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class SubjectIdDiscovery
{
    private static string StubAssemblyPath
    {
        get
        {
            var dir = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory);
            while (dir != null && !Directory.Exists(Path.Combine(dir.FullName, ".git")))
                dir = dir.Parent;
            var repoRoot = dir?.FullName ?? throw new DirectoryNotFoundException(
                "Could not locate repository root (.git directory).");

            return Path.GetFullPath(Path.Combine(
                repoRoot,
                "tests", "unit", "managed", "codegen",
                "StubAssembly", "bin", "Release", "net8.0",
                "StubAssembly.dll"));
        }
    }

    [Fact]
    public void DumpAllTypedIlSubjectIds()
    {
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: Path.GetTempPath(),
            FullAssemblyClosure: true);

        var result = new PipelinePlan().Execute(request).Value!;

        var lines = result.TypedIlIr.Methods.OrderBy(x => x.SubjectId)
            .Select(m => m.SubjectId).ToList();
        var outputPath = Path.Combine(Path.GetTempPath(), "subject_ids_discovery.txt");
        File.WriteAllLines(outputPath, lines);
        System.Console.WriteLine($"Wrote {lines.Count} subject IDs to {outputPath}");
    }
}
