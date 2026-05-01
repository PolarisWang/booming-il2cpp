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

// Auto-generated benchmark skeletons for FamilySystem.runtime.serialization.formattersSurrogateSelectionBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.serialization.formattersSurrogateSelectionBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ISerializationSurrogate.GetObjectData with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::GetObjectData:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISerializationSurrogate_GetObjectData_System_Void_P() { ISerializationSurrogate.GetObjectData(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ISerializationSurrogate.SetObjectData with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::SetObjectData:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISerializationSurrogate_SetObjectData_System_Void_P() { ISerializationSurrogate.SetObjectData(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ISurrogateSelector.ChainSelector with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::ChainSelector:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISurrogateSelector_ChainSelector_System_Void_P0() { ISurrogateSelector.ChainSelector(null!); }
    // Purpose: Benchmark native-runtime performance of ISurrogateSelector.GetNextSelector with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetNextSelector:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISurrogateSelector_GetNextSelector_System_Void() { ISurrogateSelector.GetNextSelector(); }
    // Purpose: Benchmark native-runtime performance of ISurrogateSelector.GetSurrogate with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetSurrogate:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_ISurrogateSelector_GetSurrogate_System_Void_P0_P1_P() { ISurrogateSelector.GetSurrogate(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of SurrogateSelector.SurrogateSelector: with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::SurrogateSelector::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_SurrogateSelector_Void() { _ = SurrogateSelector.SurrogateSelector:(); }
    // Purpose: Benchmark native-runtime performance of SurrogateSelector.AddSurrogate with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::AddSurrogate:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_AddSurrogate_System_Void_P0_P1_P2() { SurrogateSelector.AddSurrogate(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of SurrogateSelector.ChainSelector with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::ChainSelector:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_ChainSelector_System_Void_P0() { SurrogateSelector.ChainSelector(null!); }
    // Purpose: Benchmark native-runtime performance of SurrogateSelector.GetNextSelector with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::GetNextSelector:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_GetNextSelector_System_Void() { SurrogateSelector.GetNextSelector(); }
    // Purpose: Benchmark native-runtime performance of SurrogateSelector.GetSurrogate with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::GetSurrogate:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_GetSurrogate_System_Void_P0_P1_P2() { SurrogateSelector.GetSurrogate(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of SurrogateSelector.RemoveSurrogate with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::RemoveSurrogate:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SurrogateSelector_RemoveSurrogate_System_Void_P0_P1() { SurrogateSelector.RemoveSurrogate(null!, null!); }}
