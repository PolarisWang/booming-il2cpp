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

// Auto-generated benchmark skeletons for FamilySystem.text.jsonSourceGenerationContextBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.text.jsonSourceGenerationContextBenchmarks
{
    // Purpose: Benchmark native-runtime performance of JsonSerializerContext.JsonSerializerContext: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::JsonSerializerContext::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_JsonSerializerContext_Void_System_Object() { _ = JsonSerializerContext.JsonSerializerContext:(42); }
    // Purpose: Benchmark native-runtime performance of JsonSerializerContext.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_get_Property_System_Void() { JsonSerializerContext.Property; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerContext.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_Method1_System_Void_P0() { JsonSerializerContext.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonSerializerContext.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_Method2_System_Void_P0_P1() { JsonSerializerContext.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonSerializerContext.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_Validate_System_Void_P0() { JsonSerializerContext.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonSerializerContext.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_get_Value_System_Void() { JsonSerializerContext.Value; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerContext.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_set_Value_System_Void_P0() { JsonSerializerContext.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerContext.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_get_Count_System_Void() { JsonSerializerContext.Count; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerContext.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_set_Count_System_Void_P0() { JsonSerializerContext.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerContext.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_get_IsEmpty_System_Void() { JsonSerializerContext.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerContext.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_set_IsEmpty_System_Void_P0() { JsonSerializerContext.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerContext.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_get_Key_System_Void() { JsonSerializerContext.Key; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerContext.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_set_Key_System_Void_P0() { JsonSerializerContext.Key = null!; }}
