using Scriban;

namespace Chaos.IL2CPP.Generator.BuildSystem;

/// <summary>
/// Template catalog for the legacy CmakeGenerator build system.
/// </summary>
/// <remarks>
/// DEPRECATED: CmakeGenerator and all its templates are no longer used
/// in production.  CMakeLists.txt generation moved to TPG .scriban
/// templates (TestProjectGenerator).  Retained only for unit test compat.
/// </remarks>
[Obsolete("Legacy CmakeGenerator support — use SdkTemplateCatalog and TPG .scriban templates instead.")]
internal static class BuildSystemTemplateCatalog
{
    public const string CMakeListsTemplateRelativePath = "Templates/NativeBuild.CMakeLists.txt.scriban";

    public static Template GetCMakeListsTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(CMakeListsTemplateRelativePath);
    }
}