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

// Auto-generated benchmark skeletons for FamilySystem.text.jsonWriterBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.text.jsonWriterBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Utf8JsonWriter.Utf8JsonWriter: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonWriter::Utf8JsonWriter::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonWriter_Utf8JsonWriter_Void_System_Object() { _ = Utf8JsonWriter.Utf8JsonWriter:(42); }
    // Purpose: Benchmark native-runtime performance of Utf8JsonWriter.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonWriter::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonWriter_get_Property_System_Void() { Utf8JsonWriter.Property; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonWriter.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonWriter::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonWriter_Method1_System_Void_P0() { Utf8JsonWriter.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of Utf8JsonWriter.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonWriter::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonWriter_Method2_System_Void_P0_P1() { Utf8JsonWriter.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Utf8JsonWriter.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonWriter::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonWriter_Validate_System_Void_P0() { Utf8JsonWriter.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of Utf8JsonWriter.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonWriter::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonWriter_get_Value_System_Void() { Utf8JsonWriter.Value; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonWriter.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonWriter::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonWriter_set_Value_System_Void_P0() { Utf8JsonWriter.Value = null!; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonWriter.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonWriter::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonWriter_get_Count_System_Void() { Utf8JsonWriter.Count; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonWriter.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonWriter::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonWriter_set_Count_System_Void_P0() { Utf8JsonWriter.Count = null!; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonWriter.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonWriter::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonWriter_get_IsEmpty_System_Void() { Utf8JsonWriter.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonWriter.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonWriter::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonWriter_set_IsEmpty_System_Void_P0() { Utf8JsonWriter.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonWriter.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonWriter::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonWriter_get_Key_System_Void() { Utf8JsonWriter.Key; }
    // Purpose: Benchmark native-runtime performance of Utf8JsonWriter.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Utf8JsonWriter::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Utf8JsonWriter_set_Key_System_Void_P0() { Utf8JsonWriter.Key = null!; }}
