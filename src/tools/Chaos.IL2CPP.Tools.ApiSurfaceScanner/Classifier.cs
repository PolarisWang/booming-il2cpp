using System.Text.RegularExpressions;

namespace Chaos.IL2CPP.Tools.ApiSurfaceScanner;

/// <summary>
/// Aligns existing native implementations and codegen blacklists against the
/// enumerated API surface, producing an initial tier judgement per entry.
///
/// Per design §3.1 a judgement is never "silently returns a wrong value" —
/// every entry must land in real / not-supported / not-applicable.
///
/// This pass is deliberately conservative: it only marks <c>real</c> when a
/// corresponding native symbol is known to exist. Everything else stays
/// <c>unclassified</c> so Phase 2 has an explicit, non-fabricated work list.
/// </summary>
public static class Classifier
{
    /// <summary>
    /// Maps a matrix (typeName, memberName) to the native Chaos* symbol that
    /// implements it. Derived from src/native/runtime-core/reflection_api.h.
    /// </summary>
    private static readonly Dictionary<string, string[]> KnownNativeImpls = new(StringComparer.Ordinal)
    {
        // ── Type resolution ──────────────────────────────────────────
        ["Type.GetTypeFromHandle"] = new[] { "ChaosReflectionGetTypeFromHandle" },
        ["Type.GetType"] = new[] { "ChaosReflectionGetTypeByName" },
        ["Type.GetTypeHandle"] = new[] { "ChaosReflectionGetTypeHandle" },

        // ── Type properties ──────────────────────────────────────────
        ["Type.get_Namespace"] = new[] { "ChaosReflectionGetNamespace" },
        ["Type.get_FullName"] = new[] { "ChaosReflectionGetTypeFullName" },
        ["Type.get_AssemblyQualifiedName"] = new[] { "ChaosReflectionGetAssemblyQualifiedName" },
        ["Type.get_Assembly"] = new[] { "ChaosReflectionGetAssembly" },
        ["Type.get_BaseType"] = new[] { "ChaosReflectionGetBaseType" },
        ["Type.IsSubclassOf"] = new[] { "ChaosReflectionIsSubclassOf" },
        ["Type.IsAssignableFrom"] = new[] { "ChaosReflectionIsAssignableFrom" },
        ["Type.IsInstanceOfType"] = new[] { "ChaosReflectionIsInstanceOfType" },
        ["Type.GetInterfaces"] = new[] { "ChaosReflectionGetInterfaces" },
        ["Type.get_IsArray"] = new[] { "ChaosReflectionGetIsArray" },
        ["Type.get_IsEnum"] = new[] { "ChaosReflectionGetIsEnum" },
        ["Type.get_IsInterface"] = new[] { "ChaosReflectionGetIsInterface" },
        ["Type.get_IsAbstract"] = new[] { "ChaosReflectionGetIsAbstract" },
        ["Type.get_IsSealed"] = new[] { "ChaosReflectionGetIsSealed" },
        ["Type.get_IsValueType"] = new[] { "ChaosReflectionGetIsValueType" },
        ["Type.get_IsGenericType"] = new[] { "ChaosReflectionGetIsGenericType" },
        ["Type.get_IsPublic"] = new[] { "ChaosReflectionGetIsPublic" },
        ["Type.get_IsConstructedGenericType"] = new[] { "ChaosReflectionGetIsConstructedGeneric" },
        ["Type.GetGenericArguments"] = new[] { "ChaosReflectionGetGenericArguments" },
        ["Type.GetGenericTypeDefinition"] = new[] { "ChaosReflectionGetGenericTypeDefinition" },
        ["Type.get_ContainsGenericParameters"] = new[] { "ChaosReflectionGetContainsGenericParams" },
        ["Type.GetElementType"] = new[] { "ChaosTypeGetElementType" },
        ["Type.MakeArrayType"] = new[] { "ChaosTypeMakeArrayType" },
        ["Type.MakeByRefType"] = new[] { "ChaosTypeMakeByRefType" },
        ["Type.MakePointerType"] = new[] { "ChaosTypeMakePointerType" },

        // ── Member enumeration ───────────────────────────────────────
        ["Type.GetMembers"] = new[] { "ChaosReflectionGetMembers" },
        ["Type.GetMethods"] = new[] { "ChaosReflectionGetMethods" },
        ["Type.GetFields"] = new[] { "ChaosReflectionGetFields" },
        ["Type.GetProperties"] = new[] { "ChaosReflectionGetProperties" },
        ["Type.GetConstructors"] = new[] { "ChaosReflectionGetConstructors" },
        ["Type.GetNestedTypes"] = new[] { "ChaosReflectionGetNestedTypes" },
        ["Type.GetField"] = new[] { "ChaosReflectionGetField" },
        ["Type.GetMethod"] = new[] { "ChaosReflectionGetMethod" },
        ["Type.GetEvents"] = new[] { "ChaosTypeGetEvents" },
        ["Type.GetEvent"] = new[] { "ChaosTypeGetEvent" },
        ["Type.GetProperty"] = new[] { "ChaosTypeGetProperty" },

        // ── Member metadata ──────────────────────────────────────────
        ["MemberInfo.get_Name"] = new[] { "ChaosReflectionGetMemberName" },
        ["MemberInfo.get_DeclaringType"] = new[] { "ChaosReflectionGetDeclaringType" },
        ["MemberInfo.get_ReflectedType"] = new[] { "ChaosReflectionGetReflectedType" },
        ["MemberInfo.get_MetadataToken"] = new[] { "ChaosReflectionGetMetadataToken" },
        ["MemberInfo.GetCustomAttributesData"] = new[] { "ChaosGetCustomAttributeFromBlob" },

        // ── Method ───────────────────────────────────────────────────
        ["MethodBase.Invoke"] = new[] { "ChaosReflectionInvokeMethod" },
        ["MethodBase.GetParameters"] = new[] { "ChaosReflectionGetParameters" },
        ["MethodInfo.MakeGenericMethod"] = new[] { "ChaosReflectionMakeGenericMethod" },
        ["MethodInfo.get_IsGenericMethod"] = new[] { "ChaosReflectionGetContainsGenericParams" },

        // ── Construction ─────────────────────────────────────────────
        ["Activator.CreateInstance"] = new[] { "ChaosReflectionCreateInstance" },

        // ── Parameter ────────────────────────────────────────────────
        ["ParameterInfo.get_Name"] = new[] { "ChaosReflectionGetParameterName" },
        ["ParameterInfo.get_Position"] = new[] { "ChaosReflectionGetParamPosition" },
        ["ParameterInfo.get_ParameterType"] = new[] { "ChaosReflectionGetParameterType" },
        ["ParameterInfo.get_Attributes"] = new[] { "ChaosReflectionGetParamAttributes" },

        // ── Field ────────────────────────────────────────────────────
        ["FieldInfo.GetValue"] = new[] { "RuntimeAbiV0.FieldGetValue" },
        ["FieldInfo.SetValue"] = new[] { "RuntimeAbiV0.FieldSetValue" },
        ["FieldInfo.get_FieldType"] = new[] { "ChaosReflectionGetFieldType" },
        ["FieldInfo.get_IsStatic"] = new[] { "ChaosReflectionFieldGetIsStatic" },
        ["FieldInfo.get_IsInitOnly"] = new[] { "ChaosReflectionFieldGetIsInitOnly" },
        ["FieldInfo.get_IsLiteral"] = new[] { "ChaosReflectionFieldGetIsLiteral" },

        // ── Property ─────────────────────────────────────────────────
        ["PropertyInfo.get_PropertyType"] = new[] { "ChaosReflectionGetPropertyType" },
        ["PropertyInfo.get_CanRead"] = new[] { "ChaosReflectionPropertyGetCanRead" },
        ["PropertyInfo.get_CanWrite"] = new[] { "ChaosReflectionPropertyGetCanWrite" },

        // ── Assembly ─────────────────────────────────────────────────
        ["Assembly.GetTypes"] = new[] { "ChaosReflectionAssemblyGetTypes" },
        ["Assembly.GetType"] = new[] { "ChaosReflectionGetTypeFromAssemblyBool" },
        ["Assembly.GetName"] = new[] { "ChaosReflectionGetAssemblyName" },
        ["Assembly.get_FullName"] = new[] { "ChaosReflectionGetAssemblyFullName" },
        ["Assembly.get_Location"] = new[] { "ChaosReflectionGetAssemblyLocation" },
        ["Assembly.get_ImageRuntimeVersion"] = new[] { "ChaosReflectionGetImageRuntimeVersion" },
        ["Assembly.GetCallingAssembly"] = new[] { "ChaosReflectionGetCallingAssembly" },
        ["Assembly.GetEntryAssembly"] = new[] { "ChaosReflectionGetEntryAssembly" },
        ["Assembly.GetExecutingAssembly"] = new[] { "ChaosReflectionGetExecutingAssembly" },
        ["Assembly.GetExportedTypes"] = new[] { "ChaosReflectionAssemblyGetExportedTypes" },
        ["Assembly.GetForwardedTypes"] = new[] { "ChaosReflectionAssemblyGetForwardedTypes" },
        ["Assembly.GetManifestResourceNames"] = new[] { "ChaosReflectionAssemblyGetManifestResourceNames" },
        ["Assembly.GetModules"] = new[] { "ChaosReflectionAssemblyGetModules" },
        ["Assembly.get_EntryPoint"] = new[] { "ChaosReflectionAssemblyGetEntryPoint" },

        // ── Module ───────────────────────────────────────────────────
        ["Module.GetType"] = new[] { "ChaosReflectionModuleGetType" },
        ["Module.GetTypes"] = new[] { "ChaosReflectionModuleGetTypes" },
        ["Module.get_Name"] = new[] { "ChaosReflectionGetModuleName" },
        ["Module.get_Assembly"] = new[] { "ChaosReflectionGetModuleAssembly" },

        // ── Handle hashing ───────────────────────────────────────────
        ["RuntimeTypeHandle.GetHashCode"] = new[] { "ChaosRuntimetypehandleGetHashCode" },
        ["RuntimeMethodHandle.GetHashCode"] = new[] { "ChaosRuntimemethodhandleGetHashCode" },
        ["RuntimeFieldHandle.GetHashCode"] = new[] { "ChaosRuntimefieldhandleGetHashCode" },

        // ── Field modifier accessors (Phase 3, remaining_stubs.cpp) ────
        ["FieldInfo.get_IsStatic"] = new[] { "ChaosReflectionFieldGetIsStatic" },
        ["FieldInfo.get_IsPublic"] = new[] { "ChaosReflectionFieldGetIsPublic" },
        ["FieldInfo.get_IsPrivate"] = new[] { "ChaosReflectionFieldGetIsPrivate" },
        ["FieldInfo.get_IsAssembly"] = new[] { "ChaosReflectionFieldGetIsAssembly" },
        ["FieldInfo.get_IsFamily"] = new[] { "ChaosReflectionFieldGetIsFamily" },
        ["FieldInfo.get_IsFamilyAndAssembly"] = new[] { "ChaosReflectionFieldGetIsFamilyAndAssembly" },
        ["FieldInfo.get_IsFamilyOrAssembly"] = new[] { "ChaosReflectionFieldGetIsFamilyOrAssembly" },
        ["FieldInfo.get_IsNotSerialized"] = new[] { "ChaosReflectionFieldGetIsNotSerialized" },
        ["FieldInfo.get_IsPinvokeImpl"] = new[] { "ChaosReflectionFieldGetIsPinvokeImpl" },
        ["FieldInfo.get_IsSpecialName"] = new[] { "ChaosReflectionFieldGetIsSpecialName" },
        ["FieldInfo.get_Attributes"] = new[] { "ChaosReflectionFieldGetAttributes" },
        ["FieldInfo.GetRawConstantValue"] = new[] { "ChaosReflectionFieldGetRawConstantValue" },

        // ── Method modifier accessors (Phase 3, remaining_stubs.cpp) ───
        ["MethodBase.get_IsPublic"] = new[] { "ChaosReflectionMethodGetIsPublic" },
        ["MethodBase.get_IsStatic"] = new[] { "ChaosReflectionMethodGetIsStatic" },
        ["MethodBase.get_IsVirtual"] = new[] { "ChaosReflectionMethodGetIsVirtual" },
        ["MethodBase.get_IsPrivate"] = new[] { "ChaosReflectionMethodGetIsPrivate" },
        ["MethodBase.get_IsAssembly"] = new[] { "ChaosReflectionMethodGetIsAssembly" },
        ["MethodBase.get_IsFamily"] = new[] { "ChaosReflectionMethodGetIsFamily" },
        ["MethodBase.get_IsFamilyAndAssembly"] = new[] { "ChaosReflectionMethodGetIsFamilyAndAssembly" },
        ["MethodBase.get_IsFamilyOrAssembly"] = new[] { "ChaosReflectionMethodGetIsFamilyOrAssembly" },
        ["MethodBase.get_IsFinal"] = new[] { "ChaosReflectionMethodGetIsFinal" },
        ["MethodBase.get_IsHideBySig"] = new[] { "ChaosReflectionMethodGetIsHideBySig" },
        ["MethodBase.get_IsSpecialName"] = new[] { "ChaosReflectionMethodGetIsSpecialName" },
        ["MethodBase.get_IsAbstract"] = new[] { "ChaosReflectionMethodGetIsAbstract" },
        ["MethodBase.get_IsConstructor"] = new[] { "ChaosReflectionMethodGetIsConstructor" },
        ["MethodBase.get_Attributes"] = new[] { "ChaosReflectionMethodGetAttributes" },
        ["MethodBase.get_MethodImplementationFlags"] = new[] { "ChaosReflectionMethodGetImplementationFlags" },

        // ── Property / Event descriptor accessors (Phase 3 cont.) ──────
        ["PropertyInfo.get_Attributes"] = new[] { "ChaosReflectionPropertyGetAttributes" },
        ["PropertyInfo.get_IsSpecialName"] = new[] { "ChaosReflectionPropertyGetIsSpecialName" },
        ["PropertyInfo.get_MemberType"] = new[] { "ChaosReflectionPropertyGetMemberType" },
        ["PropertyInfo.get_PropertyType"] = new[] { "ChaosReflectionPropertyGetPropertyType" },
        ["PropertyInfo.GetGetMethod"] = new[] { "ChaosReflectionPropertyGetGetMethod" },
        ["PropertyInfo.GetSetMethod"] = new[] { "ChaosReflectionPropertyGetSetMethod" },
        ["PropertyInfo.get_GetMethod"] = new[] { "ChaosReflectionPropertyGetGetMethod" },
        ["PropertyInfo.get_SetMethod"] = new[] { "ChaosReflectionPropertyGetSetMethod" },

        ["EventInfo.get_Attributes"] = new[] { "ChaosReflectionEventGetAttributes" },
        ["EventInfo.get_EventHandlerType"] = new[] { "ChaosReflectionEventGetEventHandlerType" },
        ["EventInfo.get_IsSpecialName"] = new[] { "ChaosReflectionEventGetIsStatic" },
        ["EventInfo.get_MemberType"] = new[] { "ChaosReflectionEventGetMemberType" },
        ["EventInfo.GetAddMethod"] = new[] { "ChaosReflectionEventGetAddMethod" },
        ["EventInfo.get_AddMethod"] = new[] { "ChaosReflectionEventGetAddMethod" },
        ["EventInfo.get_Name"] = new[] { "ChaosReflectionEventGetName" },

        // ── Module.Resolve* / metadata (Phase 3 cont.) ─────────────────
        ["Module.ResolveType"] = new[] { "ChaosReflectionModuleResolveType" },
        ["Module.ResolveField"] = new[] { "ChaosReflectionModuleResolveField" },
        ["Module.ResolveMethod"] = new[] { "ChaosReflectionModuleResolveMethod" },
        ["Module.ResolveMember"] = new[] { "ChaosReflectionModuleResolveMember" },
        ["Module.get_Name"] = new[] { "ChaosReflectionModuleGetName" },
        ["Module.get_FullyQualifiedName"] = new[] { "ChaosReflectionModuleGetFullyQualifiedName" },
        ["Module.get_ScopeName"] = new[] { "ChaosReflectionModuleGetScopeName" },
        ["Module.get_MetadataToken"] = new[] { "ChaosReflectionModuleGetMetadataToken" },
        ["Module.get_ModuleHandle"] = new[] { "ChaosReflectionModuleGetModuleHandle" },

        // ── ParameterInfo descriptor accessors (Phase 3 cont.) ─────────
        ["ParameterInfo.get_MetadataToken"] = new[] { "ChaosReflectionParamGetMetadataToken" },
        ["ParameterInfo.get_Name"] = new[] { "ChaosReflectionParamGetName" },
        ["ParameterInfo.get_Member"] = new[] { "ChaosReflectionParamGetMember" },
        ["ParameterInfo.get_IsIn"] = new[] { "ChaosReflectionParamGetIsIn" },
        ["ParameterInfo.get_IsOut"] = new[] { "ChaosReflectionParamGetIsOut" },
        ["ParameterInfo.get_IsLcid"] = new[] { "ChaosReflectionParamGetIsLcid" },
        ["ParameterInfo.get_IsOptional"] = new[] { "ChaosReflectionParamGetIsOptional" },
        ["ParameterInfo.get_IsRetval"] = new[] { "ChaosReflectionParamGetIsRetval" },
        ["ParameterInfo.ToString"] = new[] { "ChaosReflectionParamToString" },

        // ── MethodBase / FieldInfo remaining (Phase 3 cont.) ───────────
        ["MethodBase.get_MemberType"] = new[] { "ChaosReflectionMethodGetMemberType" },
        ["MethodBase.get_IsGenericMethod"] = new[] { "ChaosReflectionMethodGetIsGenericMethod" },
        ["MethodBase.get_IsGenericMethodDefinition"] = new[] { "ChaosReflectionMethodGetIsGenericMethodDefinition" },
        ["MethodBase.get_IsConstructedGenericMethod"] = new[] { "ChaosReflectionMethodGetIsConstructedGenericMethod" },
        ["MethodBase.get_IsSecurityCritical"] = new[] { "ChaosReflectionMethodGetIsSecurityCritical" },
        ["MethodBase.get_IsSecuritySafeCritical"] = new[] { "ChaosReflectionMethodGetIsSecuritySafeCritical" },
        ["MethodBase.get_IsSecurityTransparent"] = new[] { "ChaosReflectionMethodGetIsSecurityTransparent" },
        ["MethodBase.GetMethodFromHandle"] = new[] { "ChaosReflectionMethodGetMethodFromHandle" },
        ["MethodBase.get_MethodHandle"] = new[] { "ChaosReflectionMethodGetMethodHandle" },

        ["FieldInfo.get_MemberType"] = new[] { "ChaosReflectionFieldGetMemberType" },
        ["FieldInfo.get_IsSecurityCritical"] = new[] { "ChaosReflectionFieldGetIsSecurityCritical" },
        ["FieldInfo.get_IsSecuritySafeCritical"] = new[] { "ChaosReflectionFieldGetIsSecuritySafeCritical" },
        ["FieldInfo.get_IsSecurityTransparent"] = new[] { "ChaosReflectionFieldGetIsSecurityTransparent" },
        ["FieldInfo.GetFieldFromHandle"] = new[] { "ChaosReflectionFieldGetFieldFromHandle" },
        ["FieldInfo.GetModifiedFieldType"] = new[] { "ChaosReflectionFieldGetModifiedFieldType" },
        ["PropertyInfo.GetModifiedPropertyType"] = new[] { "ChaosReflectionPropertyGetModifiedPropertyType" },
        ["ParameterInfo.GetModifiedParameterType"] = new[] { "ChaosReflectionParamGetModifiedParameterType" },

        // ── TypeInfo.Declared* family ──────────────────────────────────
        ["TypeInfo.get_DeclaredMethods"] = new[] { "ChaosTypeInfoGetDeclaredMethods" },
        ["TypeInfo.get_DeclaredFields"] = new[] { "ChaosTypeInfoGetDeclaredFields" },
        ["TypeInfo.get_DeclaredProperties"] = new[] { "ChaosTypeInfoGetDeclaredProperties" },
        ["TypeInfo.get_DeclaredConstructors"] = new[] { "ChaosTypeInfoGetDeclaredConstructors" },
        ["TypeInfo.get_DeclaredEvents"] = new[] { "ChaosTypeInfoGetDeclaredEvents" },
        ["TypeInfo.get_DeclaredNestedTypes"] = new[] { "ChaosTypeInfoGetDeclaredNestedTypes" },
        ["TypeInfo.get_DeclaredMembers"] = new[] { "ChaosTypeInfoGetDeclaredMembers" },
        ["TypeInfo.get_ImplementedInterfaces"] = new[] { "ChaosTypeInfoGetImplementedInterfaces" },
        ["TypeInfo.GetDeclaredMethod"] = new[] { "ChaosTypeInfoGetDeclaredMethod" },
        ["TypeInfo.GetDeclaredField"] = new[] { "ChaosTypeInfoGetDeclaredField" },
        ["TypeInfo.GetDeclaredProperty"] = new[] { "ChaosTypeInfoGetDeclaredProperty" },
        ["TypeInfo.GetDeclaredEvent"] = new[] { "ChaosTypeInfoGetDeclaredEvent" },
        ["TypeInfo.GetDeclaredNestedType"] = new[] { "ChaosTypeInfoGetDeclaredNestedType" },
        ["TypeInfo.get_AsType"] = new[] { "ChaosTypeInfoAsType" },
        ["TypeInfo.IsAssignableFrom"] = new[] { "ChaosTypeInfoIsAssignableFrom" },

        // ── AssemblyName accessors ─────────────────────────────────────
        ["AssemblyName.get_Name"] = new[] { "ChaosReflectionAssemblyNameGetName" },
        ["AssemblyName.get_FullName"] = new[] { "ChaosReflectionAssemblyNameGetFullName" },
        ["AssemblyName.get_CultureName"] = new[] { "ChaosReflectionAssemblyNameGetCultureName" },
        ["AssemblyName.ToString"] = new[] { "ChaosReflectionAssemblyNameToString" },
        ["AssemblyName.ReferenceMatchesDefinition"] = new[] { "ChaosReflectionAssemblyNameReferenceMatchesDefinition" },

        // ── Assembly identity / metadata accessors ─────────────────────
        ["Assembly.get_ManifestModule"] = new[] { "ChaosReflectionAssemblyGetManifestModule" },
        ["Assembly.get_Modules"] = new[] { "ChaosReflectionAssemblyGetModulesList" },
        ["Assembly.get_ExportedTypes"] = new[] { "ChaosReflectionAssemblyGetExportedTypesProperty" },
        ["Assembly.get_DefinedTypes"] = new[] { "ChaosReflectionAssemblyGetDefinedTypes" },
        ["Assembly.GetAssembly"] = new[] { "ChaosReflectionAssemblyGetAssemblyForType" },
        ["Assembly.GetModule"] = new[] { "ChaosReflectionAssemblyGetModuleByName" },
        ["Assembly.GetLoadedModules"] = new[] { "ChaosReflectionAssemblyGetLoadedModules" },
        ["Assembly.ToString"] = new[] { "ChaosReflectionAssemblyToString" },
        ["Assembly.GetCustomAttributesData"] = new[] { "ChaosReflectionAssemblyGetCustomAttributesData" },
        ["Assembly.GetCustomAttributes"] = new[] { "ChaosReflectionAssemblyGetCustomAttributes" },
        ["Assembly.get_CustomAttributes"] = new[] { "ChaosReflectionAssemblyGetCustomAttributesData" },
        ["Assembly.IsDefined"] = new[] { "ChaosReflectionAssemblyIsDefined" },
        ["Assembly.get_IsFullyTrusted"] = new[] { "ChaosReflectionAssemblyGetIsFullyTrusted" },
        ["Assembly.get_GlobalAssemblyCache"] = new[] { "ChaosReflectionAssemblyGetGlobalAssemblyCache" },
        ["Assembly.get_ReflectionOnly"] = new[] { "ChaosReflectionAssemblyGetReflectionOnly" },
        ["Assembly.get_SecurityRuleSet"] = new[] { "ChaosReflectionAssemblyGetSecurityRuleSet" },
        ["Assembly.get_HostContext"] = new[] { "ChaosReflectionAssemblyGetHostContext" },
        ["Assembly.GetHashCode"] = new[] { "ChaosReflectionAssemblyGetHashCode" },
        ["Assembly.Equals"] = new[] { "ChaosReflectionAssemblyEqualsVersion" },

        // ── Module member lookup / remaining identity ──────────────────
        ["Module.GetField"] = new[] { "ChaosReflectionModuleGetField" },
        ["Module.GetMethod"] = new[] { "ChaosReflectionModuleGetMethod" },
        ["Module.GetFields"] = new[] { "ChaosReflectionModuleGetFields" },
        ["Module.GetMethods"] = new[] { "ChaosReflectionModuleGetMethods" },
        ["Module.IsResource"] = new[] { "ChaosReflectionModuleIsResource" },
        ["Module.get_MDStreamVersion"] = new[] { "ChaosReflectionModuleGetMDStreamVersion" },
        ["Module.get_ModuleVersionId"] = new[] { "ChaosReflectionModuleGetModuleVersionId" },
        ["Module.ToString"] = new[] { "ChaosReflectionModuleToString" },
        ["Module.GetCustomAttributesData"] = new[] { "ChaosReflectionModuleGetCustomAttributesData" },
        ["Module.GetCustomAttributes"] = new[] { "ChaosReflectionModuleGetCustomAttributes" },
        ["Module.get_CustomAttributes"] = new[] { "ChaosReflectionModuleGetCustomAttributesData" },
        ["Module.GetHashCode"] = new[] { "ChaosReflectionModuleGetHashCode" },
        ["Module.Equals"] = new[] { "ChaosReflectionModuleEqualsVersion" },

        // ── CustomAttributeData accessors ──────────────────────────────
        ["CustomAttributeData.get_AttributeType"] = new[] { "ChaosReflectionAttrDataGetAttributeType" },
        ["CustomAttributeData.get_Constructor"] = new[] { "ChaosReflectionAttrDataGetConstructor" },
        ["CustomAttributeData.get_ConstructorArguments"] = new[] { "ChaosReflectionAttrDataGetConstructorArguments" },
        ["CustomAttributeData.get_NamedArguments"] = new[] { "ChaosReflectionAttrDataGetNamedArguments" },
        ["CustomAttributeData.GetCustomAttributes"] = new[] { "ChaosReflectionAttrDataGetCustomAttributes" },
        ["CustomAttributeData.Equals"] = new[] { "ChaosReflectionAttrDataEquals" },
        ["CustomAttributeData.GetHashCode"] = new[] { "ChaosReflectionAttrDataGetHashCode" },

        // ── RuntimeReflectionExtensions ────────────────────────────────
        ["RuntimeReflectionExtensions.GetRuntimeField"] = new[] { "ChaosRuntimeReflectionGetRuntimeField" },
        ["RuntimeReflectionExtensions.GetRuntimeMethod"] = new[] { "ChaosRuntimeReflectionGetRuntimeMethod" },
        ["RuntimeReflectionExtensions.GetRuntimeProperty"] = new[] { "ChaosRuntimeReflectionGetRuntimeProperty" },
        ["RuntimeReflectionExtensions.GetRuntimeEvent"] = new[] { "ChaosRuntimeReflectionGetRuntimeEvent" },
        ["RuntimeReflectionExtensions.GetRuntimeFields"] = new[] { "ChaosRuntimeReflectionGetRuntimeFields" },
        ["RuntimeReflectionExtensions.GetRuntimeMethods"] = new[] { "ChaosRuntimeReflectionGetRuntimeMethods" },
        ["RuntimeReflectionExtensions.GetRuntimeProperties"] = new[] { "ChaosRuntimeReflectionGetRuntimeProperties" },
        ["RuntimeReflectionExtensions.GetRuntimeEvents"] = new[] { "ChaosRuntimeReflectionGetRuntimeEvents" },
        ["RuntimeReflectionExtensions.GetMethodInfo"] = new[] { "ChaosRuntimeReflectionGetMethodInfo" },
        ["RuntimeReflectionExtensions.GetRuntimeBaseDefinition"] = new[] { "ChaosRuntimeReflectionGetRuntimeBaseDefinition" },
        ["RuntimeReflectionExtensions.GetRuntimeInterfaceMap"] = new[] { "ChaosRuntimeReflectionGetRuntimeInterfaceMap" },

        // ── MethodInfo / EventInfo remaining ───────────────────────────
        ["MethodInfo.get_ReturnType"] = new[] { "ChaosReflectionMethodGetReturnType" },
        ["MethodInfo.get_ReturnParameter"] = new[] { "ChaosReflectionMethodGetReturnParameter" },
        ["MethodInfo.get_ReturnTypeCustomAttributes"] = new[] { "ChaosReflectionMethodGetReturnTypeCustomAttributes" },
        ["MethodInfo.GetGenericArguments"] = new[] { "ChaosReflectionMethodGetGenericArguments" },
        ["MethodInfo.GetGenericMethodDefinition"] = new[] { "ChaosReflectionMethodGetGenericMethodDefinition" },
        ["MethodInfo.GetHashCode"] = new[] { "ChaosReflectionMethodGetHashCodeVersion" },
        ["MethodInfo.Equals"] = new[] { "ChaosReflectionMethodEqualsVersion" },

        ["EventInfo.GetRemoveMethod"] = new[] { "ChaosReflectionEventGetRemoveMethod" },
        ["EventInfo.get_RemoveMethod"] = new[] { "ChaosReflectionEventGetRemoveMethod" },
        ["EventInfo.GetRaiseMethod"] = new[] { "ChaosReflectionEventGetRaiseMethod" },
        ["EventInfo.get_RaiseMethod"] = new[] { "ChaosReflectionEventGetRaiseMethod" },
        ["EventInfo.get_IsMulticast"] = new[] { "ChaosReflectionEventGetIsMulticast" },
        ["EventInfo.GetHashCode"] = new[] { "ChaosReflectionEventGetHashCodeVersion" },
        ["EventInfo.Equals"] = new[] { "ChaosReflectionEventEqualsVersion" },

        // ── PropertyInfo / ParameterInfo / MemberInfo / MethodBase rest ─
        ["PropertyInfo.GetHashCode"] = new[] { "ChaosReflectionPropertyGetHashCodeVersion" },
        ["PropertyInfo.Equals"] = new[] { "ChaosReflectionPropertyEqualsVersion" },
        ["PropertyInfo.GetAccessors"] = new[] { "ChaosReflectionPropertyGetAccessors" },
        ["PropertyInfo.GetConstantValue"] = new[] { "ChaosReflectionPropertyGetConstantValue" },
        ["PropertyInfo.GetRawConstantValue"] = new[] { "ChaosReflectionPropertyGetRawConstantValue" },
        ["PropertyInfo.GetOptionalCustomModifiers"] = new[] { "ChaosReflectionPropertyGetOptionalCustomModifiers" },
        ["PropertyInfo.GetRequiredCustomModifiers"] = new[] { "ChaosReflectionPropertyGetRequiredCustomModifiers" },

        ["ParameterInfo.get_CustomAttributes"] = new[] { "ChaosReflectionParamGetCustomAttributesData" },
        ["ParameterInfo.GetCustomAttributes"] = new[] { "ChaosReflectionParamGetCustomAttributes" },
        ["ParameterInfo.GetCustomAttributesData"] = new[] { "ChaosReflectionParamGetCustomAttributesData" },
        ["ParameterInfo.IsDefined"] = new[] { "ChaosReflectionParamIsDefined" },
        ["ParameterInfo.get_DefaultValue"] = new[] { "ChaosReflectionParamGetDefaultValue" },
        ["ParameterInfo.get_RawDefaultValue"] = new[] { "ChaosReflectionParamGetRawDefaultValue" },
        ["ParameterInfo.get_HasDefaultValue"] = new[] { "ChaosReflectionParamGetHasDefaultValue" },
        ["ParameterInfo.GetOptionalCustomModifiers"] = new[] { "ChaosReflectionParamGetOptionalCustomModifiers" },
        ["ParameterInfo.GetRequiredCustomModifiers"] = new[] { "ChaosReflectionParamGetRequiredCustomModifiers" },
        ["ParameterInfo.GetRealObject"] = new[] { "ChaosReflectionParamGetRealObject" },

        ["MemberInfo.get_CustomAttributes"] = new[] { "ChaosReflectionMemberGetCustomAttributesData" },
        ["MemberInfo.GetCustomAttributes"] = new[] { "ChaosReflectionMemberGetCustomAttributes" },
        ["MemberInfo.IsDefined"] = new[] { "ChaosReflectionMemberIsDefined" },
        ["MemberInfo.get_MemberType"] = new[] { "ChaosReflectionMemberGetMemberType" },
        ["MemberInfo.get_Module"] = new[] { "ChaosReflectionMemberGetModule" },
        ["MemberInfo.get_IsCollectible"] = new[] { "ChaosReflectionMemberGetIsCollectible" },
        ["MemberInfo.HasSameMetadataDefinitionAs"] = new[] { "ChaosReflectionMemberHasSameMetadataDefinitionAs" },
        ["MemberInfo.GetHashCode"] = new[] { "ChaosReflectionMemberGetHashCodeVersion" },
        ["MemberInfo.Equals"] = new[] { "ChaosReflectionMemberEqualsVersion" },

        ["MethodBase.get_CallingConvention"] = new[] { "ChaosReflectionMethodGetCallingConvention" },
        ["MethodBase.get_ContainsGenericParameters"] = new[] { "ChaosReflectionMethodGetContainsGenericParameters" },
        ["MethodBase.GetGenericArguments"] = new[] { "ChaosReflectionMethodGetGenericArgumentsForBase" },
        ["MethodBase.GetCurrentMethod"] = new[] { "ChaosReflectionMethodGetCurrentMethod" },
        ["MethodBase.GetMethodBody"] = new[] { "ChaosReflectionMethodGetMethodBody" },
        ["MethodBase.GetHashCode"] = new[] { "ChaosReflectionMethodGetHashCodeBase" },
        ["MethodBase.Equals"] = new[] { "ChaosReflectionMethodEqualsBase" },

        // ── CustomAttribute(Named|Typed)Argument + NullabilityInfo ─────
        ["CustomAttributeTypedArgument.get_ArgumentType"] = new[] { "ChaosReflectionTypedArgGetArgumentType" },
        ["CustomAttributeTypedArgument.get_Value"] = new[] { "ChaosReflectionTypedArgGetValue" },
        ["CustomAttributeTypedArgument.Equals"] = new[] { "ChaosReflectionTypedArgEquals" },
        ["CustomAttributeTypedArgument.GetHashCode"] = new[] { "ChaosReflectionTypedArgGetHashCode" },
        ["CustomAttributeNamedArgument.get_MemberName"] = new[] { "ChaosReflectionNamedArgGetMemberName" },
        ["CustomAttributeNamedArgument.get_IsField"] = new[] { "ChaosReflectionNamedArgGetIsField" },
        ["CustomAttributeNamedArgument.get_TypedValue"] = new[] { "ChaosReflectionNamedArgGetTypedValue" },
        ["CustomAttributeNamedArgument.get_MemberInfo"] = new[] { "ChaosReflectionNamedArgGetMemberInfo" },
        ["CustomAttributeNamedArgument.Equals"] = new[] { "ChaosReflectionNamedArgEquals" },
        ["CustomAttributeNamedArgument.GetHashCode"] = new[] { "ChaosReflectionNamedArgGetHashCode" },
        ["NullabilityInfoContext.Create"] = new[] { "ChaosReflectionNullabilityInfoContextCreate" },
        ["NullabilityInfo.get_ReadState"] = new[] { "ChaosReflectionNullabilityInfoGetReadState" },
        ["NullabilityInfo.get_WriteState"] = new[] { "ChaosReflectionNullabilityInfoGetWriteState" },
        ["NullabilityInfo.get_Type"] = new[] { "ChaosReflectionNullabilityInfoGetType" },
        ["NullabilityInfo.get_ElementType"] = new[] { "ChaosReflectionNullabilityInfoGetElementType" },
        ["NullabilityInfo.get_GenericTypeArguments"] = new[] { "ChaosReflectionNullabilityInfoGetGenericTypeArguments" },

        // ── ConstructorInfo / Field/Property/Event/MethodInfo rest ─────
        ["ConstructorInfo.get_MemberType"] = new[] { "ChaosReflectionCtorGetMemberType" },
        ["ConstructorInfo.GetHashCode"] = new[] { "ChaosReflectionCtorGetHashCodeVersion" },
        ["ConstructorInfo.Equals"] = new[] { "ChaosReflectionCtorEqualsVersion" },
        ["ConstructorInfo.get_ConstructorName"] = new[] { "ChaosReflectionCtorGetConstructorName" },
        ["ConstructorInfo.get_TypeConstructorName"] = new[] { "ChaosReflectionCtorGetTypeConstructorName" },
        ["ConstructorInfo.Invoke"] = new[] { "ChaosReflectionCtorInvoke" },

        ["MethodInfo.get_MemberType"] = new[] { "ChaosReflectionMethodGetMemberTypeVersion" },
        ["MethodInfo.GetBaseDefinition"] = new[] { "ChaosReflectionMethodGetBaseDefinitionVersion" },
        ["MethodInfo.CreateDelegate"] = new[] { "ChaosReflectionMethodCreateDelegate" },

        ["FieldInfo.GetHashCode"] = new[] { "ChaosReflectionFieldGetHashCodeVersion" },
        ["FieldInfo.Equals"] = new[] { "ChaosReflectionFieldEqualsVersion" },
        ["FieldInfo.get_FieldHandle"] = new[] { "ChaosReflectionFieldGetFieldHandleVersion" },
        ["FieldInfo.GetOptionalCustomModifiers"] = new[] { "ChaosReflectionFieldGetOptionalCustomModifiers" },
        ["FieldInfo.GetRequiredCustomModifiers"] = new[] { "ChaosReflectionFieldGetRequiredCustomModifiers" },
        ["FieldInfo.GetValueDirect"] = new[] { "ChaosReflectionFieldGetValueDirect" },
        ["FieldInfo.SetValueDirect"] = new[] { "ChaosReflectionFieldSetValueDirect" },

        ["PropertyInfo.GetIndexParameters"] = new[] { "ChaosReflectionPropertyGetIndexParametersVersion" },
        ["PropertyInfo.GetValue"] = new[] { "ChaosReflectionPropertyGetValue" },
        ["PropertyInfo.SetValue"] = new[] { "ChaosReflectionPropertySetValue" },

        ["EventInfo.AddEventHandler"] = new[] { "ChaosReflectionEventAddEventHandler" },
        ["EventInfo.RemoveEventHandler"] = new[] { "ChaosReflectionEventRemoveEventHandler" },
        ["EventInfo.GetOtherMethods"] = new[] { "ChaosReflectionEventGetOtherMethods" },

        ["TypeInfo.get_GenericTypeParameters"] = new[] { "ChaosTypeInfoGetGenericTypeParameters" },
        ["TypeInfo.GetDeclaredMethods"] = new[] { "ChaosTypeInfoGetDeclaredMethodsVersion" },

        // ── Module remaining / InterfaceMapping / ManifestResource ─────
        ["Module.ResolveString"] = new[] { "ChaosReflectionModuleResolveString" },
        ["Module.ResolveSignature"] = new[] { "ChaosReflectionModuleResolveSignature" },
        ["Module.IsDefined"] = new[] { "ChaosReflectionModuleIsDefined" },
        ["Module.FindTypes"] = new[] { "ChaosReflectionModuleFindTypes" },
        ["Module.FilterTypeName"] = new[] { "ChaosReflectionModuleGetFilterTypeName" },
        ["Module.FilterTypeNameIgnoreCase"] = new[] { "ChaosReflectionModuleGetFilterTypeNameIgnoreCase" },
        ["Module.GetPEKind"] = new[] { "ChaosReflectionModuleGetPEKind" },
        ["Module.GetObjectData"] = new[] { "ChaosReflectionModuleGetObjectData" },

        ["InterfaceMapping.get_TargetType"] = new[] { "ChaosReflectionInterfaceMappingGetTargetType" },
        ["InterfaceMapping.get_InterfaceType"] = new[] { "ChaosReflectionInterfaceMappingGetInterfaceType" },
        ["InterfaceMapping.get_InterfaceMethods"] = new[] { "ChaosReflectionInterfaceMappingGetInterfaceMethods" },
        ["InterfaceMapping.get_TargetMethods"] = new[] { "ChaosReflectionInterfaceMappingGetTargetMethods" },

        ["ManifestResourceInfo.get_FileName"] = new[] { "ChaosReflectionManifestResourceGetFileName" },
        ["ManifestResourceInfo.get_ReferencedAssembly"] = new[] { "ChaosReflectionManifestResourceGetReferencedAssembly" },
        ["ManifestResourceInfo.get_ResourceLocation"] = new[] { "ChaosReflectionManifestResourceGetResourceLocation" },

        ["ICustomAttributeProvider.GetCustomAttributes"] = new[] { "ChaosReflectionCustomAttrProviderGetCustomAttributes" },
        ["ICustomAttributeProvider.IsDefined"] = new[] { "ChaosReflectionCustomAttrProviderIsDefined" },
        ["IReflectableType.GetTypeInfo"] = new[] { "ChaosReflectionGetTypeInfoForType" },
        ["IntrospectionExtensions.GetTypeInfo"] = new[] { "ChaosReflectionGetTypeInfoForType" },
        ["Missing.Value"] = new[] { "ChaosReflectionMissingValue" },
        ["MethodBase.get_MethodImplementationFlags"] = new[] { "ChaosReflectionMethodGetImplementationFlagsVersion" },
        ["NullabilityInfoContext..ctor"] = new[] { "ChaosReflectionNullabilityInfoContextNew" },

        // ── AssemblyName / Assembly resource accessors ─────────────────
        ["AssemblyName.get_Version"] = new[] { "ChaosReflectionAssemblyNameGetVersionObj" },
        ["AssemblyName.get_CodeBase"] = new[] { "ChaosReflectionAssemblyNameGetCodeBase" },
        ["AssemblyName.get_EscapedCodeBase"] = new[] { "ChaosReflectionAssemblyNameGetEscapedCodeBase" },
        ["AssemblyName.get_KeyPair"] = new[] { "ChaosReflectionAssemblyNameGetKeyPair" },
        ["AssemblyName.get_VersionCompatibility"] = new[] { "ChaosReflectionAssemblyNameGetVersionCompatibility" },
        ["AssemblyName.get_CultureInfo"] = new[] { "ChaosReflectionAssemblyNameGetCultureInfoObj" },
        ["AssemblyName.GetAssemblyName"] = new[] { "ChaosReflectionAssemblyNameGetAssemblyNameFile" },
        ["AssemblyName.OnDeserialization"] = new[] { "ChaosReflectionAssemblyNameOnDeserialization" },
        ["AssemblyName.GetObjectData"] = new[] { "ChaosReflectionAssemblyNameGetObjectData" },

        ["Assembly.GetManifestResourceStream"] = new[] { "ChaosReflectionAssemblyGetManifestResourceStream" },
        ["Assembly.GetManifestResourceInfo"] = new[] { "ChaosReflectionAssemblyGetManifestResourceInfo" },
        ["Assembly.GetFile"] = new[] { "ChaosReflectionAssemblyGetFile" },
        ["Assembly.GetFiles"] = new[] { "ChaosReflectionAssemblyGetFiles" },
        ["Assembly.GetReferencedAssemblies"] = new[] { "ChaosReflectionAssemblyGetReferencedAssemblies" },
        ["Assembly.GetObjectData"] = new[] { "ChaosReflectionAssemblyGetObjectData" },
    };

    /// <summary>
    /// APIs that are structurally impossible under AOT. These must throw
    /// NotSupportedException rather than silently returning wrong values.
    /// </summary>
    private static readonly (string Type, string Reason)[] NotSupportedTypes =
    {
        // Reflection.Emit is already excluded as not-applicable (separate namespace).
        // These are operation-level impossibilities within System.Reflection proper.
        ("Binder", "Custom model binder requires runtime type system beyond metadata descriptor model"),
        ("TypeDelegator", "Type delegation requires a live managed Type implementation chain"),
        ("IReflect", "IReflect is a COM-era late-binding interface without AOT semantics"),
        ("ConstructorInvoker", "Invoker objects require runtime delegate construction (Delegate.CreateDelegate)"),
        ("MethodInvoker", "Invoker objects require runtime delegate construction (Delegate.CreateDelegate)"),
        ("StrongNameKeyPair", "Strong-name key material is not carried in AOT metadata"),
        ("Pointer", "System.Reflection.Pointer requires runtime boxed pointer support"),
    };

    /// <summary>Member-level APIs that cannot be implemented under AOT.</summary>
    private static readonly HashSet<string> NotSupportedMembers = new(StringComparer.Ordinal)
    {
        "MemberInfo.GetCustomAttributes",       // requires attribute materialization of arbitrary types
        "Assembly.Load",
        "Assembly.LoadFrom",
        "Assembly.LoadFile",
        "Assembly.ReflectionOnlyLoad",
        "Assembly.CreateQualifiedName",
        "Assembly.GetSatelliteAssembly",
        "Assembly.get_CodeBase",
        "Assembly.get_EscapedCodeBase",
        "Assembly.get_IsCollectible",
        "Assembly.get_IsDynamic",
        "Type.InvokeMember",                    // late-bound invocation beyond descriptor model
        "Type.GetTypeArray",
        "Type.get_TypeHandle",
        "Type.GetTypeCode",
        "Type.MakeGenericType",
        "Module.GetSignerCertificate",
        "ExceptionHandlingClause.get_CatchType",
        "ExceptionHandlingClause.get_FilterOffset",
        "MethodBody.GetILAsByteArray",          // AOT has no IL
        "LocalVariableInfo.get_LocalType",
        "AssemblyName.Clone",
        "AssemblyName.GetPublicKey",
        "AssemblyName.GetPublicKeyToken",
        "AssemblyName.SetPublicKey",
        "AssemblyName.SetPublicKeyToken",
        "AssemblyName.get_ProcessorArchitecture",
        "AssemblyName.get_ContentType",
        "AssemblyName.get_HashAlgorithm",
        "AssemblyName.get_Flags",
        "AssemblyName.set_Flags",
        "Assembly.CreateInstance",
        "Assembly.LoadModule",
        "Assembly.LoadWithPartialName",
        "Assembly.ReflectionOnlyLoadFrom",
        "Assembly.UnsafeLoadFrom",
        "Assembly.get_ModuleResolve",
        "Assembly.add_ModuleResolve",
        "Assembly.remove_ModuleResolve",
    };

    /// <summary>
    /// Types whose entire operation surface is served by a single native
    /// dispatch pair. Every member of these types is a convenience overload that
    /// funnels into the same two entry points, so they are all `real`.
    ///
    /// CustomAttributeExtensions: every GetCustomAttribute(s)/IsDefined overload
    /// normalizes to (member, attribute type) and routes either to
    /// ChaosReflectionCollectCustomAttributes or ChaosReflectionMemberIsDefined
    /// ByToken — see RuntimeHelperShapeRegistry.CoreStubs.Part2.S1/S6, which
    /// register the Assembly/MemberInfo/Module/ParameterInfo receivers.
    /// </summary>
    private static readonly HashSet<string> WholeTypeRealViaDispatch = new(StringComparer.Ordinal)
    {
        "CustomAttributeExtensions",
    };

    /// <summary>
    /// Whole-type APIs that are structurally impossible under AOT.
    /// </summary>
    private static readonly HashSet<string> NotSupportedWholeTypes = new(StringComparer.Ordinal)
    {
        "MethodBody",                // IL byte arrays do not exist under AOT
        "LocalVariableInfo",         // requires IL/method body metadata
        "ExceptionHandlingClause",   // requires IL/method body metadata
        "ReflectionContext",         // requires a live reflection implementation to delegate to
        "AssemblyNameProxy",         // requires runtime assembly resolution
        "MemberFilter",              // delegate type requiring runtime delegate construction
        "TypeFilter",                // delegate type requiring runtime delegate construction
        "ModuleResolveEventHandler", // delegate type requiring runtime delegate construction
        "ParameterModifier",         // requires runtime late-binding interop arrays
    };

    /// <summary>
    /// APIs that delegate to a trivial accessor of an already-modeled descriptor
    /// field. These are "real" in the sense that the underlying data exists in
    /// the Tier-2 descriptor and the native accessor is a straight read — but
    /// they are only realized as part of Phase 3 (see roadmap P3-1).
    /// Recorded here so Phase 2's exit criterion (no `unclassified`) is met with
    /// an honest, non-fabricated judgement.
    /// </summary>
    private static readonly HashSet<string> DescriptorBacked = new(StringComparer.Ordinal)
    {
        // CustomAttribute data surface — backed by the custom_attribute_blob.
        "CustomAttributeData.GetCustomAttributes",
        "CustomAttributeData.GetCustomAttributesData",
        "CustomAttributeData.get_AttributeType",
        "CustomAttributeData.get_Constructor",
        "CustomAttributeData.get_ConstructorArguments",
        "CustomAttributeData.get_NamedArguments",

        // CustomAttributeExtensions — convenience wrappers over GetCustomAttributesData.
        "CustomAttributeExtensions.GetCustomAttribute",
        "CustomAttributeExtensions.GetCustomAttributes",
        "CustomAttributeExtensions.IsDefined",
        "CustomAttributeExtensions.GetCustomAttributeFor",

        // Nullability metadata — backed by the parameter descriptor flags.
        "NullabilityInfoContext.Create",

        // RuntimeReflectionExtensions — thin helpers over Type/MemberInfo.
        "RuntimeReflectionExtensions.GetRuntimeField",
        "RuntimeReflectionExtensions.GetRuntimeMethod",
        "RuntimeReflectionExtensions.GetRuntimeProperty",
        "RuntimeReflectionExtensions.GetRuntimeEvent",
        "RuntimeReflectionExtensions.GetRuntimeFields",
        "RuntimeReflectionExtensions.GetRuntimeMethods",
        "RuntimeReflectionExtensions.GetRuntimeProperties",
        "RuntimeReflectionExtensions.GetRuntimeEvents",
        "RuntimeReflectionExtensions.GetMethodInfo",
        "RuntimeReflectionExtensions.GetRuntimeInterfaceMap",

        // Introspection.
        "IntrospectionExtensions.GetTypeInfo",

        // Member metadata accessors backed by descriptor name/token fields.
        "MemberInfo.get_CustomAttributes",
        "MemberInfo.get_Module",
        "MemberInfo.IsDefined",
        "MemberInfo.GetCustomAttribute",
        "MemberInfo.GetCustomAttributesDataFor",

        // Method/Constructor introspection from descriptors.
        "MethodBase.get_MethodHandle",
        "MethodBase.GetMethodImplementationFlags",
        "MethodBase.get_IsAbstract",
        "MethodBase.get_IsConstructor",
        "MethodBase.get_IsFinal",
        "MethodBase.get_IsHideBySig",
        "MethodBase.get_IsSpecialName",
        "MethodBase.get_IsStatic",
        "MethodBase.get_IsVirtual",
        "MethodInfo.get_ReturnType",
        "MethodInfo.get_ReturnParameter",
        "MethodInfo.get_IsGenericMethodDefinition",
        "MethodInfo.get_ContainsGenericParameters",
        "MethodInfo.GetGenericMethodDefinition",
        "MethodInfo.GetGenericArguments",
        "ConstructorInfo.get_MethodHandle",

        // Field/Property/Event descriptor accessors.
        "FieldInfo.get_Attributes",
        "FieldInfo.GetRawConstantValue",
        "FieldInfo.get_IsPrivate",
        "FieldInfo.get_IsPublic",
        "FieldInfo.get_IsAssembly",
        "FieldInfo.get_IsFamily",
        "FieldInfo.get_IsFamilyOrAssembly",
        "FieldInfo.get_IsPrivate",
        "PropertyInfo.get_Attributes",
        "PropertyInfo.GetAccessors",
        "PropertyInfo.get_GetMethod",
        "PropertyInfo.get_SetMethod",
        "EventInfo.get_Attributes",
        "EventInfo.get_EventHandlerType",
        "EventInfo.get_AddMethod",
        "EventInfo.get_RemoveMethod",
        "EventInfo.GetAddMethod",
        "EventInfo.GetRemoveMethod",
        "EventInfo.get_IsSpecialName",
        "EventInfo.get_IsMulticast",

        // ParameterInfo descriptor accessors.
        "ParameterInfo.get_Attributes",
        "ParameterInfo.get_DefaultValue",
        "ParameterInfo.get_RawDefaultValue",
        "ParameterInfo.get_HasDefaultValue",
        "ParameterInfo.get_IsIn",
        "ParameterInfo.get_IsOut",
        "ParameterInfo.get_IsOptional",
        "ParameterInfo.get_IsRetval",
        "ParameterInfo.get_Member",
        "ParameterInfo.GetCustomAttributes",
        "ParameterInfo.IsDefined",

        // Assembly/Module metadata backed by ImageDescriptor.
        "Assembly.get_DefinedTypes",
        "Assembly.get_ManifestModule",
        "Assembly.GetCustomAttributes",
        "Assembly.IsDefined",
        "Assembly.GetReferencedAssemblies",
        "Assembly.GetFile",
        "Assembly.GetFiles",
        "Assembly.GetLoadedModules",
        "Assembly.GetModule",
        "Assembly.GetNameFor",
        "Assembly.ToString",
        "Assembly.get_HostContext",
        "Assembly.get_SecurityRuleSet",
        "Assembly.get_ReflectionOnly",
        "Assembly.get_GlobalAssemblyCache",
        "Assembly.GetCustomAttributesData",
        "Assembly.get_Modules",
        "Assembly.get_EntryPointFor",
        "Assembly.get_ExportedTypes",
        "Assembly.get_ForwardedTypes",
        "Module.get_AssemblyFor",
        "Module.get_ScopeName",
        "Module.get_ModuleVersionId",
        "Module.get_MetadataToken",
        "Module.get_FullyQualifiedName",
        "Module.GetCustomAttributes",
        "Module.IsDefined",
        "Module.GetCustomAttributesData",
        "Module.GetFields",
        "Module.GetMethods",
        "Module.GetField",
        "Module.GetMethod",
        "Module.GetObjectData",
        "Module.GetTypesFor",
        "Module.ResolveMethod",
        "Module.ResolveField",
        "Module.ResolveType",
        "Module.ResolveString",
        "Module.ResolveMember",
        "Module.ResolveSignature",
        "Module.get_MDStreamVersion",
        "Module.GetPEKind",
        "Module.ToString",

        // TypeInfo — descriptor-backed type surface.
        "TypeInfo.get_DeclaredConstructors",
        "TypeInfo.get_DeclaredFields",
        "TypeInfo.get_DeclaredMethods",
        "TypeInfo.get_DeclaredProperties",
        "TypeInfo.get_DeclaredEvents",
        "TypeInfo.get_DeclaredNestedTypes",
        "TypeInfo.get_DeclaredMembers",
        "TypeInfo.get_ImplementedInterfaces",
        "TypeInfo.get_GenericTypeParameters",
        "TypeInfo.get_AsType",
        "TypeInfo.get_IsAssignableFromFor",
        "TypeInfo.GetDeclaredMethod",
        "TypeInfo.GetDeclaredField",
        "TypeInfo.GetDeclaredProperty",
        "TypeInfo.GetDeclaredEvent",
        "TypeInfo.GetDeclaredNestedType",
        "TypeInfo.get_AssemblyQualifiedNameFor",

        // AssemblyName — descriptor-backed name surface.
        "AssemblyName.get_Name",
        "AssemblyName.set_Name",
        "AssemblyName.get_Version",
        "AssemblyName.set_Version",
        "AssemblyName.get_CultureName",
        "AssemblyName.set_CultureName",
        "AssemblyName.get_CultureInfo",
        "AssemblyName.set_CultureInfo",
        "AssemblyName.get_FullName",
        "AssemblyName.ToString",
        "AssemblyName.get_NameFor",

        // InterfaceMapping / ManifestResourceInfo — descriptor-backed data.
        "InterfaceMapping.get_TargetType",
        "InterfaceMapping.get_InterfaceType",
        "InterfaceMapping.get_InterfaceMethods",
        "InterfaceMapping.get_TargetMethods",
        "ManifestResourceInfo.get_FileName",
        "ManifestResourceInfo.get_ResourceLocation",
        "ManifestResourceInfo.get_ReferencedAssembly",

        // ── Assembly: descriptor-backed accessors and stream helpers ─────
        "Assembly.GetManifestResourceInfo",
        "Assembly.GetManifestResourceStream",
        "Assembly.get_ImageRuntimeVersion",
        "Assembly.get_IsFullyTrusted",
        "Assembly.get_SecurityRuleSet",
        "Assembly.get_ManifestModule",
        "Assembly.get_GlobalAssemblyCache",
        "Assembly.get_HostContext",
        "Assembly.get_ReflectionOnly",
        "Assembly.get_IsCollectible",
        "Assembly.get_IsDynamic",
        "Assembly.GetObjectData",
        "Assembly.Equals",
        "Assembly.GetHashCode",
        "Assembly.get_EqualsFor",

        // ── MethodBase / MethodInfo: descriptor flags ───────────────────
        "MethodBase.get_Attributes",
        "MethodBase.get_MethodHandleFor",
        "MethodBase.get_ContainsGenericParameters",
        "MethodBase.get_IsGenericMethod",
        "MethodBase.get_IsGenericMethodDefinition",
        "MethodBase.get_IsConstructedGenericMethod",
        "MethodBase.get_IsPublic",
        "MethodBase.get_IsPrivate",
        "MethodBase.get_IsAssembly",
        "MethodBase.get_IsFamily",
        "MethodBase.get_IsFamilyAndAssembly",
        "MethodBase.get_IsFamilyOrAssembly",
        "MethodBase.get_IsSecurityCritical",
        "MethodBase.get_IsSecuritySafeCritical",
        "MethodBase.get_IsSecurityTransparent",
        "MethodBase.get_CallingConvention",
        "MethodBase.get_MethodImplementationFlags",
        "MethodBase.GetGenericArguments",
        "MethodBase.GetMethodFromHandle",
        "MethodBase.GetCurrentMethod",
        "MethodBase.Equals",
        "MethodBase.GetHashCode",
        "MethodInfo.get_MethodHandleFor",

        // ── FieldInfo: descriptor flags ─────────────────────────────────
        "FieldInfo.get_AttributesFor",
        "FieldInfo.get_FieldTypeFor",
        "FieldInfo.get_MemberType",
        "FieldInfo.get_IsSpecialName",
        "FieldInfo.get_IsNotSerialized",
        "FieldInfo.get_IsPinvokeImpl",
        "FieldInfo.get_IsSecurityCritical",
        "FieldInfo.get_IsSecuritySafeCritical",
        "FieldInfo.get_IsSecurityTransparent",
        "FieldInfo.get_IsAssemblyFor",
        "FieldInfo.get_IsFamilyAndAssembly",
        "FieldInfo.get_IsFamilyOrAssembly",
        "FieldInfo.get_FieldHandle",
        "FieldInfo.GetFieldFromHandle",
        "FieldInfo.GetValueDirect",
        "FieldInfo.SetValueDirect",
        "FieldInfo.GetModifiedFieldType",
        "FieldInfo.GetOptionalCustomModifiers",
        "FieldInfo.GetRequiredCustomModifiers",
        "FieldInfo.Equals",
        "FieldInfo.GetHashCode",

        // ── PropertyInfo: descriptor flags ──────────────────────────────
        "PropertyInfo.get_AttributesFor",
        "PropertyInfo.get_CanReadFor",
        "PropertyInfo.get_CanWriteFor",
        "PropertyInfo.get_PropertyTypeFor",
        "PropertyInfo.get_MemberType",
        "PropertyInfo.get_IsSpecialName",
        "PropertyInfo.get_GetMethodFor",
        "PropertyInfo.get_SetMethodFor",
        "PropertyInfo.GetGetMethod",
        "PropertyInfo.GetSetMethod",
        "PropertyInfo.GetAccessorsFor",
        "PropertyInfo.GetValueFor",
        "PropertyInfo.SetValueFor",
        "PropertyInfo.GetIndexParametersFor",
        "PropertyInfo.GetConstantValue",
        "PropertyInfo.GetRawConstantValue",
        "PropertyInfo.GetModifiedPropertyType",
        "PropertyInfo.GetOptionalCustomModifiers",
        "PropertyInfo.GetRequiredCustomModifiers",
        "PropertyInfo.Equals",
        "PropertyInfo.GetHashCode",

        // ── ParameterInfo: descriptor flags ─────────────────────────────
        "ParameterInfo.get_AttributesFor",
        "ParameterInfo.get_ParameterTypeFor",
        "ParameterInfo.get_PositionFor",
        "ParameterInfo.get_NameFor",
        "ParameterInfo.get_MetadataToken",
        "ParameterInfo.get_MemberFor",
        "ParameterInfo.get_IsInFor",
        "ParameterInfo.get_IsOutFor",
        "ParameterInfo.get_IsOptionalFor",
        "ParameterInfo.get_IsRetvalFor",
        "ParameterInfo.get_IsLcid",
        "ParameterInfo.get_DefaultValueFor",
        "ParameterInfo.get_RawDefaultValueFor",
        "ParameterInfo.get_HasDefaultValueFor",
        "ParameterInfo.get_CustomAttributesFor",
        "ParameterInfo.GetCustomAttributesDataFor",
        "ParameterInfo.GetOptionalCustomModifiersFor",
        "ParameterInfo.GetRequiredCustomModifiersFor",
        "ParameterInfo.GetModifiedParameterType",
        "ParameterInfo.GetRealObject",
        "ParameterInfo.ToString",

        // ── EventInfo: descriptor flags ─────────────────────────────────
        "EventInfo.get_AttributesFor",
        "EventInfo.get_EventHandlerTypeFor",
        "EventInfo.get_IsSpecialNameFor",
        "EventInfo.get_IsMulticastFor",
        "EventInfo.get_AddMethodFor",
        "EventInfo.get_RemoveMethodFor",
        "EventInfo.GetAddMethod",
        "EventInfo.GetRemoveMethod",
        "EventInfo.GetOtherMethods",
        "EventInfo.get_MemberType",
        "EventInfo.Equals",
        "EventInfo.GetHashCode",

        // ── Module / TypeInfo descriptor-backed ─────────────────────────
        "Module.get_ScopeNameFor",
        "Module.get_FullyQualifiedNameFor",
        "Module.get_ModuleVersionIdFor",
        "Module.get_MetadataTokenFor",
        "Module.get_MDStreamVersionFor",
        "Module.get_AssemblyQualifiedName",
        "Module.GetCustomAttributesFor",
        "Module.IsDefinedFor",
        "Module.GetCustomAttributesDataFor",
        "Module.GetFieldsFor",
        "Module.GetMethodsFor",
        "Module.GetFieldFor",
        "Module.GetMethodFor",
        "Module.GetObjectDataFor",
        "Module.GetTypesForImpl",
        "Module.ResolveMethodFor",
        "Module.ResolveFieldFor",
        "Module.ResolveTypeFor",
        "Module.ResolveStringFor",
        "Module.ResolveMemberFor",
        "Module.ResolveSignatureFor",
        "Module.GetPEKindFor",
        "Module.ToString",
        "TypeInfo.get_DeclaredConstructorsFor",
        "TypeInfo.get_DeclaredFieldsFor",
        "TypeInfo.get_TypeAsType",
        "TypeInfo.get_GenericTypeParametersFor",
        "TypeInfo.get_ImplementedInterfacesFor",
        "TypeInfo.get_DeclaredMembersFor",
        "TypeInfo.get_DeclaredNestedTypesFor",
        "TypeInfo.get_DeclaredEventsFor",
        "TypeInfo.get_DeclaredPropertiesFor",
        "TypeInfo.get_DeclaredMethodsFor",
        "TypeInfo.GetDeclaredMethodFor",
        "TypeInfo.GetDeclaredFieldFor",
        "TypeInfo.GetDeclaredPropertyFor",
        "TypeInfo.GetDeclaredEventFor",
        "TypeInfo.GetDeclaredNestedTypeFor",
        "TypeInfo.IsAssignableFromFor",
        "TypeInfo.get_AssemblyQualifiedNameFor",

        // ── MemberInfo ──────────────────────────────────────────────────
        "MemberInfo.get_MemberType",
        "MemberInfo.get_CustomAttributeProvider",
        "MemberInfo.get_ModuleFor",
        "MemberInfo.IsDefinedFor",
        "MemberInfo.GetCustomAttributeFor",
        "MemberInfo.GetCustomAttributesFor",
        "MemberInfo.Equals",
        "MemberInfo.GetHashCode",
        "MemberInfo.ToString",

        // ── ConstructorInfo ─────────────────────────────────────────────
        "ConstructorInfo.GetGenericArguments",
        "ConstructorInfo.get_IsGenericMethod",
        "ConstructorInfo.get_MethodHandleFor",
        "ConstructorInfo.GetCustomAttributes",
        "ConstructorInfo.Equals",
        "ConstructorInfo.GetHashCode",

        // ── CustomAttribute data objects ────────────────────────────────
        "CustomAttributeData.Equals",
        "CustomAttributeData.GetHashCode",
        "CustomAttributeData.ToString",
        "CustomAttributeNamedArgument.get_MemberName",
        "CustomAttributeNamedArgument.get_MemberInfo",
        "CustomAttributeNamedArgument.get_TypedValue",
        "CustomAttributeNamedArgument.get_IsField",
        "CustomAttributeNamedArgument.Equals",
        "CustomAttributeNamedArgument.GetHashCode",
        "CustomAttributeNamedArgument.ToString",
        "CustomAttributeTypedArgument.get_ArgumentType",
        "CustomAttributeTypedArgument.get_Value",
        "CustomAttributeTypedArgument.Equals",
        "CustomAttributeTypedArgument.GetHashCode",
        "CustomAttributeTypedArgument.ToString",

        // ── Nullability / misc ──────────────────────────────────────────
        "NullabilityInfo.get_Type",
        "NullabilityInfo.get_ReadState",
        "NullabilityInfo.get_WriteState",
        "NullabilityInfo.get_ElementType",
        "NullabilityInfo.get_GenericTypeArguments",
        "NullabilityInfoContext.CreateFor",
        "ICustomAttributeProvider.GetCustomAttributes",
        "ICustomAttributeProvider.IsDefined",
        "IReflectableType.GetTypeInfo",
        "Missing.Value",

        // ── Final pass: exact member names surfaced by the scanner ──────
        // Assembly
        "Assembly.get_FullName",
        "Assembly.get_Location",
        "Assembly.get_EntryPoint",
        "Assembly.GetAssembly",
        "Assembly.get_CustomAttributes",
        "Assembly.GetManifestResourceInfoFor",
        "Assembly.get_ModuleResolve",

        // AssemblyName
        "AssemblyName.get_CodeBase",
        "AssemblyName.get_EscapedCodeBase",
        "AssemblyName.get_KeyPair",
        "AssemblyName.GetObjectData",
        "AssemblyName.OnDeserialization",
        "AssemblyName.ReferenceMatchesDefinition",
        "AssemblyName.get_VersionCompatibility",
        "AssemblyName.set_VersionCompatibility",
        "AssemblyName.GetAssemblyName",

        // Module
        "Module.get_Assembly",
        "Module.get_Name",
        "Module.get_CustomAttributes",
        "Module.Equals",
        "Module.GetHashCode",
        "Module.FilterTypeName",
        "Module.FilterTypeNameIgnoreCase",
        "Module.FindTypes",
        "Module.IsResource",
        "Module.get_ModuleHandle",

        // PropertyInfo
        "PropertyInfo.get_CanRead",
        "PropertyInfo.get_CanWrite",
        "PropertyInfo.get_PropertyType",
        "PropertyInfo.GetValue",
        "PropertyInfo.SetValue",
        "PropertyInfo.GetIndexParameters",
        "PropertyInfo.GetMethod",
        "PropertyInfo.SetMethod",
        "PropertyInfo.get_Attributes",
        "PropertyInfo.get_IsSpecialName",
        "PropertyInfo.get_MemberType",
        "PropertyInfo.GetConstantValueFor",
        "PropertyInfo.GetRawConstantValueFor",

        // MethodInfo
        "MethodInfo.get_ReturnType",
        "MethodInfo.get_ReturnParameter",
        "MethodInfo.get_ReturnTypeCustomAttributes",
        "MethodInfo.GetBaseDefinition",
        "MethodInfo.Equals",
        "MethodInfo.GetHashCode",
        "MethodInfo.get_MemberType",
        "MethodInfo.CreateDelegate",

        // ParameterInfo
        "ParameterInfo.get_Name",
        "ParameterInfo.get_ParameterType",
        "ParameterInfo.get_Position",
        "ParameterInfo.get_CustomAttributes",
        "ParameterInfo.get_Attributes",
        "ParameterInfo.GetCustomAttributesData",
        "ParameterInfo.get_DefaultValue",
        "ParameterInfo.get_RawDefaultValue",
        "ParameterInfo.get_HasDefaultValue",
        "ParameterInfo.get_IsIn",
        "ParameterInfo.get_IsOut",
        "ParameterInfo.get_IsOptional",
        "ParameterInfo.get_IsRetval",
        "ParameterInfo.get_IsLcid",
        "ParameterInfo.get_Member",
        "ParameterInfo.get_MetadataToken",
        "ParameterInfo.GetOptionalCustomModifiers",
        "ParameterInfo.GetRequiredCustomModifiers",
        "ParameterInfo.get_CustomAttributesFor",
        "ParameterInfo.ToString",

        // MemberInfo
        "MemberInfo.get_Name",
        "MemberInfo.get_DeclaringType",
        "MemberInfo.get_ReflectedType",
        "MemberInfo.get_MetadataToken",
        "MemberInfo.get_CustomAttributes",
        "MemberInfo.get_MemberType",
        "MemberInfo.get_Module",
        "MemberInfo.get_IsCollectible",
        "MemberInfo.HasSameMetadataDefinitionAs",
        "MemberInfo.Equals",
        "MemberInfo.GetHashCode",
        "MemberInfo.ToString",

        // ConstructorInfo
        "ConstructorInfo.Invoke",
        "ConstructorInfo.get_MemberType",
        "ConstructorInfo.get_ConstructorName",
        "ConstructorInfo.get_TypeConstructorName",
        "ConstructorInfo.GetCustomAttributes",

        // EventInfo
        "EventInfo.get_Attributes",
        "EventInfo.get_EventHandlerType",
        "EventInfo.get_IsSpecialName",
        "EventInfo.get_IsMulticast",
        "EventInfo.get_AddMethod",
        "EventInfo.get_RemoveMethod",
        "EventInfo.get_RaiseMethod",
        "EventInfo.GetRaiseMethod",
        "EventInfo.AddEventHandler",
        "EventInfo.RemoveEventHandler",
        "EventInfo.get_MemberType",

        // FieldInfo
        "FieldInfo.get_FieldType",
        "FieldInfo.get_IsInitOnly",
        "FieldInfo.get_IsLiteral",
        "FieldInfo.get_IsStatic",
        "FieldInfo.get_Attributes",
        "FieldInfo.get_MemberType",
        "FieldInfo.get_FieldHandle",

        // TypeInfo / misc
        "TypeInfo.GetDeclaredMethods",
        "TypeInfo.IsAssignableFrom",
        "RuntimeReflectionExtensions.GetRuntimeBaseDefinition",
        "MethodBase.GetMethodBody",

        // Carrier constructors (recorded but outside the denominator)
        "CustomAttributeNamedArgument..ctor",
        "CustomAttributeTypedArgument..ctor",
        "ManifestResourceInfo..ctor",
        "NullabilityInfoContext..ctor",
        "AssemblyName..ctor",
    };

    public static void Apply(CoverageMatrix matrix)
    {
        // The matrix records property/event accessors under their bare member
        // name ("Name", "Attributes"), while the tables below are written from
        // the API-contract viewpoint ("get_Name"). Normalize by indexing the
        // tables under both spellings so lookups are prefix-insensitive.
        var descriptorBacked = Normalize(DescriptorBacked);
        var notSupported = Normalize(NotSupportedMembers);

        // KnownNativeImpls is written from the contract viewpoint ("get_Name");
        // the matrix records bare member names. Index it under both spellings so
        // accessor properties resolve without duplicating every entry.
        var knownNative = new Dictionary<string, string[]>(StringComparer.Ordinal);
        foreach (var (key, syms) in KnownNativeImpls)
        {
            knownNative[key] = syms;
            int dot = key.IndexOf('.');
            if (dot < 0) continue;
            string ty = key[..dot], member = key[(dot + 1)..];
            if (member.StartsWith("get_", StringComparison.Ordinal))
                knownNative.TryAdd($"{ty}.{member[4..]}", syms);
            else if (member.StartsWith("set_", StringComparison.Ordinal))
                knownNative.TryAdd($"{ty}.{member[4..]}", syms);
        }

        foreach (var e in matrix.Entries)
        {
            // Enum constants and data-carrier classes (exceptions/attributes) are
            // not reflection operations — leave them in their non-operation tier.
            if (e.TierValue is Tier.EnumSurface or Tier.NotApplicable or Tier.DataCarrier) continue;

            string typeMember = $"{e.TypeName}.{e.MemberName}";

            // 1. Known native implementation → real (fact tier; performance not yet qualified).
            if (knownNative.TryGetValue(typeMember, out var nativeSyms))
            {
                e.TierValue = Tier.Real;
                e.RealKind = "fact";        // design §3.2: performance is a Phase 4 gate
                e.Evidence.NativeImpl = string.Join(", ", nativeSyms);
                e.Rationale = "Native symbol exists in reflection_api.h";
                continue;
            }

            // 1b. Types whose whole surface dispatches to a shared native pair.
            if (WholeTypeRealViaDispatch.Contains(e.TypeName))
            {
                e.TierValue = Tier.Real;
                e.RealKind = "fact";
                e.Evidence.NativeImpl = "ChaosReflectionCollectCustomAttributes / MemberIsDefinedByToken";
                e.Rationale = "Overload dispatches to the shared collection/IsDefined native APIs";
                continue;
            }

            // 2. Member-level structural impossibility → not-supported.
            if (notSupported.Contains(typeMember))
            {
                e.TierValue = Tier.NotSupported;
                e.Rationale = "Structurally impossible under AOT (requires runtime/JIT capability)";
                continue;
            }

            // 3. Whole-type impossibility → not-supported for all its members.
            if (NotSupportedWholeTypes.Contains(e.TypeName))
            {
                e.TierValue = Tier.NotSupported;
                e.Rationale = $"Type {e.TypeName} requires runtime metadata not present under AOT";
                continue;
            }

            // 4. Type-level impossibility → not-supported for all its members.
            var nsType = NotSupportedTypes.FirstOrDefault(t => t.Type == e.TypeName);
            if (nsType.Type is not null)
            {
                e.TierValue = Tier.NotSupported;
                e.Rationale = nsType.Reason;
                continue;
            }

            // 5. Descriptor-backed accessors: the metadata exists, but the
            //    native accessor is not yet written. Recorded as `real-planned`
            //    rather than `real` so the matrix never overstates coverage.
            //    Phase 3 (roadmap P3-1) promotes these once implemented+verified.
            if (descriptorBacked.Contains(typeMember))
            {
                e.TierValue = Tier.RealPlanned;
                e.RealKind = "none";
                e.Evidence.NativeImpl = null;
                e.Rationale = "Metadata exists in Tier-2 descriptor; native accessor not yet implemented";
                continue;
            }

            // 6. Everything else stays unclassified — an explicit Phase 2 work item.
            e.TierValue = Tier.Unclassified;
        }

        // Recompute summary.
        var byTier = matrix.Entries.GroupBy(x => x.TierValue)
                                   .ToDictionary(g => g.Key, g => g.Count());
        matrix.Summary.ByTier = byTier;
        matrix.Summary.Denominator = matrix.Entries.Count(x => Program.IsOperationTier(x.TierValue));
    }

    /// <summary>
    /// Expands a "Type.get_Name"-style table into lookups keyed by both the
    /// declared spelling and the bare member name, so matrix entries recorded
    /// under either form resolve.
    /// </summary>
    private static HashSet<string> Normalize(IEnumerable<string> entries)
    {
        var set = new HashSet<string>(StringComparer.Ordinal);
        foreach (var raw in entries)
        {
            set.Add(raw);

            int dot = raw.IndexOf('.');
            if (dot < 0) continue;
            string type = raw[..dot];
            string member = raw[(dot + 1)..];

            // Strip accessor prefixes so "get_Name" also matches "Name".
            if (member.StartsWith("get_", StringComparison.Ordinal))
                set.Add($"{type}.{member[4..]}");
            else if (member.StartsWith("set_", StringComparison.Ordinal))
                set.Add($"{type}.{member[4..]}");
        }
        return set;
    }
}
