using System.Collections.Concurrent;
using System.Linq;
using System.Text.RegularExpressions;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

internal static class ScribanTemplateRenderer
{
    private const int TemplateLoopLimit = 100_000;

    /// <summary>
    /// Base indentation unit. Change this to globally switch between
    /// tabs, 4-space, 2-space, etc.  Used via the <c>{{ tab }}</c>
    /// global constant in Scriban templates.
    /// </summary>
    private const string IndentUnit = "\t";

    /// <summary>
    /// Public accessor for the base indentation unit.
    /// C# code should use this to build indentation strings
    /// (e.g. <c>ScribanTemplateRenderer.Tab</c> for one level,
    /// or <c>ScribanTemplateRenderer.Indentation(2)</c> for two levels).
    /// </summary>
    public static string Tab => IndentUnit;

    /// <summary>
    /// Build an indentation string of <paramref name="depth"/> levels.
    /// </summary>
    public static string Indentation(int depth) =>
        string.Concat(Enumerable.Repeat(IndentUnit, depth));

    /// <summary>
    /// Global constants injected into every template rendering context.
    /// <c>{{ tab }}</c> expands to one level of indentation.
    /// <c>{{ indentation }}</c> is context-specific and should be
    /// provided per-call in the model.
    /// </summary>
    private static readonly ScriptObject GlobalConstants = new()
    {
        ["tab"] = IndentUnit,
    };

    /// <summary>
    /// Normalize 4-space indentation to the configured IndentUnit.
    /// Applies to strings pre-built via StringBuilder that enter
    /// Scriban as model values.
    /// </summary>
    public static string NormalizeIndentation(string code)
    {
        return Regex.Replace(code, @"^( {4})+",
            m => string.Concat(Enumerable.Repeat(IndentUnit, m.Length / 4)),
            RegexOptions.Multiline);
    }

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
        context.LoopLimit = TemplateLoopLimit;
        context.LimitToString = -1;
        context.PushGlobal(GlobalConstants);
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
