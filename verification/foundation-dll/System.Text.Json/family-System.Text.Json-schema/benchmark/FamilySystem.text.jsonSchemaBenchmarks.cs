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

// Auto-generated benchmark skeletons for FamilySystem.text.jsonSchemaBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.text.jsonSchemaBenchmarks
{
    // Purpose: Benchmark native-runtime performance of JsonSchemaExporter.JsonSchemaExporter: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::JsonSchemaExporter::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_JsonSchemaExporter_Void_System_Object() { _ = JsonSchemaExporter.JsonSchemaExporter:(42); }
    // Purpose: Benchmark native-runtime performance of JsonSchemaExporter.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_get_Property_System_Void() { JsonSchemaExporter.Property; }
    // Purpose: Benchmark native-runtime performance of JsonSchemaExporter.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_Method1_System_Void_P0() { JsonSchemaExporter.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonSchemaExporter.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_Method2_System_Void_P0_P1() { JsonSchemaExporter.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonSchemaExporter.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_Validate_System_Void_P0() { JsonSchemaExporter.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonSchemaExporter.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_get_Value_System_Void() { JsonSchemaExporter.Value; }
    // Purpose: Benchmark native-runtime performance of JsonSchemaExporter.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_set_Value_System_Void_P0() { JsonSchemaExporter.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonSchemaExporter.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_get_Count_System_Void() { JsonSchemaExporter.Count; }
    // Purpose: Benchmark native-runtime performance of JsonSchemaExporter.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_set_Count_System_Void_P0() { JsonSchemaExporter.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonSchemaExporter.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_get_IsEmpty_System_Void() { JsonSchemaExporter.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonSchemaExporter.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_set_IsEmpty_System_Void_P0() { JsonSchemaExporter.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonSchemaExporter.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_get_Key_System_Void() { JsonSchemaExporter.Key; }
    // Purpose: Benchmark native-runtime performance of JsonSchemaExporter.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_set_Key_System_Void_P0() { JsonSchemaExporter.Key = null!; }}
