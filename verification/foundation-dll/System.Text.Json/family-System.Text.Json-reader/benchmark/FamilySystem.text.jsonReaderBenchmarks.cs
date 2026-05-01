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

// Auto-generated benchmark skeletons for FamilySystem.text.jsonReaderBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.text.jsonReaderBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Utf8JsonReader.Utf8JsonReader: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::Utf8JsonReader::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonReader_Utf8JsonReader_Void_System_Object() { _ = Utf8JsonReader.Utf8JsonReader:(42); }
    // Purpose: Benchmark native-runtime performance of Utf8JsonReader.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonReader_get_Property_System_Void() { Utf8JsonReader.Property; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonReader.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonReader_Method1_System_Void_P0() { Utf8JsonReader.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of Utf8JsonReader.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonReader_Method2_System_Void_P0_P1() { Utf8JsonReader.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Utf8JsonReader.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonReader_Validate_System_Void_P0() { Utf8JsonReader.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of Utf8JsonReader.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonReader_get_Value_System_Void() { Utf8JsonReader.Value; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonReader.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonReader_set_Value_System_Void_P0() { Utf8JsonReader.Value = null!; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonReader.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonReader_get_Count_System_Void() { Utf8JsonReader.Count; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonReader.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonReader_set_Count_System_Void_P0() { Utf8JsonReader.Count = null!; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonReader.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonReader_get_IsEmpty_System_Void() { Utf8JsonReader.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonReader.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonReader_set_IsEmpty_System_Void_P0() { Utf8JsonReader.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonReader.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonReader_get_Key_System_Void() { Utf8JsonReader.Key; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonReader.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonReader_set_Key_System_Void_P0() { Utf8JsonReader.Key = null!; }}
