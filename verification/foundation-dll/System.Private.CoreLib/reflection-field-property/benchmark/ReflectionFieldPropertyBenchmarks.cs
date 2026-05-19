using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Benchmark stubs for family/System.Private.CoreLib/reflection/field-property
public static class ReflectionFieldPropertyBenchmarks
{
    // Benchmark FieldInfo.get_MemberType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_MemberType:System.Reflection.MemberTypes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_MemberType_System_Reflection_MemberTypes() { _ = default(FieldInfo)!.MemberType; }
    // Benchmark FieldInfo.get_Attributes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_Attributes:System.Reflection.FieldAttributes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_Attributes_System_Reflection_FieldAttributes() { _ = default(FieldInfo)!.Attributes; }
    // Benchmark FieldInfo.get_IsNotSerialized native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsNotSerialized:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsNotSerialized_System_Boolean() { _ = default(FieldInfo)!.IsNotSerialized; }
    // Benchmark FieldInfo.get_IsPinvokeImpl native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsPinvokeImpl:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsPinvokeImpl_System_Boolean() { _ = default(FieldInfo)!.IsPinvokeImpl; }
    // Benchmark FieldInfo.get_IsSpecialName native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsSpecialName:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsSpecialName_System_Boolean() { _ = default(FieldInfo)!.IsSpecialName; }
    // Benchmark FieldInfo.get_IsAssembly native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsAssembly:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsAssembly_System_Boolean() { _ = default(FieldInfo)!.IsAssembly; }
    // Benchmark FieldInfo.get_IsFamily native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsFamily:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsFamily_System_Boolean() { _ = default(FieldInfo)!.IsFamily; }
    // Benchmark FieldInfo.get_IsFamilyAndAssembly native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsFamilyAndAssembly:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsFamilyAndAssembly_System_Boolean() { _ = default(FieldInfo)!.IsFamilyAndAssembly; }
    // Benchmark FieldInfo.get_IsFamilyOrAssembly native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsFamilyOrAssembly:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsFamilyOrAssembly_System_Boolean() { _ = default(FieldInfo)!.IsFamilyOrAssembly; }
    // Benchmark FieldInfo.get_IsPrivate native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsPrivate:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsPrivate_System_Boolean() { _ = default(FieldInfo)!.IsPrivate; }
    // Benchmark FieldInfo.get_IsPublic native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsPublic_System_Boolean() { _ = default(FieldInfo)!.IsPublic; }
    // Benchmark FieldInfo.get_IsSecurityCritical native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsSecurityCritical:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsSecurityCritical_System_Boolean() { _ = default(FieldInfo)!.IsSecurityCritical; }
    // Benchmark FieldInfo.get_IsSecuritySafeCritical native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsSecuritySafeCritical:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsSecuritySafeCritical_System_Boolean() { _ = default(FieldInfo)!.IsSecuritySafeCritical; }
    // Benchmark FieldInfo.get_IsSecurityTransparent native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsSecurityTransparent:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsSecurityTransparent_System_Boolean() { _ = default(FieldInfo)!.IsSecurityTransparent; }
    // Benchmark FieldInfo.SetValue native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Globalization.CultureInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_SetValue_System_Void_System_Object_System_Object_System_Reflection_Bi() { default(FieldInfo)!.SetValue(42, 42, System.Reflection.BindingFlags.Default, null!, null!); }
    // Benchmark FieldInfo.GetRawConstantValue native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetRawConstantValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_GetRawConstantValue_System_Object() { _ = default(FieldInfo)!.GetRawConstantValue(); }
    // Benchmark FieldInfo.GetOptionalCustomModifiers native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetOptionalCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_GetOptionalCustomModifiers_System_Type() { _ = default(FieldInfo)!.GetOptionalCustomModifiers(); }
    // Benchmark FieldInfo.GetRequiredCustomModifiers native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::GetRequiredCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_FieldInfo_GetRequiredCustomModifiers_System_Type() { _ = default(FieldInfo)!.GetRequiredCustomModifiers(); }
    // Benchmark PropertyInfo.get_Attributes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_Attributes:System.Reflection.PropertyAttributes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_get_Attributes_System_Reflection_PropertyAttributes() { _ = default(PropertyInfo)!.Attributes; }
    // Benchmark PropertyInfo.get_IsSpecialName native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_IsSpecialName:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_get_IsSpecialName_System_Boolean() { _ = default(PropertyInfo)!.IsSpecialName; }
    // Benchmark PropertyInfo.GetAccessors native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetAccessors:System.Reflection.MethodInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetAccessors_System_Reflection_MethodInfo() { _ = default(PropertyInfo)!.GetAccessors(); }
    // Benchmark PropertyInfo.GetAccessors native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetAccessors:System.Reflection.MethodInfo[](System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetAccessors_System_Reflection_MethodInfo_System_Boolean() { _ = default(PropertyInfo)!.GetAccessors(true); }
    // Benchmark PropertyInfo.get_GetMethod native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_GetMethod:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_get_GetMethod_System_Reflection_MethodInfo() { _ = default(PropertyInfo)!.GetMethod; }
    // Benchmark PropertyInfo.GetGetMethod native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetGetMethod:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetGetMethod_System_Reflection_MethodInfo() { _ = default(PropertyInfo)!.GetGetMethod(); }
    // Benchmark PropertyInfo.GetGetMethod native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetGetMethod:System.Reflection.MethodInfo(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetGetMethod_System_Reflection_MethodInfo_System_Boolean() { _ = default(PropertyInfo)!.GetGetMethod(true); }
    // Benchmark PropertyInfo.get_SetMethod native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_SetMethod:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_get_SetMethod_System_Reflection_MethodInfo() { _ = default(PropertyInfo)!.SetMethod; }
    // Benchmark PropertyInfo.GetSetMethod native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetSetMethod:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetSetMethod_System_Reflection_MethodInfo() { _ = default(PropertyInfo)!.GetSetMethod(); }
    // Benchmark PropertyInfo.GetSetMethod native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetSetMethod:System.Reflection.MethodInfo(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetSetMethod_System_Reflection_MethodInfo_System_Boolean() { _ = default(PropertyInfo)!.GetSetMethod(true); }
    // Benchmark PropertyInfo.GetValue native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetValue_System_Object_System_Object_System_Reflection_BindingFlag() { _ = default(PropertyInfo)!.GetValue(42, System.Reflection.BindingFlags.Default, null!, Array.Empty<System.Object>(), null!); }
    // Benchmark PropertyInfo.GetConstantValue native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetConstantValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetConstantValue_System_Object() { _ = default(PropertyInfo)!.GetConstantValue(); }
    // Benchmark PropertyInfo.GetRawConstantValue native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetRawConstantValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetRawConstantValue_System_Object() { _ = default(PropertyInfo)!.GetRawConstantValue(); }
    // Benchmark PropertyInfo.SetValue native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_SetValue_System_Void_System_Object_System_Object_System_Reflection() { default(PropertyInfo)!.SetValue(42, 42, System.Reflection.BindingFlags.Default, null!, Array.Empty<System.Object>(), null!); }
    // Benchmark PropertyInfo.GetOptionalCustomModifiers native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetOptionalCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetOptionalCustomModifiers_System_Type() { _ = default(PropertyInfo)!.GetOptionalCustomModifiers(); }
    // Benchmark PropertyInfo.GetRequiredCustomModifiers native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetRequiredCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionFieldProperty)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_PropertyInfo_GetRequiredCustomModifiers_System_Type() { _ = default(PropertyInfo)!.GetRequiredCustomModifiers(); }}
