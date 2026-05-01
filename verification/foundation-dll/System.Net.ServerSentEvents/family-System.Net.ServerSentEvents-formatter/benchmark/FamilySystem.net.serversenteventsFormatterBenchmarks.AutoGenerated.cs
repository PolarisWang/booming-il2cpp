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

// Auto-generated benchmark skeletons for FamilySystem.net.serversenteventsFormatterBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.net.serversenteventsFormatterBenchmarks
{
    // Purpose: Benchmark native-runtime performance of SseFormatter.SseFormatter: with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::SseFormatter::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_SseFormatter_Void_System_Object() { _ = SseFormatter.SseFormatter:(42); }
    // Purpose: Benchmark native-runtime performance of SseFormatter.get_Property with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_get_Property_System_Void() { SseFormatter.Property; }
    // Purpose: Benchmark native-runtime performance of SseFormatter.Method1 with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_Method1_System_Void_P0() { SseFormatter.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of SseFormatter.Method2 with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_Method2_System_Void_P0_P1() { SseFormatter.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of SseFormatter.Validate with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_Validate_System_Void_P0() { SseFormatter.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of SseFormatter.get_Value with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_get_Value_System_Void() { SseFormatter.Value; }
    // Purpose: Benchmark native-runtime performance of SseFormatter.set_Value with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_set_Value_System_Void_P0() { SseFormatter.Value = null!; }
    // Purpose: Benchmark native-runtime performance of SseFormatter.get_Count with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_get_Count_System_Void() { SseFormatter.Count; }
    // Purpose: Benchmark native-runtime performance of SseFormatter.set_Count with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_set_Count_System_Void_P0() { SseFormatter.Count = null!; }
    // Purpose: Benchmark native-runtime performance of SseFormatter.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_get_IsEmpty_System_Void() { SseFormatter.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of SseFormatter.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_set_IsEmpty_System_Void_P0() { SseFormatter.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of SseFormatter.get_Key with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_get_Key_System_Void() { SseFormatter.Key; }
    // Purpose: Benchmark native-runtime performance of SseFormatter.set_Key with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseFormatter::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatter_set_Key_System_Void_P0() { SseFormatter.Key = null!; }}
