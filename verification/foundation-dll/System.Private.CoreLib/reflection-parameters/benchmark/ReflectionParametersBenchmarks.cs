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

// Benchmark stubs for family/System.Private.CoreLib/reflection/parameters
public static class ReflectionParametersBenchmarks
{
    // Benchmark ParameterInfo.get_Name native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_Name_System_String() { _ = default(ParameterInfo)!.Name; }
    // Benchmark ParameterInfo.get_ParameterType native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_ParameterType_System_Type() { _ = default(ParameterInfo)!.ParameterType; }
    // Benchmark ParameterInfo.get_DefaultValue native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_DefaultValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_DefaultValue_System_Object() { _ = default(ParameterInfo)!.DefaultValue; }
    // Benchmark ParameterInfo.get_RawDefaultValue native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_RawDefaultValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_RawDefaultValue_System_Object() { _ = default(ParameterInfo)!.RawDefaultValue; }
    // Benchmark ParameterInfo.get_HasDefaultValue native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_HasDefaultValue_System_Boolean() { _ = default(ParameterInfo)!.HasDefaultValue; }
    // Benchmark ParameterInfo.get_Attributes native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Attributes:System.Reflection.ParameterAttributes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_Attributes_System_Reflection_ParameterAttributes() { _ = default(ParameterInfo)!.Attributes; }
    // Benchmark ParameterInfo.get_Position native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Position:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_Position_System_Int32() { _ = default(ParameterInfo)!.Position; }
    // Benchmark ParameterInfo.GetRequiredCustomModifiers native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::GetRequiredCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_GetRequiredCustomModifiers_System_Type() { _ = default(ParameterInfo)!.GetRequiredCustomModifiers(); }
    // Benchmark ParameterInfo.GetOptionalCustomModifiers native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::GetOptionalCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_GetOptionalCustomModifiers_System_Type() { _ = default(ParameterInfo)!.GetOptionalCustomModifiers(); }
    // Benchmark MethodBase.GetParameters native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_GetParameters_System_Reflection_ParameterInfo() { _ = default(MethodInfo)!.GetParameters(); }}
