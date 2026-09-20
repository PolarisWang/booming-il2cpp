using System.Text.RegularExpressions;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator.SnapshotTests;

/// <summary>
/// Base class for snapshot tests. Provides fixture loading,
/// emitter invocation, and baseline comparison logic.
/// </summary>
public abstract class SnapshotTestBase
{
    /// <summary>
    /// Assert that the emitter output for the given fixture matches
    /// the committed baseline. Set SNAPSHOT_UPDATE=1 to update baselines.
    /// </summary>
    protected static void AssertSnapshotMatches(string fixtureName)
    {
        var repoRoot = LocateRepoRoot();
        var testProjectDir = Path.Combine(
            repoRoot, "tests", "unit", "managed", "snapshot");
        var fixtureDir = Path.Combine(testProjectDir, "Fixtures", fixtureName);
        var baselineDir = Path.Combine(testProjectDir, "Baselines", fixtureName);

        if (!Directory.Exists(fixtureDir))
        {
            throw new DirectoryNotFoundException(
                $"Fixture not found: {fixtureDir}");
        }

        // Copy fixture files to a temp directory
        var tempDir = Path.Combine(
            Path.GetTempPath(),
            $"snapshot-test-{fixtureName}-{Guid.NewGuid():N}");

        var originalCwd = Environment.CurrentDirectory;
        try
        {
            // Copy all fixture files preserving subdirectory structure
            CopyDirectory(fixtureDir, tempDir);

            // Copy the shared FixtureAssembly .dll alongside the manifest
            // so Path.GetFullPath("SnapshotTestFixtures.dll") resolves inside tempDir
            var fixtureAssemblyDll = Path.Combine(
                testProjectDir, "FixtureAssembly", "bin", "SnapshotTestFixtures.dll");
            var dllDest = Path.Combine(tempDir, "SnapshotTestFixtures.dll");
            if (!File.Exists(fixtureAssemblyDll))
            {
                // Fail here, naming the missing prerequisite.  Silently skipping
                // the copy (the previous behaviour) let the emitter run without
                // its input assembly and surface as "Entry method ... not found"
                // / FileNotFoundException from deep inside the planner, which
                // reads as a codegen defect rather than an unbuilt fixture.
                throw new FileNotFoundException(
                    $"Fixture support assembly not built: {fixtureAssemblyDll}. " +
                    "Run: dotnet build tests/unit/managed/snapshot/FixtureAssembly/ " +
                    "(the project is not a ProjectReference of the test csproj, so " +
                    "`dotnet test` alone does not build it).", fixtureAssemblyDll);
            }
            File.Copy(fixtureAssemblyDll, dllDest, overwrite: true);

            // Switch CWD to tempDir so relative paths in closure manifest resolve correctly
            Environment.CurrentDirectory = tempDir;

            // Call the emitter
            var request = new NativeAotRequest(tempDir, tempDir);
            var emitter = new NativeAotEmitter();
            NativeAotResult result;
            try
            {
                result = emitter.Generate(request);
            }
            catch (Exception ex)
            {
                throw new InvalidOperationException(
                    $"Emitter.Generate() failed for fixture '{fixtureName}': {ex.Message}", ex);
            }

            var isUpdateMode = Environment.GetEnvironmentVariable("SNAPSHOT_UPDATE") == "1";

            // Check each generated source against baseline
            foreach (var source in result.GeneratedSources)
            {
                var baselineFile = Path.Combine(baselineDir, source.RelativePath);
                var normalizedContent = NormalizeVolatileFields(
                    NormalizeLineEndings(source.Contents), source.RelativePath);

                if (isUpdateMode || !File.Exists(baselineFile))
                {
                    // Write/update baseline in source tree
                    // Auto-generates missing baselines (handles new fixtures / regenerated baselines)
                    Directory.CreateDirectory(Path.GetDirectoryName(baselineFile)!);
                    File.WriteAllText(baselineFile, normalizedContent);
                    Console.WriteLine($"[SNAPSHOT_UPDATE] Wrote baseline: {baselineFile}");
                }
                else
                {
                    var baselineContent = NormalizeVolatileFields(
                        NormalizeLineEndings(File.ReadAllText(baselineFile)),
                        source.RelativePath);

                    if (!string.Equals(baselineContent, normalizedContent, StringComparison.Ordinal))
                    {
                        // Find first difference for diagnostic
                        var diffIndex = FindFirstDiffIndex(
                            baselineContent, normalizedContent);

                        throw new SnapshotMismatchException(
                            fixtureName, source.RelativePath, diffIndex);
                    }
                }
            }
        }
        finally
        {
            // Restore original CWD
            Environment.CurrentDirectory = originalCwd;

            // Clean up temp directory
            if (Directory.Exists(tempDir))
            {
                try { Directory.Delete(tempDir, recursive: true); }
                catch { /* best-effort cleanup */ }
            }
        }
    }

    /// <summary>
    /// Locate the repository root by walking up from the assembly
    /// directory until we find a .git entry.
    ///
    /// NOTE: this must accept a .git FILE as well as a directory. Inside a git
    /// worktree .git is a file holding "gitdir: ...", and testing only for a
    /// directory makes the walk skip past the worktree root and resolve to the
    /// main checkout instead — tests then read the wrong tree's fixtures.
    /// </summary>
    private static string LocateRepoRoot()
    {
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        while (dir != null)
        {
            // `.git` is a DIRECTORY in a normal checkout but a FILE (a gitdir
            // pointer) inside a linked worktree -- accept both.
            if (File.Exists(Path.Combine(dir.FullName, ".git")) ||
                Directory.Exists(Path.Combine(dir.FullName, ".git")))
                return dir.FullName;
            dir = dir.Parent;
        }
        throw new DirectoryNotFoundException(
            "Could not locate repository root (.git entry). " +
            "Run tests from within the booming-il2cpp repository.");
    }

    /// <summary>
    /// Emit a fixture once in a scratch directory and return the
    /// <c>RelativePath</c> of the FIRST generated source — i.e. the file whose
    /// baseline a mismatch would be reported against.
    /// </summary>
    /// <remarks>
    /// Exists so the negation test does not pin emission ORDER.  Asserting a
    /// specific file name couples the test to an implementation detail that has
    /// already changed once (the capability manifest now precedes the .cpp), and
    /// it broke for that reason alone.  The scratch emit never touches the
    /// committed baselines.
    /// </remarks>
    protected static string ProbeFirstEmittedSourceRelativePath(string fixtureName)
    {
        var repoRoot = LocateRepoRoot();
        var testProjectDir = Path.Combine(
            repoRoot, "tests", "snapshots", "Chaos.IL2CPP.CodeGen.SnapshotTests");
        // Honour whichever tree this test assembly actually came from.
        var baseDir = AppContext.BaseDirectory;
        if (baseDir.Contains("tests" + Path.DirectorySeparatorChar + "unit"))
        {
            testProjectDir = Path.Combine(repoRoot, "tests", "unit", "managed", "snapshot");
        }

        var fixtureDir = Path.Combine(testProjectDir, "Fixtures", fixtureName);
        var tempDir = Path.Combine(
            Path.GetTempPath(), $"snapshot-probe-{fixtureName}-{Guid.NewGuid():N}");

        var originalCwd = Environment.CurrentDirectory;
        try
        {
            CopyDirectory(fixtureDir, tempDir);

            var fixtureAssemblyDll = Path.Combine(
                testProjectDir, "FixtureAssembly", "bin", "SnapshotTestFixtures.dll");
            if (File.Exists(fixtureAssemblyDll))
            {
                File.Copy(fixtureAssemblyDll,
                    Path.Combine(tempDir, "SnapshotTestFixtures.dll"), overwrite: true);
            }

            Environment.CurrentDirectory = tempDir;
            var result = new NativeAotEmitter().Generate(
                new NativeAotRequest(tempDir, tempDir));

            var first = result.GeneratedSources.FirstOrDefault();
            if (first == null)
            {
                throw new InvalidOperationException(
                    $"Fixture '{fixtureName}' emitted no sources; cannot determine " +
                    "which baseline the harness checks first.");
            }
            return first.RelativePath;
        }
        finally
        {
            Environment.CurrentDirectory = originalCwd;
            try { Directory.Delete(tempDir, recursive: true); }
            catch (IOException) { /* best effort */ }
            catch (UnauthorizedAccessException) { /* best effort */ }
        }
    }

    /// <summary>
    /// Replace fields that vary run-to-run with a fixed placeholder so a
    /// committed baseline can ever match.
    /// </summary>
    /// <remarks>
    /// The capability manifest stamps <c>"generatedAt": "&lt;DateTime.UtcNow&gt;"</c>
    /// (RuntimeHelperShapeRegistry.ExportManifest).  Left as-is, every run emits
    /// different bytes, so no baseline is ever equal to the output: the suite
    /// reports a mismatch on all 85 fixtures, and <c>SNAPSHOT_UPDATE=1</c>
    /// "passes" only by writing that run's own timestamp back — which the very
    /// next clean run then rejects again.  The timestamp carries no information
    /// about the codegen under test, so it is normalised out of the comparison
    /// on both sides.
    /// </remarks>
    private static string NormalizeVolatileFields(string content, string relativePath)
    {
        if (!relativePath.EndsWith("aot-capability-manifest.json", StringComparison.Ordinal))
            return content;

        return Regex.Replace(content,
            "(\"generatedAt\"\\s*:\\s*)\"[^\"]*\"",
            "$1\"<normalized>\"");
    }

    /// <summary>
    /// Recursively copy a directory's contents.
    /// </summary>
    private static void CopyDirectory(string sourceDir, string destDir)
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

    /// <summary>
    /// Normalize line endings to LF for comparison.
    /// </summary>
    internal static string NormalizeLineEndings(string text)
    {
        return Regex.Replace(text, @"\r\n?", "\n");
    }

    /// <summary>
    /// Find the first index where two strings differ.
    /// Returns -1 if they are identical.
    /// </summary>
    internal static int FindFirstDiffIndex(string a, string b)
    {
        var minLen = Math.Min(a.Length, b.Length);
        for (var i = 0; i < minLen; i++)
        {
            if (a[i] != b[i])
                return i;
        }
        return a.Length == b.Length ? -1 : minLen;
    }
}

/// <summary>
/// Thrown when a generated file does not match its baseline.
/// </summary>
public sealed class SnapshotMismatchException : Exception
{
    public string FixtureName { get; }
    public string SourceRelativePath { get; }
    public int DiffIndex { get; }

    public SnapshotMismatchException(
        string fixtureName,
        string sourceRelativePath,
        int diffIndex)
        : base(FormatMessage(fixtureName, sourceRelativePath, diffIndex))
    {
        FixtureName = fixtureName;
        SourceRelativePath = sourceRelativePath;
        DiffIndex = diffIndex;
    }

    private static string FormatMessage(
        string fixtureName,
        string sourceRelativePath,
        int diffIndex)
    {
        var msg = $"Snapshot mismatch for fixture '{fixtureName}', " +
                  $"file '{sourceRelativePath}'.";
        if (diffIndex >= 0)
            msg += $" First difference at character index {diffIndex}.";
        msg += " If the change is intentional, run with SNAPSHOT_UPDATE=1 " +
               "to update baselines, then review via git diff.";
        return msg;
    }
}
