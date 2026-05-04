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

// Auto-generated skeleton for Attributes And Custom Metadata (family/System.Private.CoreLib/attributes/custom-metadata).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class AttributesCustomMetadata
{
    // Attribute.GetCustomAttribute
    [MethodSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_MemberInfo_System_Type()
    {
    _ = typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute));
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_MemberInfo_System_Type() { _ = typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute)); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_MemberInfo_System_Type() { _ = typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute)); }

    // Attribute.GetCustomAttribute
    [MethodSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_Assembly_System_Type()
    {
    _ = typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute));
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_Assembly_System_Type() { _ = typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute)); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_Assembly_System_Type() { _ = typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute)); }

    // Attribute.GetCustomAttributes
    [MethodSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo_System_Type()
    {
    // TODO: needs-manual — GetCustomAttributes with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo_System_Type() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo_System_Type() { }

    // Attribute.GetCustomAttributes
    [MethodSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly_System_Type()
    {
    // TODO: needs-manual — GetCustomAttributes with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly_System_Type() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly_System_Type() { }

    // Attribute.GetCustomAttributes
    [MethodSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo()
    {
    // TODO: needs-manual — GetCustomAttributes with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo() { }

    // Attribute.GetCustomAttributes
    [MethodSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly()
    {
    // TODO: needs-manual — GetCustomAttributes with 1 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly() { }

    // Attribute.IsDefined
    [MethodSubjectId("System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_MemberInfo_System_Type()
    {
    _ = typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute));
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_MemberInfo_System_Type() { _ = typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute)); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_MemberInfo_System_Type() { _ = typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute)); }

    // Attribute.IsDefined
    [MethodSubjectId("System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_Assembly_System_Type()
    {
    _ = typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute));
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_Assembly_System_Type() { _ = typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute)); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_Assembly_System_Type() { _ = typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute)); }

    // Attribute.get_TypeId
    [MethodSubjectId("System.Private.CoreLib/System.Attribute::get_TypeId:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Attribute_get_TypeId_System_Object()
    {
    // TODO: needs-manual — get_TypeId with 0 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::get_TypeId:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_get_TypeId_System_Object() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::get_TypeId:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Attribute_get_TypeId_System_Object() { }
}
