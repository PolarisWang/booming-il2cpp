using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Loader;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

/// <summary>
/// ASYNC-P1-5 regression: the ASYNC test fixtures must be loaded from THIS
/// worktree, not from a sibling checkout.
///
/// <para>
/// Historically every codegen test resolved its input assembly with
/// <c>while (dir != null &amp;&amp; !Directory.Exists(Path.Combine(dir.FullName, ".git")))</c>.
/// Inside a git worktree <c>.git</c> is a FILE, so that walk skipped the
/// worktree root and kept climbing to the main checkout's real <c>.git</c>
/// directory. Tests then silently ran against the main checkout's stale
/// <c>AsyncTestAssembly.dll</c> — edits and rebuilds in the worktree had no
/// observable effect, which is exactly how the "loader drops non-async
/// methods" false lead was manufactured.
/// </para>
///
/// <para>
/// These assertions pin both halves: the locator resolves inside the repo that
/// actually owns the test binaries, and the Loader surfaces the plain
/// (non-async) methods that the stale copy was missing.
/// </para>
/// </summary>
public sealed class LoaderFixtureFreshnessTests
{
    [Fact]
    public void RepoRoot_ResolvesToTheTreeOwningTheseTestBinaries()
    {
        var repoRoot = RepoRootLocator.FindFromBaseDirectory();

        // The test binaries live under <repoRoot>/tests/unit/managed/codegen/bin/...
        // so repoRoot must be an ancestor of the assembly's base directory.
        var baseDir = Path.GetFullPath(AppDomain.CurrentDomain.BaseDirectory);
        Assert.StartsWith(
            Path.GetFullPath(repoRoot) + Path.DirectorySeparatorChar,
            baseDir,
            StringComparison.OrdinalIgnoreCase);

        Assert.True(
            File.Exists(Path.Combine(repoRoot, ".git")) ||
            Directory.Exists(Path.Combine(repoRoot, ".git")),
            $"expected a .git entry at '{repoRoot}'");
    }

    [Fact]
    public void AsyncFixtureAssembly_IsTheOneUnderThisRepoRoot()
    {
        var repoRoot = RepoRootLocator.FindFromBaseDirectory();
        var subjectPath = Path.GetFullPath(AsyncPipelineTests.AsyncAssemblyPath);

        Assert.StartsWith(
            Path.GetFullPath(repoRoot) + Path.DirectorySeparatorChar,
            subjectPath,
            StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void Loader_SurfacesNonAsyncMethodsAlongsideAsyncOnes()
    {
        var request = new ManagedClosureRequest(
            AsyncPipelineTests.AsyncAssemblyPath,
            Path.Combine(Path.GetTempPath(), "ChaosLoaderCoverage_" + Guid.NewGuid().ToString("N")),
            EntryPointSubjectIdOverride: null,
            AdditionalAssemblyPaths: null,
            FullAssemblyClosure: true);

        var loaded = new LoaderStage().LoadMultiple(request);
        Assert.True(loaded.IsSuccess, loaded.Error?.Message);

        var subjectIds = loaded.Value!.Methods.Select(m => m.SubjectId).ToList();

        // Async surface.
        Assert.Contains(subjectIds, id => id.Contains("AsyncMethods::GetOne"));
        Assert.Contains(subjectIds, id => id.Contains(">d__") && id.Contains("::MoveNext"));

        // Plain non-async surface. If these are missing, the loader is reading a
        // stale assembly and the async expectations above are meaningless.
        Assert.Contains(subjectIds, id => id.Contains("AsyncMethods::RunAction"));
        Assert.Contains(subjectIds, id => id.Contains("AsyncMethods::AliveSentinel"));

        // AwaitRunAction is async AND takes a parameter — it is the case that
        // exposed the stale fixture, because it was added in the same edit as
        // RunAction but only compiled into the fresh copy.
        Assert.Contains(subjectIds, id => id.Contains("AsyncMethods::AwaitRunAction"));
    }
}
