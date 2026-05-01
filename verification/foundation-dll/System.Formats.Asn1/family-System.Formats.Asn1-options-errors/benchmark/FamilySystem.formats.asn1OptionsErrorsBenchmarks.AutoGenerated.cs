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

// Auto-generated benchmark skeletons for FamilySystem.formats.asn1OptionsErrorsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.formats.asn1OptionsErrorsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of AsnContentException.AsnContentException: with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnContentException::AsnContentException::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnContentException_AsnContentException_Void_System_Object() { _ = AsnContentException.AsnContentException:(42); }
    // Purpose: Benchmark native-runtime performance of AsnContentException.get_Property with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnContentException::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnContentException_get_Property_System_Void() { AsnContentException.Property; }
    // Purpose: Benchmark native-runtime performance of AsnContentException.Method1 with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnContentException::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnContentException_Method1_System_Void_P0() { AsnContentException.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of AsnContentException.Method2 with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnContentException::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnContentException_Method2_System_Void_P0_P1() { AsnContentException.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of AsnContentException.Validate with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnContentException::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnContentException_Validate_System_Void_P0() { AsnContentException.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of AsnReaderOptions.AsnReaderOptions: with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::AsnReaderOptions::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReaderOptions_AsnReaderOptions_Void_System_Object() { _ = AsnReaderOptions.AsnReaderOptions:(42); }
    // Purpose: Benchmark native-runtime performance of AsnReaderOptions.get_Property with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReaderOptions_get_Property_System_Void() { AsnReaderOptions.Property; }
    // Purpose: Benchmark native-runtime performance of AsnReaderOptions.Method1 with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReaderOptions_Method1_System_Void_P0() { AsnReaderOptions.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of AsnReaderOptions.Method2 with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReaderOptions_Method2_System_Void_P0_P1() { AsnReaderOptions.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of AsnReaderOptions.Validate with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReaderOptions_Validate_System_Void_P0() { AsnReaderOptions.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of AsnContentException.get_Value with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnContentException::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnContentException_get_Value_System_Void() { AsnContentException.Value; }
    // Purpose: Benchmark native-runtime performance of AsnContentException.set_Value with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnContentException::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnContentException_set_Value_System_Void_P0() { AsnContentException.Value = null!; }
    // Purpose: Benchmark native-runtime performance of AsnContentException.get_Count with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnContentException::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnContentException_get_Count_System_Void() { AsnContentException.Count; }
    // Purpose: Benchmark native-runtime performance of AsnContentException.set_Count with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnContentException::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnContentException_set_Count_System_Void_P0() { AsnContentException.Count = null!; }
    // Purpose: Benchmark native-runtime performance of AsnContentException.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnContentException::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnContentException_get_IsEmpty_System_Void() { AsnContentException.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of AsnContentException.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnContentException::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnContentException_set_IsEmpty_System_Void_P0() { AsnContentException.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of AsnContentException.get_Key with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnContentException::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnContentException_get_Key_System_Void() { AsnContentException.Key; }
    // Purpose: Benchmark native-runtime performance of AsnContentException.set_Key with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnContentException::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnContentException_set_Key_System_Void_P0() { AsnContentException.Key = null!; }
    // Purpose: Benchmark native-runtime performance of AsnReaderOptions.get_Value with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReaderOptions_get_Value_System_Void() { AsnReaderOptions.Value; }
    // Purpose: Benchmark native-runtime performance of AsnReaderOptions.set_Value with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReaderOptions_set_Value_System_Void_P0() { AsnReaderOptions.Value = null!; }
    // Purpose: Benchmark native-runtime performance of AsnReaderOptions.get_Count with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReaderOptions_get_Count_System_Void() { AsnReaderOptions.Count; }
    // Purpose: Benchmark native-runtime performance of AsnReaderOptions.set_Count with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReaderOptions_set_Count_System_Void_P0() { AsnReaderOptions.Count = null!; }
    // Purpose: Benchmark native-runtime performance of AsnReaderOptions.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReaderOptions_get_IsEmpty_System_Void() { AsnReaderOptions.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of AsnReaderOptions.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReaderOptions_set_IsEmpty_System_Void_P0() { AsnReaderOptions.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of AsnReaderOptions.get_Key with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReaderOptions_get_Key_System_Void() { AsnReaderOptions.Key; }
    // Purpose: Benchmark native-runtime performance of AsnReaderOptions.set_Key with typical input
    [BenchmarkSubjectId("System.Formats.Asn1/System.Formats.Asn1.AsnReaderOptions::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Formats_Asn1_System_Formats_Asn1_AsnReaderOptions_set_Key_System_Void_P0() { AsnReaderOptions.Key = null!; }}
