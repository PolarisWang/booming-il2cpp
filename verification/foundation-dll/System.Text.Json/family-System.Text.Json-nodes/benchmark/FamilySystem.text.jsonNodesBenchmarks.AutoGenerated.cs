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

// Auto-generated benchmark skeletons for FamilySystem.text.jsonNodesBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.text.jsonNodesBenchmarks
{
    // Purpose: Benchmark native-runtime performance of JsonArray.JsonArray: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonArray::JsonArray::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonArray_JsonArray_Void_System_Object() { _ = JsonArray.JsonArray:(42); }
    // Purpose: Benchmark native-runtime performance of JsonArray.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonArray::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonArray_get_Property_System_Void() { JsonArray.Property; }
    // Purpose: Benchmark native-runtime performance of JsonArray.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonArray::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonArray_Method1_System_Void_P0() { JsonArray.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonArray.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonArray::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonArray_Method2_System_Void_P0_P1() { JsonArray.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonArray.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonArray::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonArray_Validate_System_Void_P0() { JsonArray.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonNode.JsonNode: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonNode::JsonNode::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonNode_JsonNode_Void_System_Object() { _ = JsonNode.JsonNode:(42); }
    // Purpose: Benchmark native-runtime performance of JsonNode.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonNode_get_Property_System_Void() { JsonNode.Property; }
    // Purpose: Benchmark native-runtime performance of JsonNode.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonNode::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonNode_Method1_System_Void_P0() { JsonNode.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonNode.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonNode::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonNode_Method2_System_Void_P0_P1() { JsonNode.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonNode.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonNode::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonNode_Validate_System_Void_P0() { JsonNode.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonObject.JsonObject: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonObject::JsonObject::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonObject_JsonObject_Void_System_Object() { _ = JsonObject.JsonObject:(42); }
    // Purpose: Benchmark native-runtime performance of JsonObject.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonObject::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonObject_get_Property_System_Void() { JsonObject.Property; }
    // Purpose: Benchmark native-runtime performance of JsonObject.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonObject::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonObject_Method1_System_Void_P0() { JsonObject.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonObject.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonObject::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonObject_Method2_System_Void_P0_P1() { JsonObject.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonObject.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonObject::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonObject_Validate_System_Void_P0() { JsonObject.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonValue.JsonValue: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonValue::JsonValue::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonValue_JsonValue_Void_System_Object() { _ = JsonValue.JsonValue:(42); }
    // Purpose: Benchmark native-runtime performance of JsonValue.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonValue::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonValue_get_Property_System_Void() { JsonValue.Property; }
    // Purpose: Benchmark native-runtime performance of JsonValue.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonValue::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonValue_Method1_System_Void_P0() { JsonValue.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonValue.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonValue::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonValue_Method2_System_Void_P0_P1() { JsonValue.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonValue.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonValue::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonValue_Validate_System_Void_P0() { JsonValue.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonArray.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonArray::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonArray_get_Value_System_Void() { JsonArray.Value; }
    // Purpose: Benchmark native-runtime performance of JsonArray.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonArray::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonArray_set_Value_System_Void_P0() { JsonArray.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonArray.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonArray::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonArray_get_Count_System_Void() { JsonArray.Count; }
    // Purpose: Benchmark native-runtime performance of JsonArray.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonArray::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonArray_set_Count_System_Void_P0() { JsonArray.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonArray.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonArray::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonArray_get_IsEmpty_System_Void() { JsonArray.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonArray.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonArray::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonArray_set_IsEmpty_System_Void_P0() { JsonArray.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonArray.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonArray::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonArray_get_Key_System_Void() { JsonArray.Key; }
    // Purpose: Benchmark native-runtime performance of JsonArray.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonArray::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonArray_set_Key_System_Void_P0() { JsonArray.Key = null!; }
    // Purpose: Benchmark native-runtime performance of JsonNode.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonNode_get_Value_System_Void() { JsonNode.Value; }
    // Purpose: Benchmark native-runtime performance of JsonNode.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonNode_set_Value_System_Void_P0() { JsonNode.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonNode.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonNode_get_Count_System_Void() { JsonNode.Count; }
    // Purpose: Benchmark native-runtime performance of JsonNode.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonNode_set_Count_System_Void_P0() { JsonNode.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonNode.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonNode::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonNode_get_IsEmpty_System_Void() { JsonNode.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonNode.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonNode::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonNode_set_IsEmpty_System_Void_P0() { JsonNode.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonNode.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonNode::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonNode_get_Key_System_Void() { JsonNode.Key; }
    // Purpose: Benchmark native-runtime performance of JsonNode.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonNode::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonNode_set_Key_System_Void_P0() { JsonNode.Key = null!; }
    // Purpose: Benchmark native-runtime performance of JsonObject.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonObject::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonObject_get_Value_System_Void() { JsonObject.Value; }
    // Purpose: Benchmark native-runtime performance of JsonObject.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonObject::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonObject_set_Value_System_Void_P0() { JsonObject.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonObject.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonObject::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonObject_get_Count_System_Void() { JsonObject.Count; }
    // Purpose: Benchmark native-runtime performance of JsonObject.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonObject::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonObject_set_Count_System_Void_P0() { JsonObject.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonObject.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonObject::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonObject_get_IsEmpty_System_Void() { JsonObject.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonObject.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonObject::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonObject_set_IsEmpty_System_Void_P0() { JsonObject.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonObject.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonObject::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonObject_get_Key_System_Void() { JsonObject.Key; }
    // Purpose: Benchmark native-runtime performance of JsonObject.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonObject::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonObject_set_Key_System_Void_P0() { JsonObject.Key = null!; }
    // Purpose: Benchmark native-runtime performance of JsonValue.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonValue::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonValue_get_Value_System_Void() { JsonValue.Value; }
    // Purpose: Benchmark native-runtime performance of JsonValue.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonValue::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonValue_set_Value_System_Void_P0() { JsonValue.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonValue.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonValue::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonValue_get_Count_System_Void() { JsonValue.Count; }
    // Purpose: Benchmark native-runtime performance of JsonValue.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonValue::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonValue_set_Count_System_Void_P0() { JsonValue.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonValue.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonValue::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonValue_get_IsEmpty_System_Void() { JsonValue.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonValue.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonValue::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonValue_set_IsEmpty_System_Void_P0() { JsonValue.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonValue.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonValue::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonValue_get_Key_System_Void() { JsonValue.Key; }
    // Purpose: Benchmark native-runtime performance of JsonValue.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Nodes.JsonValue::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Nodes_JsonValue_set_Key_System_Void_P0() { JsonValue.Key = null!; }}
