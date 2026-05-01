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

// Auto-generated benchmark skeletons for FamilySystem.text.jsonMetadataTypeinfoBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.text.jsonMetadataTypeinfoBenchmarks
{
    // Purpose: Benchmark native-runtime performance of JsonTypeInfo.JsonTypeInfo: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::JsonTypeInfo::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_JsonTypeInfo_Void_System_Object() { _ = JsonTypeInfo.JsonTypeInfo:(42); }
    // Purpose: Benchmark native-runtime performance of JsonTypeInfo.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_get_Property_System_Void() { JsonTypeInfo.Property; }
    // Purpose: Benchmark native-runtime performance of JsonTypeInfo.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_Method1_System_Void_P0() { JsonTypeInfo.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonTypeInfo.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_Method2_System_Void_P0_P1() { JsonTypeInfo.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonTypeInfo.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_Validate_System_Void_P0() { JsonTypeInfo.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonTypeInfo.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_get_Value_System_Void() { JsonTypeInfo.Value; }
    // Purpose: Benchmark native-runtime performance of JsonTypeInfo.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_set_Value_System_Void_P0() { JsonTypeInfo.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonTypeInfo.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_get_Count_System_Void() { JsonTypeInfo.Count; }
    // Purpose: Benchmark native-runtime performance of JsonTypeInfo.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_set_Count_System_Void_P0() { JsonTypeInfo.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonTypeInfo.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_get_IsEmpty_System_Void() { JsonTypeInfo.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonTypeInfo.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_set_IsEmpty_System_Void_P0() { JsonTypeInfo.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonTypeInfo.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_get_Key_System_Void() { JsonTypeInfo.Key; }
    // Purpose: Benchmark native-runtime performance of JsonTypeInfo.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_set_Key_System_Void_P0() { JsonTypeInfo.Key = null!; }}
