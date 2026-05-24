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

// Benchmark stubs for family/System.Private.CoreLib/error/info/basic
public static class ErrorInfoBasicBenchmarks
{
    // Benchmark ErrorInfoBasicNativeEntry.Run native-runtime throughput
    [BenchmarkSubjectId("ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::Run:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ErrorInfoBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ErrorInfoBasicNativeEntry_ErrorInfoBasicNativeEntry_Run_System_Int32_System_Int32() { _ = ErrorInfoBasicNativeEntry.Run(42); }
    // Benchmark ErrorInfoBasicNativeEntry.TestGetExceptionForHR native-runtime throughput
    [BenchmarkSubjectId("ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetExceptionForHR:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ErrorInfoBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ErrorInfoBasicNativeEntry_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_System_Int32() { _ = ErrorInfoBasicNativeEntry.TestGetExceptionForHR(); }
    // Benchmark ErrorInfoBasicNativeEntry.TestGetHRForException native-runtime throughput
    [BenchmarkSubjectId("ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetHRForException:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ErrorInfoBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ErrorInfoBasicNativeEntry_ErrorInfoBasicNativeEntry_TestGetHRForException_System_Int32() { _ = ErrorInfoBasicNativeEntry.TestGetHRForException(); }
    // Benchmark ErrorInfoBasicNativeEntry.TestThrowExceptionForHR_S_OK native-runtime throughput
    [BenchmarkSubjectId("ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestThrowExceptionForHR_S_OK:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ErrorInfoBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ErrorInfoBasicNativeEntry_ErrorInfoBasicNativeEntry_TestThrowExceptionForHR_S_OK_System_Int32() { _ = ErrorInfoBasicNativeEntry.TestThrowExceptionForHR_S_OK(); }
    // Benchmark ErrorInfoBasicNativeEntry.TestGetExceptionForHR_Zero native-runtime throughput
    [BenchmarkSubjectId("ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetExceptionForHR_Zero:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ErrorInfoBasic)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ErrorInfoBasicNativeEntry_ErrorInfoBasicNativeEntry_TestGetExceptionForHR_Zero_System_Int32() { _ = ErrorInfoBasicNativeEntry.TestGetExceptionForHR_Zero(); }}
