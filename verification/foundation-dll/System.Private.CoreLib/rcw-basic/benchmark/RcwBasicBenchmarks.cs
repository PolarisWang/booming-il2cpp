using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Benchmark stubs for family/System.Private.CoreLib/rcw/basic
public static class RcwBasicBenchmarks
{
    // Benchmark RcwBasicNativeEntry.Run native-runtime throughput
    [BenchmarkSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::Run:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_RcwBasicNativeEntry_RcwBasicNativeEntry_Run_System_Int32_System_Int32() { _ = RcwBasicNativeEntry.Run(42); }
    // Benchmark RcwBasicNativeEntry.TestRcwRoundTripIdentity native-runtime throughput
    [BenchmarkSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwRoundTripIdentity:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_RcwBasicNativeEntry_RcwBasicNativeEntry_TestRcwRoundTripIdentity_System_Int32() { _ = RcwBasicNativeEntry.TestRcwRoundTripIdentity(); }
    // Benchmark RcwBasicNativeEntry.TestRcwRoundTripQi native-runtime throughput
    [BenchmarkSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwRoundTripQi:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_RcwBasicNativeEntry_RcwBasicNativeEntry_TestRcwRoundTripQi_System_Int32() { _ = RcwBasicNativeEntry.TestRcwRoundTripQi(); }
    // Benchmark RcwBasicNativeEntry.TestRcwMultipleWrappers native-runtime throughput
    [BenchmarkSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwMultipleWrappers:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_RcwBasicNativeEntry_RcwBasicNativeEntry_TestRcwMultipleWrappers_System_Int32() { _ = RcwBasicNativeEntry.TestRcwMultipleWrappers(); }
    // Benchmark RcwBasicNativeEntry.TestRcwQiUnknownInterface native-runtime throughput
    [BenchmarkSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwQiUnknownInterface:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_RcwBasicNativeEntry_RcwBasicNativeEntry_TestRcwQiUnknownInterface_System_Int32() { _ = RcwBasicNativeEntry.TestRcwQiUnknownInterface(); }
    // Benchmark RcwBasicNativeEntry.TestRcwVtableMethodCall native-runtime throughput
    [BenchmarkSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwVtableMethodCall:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_RcwBasicNativeEntry_RcwBasicNativeEntry_TestRcwVtableMethodCall_System_Int32() { _ = RcwBasicNativeEntry.TestRcwVtableMethodCall(); }
    // Benchmark RcwBasicNativeEntry.TestRcwDirectVtable native-runtime throughput
    [BenchmarkSubjectId("RcwBasicNativeEntry/RcwBasicNativeEntry::TestRcwDirectVtable:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RcwBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_RcwBasicNativeEntry_RcwBasicNativeEntry_TestRcwDirectVtable_System_Int32() { _ = RcwBasicNativeEntry.TestRcwDirectVtable(); }}
