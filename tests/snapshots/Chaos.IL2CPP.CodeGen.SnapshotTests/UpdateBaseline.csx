using System;
using System.IO;
using System.Linq;
using Chaos.IL2CPP.Generator;

var repoRoot = FindRepoRoot();
var fixtureName = "54-throw-rethrow";
var testProjectDir = Path.Combine(repoRoot, "tests", "snapshots", "Chaos.IL2CPP.Generator.SnapshotTests");
var fixtureDir = Path.Combine(testProjectDir, "Fixtures", fixtureName);
var baselineDir = Path.Combine(testProjectDir, "Baselines", fixtureName);

// Copy fixture to temp
var tempDir = Path.Combine(Path.GetTempPath(), $"snapshot-fix-{Guid.NewGuid():N}");
CopyDirectory(fixtureDir, tempDir);

// Copy the assembly dll
var dllSrc = Path.Combine(testProjectDir, "FixtureAssembly", "bin", "SnapshotTestFixtures.dll");
var dllDst = Path.Combine(tempDir, "SnapshotTestFixtures.dll");
if (File.Exists(dllSrc)) File.Copy(dllSrc, dllDst, overwrite: true);

var oldCwd = Environment.CurrentDirectory;
Environment.CurrentDirectory = tempDir;

try
{
    var request = new NativeAotRequest(tempDir, tempDir);
    var emitter = new NativeAotEmitter();
    var result = emitter.Generate(request);

    foreach (var source in result.GeneratedSources)
    {
        var baselineFile = Path.Combine(baselineDir, source.RelativePath);
        Directory.CreateDirectory(Path.GetDirectoryName(baselineFile)!);
        File.WriteAllText(baselineFile, source.Contents.Replace("\r\n", "\n"));
        Console.WriteLine($"Wrote: {baselineFile}");
    }

    Console.WriteLine("Done! All baselines updated.");
}
finally
{
    Environment.CurrentDirectory = oldCwd;
    try { Directory.Delete(tempDir, recursive: true); } catch { }
}

static string FindRepoRoot()
{
    var dir = new DirectoryInfo(AppContext.BaseDirectory);
    while (dir != null)
    {
        if (Directory.Exists(Path.Combine(dir.FullName, ".git")))
            return dir.FullName;
        dir = dir.Parent;
    }
    throw new DirectoryNotFoundException("Repo root not found");
}

static void CopyDirectory(string sourceDir, string destDir)
{
    Directory.CreateDirectory(destDir);
    foreach (var file in Directory.GetFiles(sourceDir))
    {
        var relPath = Path.GetRelativePath(sourceDir, file);
        var destFile = Path.Combine(destDir, relPath);
        Directory.CreateDirectory(Path.GetDirectoryName(destFile)!);
        File.Copy(file, destFile, overwrite: true);
    }
    foreach (var subDir in Directory.GetDirectories(sourceDir))
    {
        var relPath = Path.GetRelativePath(sourceDir, subDir);
        CopyDirectory(subDir, Path.Combine(destDir, relPath));
    }
}
