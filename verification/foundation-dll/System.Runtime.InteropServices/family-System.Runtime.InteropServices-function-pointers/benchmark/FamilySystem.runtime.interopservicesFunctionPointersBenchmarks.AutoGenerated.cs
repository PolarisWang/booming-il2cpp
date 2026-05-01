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

// Auto-generated benchmark skeletons for FamilySystem.runtime.interopservicesFunctionPointersBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.interopservicesFunctionPointersBenchmarks
{
    // Purpose: Benchmark native-runtime performance of UnmanagedCallersOnlyAttribute.UnmanagedCallersOnlyAttribute: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.UnmanagedCallersOnlyAttribute::UnmanagedCallersOnlyAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_UnmanagedCallersOnlyAttribute_UnmanagedCallersOnlyAttribut() { _ = UnmanagedCallersOnlyAttribute.UnmanagedCallersOnlyAttribute:(); }}
