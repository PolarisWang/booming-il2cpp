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

// Benchmark stubs for family/System.Private.CoreLib/pinvoke/dllimport
public static class PinvokeDllimportBenchmarks
{
    // Benchmark PinvokeDllimportNativeEntry.TestGetCurrentProcess native-runtime throughput
    [BenchmarkSubjectId("PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetCurrentProcess:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PinvokeDllimport)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PinvokeDllimportNativeEntry_PinvokeDllimportNativeEntry_TestGetCurrentProcess_System_Int32() { _ = PinvokeDllimportNativeEntry.TestGetCurrentProcess(); }
    // Benchmark PinvokeDllimportNativeEntry.TestGetCurrentThread native-runtime throughput
    [BenchmarkSubjectId("PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetCurrentThread:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PinvokeDllimport)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PinvokeDllimportNativeEntry_PinvokeDllimportNativeEntry_TestGetCurrentThread_System_Int32() { _ = PinvokeDllimportNativeEntry.TestGetCurrentThread(); }
    // Benchmark PinvokeDllimportNativeEntry.TestGetProcessId native-runtime throughput
    [BenchmarkSubjectId("PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetProcessId:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PinvokeDllimport)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PinvokeDllimportNativeEntry_PinvokeDllimportNativeEntry_TestGetProcessId_System_Int32() { _ = PinvokeDllimportNativeEntry.TestGetProcessId(); }
    // Benchmark PinvokeDllimportNativeEntry.TestGetModuleHandle native-runtime throughput
    [BenchmarkSubjectId("PinvokeDllimportNativeEntry/PinvokeDllimportNativeEntry::TestGetModuleHandle:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PinvokeDllimport)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_PinvokeDllimportNativeEntry_PinvokeDllimportNativeEntry_TestGetModuleHandle_System_Int32() { _ = PinvokeDllimportNativeEntry.TestGetModuleHandle(); }}
