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
            var repoRoot = RepoRootLocator.FindFromBaseDirectory();

            return Path.GetFullPath(Path.Combine(
                repoRoot,
                "tests", "unit", "managed", "codegen",
                "StubAssembly", "bin", ResolveConfigurationDir(), "net8.0",
                "StubAssembly.dll"));
        }
    }

    /// <summary>
    /// Derive the active build configuration from this test assembly's output path
    /// (.../bin/&lt;config&gt;/&lt;tfm&gt;/).  Hardcoding "Release" here meant the test only
    /// ever found a stub DLL that some other build had left behind; in a fresh tree
    /// (or a worktree built as Debug) the path did not exist and the pipeline
    /// dereferenced a null result.
    /// </summary>
    private static string ResolveConfigurationDir()
    {
        var baseDir = AppDomain.CurrentDomain.BaseDirectory;
        foreach (var cfg in new[] { "Debug", "Release", "RelWithDebInfo", "Check" })
        {
            if (baseDir.Contains($"/bin/{cfg}/") || baseDir.Contains($"\\bin\\{cfg}\\"))
                return cfg;
        }
        return "Debug";
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
