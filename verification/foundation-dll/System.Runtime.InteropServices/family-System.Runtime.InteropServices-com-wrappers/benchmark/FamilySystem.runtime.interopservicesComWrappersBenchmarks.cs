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

// Auto-generated benchmark skeletons for FamilySystem.runtime.interopservicesComWrappersBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.interopservicesComWrappersBenchmarks
{
    // Purpose: Benchmark native-runtime performance of ComWrappers.ComWrappers: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::ComWrappers::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_ComWrappers_Void() { _ = ComWrappers.ComWrappers:(); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.ComputeVtables with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::ComputeVtables:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_ComputeVtables_System_Void_P0_P1_P2() { ComWrappers.ComputeVtables(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.CreateObject with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::CreateObject:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_CreateObject_System_Void_P0_P1() { ComWrappers.CreateObject(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.CreateObject with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::CreateObject:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_CreateObject_System_Void_P0_P1_P2_P3() { ComWrappers.CreateObject(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.GetIUnknownImpl with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetIUnknownImpl:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetIUnknownImpl_System_Void_P0_P1_P2() { ComWrappers.GetIUnknownImpl(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.GetOrCreateComInterfaceForObject with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateComInterfaceForObject:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrCreateComInterfaceForObject_System_Void_P() { ComWrappers.GetOrCreateComInterfaceForObject(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.GetOrCreateObjectForComInstance with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrCreateObjectForComInstance_System_Void_P0() { ComWrappers.GetOrCreateObjectForComInstance(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.GetOrCreateObjectForComInstance with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrCreateObjectForComInstance:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrCreateObjectForComInstance_System_Void_P0() { ComWrappers.GetOrCreateObjectForComInstance(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.GetOrRegisterObjectForComInstance with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrRegisterObjectForComInstance_System_Void_() { ComWrappers.GetOrRegisterObjectForComInstance(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.GetOrRegisterObjectForComInstance with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::GetOrRegisterObjectForComInstance:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_GetOrRegisterObjectForComInstance_System_Void_() { ComWrappers.GetOrRegisterObjectForComInstance(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.RegisterForMarshalling with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForMarshalling:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_RegisterForMarshalling_System_Void_P0() { ComWrappers.RegisterForMarshalling(null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.RegisterForTrackerSupport with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::RegisterForTrackerSupport:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_RegisterForTrackerSupport_System_Void_P0() { ComWrappers.RegisterForTrackerSupport(null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.ReleaseObjects with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::ReleaseObjects:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_ReleaseObjects_System_Void_P0() { ComWrappers.ReleaseObjects(null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.TryGetComInstance with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetComInstance:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_TryGetComInstance_System_Void_P0_P1() { ComWrappers.TryGetComInstance(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers.TryGetObject with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers::TryGetObject:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_TryGetObject_System_Void_P0_P1() { ComWrappers.TryGetObject(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ComWrappers+ComInterfaceDispatch.GetInstance`1 with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.ComWrappers+ComInterfaceDispatch::GetInstance`1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_ComWrappers_ComInterfaceDispatch_GetInstance_1_System_Void() { ComWrappers+ComInterfaceDispatch.GetInstance`1(null!); }}
