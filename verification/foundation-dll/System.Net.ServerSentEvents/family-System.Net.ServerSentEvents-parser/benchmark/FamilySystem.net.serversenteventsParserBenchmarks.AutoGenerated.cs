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

// Auto-generated benchmark skeletons for FamilySystem.net.serversenteventsParserBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.net.serversenteventsParserBenchmarks
{
    // Purpose: Benchmark native-runtime performance of SseParser.SseParser: with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::SseParser::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_SseParser_Void_System_Object() { _ = SseParser.SseParser:(42); }
    // Purpose: Benchmark native-runtime performance of SseParser.get_Property with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_get_Property_System_Void() { SseParser.Property; }
    // Purpose: Benchmark native-runtime performance of SseParser.Method1 with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_Method1_System_Void_P0() { SseParser.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of SseParser.Method2 with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_Method2_System_Void_P0_P1() { SseParser.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of SseParser.Validate with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_Validate_System_Void_P0() { SseParser.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of SseParser.get_Value with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_get_Value_System_Void() { SseParser.Value; }
    // Purpose: Benchmark native-runtime performance of SseParser.set_Value with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_set_Value_System_Void_P0() { SseParser.Value = null!; }
    // Purpose: Benchmark native-runtime performance of SseParser.get_Count with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_get_Count_System_Void() { SseParser.Count; }
    // Purpose: Benchmark native-runtime performance of SseParser.set_Count with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_set_Count_System_Void_P0() { SseParser.Count = null!; }
    // Purpose: Benchmark native-runtime performance of SseParser.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_get_IsEmpty_System_Void() { SseParser.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of SseParser.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_set_IsEmpty_System_Void_P0() { SseParser.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of SseParser.get_Key with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_get_Key_System_Void() { SseParser.Key; }
    // Purpose: Benchmark native-runtime performance of SseParser.set_Key with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_set_Key_System_Void_P0() { SseParser.Key = null!; }}
