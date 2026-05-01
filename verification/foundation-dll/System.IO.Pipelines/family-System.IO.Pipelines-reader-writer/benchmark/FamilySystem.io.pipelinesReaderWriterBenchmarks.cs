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

// Auto-generated benchmark skeletons for FamilySystem.io.pipelinesReaderWriterBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.io.pipelinesReaderWriterBenchmarks
{
    // Purpose: Benchmark native-runtime performance of PipeReader.PipeReader: with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::PipeReader::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeReader_PipeReader_Void_System_Object() { _ = PipeReader.PipeReader:(42); }
    // Purpose: Benchmark native-runtime performance of PipeReader.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_Property_System_Void() { PipeReader.Property; }
    // Purpose: Benchmark native-runtime performance of PipeReader.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeReader_Method1_System_Void_P0() { PipeReader.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of PipeReader.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeReader_Method2_System_Void_P0_P1() { PipeReader.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of PipeReader.Validate with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeReader_Validate_System_Void_P0() { PipeReader.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of PipeWriter.PipeWriter: with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::PipeWriter::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_PipeWriter_Void_System_Object() { _ = PipeWriter.PipeWriter:(42); }
    // Purpose: Benchmark native-runtime performance of PipeWriter.get_Property with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_Property_System_Void() { PipeWriter.Property; }
    // Purpose: Benchmark native-runtime performance of PipeWriter.Method1 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_Method1_System_Void_P0() { PipeWriter.Method1(null!); }
    // Purpose: Benchmark native-runtime performance of PipeWriter.Method2 with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_Method2_System_Void_P0_P1() { PipeWriter.Method2(null!, null!); }
    // Purpose: Benchmark native-runtime performance of PipeWriter.Validate with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_Validate_System_Void_P0() { PipeWriter.Validate(null!); }
    // Purpose: Benchmark native-runtime performance of PipeReader.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_Value_System_Void() { PipeReader.Value; }
    // Purpose: Benchmark native-runtime performance of PipeReader.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeReader_set_Value_System_Void_P0() { PipeReader.Value = null!; }
    // Purpose: Benchmark native-runtime performance of PipeReader.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_Count_System_Void() { PipeReader.Count; }
    // Purpose: Benchmark native-runtime performance of PipeReader.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeReader_set_Count_System_Void_P0() { PipeReader.Count = null!; }
    // Purpose: Benchmark native-runtime performance of PipeReader.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_IsEmpty_System_Void() { PipeReader.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of PipeReader.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeReader_set_IsEmpty_System_Void_P0() { PipeReader.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of PipeReader.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeReader_get_Key_System_Void() { PipeReader.Key; }
    // Purpose: Benchmark native-runtime performance of PipeReader.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeReader::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeReader_set_Key_System_Void_P0() { PipeReader.Key = null!; }
    // Purpose: Benchmark native-runtime performance of PipeWriter.get_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_Value_System_Void() { PipeWriter.Value; }
    // Purpose: Benchmark native-runtime performance of PipeWriter.set_Value with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_set_Value_System_Void_P0() { PipeWriter.Value = null!; }
    // Purpose: Benchmark native-runtime performance of PipeWriter.get_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_Count_System_Void() { PipeWriter.Count; }
    // Purpose: Benchmark native-runtime performance of PipeWriter.set_Count with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_set_Count_System_Void_P0() { PipeWriter.Count = null!; }
    // Purpose: Benchmark native-runtime performance of PipeWriter.get_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_IsEmpty_System_Void() { PipeWriter.IsEmpty; }
    // Purpose: Benchmark native-runtime performance of PipeWriter.set_IsEmpty with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_set_IsEmpty_System_Void_P0() { PipeWriter.IsEmpty = null!; }
    // Purpose: Benchmark native-runtime performance of PipeWriter.get_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_get_Key_System_Void() { PipeWriter.Key; }
    // Purpose: Benchmark native-runtime performance of PipeWriter.set_Key with typical input
    [BenchmarkSubjectId("System.IO.Pipelines/System.IO.Pipelines.PipeWriter::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_IO_Pipelines_System_IO_Pipelines_PipeWriter_set_Key_System_Void_P0() { PipeWriter.Key = null!; }}
