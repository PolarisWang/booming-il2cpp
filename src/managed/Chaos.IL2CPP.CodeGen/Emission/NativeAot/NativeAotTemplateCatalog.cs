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
    public const string StaticInitializationDefinitionTemplateRelativePath = "Templates/NativeAot.StaticInitializationDefinition.cpp.scriban";
    public const string StaticInitializationActionTemplateRelativePath = "Templates/NativeAot.StaticInitializationAction.cpp.scriban";
    public const string StaticInitializationCallTemplateRelativePath = "Templates/NativeAot.StaticInitializationCall.cpp.scriban";
    public const string MethodReturnTemplateRelativePath = "Templates/NativeAot.MethodReturn.cpp.scriban";
    public const string AbiReturnPushTemplateRelativePath = "Templates/NativeAot.AbiReturnPush.cpp.scriban";
    public const string AbiArgumentInitializationTemplateRelativePath = "Templates/NativeAot.AbiArgumentInitialization.cpp.scriban";
    public const string StringJoinInt32EnumerableTemplateRelativePath = "Templates/NativeAot.StringJoinInt32Enumerable.cpp.scriban";
    public const string StringJoinStringEnumerableTemplateRelativePath = "Templates/NativeAot.StringJoinStringEnumerable.cpp.scriban";
    public const string SimpleExternalRuntimeHelperTemplateRelativePath = "Templates/NativeAot.SimpleExternalRuntimeHelper.cpp.scriban";
    public const string TranslationUnitNamespaceProperty = "codegen_namespace";
    public const string ReflectionGetObjectTypeTemplateRelativePath = "Templates/NativeAot.ReflectionGetObjectType.cpp.scriban";
    public const string ReflectionCreateInstanceTemplateRelativePath = "Templates/NativeAot.ReflectionCreateInstance.cpp.scriban";
    public const string RuntimeHelperShapeDispatchHeaderTemplateRelativePath = "Templates/NativeAot.RuntimeHelperShapeDispatch.h.scriban";
    public const string TypeDeclarationsHeaderTemplateRelativePath = "Templates/NativeAot.TypeDeclarations.h.scriban";
    public const string StringIdTableTemplateRelativePath = "Templates/NativeAot.StringIdTable.cpp.scriban";
    public const string ObjectEqualityTemplateRelativePath = "Templates/NativeAot.ObjectEquality.cpp.scriban";
    public const string GenericRegistrationTemplateRelativePath = "Templates/NativeAot.GenericRegistration.cpp.scriban";
    public const string GenericAotRegistrationTemplateRelativePath = "Templates/NativeAot.GenericAotRegistration.cpp.scriban";
    public const string GenericRegistrationHelperTemplateRelativePath = "Templates/NativeAot.GenericRegistrationHelper.cpp.scriban";
    public const string InterfaceTypeIdTemplateRelativePath = "Templates/NativeAot.InterfaceTypeId.cpp.scriban";
    public const string IfaceMapArrayTemplateRelativePath = "Templates/NativeAot.IfaceMapArray.cpp.scriban";
    public const string TypeInfoTemplateRelativePath = "Templates/NativeAot.TypeInfo.cpp.scriban";
    public const string VTableArrayTemplateRelativePath = "Templates/NativeAot.VTableArray.cpp.scriban";
    public const string ModuleRegistrationTemplateRelativePath = "Templates/NativeAot.ModuleRegistration.cpp.scriban";
    public const string DispatchEntryCodeTemplateRelativePath = "Templates/NativeAot.DispatchEntryCode.cpp.scriban";
    public const string CodeRegistrationTemplateRelativePath = "Templates/NativeAot.CodeRegistration.cpp.scriban";
    public const string AbiManifestTemplateRelativePath = "Templates/NativeAot.AbiManifest.cpp.scriban";
    public const string ExternalRuntimeDispatchTableTemplateRelativePath = "Templates/NativeAot.ExternalRuntimeDispatchTable.cpp.scriban";
    public const string HotpatchTableTemplateRelativePath = "Templates/NativeAot.HotpatchTable.cpp.scriban";
    public const string ReflectionQueryImageTemplateRelativePath = "Templates/NativeAot.ReflectionQueryImage.cpp.scriban";
    public const string MethodTableInitializationTemplateRelativePath = "Templates/NativeAot.MethodTableInitialization.cpp.scriban";
    public const string DelegateRuntimeSupportTemplateRelativePath = "Templates/NativeAot.DelegateRuntimeSupport.cpp.scriban";
    public const string StructMarshallingDescriptorsTemplateRelativePath = "Templates/NativeAot.StructMarshallingDescriptors.cpp.scriban";
    public const string DelegateThunksTemplateRelativePath = "Templates/NativeAot.DelegateThunks.cpp.scriban";
    public const string GeneratedModuleHeaderTemplateRelativePath = "Templates/NativeAot.GeneratedModule.h.scriban";
    public const string GeneratedModuleSourceTemplateRelativePath = "Templates/NativeAot.GeneratedModule.cpp.scriban";

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

    public static Template GetStringIdTableTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(StringIdTableTemplateRelativePath);
    }

    public static Template GetObjectEqualityTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(ObjectEqualityTemplateRelativePath);
    }

    public static Template GetGenericRegistrationTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(GenericRegistrationTemplateRelativePath);
    }

    public static Template GetGenericAotRegistrationTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(GenericAotRegistrationTemplateRelativePath);
    }

    public static Template GetGenericRegistrationHelperTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(GenericRegistrationHelperTemplateRelativePath);
    }

    public static Template GetModuleRegistrationTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(ModuleRegistrationTemplateRelativePath);
    }

    public static Template GetDispatchEntryCodeTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(DispatchEntryCodeTemplateRelativePath);
    }

    public static Template GetCodeRegistrationTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(CodeRegistrationTemplateRelativePath);
    }

    public static Template GetAbiManifestTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(AbiManifestTemplateRelativePath);
    }

    public static Template GetExternalRuntimeDispatchTableTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(ExternalRuntimeDispatchTableTemplateRelativePath);
    }

    public static Template GetHotpatchTableTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(HotpatchTableTemplateRelativePath);
    }

    public static Template GetReflectionQueryImageTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(ReflectionQueryImageTemplateRelativePath);
    }

    public static Template GetMethodTableInitializationTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(MethodTableInitializationTemplateRelativePath);
    }

    public static Template GetDelegateRuntimeSupportTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(DelegateRuntimeSupportTemplateRelativePath);
    }

    public static Template GetStructMarshallingDescriptorsTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(StructMarshallingDescriptorsTemplateRelativePath);
    }

    public static Template GetDelegateThunksTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(DelegateThunksTemplateRelativePath);
    }

    public static Template GetGeneratedModuleHeaderTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(GeneratedModuleHeaderTemplateRelativePath);
    }

    public static Template GetGeneratedModuleSourceTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(GeneratedModuleSourceTemplateRelativePath);
    }

    public static Template GetInterfaceTypeIdTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(InterfaceTypeIdTemplateRelativePath);
    }

    public static Template GetIfaceMapArrayTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(IfaceMapArrayTemplateRelativePath);
    }

    public static Template GetTypeInfoTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(TypeInfoTemplateRelativePath);
    }

    public static Template GetVTableArrayTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(VTableArrayTemplateRelativePath);
    }
}
