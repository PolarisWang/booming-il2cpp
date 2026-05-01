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

// Auto-generated benchmark skeletons for FamilySystem.net.serversenteventsItemParserBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.net.serversenteventsItemParserBenchmarks
{
    // Purpose: Benchmark native-runtime performance of SseItemParser.SseItemParser: with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser::SseItemParser::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItemParser_SseItemParser_Void_System_Object() { _ = SseItemParser.SseItemParser:(42); }
    // Purpose: Benchmark native-runtime performance of SseItemParser.get_Property with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItemParser_get_Property_System_Void() { SseItemParser.Property; }
    // Purpose: Benchmark native-runtime performance of SseItemParser.Method1 with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItemParser_Method1_System_Void_P0() { SseItemParser.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of SseItemParser.Method2 with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItemParser_Method2_System_Void_P0_P1() { SseItemParser.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of SseItemParser.Validate with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItemParser_Validate_System_Void_P0() { SseItemParser.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of SseItemParser.get_Value with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItemParser_get_Value_System_Void() { SseItemParser.Value; }
    // Purpose: Benchmark native-runtime performance of SseItemParser.set_Value with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItemParser_set_Value_System_Void_P0() { SseItemParser.Value = null!; }
    // Purpose: Benchmark native-runtime performance of SseItemParser.get_Count with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItemParser_get_Count_System_Void() { SseItemParser.Count; }
    // Purpose: Benchmark native-runtime performance of SseItemParser.set_Count with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItemParser_set_Count_System_Void_P0() { SseItemParser.Count = null!; }
    // Purpose: Benchmark native-runtime performance of SseItemParser.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItemParser_get_IsEmpty_System_Void() { SseItemParser.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of SseItemParser.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItemParser_set_IsEmpty_System_Void_P0() { SseItemParser.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of SseItemParser.get_Key with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItemParser_get_Key_System_Void() { SseItemParser.Key; }
    // Purpose: Benchmark native-runtime performance of SseItemParser.set_Key with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItemParser::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItemParser_set_Key_System_Void_P0() { SseItemParser.Key = null!; }}
