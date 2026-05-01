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

// Auto-generated benchmark skeletons for FamilySystem.formats.asn1WriterBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.formats.asn1WriterBenchmarks
{
    // Purpose: Benchmark native-runtime performance of AsnWriter.AsnWriter: with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::AsnWriter::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_AsnWriter_Void_System_Object() { _ = AsnWriter.AsnWriter:(42); }
    // Purpose: Benchmark native-runtime performance of AsnWriter.get_Property with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_get_Property_System_Void() { AsnWriter.Property; }
    // Purpose: Benchmark native-runtime performance of AsnWriter.Method1 with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_Method1_System_Void_P0() { AsnWriter.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of AsnWriter.Method2 with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_Method2_System_Void_P0_P1() { AsnWriter.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of AsnWriter.Validate with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_Validate_System_Void_P0() { AsnWriter.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of AsnWriter.get_Value with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_get_Value_System_Void() { AsnWriter.Value; }
    // Purpose: Benchmark native-runtime performance of AsnWriter.set_Value with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_set_Value_System_Void_P0() { AsnWriter.Value = null!; }
    // Purpose: Benchmark native-runtime performance of AsnWriter.get_Count with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_get_Count_System_Void() { AsnWriter.Count; }
    // Purpose: Benchmark native-runtime performance of AsnWriter.set_Count with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_set_Count_System_Void_P0() { AsnWriter.Count = null!; }
    // Purpose: Benchmark native-runtime performance of AsnWriter.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_get_IsEmpty_System_Void() { AsnWriter.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of AsnWriter.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_set_IsEmpty_System_Void_P0() { AsnWriter.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of AsnWriter.get_Key with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_get_Key_System_Void() { AsnWriter.Key; }
    // Purpose: Benchmark native-runtime performance of AsnWriter.set_Key with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnWriter::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnWriter_set_Key_System_Void_P0() { AsnWriter.Key = null!; }}
