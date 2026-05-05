using Scriban;

namespace Chaos.IL2CPP.CodeGen;

internal static class NativeAotTemplateCatalog
{
    public static IReadOnlyList<ICodegenTemplateBundlePlugin> GetRegisteredTemplateBundles() =>
        NativeAotTemplateBundleRegistry.Plugins;

    public const string TranslationUnitTemplateRelativePath = "Templates/NativeAot.TranslationUnit.cpp.scriban";
    public const string ObjectModelTemplateRelativePath = "Templates/NativeAot.ObjectModel.cpp.scriban";
    public const string MethodTemplateRelativePath = "Templates/NativeAot.Method.cpp.scriban";
    public const string RuntimePreludeTemplateRelativePath = "Templates/NativeAot.RuntimePrelude.cpp.scriban";
    public const string SpanRuntimePreludeTemplateRelativePath = "Templates/NativeAot.SpanRuntimePrelude.cpp.scriban";
    public const string CollectionRuntimePreludeTemplateRelativePath = "Templates/NativeAot.CollectionRuntimePrelude.cpp.scriban";
    public const string MonitorRuntimePreludeTemplateRelativePath = "Templates/NativeAot.MonitorRuntimePrelude.cpp.scriban";
    public const string ThreadRuntimePreludeTemplateRelativePath = "Templates/NativeAot.ThreadRuntimePrelude.cpp.scriban";
    public const string AsyncRuntimePreludeTemplateRelativePath = "Templates/NativeAot.AsyncRuntimePrelude.cpp.scriban";
    public const string StaticInitializationDefinitionTemplateRelativePath = "Templates/NativeAot.StaticInitializationDefinition.cpp.scriban";
    public const string StaticInitializationActionTemplateRelativePath = "Templates/NativeAot.StaticInitializationAction.cpp.scriban";
    public const string StaticInitializationCallTemplateRelativePath = "Templates/NativeAot.StaticInitializationCall.cpp.scriban";
    public const string MethodReturnTemplateRelativePath = "Templates/NativeAot.MethodReturn.cpp.scriban";
    public const string AbiReturnPushTemplateRelativePath = "Templates/NativeAot.AbiReturnPush.cpp.scriban";
    public const string AbiArgumentInitializationTemplateRelativePath = "Templates/NativeAot.AbiArgumentInitialization.cpp.scriban";
    public const string StringJoinInt32EnumerableTemplateRelativePath = "Templates/NativeAot.StringJoinInt32Enumerable.cpp.scriban";
    public const string StringJoinStringEnumerableTemplateRelativePath = "Templates/NativeAot.StringJoinStringEnumerable.cpp.scriban";
    public const string SimpleExternalRuntimeHelperTemplateRelativePath = "Templates/NativeAot.SimpleExternalRuntimeHelper.cpp.scriban";
    public const string ReflectionGetObjectTypeTemplateRelativePath = "Templates/NativeAot.ReflectionGetObjectType.cpp.scriban";
    public const string ReflectionCreateInstanceTemplateRelativePath = "Templates/NativeAot.ReflectionCreateInstance.cpp.scriban";
    public const string RuntimeHelperShapeDispatchHeaderTemplateRelativePath = "Templates/NativeAot.RuntimeHelperShapeDispatch.h.scriban";
    public const string TypeDeclarationsHeaderTemplateRelativePath = "Templates/NativeAot.TypeDeclarations.h.scriban";

    public static Template GetTypeDeclarationsTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(TypeDeclarationsHeaderTemplateRelativePath);
    }

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

    public static Template GetRuntimePreludeTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimePreludeTemplateRelativePath);
    }

    public static Template GetSpanRuntimePreludeTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(SpanRuntimePreludeTemplateRelativePath);
    }

    public static Template GetCollectionRuntimePreludeTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(CollectionRuntimePreludeTemplateRelativePath);
    }

    public static Template GetMonitorRuntimePreludeTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(MonitorRuntimePreludeTemplateRelativePath);
    }

    public static Template GetThreadRuntimePreludeTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(ThreadRuntimePreludeTemplateRelativePath);
    }

    public static Template GetAsyncRuntimePreludeTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(AsyncRuntimePreludeTemplateRelativePath);
    }

    public static Template GetStaticInitializationDefinitionTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(StaticInitializationDefinitionTemplateRelativePath);
    }

    public static Template GetStaticInitializationActionTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(StaticInitializationActionTemplateRelativePath);
    }

    public static Template GetStaticInitializationCallTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(StaticInitializationCallTemplateRelativePath);
    }

    public static Template GetMethodReturnTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(MethodReturnTemplateRelativePath);
    }

    public static Template GetAbiReturnPushTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(AbiReturnPushTemplateRelativePath);
    }

    public static Template GetAbiArgumentInitializationTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(AbiArgumentInitializationTemplateRelativePath);
    }

    public static Template GetStringJoinInt32EnumerableTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(StringJoinInt32EnumerableTemplateRelativePath);
    }

    public static Template GetStringJoinStringEnumerableTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(StringJoinStringEnumerableTemplateRelativePath);
    }

    public static Template GetSimpleExternalRuntimeHelperTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(SimpleExternalRuntimeHelperTemplateRelativePath);
    }

    public static Template GetReflectionGetObjectTypeTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(ReflectionGetObjectTypeTemplateRelativePath);
    }

    public static Template GetReflectionCreateInstanceTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(ReflectionCreateInstanceTemplateRelativePath);
    }

    public static Template GetRuntimeHelperShapeDispatchHeaderTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeHelperShapeDispatchHeaderTemplateRelativePath);
    }

}
