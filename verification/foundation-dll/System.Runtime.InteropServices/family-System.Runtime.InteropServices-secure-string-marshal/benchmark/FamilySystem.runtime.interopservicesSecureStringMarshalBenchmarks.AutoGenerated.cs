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

// Auto-generated benchmark skeletons for FamilySystem.runtime.interopservicesSecureStringMarshalBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.interopservicesSecureStringMarshalBenchmarks
{
    // Purpose: Benchmark native-runtime performance of SecureStringMarshal.SecureStringToCoTaskMemAnsi with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemAnsi:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToCoTaskMemAnsi_System_Void_P0() { SecureStringMarshal.SecureStringToCoTaskMemAnsi(null!); }
    // Purpose: Benchmark native-runtime performance of SecureStringMarshal.SecureStringToCoTaskMemUnicode with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemUnicode:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToCoTaskMemUnicode_System_Void_P0() { SecureStringMarshal.SecureStringToCoTaskMemUnicode(null!); }
    // Purpose: Benchmark native-runtime performance of SecureStringMarshal.SecureStringToGlobalAllocAnsi with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocAnsi:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToGlobalAllocAnsi_System_Void_P0() { SecureStringMarshal.SecureStringToGlobalAllocAnsi(null!); }
    // Purpose: Benchmark native-runtime performance of SecureStringMarshal.SecureStringToGlobalAllocUnicode with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocUnicode:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToGlobalAllocUnicode_System_Void_P0() { SecureStringMarshal.SecureStringToGlobalAllocUnicode(null!); }}
