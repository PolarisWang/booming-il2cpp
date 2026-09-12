using System;
using System.IO;

namespace Chaos.IL2CPP.Generator.Tests;

/// <summary>
/// Locates the repository root from a test's output directory.
///
/// <para>
/// <b>Why this exists.</b> The obvious incantation —
/// <c>while (dir != null &amp;&amp; !Directory.Exists(Path.Combine(dir.FullName, ".git"))) dir = dir.Parent;</c>
/// — is silently WRONG inside a git worktree. In a worktree, <c>.git</c> is a
/// <i>file</i> containing <c>gitdir: ...</c>, not a directory. The walk therefore
/// skips past the worktree root and keeps climbing until it reaches the main
/// checkout's real <c>.git</c> directory.
/// </para>
///
/// <para>
/// The consequence is severe and easy to miss: any test that resolves its input
/// assembly through such a helper reads the <b>main checkout's</b> file, not the
/// worktree's. Edits made in the worktree appear to have no effect, freshly
/// built fixtures are never observed, and the test passes or fails against a
/// stale artifact. See <c>ASYNC-P1-5</c> for the incident that surfaced this.
/// </para>
///
/// <para>
/// Detection must accept either a <c>.git</c> directory (normal checkout, or a
/// submodule) or a <c>.git</c> file (worktree). <see cref="File.Exists"/> covers
/// both.
/// </para>
/// </summary>
internal static class RepoRootLocator
{
    /// <summary>
    /// Walks up from <paramref name="startDirectory"/> until it finds the
    /// directory that contains a <c>.git</c> entry (file or directory).
    /// </summary>
    /// <exception cref="DirectoryNotFoundException">
    /// No enclosing directory contains a <c>.git</c> entry.
    /// </exception>
    public static string Find(string startDirectory)
    {
        var dir = new DirectoryInfo(startDirectory);
        while (dir != null && !File.Exists(Path.Combine(dir.FullName, ".git"))
            && !Directory.Exists(Path.Combine(dir.FullName, ".git")))
        {
            dir = dir.Parent;
        }

        return dir?.FullName ?? throw new DirectoryNotFoundException(
            $"Could not locate repository root (.git) above '{startDirectory}'.");
    }

    /// <summary>
    /// Convenience overload starting from the test assembly's output directory.
    /// </summary>
    public static string FindFromBaseDirectory()
        => Find(AppDomain.CurrentDomain.BaseDirectory);
}
