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

// Benchmark stubs for family/System.Private.CoreLib/attributes/custom-metadata
public static class AttributesCustomMetadataBenchmarks
{
    // Benchmark Attribute.GetCustomAttribute native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_MemberInfo_System_Type() { _ = typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute)); }
    // Benchmark Attribute.GetCustomAttribute native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_Assembly_System_Type() { _ = typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute)); }
    // Benchmark Attribute.GetCustomAttributes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo_System_Type() { }
    // Benchmark Attribute.GetCustomAttributes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly_System_Type() { }
    // Benchmark Attribute.GetCustomAttributes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo() { }
    // Benchmark Attribute.GetCustomAttributes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly() { }
    // Benchmark Attribute.IsDefined native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_MemberInfo_System_Type() { _ = typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute)); }
    // Benchmark Attribute.IsDefined native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_Assembly_System_Type() { _ = typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute)); }
    // Benchmark Attribute.get_TypeId native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Attribute::get_TypeId:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    public static void Benchmark_System_Private_CoreLib_System_Attribute_get_TypeId_System_Object() { }}
