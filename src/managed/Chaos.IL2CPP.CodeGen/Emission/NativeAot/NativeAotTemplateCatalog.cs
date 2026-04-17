using Scriban;

namespace Chaos.IL2CPP.CodeGen;

internal static class NativeAotTemplateCatalog
{
    public const string TranslationUnitTemplateRelativePath = "Templates/NativeAot.TranslationUnit.cpp.scriban";
    public const string ObjectModelTemplateRelativePath = "Templates/NativeAot.ObjectModel.cpp.scriban";
    public const string MethodTemplateRelativePath = "Templates/NativeAot.Method.cpp.scriban";

    public static Template GetTranslationUnitTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(TranslationUnitTemplateRelativePath);
    }

    public static Template GetObjectModelTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(ObjectModelTemplateRelativePath);
    }

    public static Template GetMethodTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(MethodTemplateRelativePath);
    }
}
