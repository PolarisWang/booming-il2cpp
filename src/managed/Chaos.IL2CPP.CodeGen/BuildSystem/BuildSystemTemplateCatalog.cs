using Scriban;

namespace Chaos.IL2CPP.CodeGen.BuildSystem;

internal static class BuildSystemTemplateCatalog
{
    public const string CMakeListsTemplateRelativePath = "Templates/NativeBuild.CMakeLists.txt.scriban";

    public static Template GetCMakeListsTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(CMakeListsTemplateRelativePath);
    }
}