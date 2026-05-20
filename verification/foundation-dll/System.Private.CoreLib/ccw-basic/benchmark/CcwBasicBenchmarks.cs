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

// Benchmark stubs for family/System.Private.CoreLib/ccw/basic
public static class CcwBasicBenchmarks
{
    // Benchmark CcwBasicNativeEntry.Run native-runtime throughput
    [BenchmarkSubjectId("CcwBasicNativeEntry/CcwBasicNativeEntry::Run:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CcwBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_CcwBasicNativeEntry_CcwBasicNativeEntry_Run_System_Int32_System_Int32() { _ = CcwBasicNativeEntry.Run(42); }
    // Benchmark CcwBasicNativeEntry.TestSimpleMathAdd native-runtime throughput
    [BenchmarkSubjectId("CcwBasicNativeEntry/CcwBasicNativeEntry::TestSimpleMathAdd:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CcwBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_CcwBasicNativeEntry_CcwBasicNativeEntry_TestSimpleMathAdd_System_Int32() { _ = CcwBasicNativeEntry.TestSimpleMathAdd(); }
    // Benchmark CcwBasicNativeEntry.TestSimpleMathMul native-runtime throughput
    [BenchmarkSubjectId("CcwBasicNativeEntry/CcwBasicNativeEntry::TestSimpleMathMul:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CcwBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_CcwBasicNativeEntry_CcwBasicNativeEntry_TestSimpleMathMul_System_Int32() { _ = CcwBasicNativeEntry.TestSimpleMathMul(); }
    // Benchmark CcwBasicNativeEntry.TestConstantValue native-runtime throughput
    [BenchmarkSubjectId("CcwBasicNativeEntry/CcwBasicNativeEntry::TestConstantValue:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CcwBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_CcwBasicNativeEntry_CcwBasicNativeEntry_TestConstantValue_System_Int32() { _ = CcwBasicNativeEntry.TestConstantValue(); }
    // Benchmark CcwBasicNativeEntry.TestDualInterface native-runtime throughput
    [BenchmarkSubjectId("CcwBasicNativeEntry/CcwBasicNativeEntry::TestDualInterface:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CcwBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_CcwBasicNativeEntry_CcwBasicNativeEntry_TestDualInterface_System_Int32() { _ = CcwBasicNativeEntry.TestDualInterface(); }}
