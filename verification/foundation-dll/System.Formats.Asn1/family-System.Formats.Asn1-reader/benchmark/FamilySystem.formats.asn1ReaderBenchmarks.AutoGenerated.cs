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

// Auto-generated benchmark skeletons for FamilySystem.formats.asn1ReaderBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.formats.asn1ReaderBenchmarks
{
    // Purpose: Benchmark native-runtime performance of AsnReader.AsnReader: with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::AsnReader::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReader_AsnReader_Void_System_Object() { _ = AsnReader.AsnReader:(42); }
    // Purpose: Benchmark native-runtime performance of AsnReader.get_Property with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_Property_System_Void() { AsnReader.Property; }
    // Purpose: Benchmark native-runtime performance of AsnReader.Method1 with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReader_Method1_System_Void_P0() { AsnReader.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of AsnReader.Method2 with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReader_Method2_System_Void_P0_P1() { AsnReader.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of AsnReader.Validate with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReader_Validate_System_Void_P0() { AsnReader.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of AsnReader.get_Value with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_Value_System_Void() { AsnReader.Value; }
    // Purpose: Benchmark native-runtime performance of AsnReader.set_Value with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReader_set_Value_System_Void_P0() { AsnReader.Value = null!; }
    // Purpose: Benchmark native-runtime performance of AsnReader.get_Count with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_Count_System_Void() { AsnReader.Count; }
    // Purpose: Benchmark native-runtime performance of AsnReader.set_Count with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReader_set_Count_System_Void_P0() { AsnReader.Count = null!; }
    // Purpose: Benchmark native-runtime performance of AsnReader.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_IsEmpty_System_Void() { AsnReader.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of AsnReader.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReader_set_IsEmpty_System_Void_P0() { AsnReader.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of AsnReader.get_Key with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReader_get_Key_System_Void() { AsnReader.Key; }
    // Purpose: Benchmark native-runtime performance of AsnReader.set_Key with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReader::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReader_set_Key_System_Void_P0() { AsnReader.Key = null!; }}
