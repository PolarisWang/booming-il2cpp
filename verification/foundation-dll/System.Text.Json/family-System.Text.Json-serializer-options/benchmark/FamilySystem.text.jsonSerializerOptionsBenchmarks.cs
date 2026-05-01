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

// Auto-generated benchmark skeletons for FamilySystem.text.jsonSerializerOptionsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.text.jsonSerializerOptionsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of JsonNamingPolicy.JsonNamingPolicy: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonNamingPolicy::JsonNamingPolicy::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonNamingPolicy_JsonNamingPolicy_Void_System_Object() { _ = JsonNamingPolicy.JsonNamingPolicy:(42); }
    // Purpose: Benchmark native-runtime performance of JsonNamingPolicy.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonNamingPolicy::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonNamingPolicy_get_Property_System_Void() { JsonNamingPolicy.Property; }
    // Purpose: Benchmark native-runtime performance of JsonNamingPolicy.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonNamingPolicy::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonNamingPolicy_Method1_System_Void_P0() { JsonNamingPolicy.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonNamingPolicy.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonNamingPolicy::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonNamingPolicy_Method2_System_Void_P0_P1() { JsonNamingPolicy.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonNamingPolicy.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonNamingPolicy::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonNamingPolicy_Validate_System_Void_P0() { JsonNamingPolicy.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonSerializerOptions.JsonSerializerOptions: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializerOptions::JsonSerializerOptions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializerOptions_JsonSerializerOptions_Void_System_Object() { _ = JsonSerializerOptions.JsonSerializerOptions:(42); }
    // Purpose: Benchmark native-runtime performance of JsonSerializerOptions.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializerOptions_get_Property_System_Void() { JsonSerializerOptions.Property; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerOptions.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializerOptions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializerOptions_Method1_System_Void_P0() { JsonSerializerOptions.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonSerializerOptions.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializerOptions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializerOptions_Method2_System_Void_P0_P1() { JsonSerializerOptions.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonSerializerOptions.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializerOptions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializerOptions_Validate_System_Void_P0() { JsonSerializerOptions.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonNamingPolicy.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonNamingPolicy::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonNamingPolicy_get_Value_System_Void() { JsonNamingPolicy.Value; }
    // Purpose: Benchmark native-runtime performance of JsonNamingPolicy.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonNamingPolicy::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonNamingPolicy_set_Value_System_Void_P0() { JsonNamingPolicy.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonNamingPolicy.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonNamingPolicy::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonNamingPolicy_get_Count_System_Void() { JsonNamingPolicy.Count; }
    // Purpose: Benchmark native-runtime performance of JsonNamingPolicy.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonNamingPolicy::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonNamingPolicy_set_Count_System_Void_P0() { JsonNamingPolicy.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonNamingPolicy.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonNamingPolicy::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonNamingPolicy_get_IsEmpty_System_Void() { JsonNamingPolicy.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonNamingPolicy.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonNamingPolicy::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonNamingPolicy_set_IsEmpty_System_Void_P0() { JsonNamingPolicy.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonNamingPolicy.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonNamingPolicy::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonNamingPolicy_get_Key_System_Void() { JsonNamingPolicy.Key; }
    // Purpose: Benchmark native-runtime performance of JsonNamingPolicy.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonNamingPolicy::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonNamingPolicy_set_Key_System_Void_P0() { JsonNamingPolicy.Key = null!; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerOptions.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializerOptions_get_Value_System_Void() { JsonSerializerOptions.Value; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerOptions.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializerOptions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializerOptions_set_Value_System_Void_P0() { JsonSerializerOptions.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerOptions.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializerOptions_get_Count_System_Void() { JsonSerializerOptions.Count; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerOptions.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializerOptions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializerOptions_set_Count_System_Void_P0() { JsonSerializerOptions.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerOptions.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializerOptions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializerOptions_get_IsEmpty_System_Void() { JsonSerializerOptions.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerOptions.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializerOptions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializerOptions_set_IsEmpty_System_Void_P0() { JsonSerializerOptions.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerOptions.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializerOptions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializerOptions_get_Key_System_Void() { JsonSerializerOptions.Key; }
    // Purpose: Benchmark native-runtime performance of JsonSerializerOptions.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializerOptions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializerOptions_set_Key_System_Void_P0() { JsonSerializerOptions.Key = null!; }}
