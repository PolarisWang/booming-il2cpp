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

// Auto-generated benchmark skeletons for FamilySystem.text.jsonConvertersBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.text.jsonConvertersBenchmarks
{
    // Purpose: Benchmark native-runtime performance of JsonConverter.JsonConverter: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::JsonConverter::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverter_JsonConverter_Void_System_Object() { _ = JsonConverter.JsonConverter:(42); }
    // Purpose: Benchmark native-runtime performance of JsonConverter.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverter_get_Property_System_Void() { JsonConverter.Property; }
    // Purpose: Benchmark native-runtime performance of JsonConverter.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverter_Method1_System_Void_P0() { JsonConverter.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonConverter.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverter_Method2_System_Void_P0_P1() { JsonConverter.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonConverter.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverter_Validate_System_Void_P0() { JsonConverter.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonConverter.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverter_get_Value_System_Void() { JsonConverter.Value; }
    // Purpose: Benchmark native-runtime performance of JsonConverter.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverter_set_Value_System_Void_P0() { JsonConverter.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonConverter.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverter_get_Count_System_Void() { JsonConverter.Count; }
    // Purpose: Benchmark native-runtime performance of JsonConverter.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverter_set_Count_System_Void_P0() { JsonConverter.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonConverter.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverter_get_IsEmpty_System_Void() { JsonConverter.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonConverter.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverter_set_IsEmpty_System_Void_P0() { JsonConverter.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonConverter.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverter_get_Key_System_Void() { JsonConverter.Key; }
    // Purpose: Benchmark native-runtime performance of JsonConverter.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverter_set_Key_System_Void_P0() { JsonConverter.Key = null!; }}
