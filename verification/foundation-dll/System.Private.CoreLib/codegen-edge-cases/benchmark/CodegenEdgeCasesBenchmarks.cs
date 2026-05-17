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

// Benchmark stubs for family/System.Private.CoreLib/codegen/codegen-edge-cases
public static class CodegenEdgeCasesBenchmarks
{
    // Benchmark multi-latch loop native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_0:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CodegenEdgeCases)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Subject_0() { CodegenEdgeCasesSubjects.Subject_0(); }

    // Benchmark switch fallthrough native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_1:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CodegenEdgeCases)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Subject_1() { CodegenEdgeCasesSubjects.Subject_1(0); }

    // Benchmark deep EH nesting native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_2:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CodegenEdgeCases)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Subject_2() { CodegenEdgeCasesSubjects.Subject_2(); }

    // Benchmark finally control flow native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/CodegenEdgeCasesSubjects::Subject_3:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CodegenEdgeCases)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Subject_3() { CodegenEdgeCasesSubjects.Subject_3(5); }
}
