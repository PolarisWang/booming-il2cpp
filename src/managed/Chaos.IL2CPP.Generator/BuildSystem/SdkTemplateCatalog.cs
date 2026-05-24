using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator.BuildSystem;

/// <summary>
/// Template catalog for chaos-il2cpp SDK output files.
/// Provides cached access to chaos-config.cmake, chaos-targets.cmake,
/// and chaos.h Scriban templates, plus convenience render methods.
/// </summary>
public static class SdkTemplateCatalog
{
    public const string ChaosConfigTemplateRelativePath = "Templates/chaos-config.cmake.scriban";
    public const string ChaosTargetsTemplateRelativePath = "Templates/chaos-targets.cmake.scriban";
    public const string ChaosHeaderTemplateRelativePath = "Templates/chaos.h.scriban";

    /// <summary>
    /// Render the chaos-config.cmake template with the given model.
    /// </summary>
    public static string RenderChaosConfig(ScriptObject? model = null)
    {
        var template = ScribanTemplateRenderer.LoadTemplate(ChaosConfigTemplateRelativePath);
        return ScribanTemplateRenderer.RenderTemplate(template, model ?? new ScriptObject());
    }

    /// <summary>
    /// Render the chaos-targets.cmake template with the given model.
    /// </summary>
    public static string RenderChaosTargets(ScriptObject? model = null)
    {
        var template = ScribanTemplateRenderer.LoadTemplate(ChaosTargetsTemplateRelativePath);
        return ScribanTemplateRenderer.RenderTemplate(template, model ?? new ScriptObject());
    }

    /// <summary>
    /// Render the chaos.h header template with the given model.
    /// </summary>
    public static string RenderChaosHeader(ScriptObject? model = null)
    {
        var template = ScribanTemplateRenderer.LoadTemplate(ChaosHeaderTemplateRelativePath);
        return ScribanTemplateRenderer.RenderTemplate(template, model ?? new ScriptObject());
    }
}
