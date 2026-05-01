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

// Auto-generated benchmark skeletons for FamilySystem.text.jsonDocumentElementBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.text.jsonDocumentElementBenchmarks
{
    // Purpose: Benchmark native-runtime performance of JsonDocument.JsonDocument: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonDocument::JsonDocument::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonDocument_JsonDocument_Void_System_Object() { _ = JsonDocument.JsonDocument:(42); }
    // Purpose: Benchmark native-runtime performance of JsonDocument.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonDocument::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonDocument_get_Property_System_Void() { JsonDocument.Property; }
    // Purpose: Benchmark native-runtime performance of JsonDocument.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonDocument::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonDocument_Method1_System_Void_P0() { JsonDocument.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonDocument.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonDocument::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonDocument_Method2_System_Void_P0_P1() { JsonDocument.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonDocument.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonDocument::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonDocument_Validate_System_Void_P0() { JsonDocument.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonElement.JsonElement: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonElement::JsonElement::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonElement_JsonElement_Void_System_Object() { _ = JsonElement.JsonElement:(42); }
    // Purpose: Benchmark native-runtime performance of JsonElement.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonElement::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonElement_get_Property_System_Void() { JsonElement.Property; }
    // Purpose: Benchmark native-runtime performance of JsonElement.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonElement::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonElement_Method1_System_Void_P0() { JsonElement.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonElement.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonElement::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonElement_Method2_System_Void_P0_P1() { JsonElement.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonElement.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonElement::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonElement_Validate_System_Void_P0() { JsonElement.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonDocument.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonDocument::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonDocument_get_Value_System_Void() { JsonDocument.Value; }
    // Purpose: Benchmark native-runtime performance of JsonDocument.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonDocument::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonDocument_set_Value_System_Void_P0() { JsonDocument.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonDocument.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonDocument::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonDocument_get_Count_System_Void() { JsonDocument.Count; }
    // Purpose: Benchmark native-runtime performance of JsonDocument.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonDocument::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonDocument_set_Count_System_Void_P0() { JsonDocument.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonDocument.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonDocument::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonDocument_get_IsEmpty_System_Void() { JsonDocument.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonDocument.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonDocument::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonDocument_set_IsEmpty_System_Void_P0() { JsonDocument.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonDocument.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonDocument::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonDocument_get_Key_System_Void() { JsonDocument.Key; }
    // Purpose: Benchmark native-runtime performance of JsonDocument.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonDocument::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonDocument_set_Key_System_Void_P0() { JsonDocument.Key = null!; }
    // Purpose: Benchmark native-runtime performance of JsonElement.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonElement::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonElement_get_Value_System_Void() { JsonElement.Value; }
    // Purpose: Benchmark native-runtime performance of JsonElement.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonElement::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonElement_set_Value_System_Void_P0() { JsonElement.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonElement.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonElement::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonElement_get_Count_System_Void() { JsonElement.Count; }
    // Purpose: Benchmark native-runtime performance of JsonElement.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonElement::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonElement_set_Count_System_Void_P0() { JsonElement.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonElement.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonElement::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonElement_get_IsEmpty_System_Void() { JsonElement.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonElement.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonElement::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonElement_set_IsEmpty_System_Void_P0() { JsonElement.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonElement.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonElement::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonElement_get_Key_System_Void() { JsonElement.Key; }
    // Purpose: Benchmark native-runtime performance of JsonElement.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.JsonElement::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_JsonElement_set_Key_System_Void_P0() { JsonElement.Key = null!; }}
