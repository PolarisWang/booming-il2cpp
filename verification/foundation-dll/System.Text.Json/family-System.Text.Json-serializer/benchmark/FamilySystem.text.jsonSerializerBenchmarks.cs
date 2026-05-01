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

// Auto-generated benchmark skeletons for FamilySystem.text.jsonSerializerBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.text.jsonSerializerBenchmarks
{
    // Purpose: Benchmark native-runtime performance of JsonSerializer.JsonSerializer: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::JsonSerializer::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializer_JsonSerializer_Void_System_Object() { _ = JsonSerializer.JsonSerializer:(42); }
    // Purpose: Benchmark native-runtime performance of JsonSerializer.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializer_get_Property_System_Void() { JsonSerializer.Property; }
    // Purpose: Benchmark native-runtime performance of JsonSerializer.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializer_Method1_System_Void_P0() { JsonSerializer.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonSerializer.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializer_Method2_System_Void_P0_P1() { JsonSerializer.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonSerializer.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializer_Validate_System_Void_P0() { JsonSerializer.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonSerializer.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializer_get_Value_System_Void() { JsonSerializer.Value; }
    // Purpose: Benchmark native-runtime performance of JsonSerializer.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializer_set_Value_System_Void_P0() { JsonSerializer.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonSerializer.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializer_get_Count_System_Void() { JsonSerializer.Count; }
    // Purpose: Benchmark native-runtime performance of JsonSerializer.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializer_set_Count_System_Void_P0() { JsonSerializer.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonSerializer.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializer_get_IsEmpty_System_Void() { JsonSerializer.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonSerializer.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializer_set_IsEmpty_System_Void_P0() { JsonSerializer.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonSerializer.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializer_get_Key_System_Void() { JsonSerializer.Key; }
    // Purpose: Benchmark native-runtime performance of JsonSerializer.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonSerializer_set_Key_System_Void_P0() { JsonSerializer.Key = null!; }}
