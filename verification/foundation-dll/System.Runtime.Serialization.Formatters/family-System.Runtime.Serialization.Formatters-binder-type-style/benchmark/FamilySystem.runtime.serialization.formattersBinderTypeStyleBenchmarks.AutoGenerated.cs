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

// Auto-generated benchmark skeletons for FamilySystem.runtime.serialization.formattersBinderTypeStyleBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.serialization.formattersBinderTypeStyleBenchmarks
{
    // Purpose: Benchmark native-runtime performance of SerializationBinder.SerializationBinder: with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::SerializationBinder::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationBinder_SerializationBinder_Void() { _ = SerializationBinder.SerializationBinder:(); }
    // Purpose: Benchmark native-runtime performance of SerializationBinder.BindToName with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToName:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationBinder_BindToName_System_Void_P0_P1_P2() { SerializationBinder.BindToName(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of SerializationBinder.BindToType with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToType:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationBinder_BindToType_System_Void_P0_P1() { SerializationBinder.BindToType(null!, null!); }}
