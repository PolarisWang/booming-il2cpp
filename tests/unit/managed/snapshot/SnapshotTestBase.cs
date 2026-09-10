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
            if (File.Exists(fixtureAssemblyDll))
            {
                File.Copy(fixtureAssemblyDll, dllDest, overwrite: true);
            }

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
                var normalizedContent = NormalizeLineEndings(source.Contents);

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
                    var baselineContent = NormalizeLineEndings(
                        File.ReadAllText(baselineFile));

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
    /// directory until we find a .git directory.
    /// </summary>
    private static string LocateRepoRoot()
    {
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        while (dir != null)
        {
            if (Directory.Exists(Path.Combine(dir.FullName, ".git")))
                return dir.FullName;
            dir = dir.Parent;
        }
        throw new DirectoryNotFoundException(
            "Could not locate repository root (.git directory). " +
            "Run tests from within the booming-il2cpp repository.");
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
