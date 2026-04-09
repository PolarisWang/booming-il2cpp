using System;
using System.IO;
using System.Linq;

namespace MainlineFeaturePack.Subject.UnitTests;

internal static class RepoFiles
{
    public static string ReadText(params string[] segments)
    {
        var path = Path.Combine(new[] { FindRepoRoot().FullName }.Concat(segments).ToArray());
        return File.ReadAllText(path);
    }

    private static DirectoryInfo FindRepoRoot()
    {
        DirectoryInfo? current = new(AppContext.BaseDirectory);
        while (current is not null)
        {
            var subjectsDir = Path.Combine(current.FullName, "subjects");
            var contractsDir = Path.Combine(current.FullName, "contracts");
            if (Directory.Exists(subjectsDir) && Directory.Exists(contractsDir))
            {
                return current;
            }

            current = current.Parent;
        }

        throw new InvalidOperationException("unable to locate repository root from test output directory");
    }
}
