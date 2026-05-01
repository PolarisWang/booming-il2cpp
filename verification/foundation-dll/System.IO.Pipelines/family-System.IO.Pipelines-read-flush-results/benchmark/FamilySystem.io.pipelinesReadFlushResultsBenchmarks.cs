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

// Auto-generated benchmark skeletons for FamilySystem.io.pipelinesReadFlushResultsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.io.pipelinesReadFlushResultsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of FlushResult.FlushResult: with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::FlushResult::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_FlushResult_FlushResult_Void_System_Object() { _ = FlushResult.FlushResult:(42); }
    // Purpose: Benchmark native-runtime performance of FlushResult.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_Property_System_Void() { FlushResult.Property; }
    // Purpose: Benchmark native-runtime performance of FlushResult.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_FlushResult_Method1_System_Void_P0() { FlushResult.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of FlushResult.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_FlushResult_Method2_System_Void_P0_P1() { FlushResult.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of FlushResult.Validate with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_FlushResult_Validate_System_Void_P0() { FlushResult.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of ReadResult.ReadResult: with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::ReadResult::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_ReadResult_ReadResult_Void_System_Object() { _ = ReadResult.ReadResult:(42); }
    // Purpose: Benchmark native-runtime performance of ReadResult.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_Property_System_Void() { ReadResult.Property; }
    // Purpose: Benchmark native-runtime performance of ReadResult.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_ReadResult_Method1_System_Void_P0() { ReadResult.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of ReadResult.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_ReadResult_Method2_System_Void_P0_P1() { ReadResult.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ReadResult.Validate with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_ReadResult_Validate_System_Void_P0() { ReadResult.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of FlushResult.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_Value_System_Void() { FlushResult.Value; }
    // Purpose: Benchmark native-runtime performance of FlushResult.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_FlushResult_set_Value_System_Void_P0() { FlushResult.Value = null!; }
    // Purpose: Benchmark native-runtime performance of FlushResult.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_Count_System_Void() { FlushResult.Count; }
    // Purpose: Benchmark native-runtime performance of FlushResult.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_FlushResult_set_Count_System_Void_P0() { FlushResult.Count = null!; }
    // Purpose: Benchmark native-runtime performance of FlushResult.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_IsEmpty_System_Void() { FlushResult.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of FlushResult.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_FlushResult_set_IsEmpty_System_Void_P0() { FlushResult.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of FlushResult.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_FlushResult_get_Key_System_Void() { FlushResult.Key; }
    // Purpose: Benchmark native-runtime performance of FlushResult.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.FlushResult::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_FlushResult_set_Key_System_Void_P0() { FlushResult.Key = null!; }
    // Purpose: Benchmark native-runtime performance of ReadResult.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_Value_System_Void() { ReadResult.Value; }
    // Purpose: Benchmark native-runtime performance of ReadResult.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_ReadResult_set_Value_System_Void_P0() { ReadResult.Value = null!; }
    // Purpose: Benchmark native-runtime performance of ReadResult.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_Count_System_Void() { ReadResult.Count; }
    // Purpose: Benchmark native-runtime performance of ReadResult.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_ReadResult_set_Count_System_Void_P0() { ReadResult.Count = null!; }
    // Purpose: Benchmark native-runtime performance of ReadResult.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_IsEmpty_System_Void() { ReadResult.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of ReadResult.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_ReadResult_set_IsEmpty_System_Void_P0() { ReadResult.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of ReadResult.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_ReadResult_get_Key_System_Void() { ReadResult.Key; }
    // Purpose: Benchmark native-runtime performance of ReadResult.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.ReadResult::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_ReadResult_set_Key_System_Void_P0() { ReadResult.Key = null!; }}
