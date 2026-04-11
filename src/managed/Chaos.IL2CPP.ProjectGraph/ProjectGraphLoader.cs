using System.Xml.Linq;

namespace Chaos.IL2CPP.ProjectGraph;

public sealed class ProjectGraphLoader
{
    private readonly PackageResolver _packageResolver = new();

    public ProjectGraphModel LoadFromEntryProject(string entryProjectPath, string outputDirectory)
    {
        var normalizedEntryProjectPath = Path.GetFullPath(entryProjectPath);
        var normalizedOutputDirectory = Path.GetFullPath(outputDirectory);
        var visited = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        var queue = new Queue<string>();
        var projects = new List<ProjectGraphProjectModel>();
        var dependencies = new List<ProjectGraphDependencyModel>();
        var projectAssemblyNames = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);

        queue.Enqueue(normalizedEntryProjectPath);

        while (queue.Count > 0)
        {
            var currentProjectPath = queue.Dequeue();
            if (!visited.Add(currentProjectPath))
            {
                continue;
            }

            var projectDocument = XDocument.Load(currentProjectPath);
            var assemblyName = ResolveAssemblyName(projectDocument, currentProjectPath);
            projectAssemblyNames[currentProjectPath] = assemblyName;
            projects.Add(new ProjectGraphProjectModel
            {
                AssemblyName = assemblyName,
                ProjectPath = currentProjectPath,
                OutputAssemblyPath = Path.Combine(normalizedOutputDirectory, $"{assemblyName}.dll"),
            });

            foreach (var referencedProjectPath in ResolveProjectReferencePaths(projectDocument, currentProjectPath))
            {
                var referencedAssemblyName = ResolveAssemblyName(XDocument.Load(referencedProjectPath), referencedProjectPath);
                dependencies.Add(new ProjectGraphDependencyModel
                {
                    DependencyKind = "project-reference",
                    FromAssemblyName = assemblyName,
                    ToAssemblyName = referencedAssemblyName,
                });
                queue.Enqueue(referencedProjectPath);
            }
        }

        foreach (var project in projects)
        {
            var assetsFilePath = Path.Combine(Path.GetDirectoryName(project.ProjectPath)!, "obj", "project.assets.json");
            dependencies.AddRange(_packageResolver.ResolveFromAssetsFile(assetsFilePath, project.AssemblyName));
        }

        var entryProject = projects.First(project =>
            string.Equals(project.ProjectPath, normalizedEntryProjectPath, StringComparison.OrdinalIgnoreCase));

        return new ProjectGraphModel
        {
            EntryProject = entryProject,
            Projects = projects,
            Dependencies = dependencies,
        };
    }

    private static string ResolveAssemblyName(XDocument projectDocument, string projectPath)
    {
        var assemblyName = projectDocument
            .Descendants()
            .FirstOrDefault(element => string.Equals(element.Name.LocalName, "AssemblyName", StringComparison.Ordinal))
            ?.Value;
        if (!string.IsNullOrWhiteSpace(assemblyName))
        {
            return assemblyName;
        }

        return Path.GetFileNameWithoutExtension(projectPath);
    }

    private static IReadOnlyList<string> ResolveProjectReferencePaths(XDocument projectDocument, string projectPath)
    {
        var projectDirectory = Path.GetDirectoryName(projectPath)!;
        return projectDocument
            .Descendants()
            .Where(element => string.Equals(element.Name.LocalName, "ProjectReference", StringComparison.Ordinal))
            .Select(element => element.Attribute("Include")?.Value)
            .Where(value => !string.IsNullOrWhiteSpace(value))
            .Select(value => Path.GetFullPath(Path.Combine(projectDirectory, value!)))
            .ToList();
    }
}
