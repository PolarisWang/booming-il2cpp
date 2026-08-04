using Xunit;

namespace Chaos.IL2CPP.Generator.SnapshotTests;

/// <summary>
/// Negation tests that verify the snapshot test infrastructure
/// can detect changes (i.e., tests don't always pass).
/// </summary>
public sealed class NegationTests : SnapshotTestBase
{
    /// <summary>
    /// Fixture names that are negative emitter tests.
    /// These are excluded from SnapshotTests.EmittedCodeMatchesBaseline.
    /// </summary>
    internal static readonly HashSet<string> NegativeFixtureNames = new()
    {
        "73-negative-empty-assembly",
        "74-negative-missing-artifact",
        "75-negative-invalid-plan-kind",
    };

    [Fact]
    public void Emitter_EmptyAssembly_ThrowsEntryMethodMissing()
    {
        var ex = Assert.Throws<InvalidOperationException>(() =>
            AssertSnapshotMatches("73-negative-empty-assembly"));
        Assert.Contains("entry method", ex.Message, StringComparison.OrdinalIgnoreCase);
        Assert.Contains("missing", ex.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Emitter_MissingMetadataRegistration_ThrowsLoadError()
    {
        var ex = Assert.Throws<InvalidOperationException>(() =>
            AssertSnapshotMatches("74-negative-missing-artifact"));
        Assert.Contains("load", ex.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Emitter_InvalidPlanKind_ThrowsNotSupported()
    {
        var ex = Assert.Throws<InvalidOperationException>(() =>
            AssertSnapshotMatches("75-negative-invalid-plan-kind"));
        Assert.Contains("plan kind", ex.Message, StringComparison.OrdinalIgnoreCase);
        Assert.Contains("invalid-plan-type", ex.Message, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void SnapshotTest_Fails_WhenBaselineDiffers()
    {
        var repoRoot = LocateRepoRoot();
        var testProjectDir = Path.Combine(
            repoRoot, "tests", "snapshots", "Chaos.IL2CPP.CodeGen.SnapshotTests");
        var baselineFile = Path.Combine(
            testProjectDir, "Baselines", "01-simple-add", "native-aot.generated.cpp");

        Assert.True(File.Exists(baselineFile),
            "Baseline file must exist for negation test");

        // Backup the original baseline
        var backup = baselineFile + ".negation-test-bak";
        File.Copy(baselineFile, backup, overwrite: true);

        try
        {
            // Corrupt the baseline with obvious garbage
            File.WriteAllText(baselineFile,
                "// CORRUPTED by negation test -- expected failure\n");

            // Temporarily unset SNAPSHOT_UPDATE so AssertSnapshotMatches compares (not updates)
            var savedSnapshotUpdate = Environment.GetEnvironmentVariable("SNAPSHOT_UPDATE");
            Environment.SetEnvironmentVariable("SNAPSHOT_UPDATE", null);
            try
            {
                // Run the snapshot test -- it MUST throw
                var ex = Assert.Throws<SnapshotMismatchException>(() =>
                    AssertSnapshotMatches("01-simple-add"));

                Assert.Equal("01-simple-add", ex.FixtureName);
                Assert.Equal("native-aot.generated.cpp", ex.SourceRelativePath);
            }
            finally
            {
                Environment.SetEnvironmentVariable("SNAPSHOT_UPDATE", savedSnapshotUpdate);
            }
        }
        finally
        {
            // Restore the original baseline
            if (File.Exists(backup))
            {
                File.Copy(backup, baselineFile, overwrite: true);
                File.Delete(backup);
            }
        }
    }

    [Fact]
    public void SnapshotTest_Fails_WhenFixtureMissing()
    {
        var repoRoot = LocateRepoRoot();
        var testProjectDir = Path.Combine(
            repoRoot, "tests", "snapshots", "Chaos.IL2CPP.CodeGen.SnapshotTests");
        var fixtureDir = Path.Combine(
            testProjectDir, "Fixtures", "__nonexistent__negation_test");

        // Ensure the fake fixture directory does NOT exist
        if (Directory.Exists(fixtureDir))
            Directory.Delete(fixtureDir, recursive: true);

        var ex = Assert.Throws<DirectoryNotFoundException>(() =>
            AssertSnapshotMatches("__nonexistent__negation_test"));

        Assert.Contains("Fixture not found", ex.Message);
    }

    [Fact]
    public void FindFirstDiffIndex_DetectsIdenticalStrings()
    {
        const string text = "hello world";
        var result = SnapshotTestBase.FindFirstDiffIndex(text, text);
        Assert.Equal(-1, result);
    }

    [Fact]
    public void FindFirstDiffIndex_DetectsDifferences()
    {
        const string a = "hello world";
        const string b = "hello xorld";
        var result = SnapshotTestBase.FindFirstDiffIndex(a, b);
        Assert.Equal(6, result);
    }

    [Fact]
    public void FindFirstDiffIndex_DetectsLengthMismatch()
    {
        const string a = "hello";
        const string b = "hello world";
        var result = SnapshotTestBase.FindFirstDiffIndex(a, b);
        Assert.Equal(5, result);
    }

    [Fact]
    public void NormalizeLineEndings_ConvertsCrLf()
    {
        var result = SnapshotTestBase.NormalizeLineEndings("line1\r\nline2\r\n");
        Assert.Equal("line1\nline2\n", result);
    }

    [Fact]
    public void NormalizeLineEndings_ConvertsMacStyleCr()
    {
        var result = SnapshotTestBase.NormalizeLineEndings("line1\rline2\r");
        Assert.Equal("line1\nline2\n", result);
    }

    [Fact]
    public void NormalizeLineEndings_LeavesLf()
    {
        const string input = "line1\nline2\n";
        var result = SnapshotTestBase.NormalizeLineEndings(input);
        Assert.Equal(input, result);
    }

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
            "Could not locate repository root (.git directory).");
    }
}
