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

// Benchmark stubs for family/System.Private.CoreLib/dispatch/basic
public static class DispatchBasicBenchmarks
{
    // Benchmark DispatchBasicNativeEntry.Run native-runtime throughput
    [BenchmarkSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::Run:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DispatchBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_DispatchBasicNativeEntry_DispatchBasicNativeEntry_Run_System_Int32_System_Int32() { _ = DispatchBasicNativeEntry.Run(42); }
    // Benchmark DispatchBasicNativeEntry.TestIDispatchAdd native-runtime throughput
    [BenchmarkSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchAdd:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DispatchBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_DispatchBasicNativeEntry_DispatchBasicNativeEntry_TestIDispatchAdd_System_Int32() { _ = DispatchBasicNativeEntry.TestIDispatchAdd(); }
    // Benchmark DispatchBasicNativeEntry.TestIDispatchSub native-runtime throughput
    [BenchmarkSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIDispatchSub:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DispatchBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_DispatchBasicNativeEntry_DispatchBasicNativeEntry_TestIDispatchSub_System_Int32() { _ = DispatchBasicNativeEntry.TestIDispatchSub(); }
    // Benchmark DispatchBasicNativeEntry.TestDualGetStatus native-runtime throughput
    [BenchmarkSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestDualGetStatus:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DispatchBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_DispatchBasicNativeEntry_DispatchBasicNativeEntry_TestDualGetStatus_System_Int32() { _ = DispatchBasicNativeEntry.TestDualGetStatus(); }
    // Benchmark DispatchBasicNativeEntry.TestIUnknownGetValue native-runtime throughput
    [BenchmarkSubjectId("DispatchBasicNativeEntry/DispatchBasicNativeEntry::TestIUnknownGetValue:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DispatchBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_DispatchBasicNativeEntry_DispatchBasicNativeEntry_TestIUnknownGetValue_System_Int32() { _ = DispatchBasicNativeEntry.TestIUnknownGetValue(); }}
