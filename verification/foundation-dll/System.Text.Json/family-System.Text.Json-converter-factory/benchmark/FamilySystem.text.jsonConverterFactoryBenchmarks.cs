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

// Auto-generated benchmark skeletons for FamilySystem.text.jsonConverterFactoryBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.text.jsonConverterFactoryBenchmarks
{
    // Purpose: Benchmark native-runtime performance of JsonConverterFactory.JsonConverterFactory: with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::JsonConverterFactory::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactory_JsonConverterFactory_Void_System_Object() { _ = JsonConverterFactory.JsonConverterFactory:(42); }
    // Purpose: Benchmark native-runtime performance of JsonConverterFactory.get_Property with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactory_get_Property_System_Void() { JsonConverterFactory.Property; }
    // Purpose: Benchmark native-runtime performance of JsonConverterFactory.Method1 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactory_Method1_System_Void_P0() { JsonConverterFactory.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of JsonConverterFactory.Method2 with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactory_Method2_System_Void_P0_P1() { JsonConverterFactory.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of JsonConverterFactory.Validate with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactory_Validate_System_Void_P0() { JsonConverterFactory.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of JsonConverterFactory.get_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactory_get_Value_System_Void() { JsonConverterFactory.Value; }
    // Purpose: Benchmark native-runtime performance of JsonConverterFactory.set_Value with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactory_set_Value_System_Void_P0() { JsonConverterFactory.Value = null!; }
    // Purpose: Benchmark native-runtime performance of JsonConverterFactory.get_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactory_get_Count_System_Void() { JsonConverterFactory.Count; }
    // Purpose: Benchmark native-runtime performance of JsonConverterFactory.set_Count with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactory_set_Count_System_Void_P0() { JsonConverterFactory.Count = null!; }
    // Purpose: Benchmark native-runtime performance of JsonConverterFactory.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactory_get_IsEmpty_System_Void() { JsonConverterFactory.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of JsonConverterFactory.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactory_set_IsEmpty_System_Void_P0() { JsonConverterFactory.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of JsonConverterFactory.get_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactory_get_Key_System_Void() { JsonConverterFactory.Key; }
    // Purpose: Benchmark native-runtime performance of JsonConverterFactory.set_Key with typical input
    [BenchmarkSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactory_set_Key_System_Void_P0() { JsonConverterFactory.Key = null!; }}
