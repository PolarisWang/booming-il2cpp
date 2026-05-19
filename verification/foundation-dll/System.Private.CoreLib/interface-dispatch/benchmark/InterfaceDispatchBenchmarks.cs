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

// Benchmark stubs for family/System.Private.CoreLib/interface/dispatch
public static class InterfaceDispatchBenchmarks
{
    // Benchmark InterfaceDispatchNativeEntry.Run native-runtime throughput
    [BenchmarkSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::Run:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_Run_System_Int32_System_Int32() { _ = InterfaceDispatchNativeEntry.Run(42); }
    // Benchmark InterfaceDispatchNativeEntry.TestSingleImplSimple native-runtime throughput
    [BenchmarkSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestSingleImplSimple:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestSingleImplSimple_System_Int32() { _ = InterfaceDispatchNativeEntry.TestSingleImplSimple(); }
    // Benchmark InterfaceDispatchNativeEntry.TestSingleImplDefault native-runtime throughput
    [BenchmarkSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestSingleImplDefault:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestSingleImplDefault_System_Int32() { _ = InterfaceDispatchNativeEntry.TestSingleImplDefault(); }
    // Benchmark InterfaceDispatchNativeEntry.TestMultiImplCalc native-runtime throughput
    [BenchmarkSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestMultiImplCalc:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestMultiImplCalc_System_Int32() { _ = InterfaceDispatchNativeEntry.TestMultiImplCalc(); }
    // Benchmark InterfaceDispatchNativeEntry.TestIsCheck native-runtime throughput
    [BenchmarkSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestIsCheck:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestIsCheck_System_Int32() { _ = InterfaceDispatchNativeEntry.TestIsCheck(); }
    // Benchmark InterfaceDispatchNativeEntry.TestAsCheck native-runtime throughput
    [BenchmarkSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestAsCheck:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestAsCheck_System_Int32() { _ = InterfaceDispatchNativeEntry.TestAsCheck(); }
    // Benchmark InterfaceDispatchNativeEntry.TestDiamondBase native-runtime throughput
    [BenchmarkSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondBase:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestDiamondBase_System_Int32() { _ = InterfaceDispatchNativeEntry.TestDiamondBase(); }
    // Benchmark InterfaceDispatchNativeEntry.TestDiamondDerived native-runtime throughput
    [BenchmarkSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondDerived:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestDiamondDerived_System_Int32() { _ = InterfaceDispatchNativeEntry.TestDiamondDerived(); }
    // Benchmark InterfaceDispatchNativeEntry.TestDiamondMulti native-runtime throughput
    [BenchmarkSubjectId("InterfaceDispatchNativeEntry/InterfaceDispatchNativeEntry::TestDiamondMulti:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_InterfaceDispatch)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_InterfaceDispatchNativeEntry_InterfaceDispatchNativeEntry_TestDiamondMulti_System_Int32() { _ = InterfaceDispatchNativeEntry.TestDiamondMulti(); }}
