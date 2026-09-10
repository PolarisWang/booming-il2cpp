using System.Reflection;

namespace Chaos.IL2CPP.Generator.SnapshotTests;

/// <summary>
/// Discovers fixture directories under Fixtures/ in the test project output.
/// Each subdirectory = one fixture (contains 5 required artifact JSON files).
/// </summary>
internal static class FixtureRepository
{
    /// <summary>
    /// Returns (fixtureName, fixtureDirectory) pairs for all discovered fixtures.
    /// </summary>
    public static IEnumerable<object[]> DiscoverFixtures()
    {
        var fixturesRoot = GetFixturesRoot();
        if (!Directory.Exists(fixturesRoot))
        {
            throw new DirectoryNotFoundException(
                $"Fixtures root not found: {fixturesRoot}");
        }

        return Directory
            .GetDirectories(fixturesRoot)
            .OrderBy(d => d)
            .Select(dir => new object[] { Path.GetFileName(dir) })
            .ToList();
    }

    /// <summary>
    /// Resolves the Fixtures/ directory relative to the test assembly location.
    /// </summary>
    public static string GetFixturesRoot()
    {
        var assemblyDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!;
        return Path.GetFullPath(Path.Combine(assemblyDir, "Fixtures"));
    }

    /// <summary>
    /// Returns the full path to a specific fixture directory.
    /// </summary>
    public static string GetFixturePath(string fixtureName)
    {
        return Path.Combine(GetFixturesRoot(), fixtureName);
    }

    /// <summary>
    /// Returns the Baselines/ directory path.
    /// </summary>
    public static string GetBaselinesRoot()
    {
        var assemblyDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!;
        return Path.GetFullPath(Path.Combine(assemblyDir, "Baselines"));
    }

    /// <summary>
    /// Returns the full path to a specific fixture's baseline directory.
    /// </summary>
    public static string GetBaselinePath(string fixtureName)
    {
        return Path.Combine(GetBaselinesRoot(), fixtureName);
    }
}
