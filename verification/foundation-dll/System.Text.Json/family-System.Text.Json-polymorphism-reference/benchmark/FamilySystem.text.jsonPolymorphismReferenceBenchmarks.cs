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

// Auto-generated benchmark skeletons for FamilySystem.text.jsonPolymorphismReferenceBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.text.jsonPolymorphismReferenceBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ReferenceHandler.ReferenceHandler: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::ReferenceHandler::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_ReferenceHandler_Void_System_Object() { _ = ReferenceHandler.ReferenceHandler:(42); }
    // Purpose: Benchmark native-runtime performance of ReferenceHandler.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_Property_System_Void() { ReferenceHandler.Property; }
    // Purpose: Benchmark native-runtime performance of ReferenceHandler.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_Method1_System_Void_P0() { ReferenceHandler.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ReferenceHandler.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_Method2_System_Void_P0_P1() { ReferenceHandler.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ReferenceHandler.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_Validate_System_Void_P0() { ReferenceHandler.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ReferenceResolver.ReferenceResolver: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ReferenceResolver::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_ReferenceResolver_Void_System_Object() { _ = ReferenceResolver.ReferenceResolver:(42); }
    // Purpose: Benchmark native-runtime performance of ReferenceResolver.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_Property_System_Void() { ReferenceResolver.Property; }
    // Purpose: Benchmark native-runtime performance of ReferenceResolver.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_Method1_System_Void_P0() { ReferenceResolver.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ReferenceResolver.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_Method2_System_Void_P0_P1() { ReferenceResolver.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ReferenceResolver.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_Validate_System_Void_P0() { ReferenceResolver.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ReferenceHandler.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_Value_System_Void() { ReferenceHandler.Value; }
    // Purpose: Benchmark native-runtime performance of ReferenceHandler.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_set_Value_System_Void_P0() { ReferenceHandler.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ReferenceHandler.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_Count_System_Void() { ReferenceHandler.Count; }
    // Purpose: Benchmark native-runtime performance of ReferenceHandler.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_set_Count_System_Void_P0() { ReferenceHandler.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ReferenceHandler.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_IsEmpty_System_Void() { ReferenceHandler.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ReferenceHandler.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_set_IsEmpty_System_Void_P0() { ReferenceHandler.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ReferenceHandler.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_Key_System_Void() { ReferenceHandler.Key; }
    // Purpose: Benchmark native-runtime performance of ReferenceHandler.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_set_Key_System_Void_P0() { ReferenceHandler.Key = null!; }
    // Purpose: Benchmark native-runtime performance of ReferenceResolver.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_Value_System_Void() { ReferenceResolver.Value; }
    // Purpose: Benchmark native-runtime performance of ReferenceResolver.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_set_Value_System_Void_P0() { ReferenceResolver.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ReferenceResolver.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_Count_System_Void() { ReferenceResolver.Count; }
    // Purpose: Benchmark native-runtime performance of ReferenceResolver.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_set_Count_System_Void_P0() { ReferenceResolver.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ReferenceResolver.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_IsEmpty_System_Void() { ReferenceResolver.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ReferenceResolver.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_set_IsEmpty_System_Void_P0() { ReferenceResolver.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ReferenceResolver.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_Key_System_Void() { ReferenceResolver.Key; }
    // Purpose: Benchmark native-runtime performance of ReferenceResolver.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_set_Key_System_Void_P0() { ReferenceResolver.Key = null!; }}
