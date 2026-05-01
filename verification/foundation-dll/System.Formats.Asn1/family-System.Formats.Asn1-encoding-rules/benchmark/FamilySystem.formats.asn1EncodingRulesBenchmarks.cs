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

// Auto-generated benchmark skeletons for FamilySystem.formats.asn1EncodingRulesBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.formats.asn1EncodingRulesBenchmarks
{
    // Purpose: Benchmark native-runtime performance of AsnEncodingRules.AsnEncodingRules: with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::AsnEncodingRules::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_AsnEncodingRules_Void_System_Object() { _ = AsnEncodingRules.AsnEncodingRules:(42); }
    // Purpose: Benchmark native-runtime performance of AsnEncodingRules.get_Property with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_get_Property_System_Void() { AsnEncodingRules.Property; }
    // Purpose: Benchmark native-runtime performance of AsnEncodingRules.Method1 with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_Method1_System_Void_P0() { AsnEncodingRules.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of AsnEncodingRules.Method2 with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_Method2_System_Void_P0_P1() { AsnEncodingRules.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of AsnEncodingRules.Validate with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_Validate_System_Void_P0() { AsnEncodingRules.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of AsnEncodingRules.get_Value with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_get_Value_System_Void() { AsnEncodingRules.Value; }
    // Purpose: Benchmark native-runtime performance of AsnEncodingRules.set_Value with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_set_Value_System_Void_P0() { AsnEncodingRules.Value = null!; }
    // Purpose: Benchmark native-runtime performance of AsnEncodingRules.get_Count with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_get_Count_System_Void() { AsnEncodingRules.Count; }
    // Purpose: Benchmark native-runtime performance of AsnEncodingRules.set_Count with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_set_Count_System_Void_P0() { AsnEncodingRules.Count = null!; }
    // Purpose: Benchmark native-runtime performance of AsnEncodingRules.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_get_IsEmpty_System_Void() { AsnEncodingRules.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of AsnEncodingRules.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_set_IsEmpty_System_Void_P0() { AsnEncodingRules.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of AsnEncodingRules.get_Key with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_get_Key_System_Void() { AsnEncodingRules.Key; }
    // Purpose: Benchmark native-runtime performance of AsnEncodingRules.set_Key with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnEncodingRules::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnEncodingRules_set_Key_System_Void_P0() { AsnEncodingRules.Key = null!; }}
