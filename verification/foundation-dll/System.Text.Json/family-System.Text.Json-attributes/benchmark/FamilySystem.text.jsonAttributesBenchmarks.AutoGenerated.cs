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

// Auto-generated benchmark skeletons for FamilySystem.text.jsonAttributesBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.text.jsonAttributesBenchmarks
{
    // Purpose: Benchmark native-runtime performance of JsonAttribute.JsonAttribute: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::JsonAttribute::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_JsonAttribute_Void_System_Object() { _ = JsonAttribute.JsonAttribute:(42); }
    // Purpose: Benchmark native-runtime performance of JsonAttribute.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_get_Property_System_Void() { JsonAttribute.Property; }
    // Purpose: Benchmark native-runtime performance of JsonAttribute.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_Method1_System_Void_P0() { JsonAttribute.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonAttribute.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_Method2_System_Void_P0_P1() { JsonAttribute.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonAttribute.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_Validate_System_Void_P0() { JsonAttribute.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonAttribute.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_get_Value_System_Void() { JsonAttribute.Value; }
    // Purpose: Benchmark native-runtime performance of JsonAttribute.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_set_Value_System_Void_P0() { JsonAttribute.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonAttribute.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_get_Count_System_Void() { JsonAttribute.Count; }
    // Purpose: Benchmark native-runtime performance of JsonAttribute.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_set_Count_System_Void_P0() { JsonAttribute.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonAttribute.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_get_IsEmpty_System_Void() { JsonAttribute.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonAttribute.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_set_IsEmpty_System_Void_P0() { JsonAttribute.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonAttribute.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_get_Key_System_Void() { JsonAttribute.Key; }
    // Purpose: Benchmark native-runtime performance of JsonAttribute.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_set_Key_System_Void_P0() { JsonAttribute.Key = null!; }}
