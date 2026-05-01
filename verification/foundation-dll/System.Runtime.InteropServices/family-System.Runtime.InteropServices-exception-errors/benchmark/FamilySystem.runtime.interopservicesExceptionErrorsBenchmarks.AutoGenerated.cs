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

// Auto-generated benchmark skeletons for FamilySystem.runtime.interopservicesExceptionErrorsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.interopservicesExceptionErrorsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of DllNotFoundException.DllNotFoundException: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.DllNotFoundException::DllNotFoundException::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_DllNotFoundException_DllNotFoundException_Void() { _ = DllNotFoundException.DllNotFoundException:(); }
    // Purpose: Benchmark native-runtime performance of DllNotFoundException.DllNotFoundException: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.DllNotFoundException::DllNotFoundException::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_DllNotFoundException_DllNotFoundException_Void_System_Object() { _ = DllNotFoundException.DllNotFoundException:(42); }
    // Purpose: Benchmark native-runtime performance of DllNotFoundException.DllNotFoundException: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.DllNotFoundException::DllNotFoundException::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_DllNotFoundException_DllNotFoundException_Void_System_Object_System_Object() { _ = DllNotFoundException.DllNotFoundException:(42, 42); }
    // Purpose: Benchmark native-runtime performance of COMException.COMException: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::COMException::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_COMException_COMException_Void() { _ = COMException.COMException:(); }
    // Purpose: Benchmark native-runtime performance of COMException.COMException: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::COMException::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_COMException_COMException_Void_System_Object() { _ = COMException.COMException:(42); }
    // Purpose: Benchmark native-runtime performance of COMException.COMException: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::COMException::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_COMException_COMException_Void_System_Object_System_Object() { _ = COMException.COMException:(42, 42); }
    // Purpose: Benchmark native-runtime performance of COMException.ToString with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::ToString:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_COMException_ToString_System_Void() { COMException.ToString(); }
    // Purpose: Benchmark native-runtime performance of SEHException.SEHException: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::SEHException::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_SEHException_SEHException_Void() { _ = SEHException.SEHException:(); }
    // Purpose: Benchmark native-runtime performance of SEHException.SEHException: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::SEHException::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_SEHException_SEHException_Void_System_Object() { _ = SEHException.SEHException:(42); }
    // Purpose: Benchmark native-runtime performance of SEHException.SEHException: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::SEHException::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_SEHException_SEHException_Void_System_Object_System_Object() { _ = SEHException.SEHException:(42, 42); }
    // Purpose: Benchmark native-runtime performance of SEHException.CanResume with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::CanResume:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_SEHException_CanResume_System_Void() { SEHException.CanResume(); }}
