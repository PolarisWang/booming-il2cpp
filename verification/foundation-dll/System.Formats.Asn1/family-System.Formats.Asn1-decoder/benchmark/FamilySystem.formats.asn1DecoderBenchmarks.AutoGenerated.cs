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

// Auto-generated benchmark skeletons for FamilySystem.formats.asn1DecoderBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.formats.asn1DecoderBenchmarks
{
    // Purpose: Benchmark native-runtime performance of AsnDecoder.AsnDecoder: with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::AsnDecoder::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_AsnDecoder_Void_System_Object() { _ = AsnDecoder.AsnDecoder:(42); }
    // Purpose: Benchmark native-runtime performance of AsnDecoder.get_Property with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_get_Property_System_Void() { AsnDecoder.Property; }
    // Purpose: Benchmark native-runtime performance of AsnDecoder.Method1 with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_Method1_System_Void_P0() { AsnDecoder.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of AsnDecoder.Method2 with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_Method2_System_Void_P0_P1() { AsnDecoder.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of AsnDecoder.Validate with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_Validate_System_Void_P0() { AsnDecoder.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of AsnDecoder.get_Value with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_get_Value_System_Void() { AsnDecoder.Value; }
    // Purpose: Benchmark native-runtime performance of AsnDecoder.set_Value with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_set_Value_System_Void_P0() { AsnDecoder.Value = null!; }
    // Purpose: Benchmark native-runtime performance of AsnDecoder.get_Count with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_get_Count_System_Void() { AsnDecoder.Count; }
    // Purpose: Benchmark native-runtime performance of AsnDecoder.set_Count with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_set_Count_System_Void_P0() { AsnDecoder.Count = null!; }
    // Purpose: Benchmark native-runtime performance of AsnDecoder.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_get_IsEmpty_System_Void() { AsnDecoder.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of AsnDecoder.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_set_IsEmpty_System_Void_P0() { AsnDecoder.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of AsnDecoder.get_Key with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_get_Key_System_Void() { AsnDecoder.Key; }
    // Purpose: Benchmark native-runtime performance of AsnDecoder.set_Key with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnDecoder::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnDecoder_set_Key_System_Void_P0() { AsnDecoder.Key = null!; }}
