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

// Auto-generated benchmark skeletons for FamilySystem.net.serversenteventsSseItemBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.net.serversenteventsSseItemBenchmarks
{
    // Purpose: Benchmark native-runtime performance of SseItem.SseItem: with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::SseItem::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_SseItem_Void_System_Object() { _ = SseItem.SseItem:(42); }
    // Purpose: Benchmark native-runtime performance of SseItem.get_Property with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_Property_System_Void() { SseItem.Property; }
    // Purpose: Benchmark native-runtime performance of SseItem.Method1 with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_Method1_System_Void_P0() { SseItem.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of SseItem.Method2 with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_Method2_System_Void_P0_P1() { SseItem.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of SseItem.Validate with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_Validate_System_Void_P0() { SseItem.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of SseItem.get_Value with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_Value_System_Void() { SseItem.Value; }
    // Purpose: Benchmark native-runtime performance of SseItem.set_Value with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_set_Value_System_Void_P0() { SseItem.Value = null!; }
    // Purpose: Benchmark native-runtime performance of SseItem.get_Count with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_Count_System_Void() { SseItem.Count; }
    // Purpose: Benchmark native-runtime performance of SseItem.set_Count with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_set_Count_System_Void_P0() { SseItem.Count = null!; }
    // Purpose: Benchmark native-runtime performance of SseItem.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_IsEmpty_System_Void() { SseItem.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of SseItem.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_set_IsEmpty_System_Void_P0() { SseItem.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of SseItem.get_Key with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_Key_System_Void() { SseItem.Key; }
    // Purpose: Benchmark native-runtime performance of SseItem.set_Key with typical input
    [BenchmarkSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_set_Key_System_Void_P0() { SseItem.Key = null!; }}
