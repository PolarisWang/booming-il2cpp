using System.Text.Json;

namespace Chaos.IL2CPP.ProjectGraph;

public sealed class PackageResolver
{
    public IReadOnlyList<ProjectGraphDependencyModel> ResolveFromAssetsFile(
        string assetsFilePath,
        string fromAssemblyName)
    {
        if (!File.Exists(assetsFilePath))
        {
            return [];
        }

        using var document = JsonDocument.Parse(File.ReadAllText(assetsFilePath));
        var root = document.RootElement;
        var packageFolders = root.TryGetProperty("packageFolders", out var packageFoldersElement)
            ? packageFoldersElement.EnumerateObject().Select(property => property.Name).ToList()
            : [];
        var libraries = root.TryGetProperty("libraries", out var librariesElement)
            ? librariesElement
            : default;
        var directPackageIds = CollectDirectPackageIds(root);
        if (directPackageIds.Count == 0 ||
            !root.TryGetProperty("targets", out var targetsElement))
        {
            return [];
        }

        var resolved = new Dictionary<string, ProjectGraphDependencyModel>(StringComparer.OrdinalIgnoreCase);
        foreach (var frameworkTarget in targetsElement.EnumerateObject())
        {
            foreach (var packageEntry in frameworkTarget.Value.EnumerateObject())
            {
                var entryName = packageEntry.Name;
                var separatorIndex = entryName.IndexOf('/', StringComparison.Ordinal);
                if (separatorIndex <= 0)
                {
                    continue;
                }

                var packageId = entryName[..separatorIndex];
                if (!directPackageIds.Contains(packageId))
                {
                    continue;
                }

                var version = entryName[(separatorIndex + 1)..];
                var assetPath = ResolveAssemblyAssetPath(packageEntry.Value);
                var assemblyPath = TryResolveFullAssemblyPath(packageFolders, libraries, entryName, assetPath);
                if (assemblyPath is null)
                {
                    continue;
                }

                resolved[packageId] = new ProjectGraphDependencyModel
                {
                    DependencyKind = "package-reference",
                    FromAssemblyName = fromAssemblyName,
                    PackageId = packageId,
                    PackageVersion = version,
                    AssemblyPath = assemblyPath,
                };
            }
        }

        return resolved.Values.ToList();
    }

    private static HashSet<string> CollectDirectPackageIds(JsonElement root)
    {
        var packageIds = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        if (!root.TryGetProperty("project", out var projectElement) ||
            !projectElement.TryGetProperty("frameworks", out var frameworksElement))
        {
            return packageIds;
        }

        foreach (var framework in frameworksElement.EnumerateObject())
        {
            if (!framework.Value.TryGetProperty("dependencies", out var dependenciesElement))
            {
                continue;
            }

            foreach (var dependency in dependenciesElement.EnumerateObject())
            {
                packageIds.Add(dependency.Name);
            }
        }

        return packageIds;
    }

    private static string? ResolveAssemblyAssetPath(JsonElement packageTarget)
    {
        foreach (var sectionName in new[] { "runtime", "compile" })
        {
            if (!packageTarget.TryGetProperty(sectionName, out var section))
            {
                continue;
            }

            foreach (var asset in section.EnumerateObject())
            {
                if (asset.Name.EndsWith(".dll", StringComparison.OrdinalIgnoreCase))
                {
                    return asset.Name.Replace('/', Path.DirectorySeparatorChar);
                }
            }
        }

        return null;
    }

    private static string? TryResolveFullAssemblyPath(
        IReadOnlyList<string> packageFolders,
        JsonElement librariesElement,
        string packageEntryName,
        string? assetPath)
    {
        if (assetPath is null ||
            librariesElement.ValueKind == JsonValueKind.Undefined ||
            !librariesElement.TryGetProperty(packageEntryName, out var libraryElement) ||
            !libraryElement.TryGetProperty("path", out var packagePathElement))
        {
            return null;
        }

        var packagePath = packagePathElement.GetString();
        if (string.IsNullOrWhiteSpace(packagePath))
        {
            return null;
        }

        foreach (var packageFolder in packageFolders)
        {
            var candidate = Path.GetFullPath(Path.Combine(packageFolder, packagePath, assetPath));
            if (File.Exists(candidate))
            {
                return candidate;
            }
        }

        return Path.GetFullPath(Path.Combine(packageFolders.FirstOrDefault() ?? string.Empty, packagePath, assetPath));
    }
}
