using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator.Templating;

/// <summary>
/// Loads and caches .scriban templates from the Templates/ directory.
/// </summary>
internal static class TemplateCatalog
{
    private const string TemplatesPrefix = "Templates/";
    private const int LoopLimit = 100_000;

    private static readonly string[] TemplateNames =
    [
        "TestProject.CMakeLists.txt.scriban",
        "TestProject.CMakePresets.json.scriban",
        "TestProject.Entry.cpp.scriban",
        "TestProject.Entry.h.scriban",
        "TestProject.Dispatch.cpp.scriban",
        "TestProject.RuntimePatchdata.cpp.scriban",
        "TestProject.chaos-config.cmake.scriban",
        "TestProject.chaos-targets.cmake.scriban",
        "TestProject.metadata.json.scriban",
    ];

    private static readonly Dictionary<string, Template> Cache = new(StringComparer.Ordinal);

    static TemplateCatalog()
    {
        var baseDir = AppContext.BaseDirectory;
        foreach (var name in TemplateNames)
        {
            var relativePath = TemplatesPrefix + name;
            var fullPath = Path.Combine(baseDir, relativePath.Replace('/', Path.DirectorySeparatorChar));
            if (!File.Exists(fullPath))
                throw new FileNotFoundException($"Scriban template not found: {fullPath}", fullPath);

            var content = File.ReadAllText(fullPath);
            var template = Template.Parse(content, relativePath);
            if (template.HasErrors)
                throw new InvalidOperationException(
                    $"Failed to parse template '{name}':{Environment.NewLine}" +
                    string.Join(Environment.NewLine, template.Messages));

            Cache[name] = template;
        }
    }

    public static string Render(string templateName, ScriptObject model)
    {
        if (!Cache.TryGetValue(templateName, out var template))
            throw new ArgumentException($"Unknown template: {templateName}", nameof(templateName));

        var context = new TemplateContext();
        context.LoopLimit = LoopLimit;
        context.PushGlobal(model);
        try
        {
            return template.Render(context).TrimEnd();
        }
        finally
        {
            context.PopGlobal();
        }
    }
}
