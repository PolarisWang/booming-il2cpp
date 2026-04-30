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

// Auto-generated benchmark skeletons for ReflectionParametersBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class ReflectionParametersBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ParameterInfo.get_Name with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_Name_System_String() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].Name; }
    // Purpose: Benchmark native-runtime performance of ParameterInfo.get_ParameterType with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_ParameterType_System_Type() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].ParameterType; }
    // Purpose: Benchmark native-runtime performance of ParameterInfo.get_DefaultValue with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_DefaultValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_DefaultValue_System_Object() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].DefaultValue; }
    // Purpose: Benchmark native-runtime performance of ParameterInfo.get_RawDefaultValue with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_RawDefaultValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_RawDefaultValue_System_Object() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].RawDefaultValue; }
    // Purpose: Benchmark native-runtime performance of ParameterInfo.get_HasDefaultValue with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_HasDefaultValue_System_Boolean() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].HasDefaultValue; }
    // Purpose: Benchmark native-runtime performance of ParameterInfo.get_Attributes with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Attributes:System.Reflection.ParameterAttributes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_Attributes_System_Reflection_ParameterAttributes() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].Attributes; }
    // Purpose: Benchmark native-runtime performance of ParameterInfo.get_Position with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Position:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_get_Position_System_Int32() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].Position; }
    // Purpose: Benchmark native-runtime performance of ParameterInfo.GetRequiredCustomModifiers with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::GetRequiredCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_GetRequiredCustomModifiers_System_Type() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].GetRequiredCustomModifiers(); }
    // Purpose: Benchmark native-runtime performance of ParameterInfo.GetOptionalCustomModifiers with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::GetOptionalCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_ParameterInfo_GetOptionalCustomModifiers_System_Type() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].GetOptionalCustomModifiers(); }
    // Purpose: Benchmark native-runtime performance of MethodBase.GetParameters with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Reflection_MethodBase_GetParameters_System_Reflection_ParameterInfo() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.GetParameters(); }}
