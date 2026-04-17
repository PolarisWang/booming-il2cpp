using System.Collections.Concurrent;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

internal static class ScribanTemplateRenderer
{
    private static readonly ConcurrentDictionary<string, Lazy<Template>> TemplateByRelativePath =
        new(StringComparer.Ordinal);

    public static Template LoadTemplate(string relativeTemplatePath)
    {
        return TemplateByRelativePath.GetOrAdd(
            relativeTemplatePath,
            static path => new Lazy<Template>(() => ParseTemplate(path))).Value;
    }

    public static string RenderTemplate(Template template, ScriptObject model)
    {
        var context = new TemplateContext();
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

    private static Template ParseTemplate(string relativeTemplatePath)
    {
        var templatePath = Path.Combine(AppContext.BaseDirectory, relativeTemplatePath.Replace('/', Path.DirectorySeparatorChar));
        if (!File.Exists(templatePath))
        {
            throw new FileNotFoundException("required Scriban template is missing", templatePath);
        }

        var template = Template.Parse(File.ReadAllText(templatePath), templatePath);
        if (!template.HasErrors)
        {
            return template;
        }

        throw new InvalidOperationException(
            $"failed to parse Scriban template '{relativeTemplatePath}':{Environment.NewLine}{string.Join(Environment.NewLine, template.Messages)}");
    }
}
