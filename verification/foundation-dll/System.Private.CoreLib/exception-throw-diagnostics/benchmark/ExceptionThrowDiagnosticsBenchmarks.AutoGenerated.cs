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

// Auto-generated benchmark skeletons for ExceptionThrowDiagnosticsBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class ExceptionThrowDiagnosticsBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Exception..ctor with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String() { new Exception("hello"); }
    // Purpose: Benchmark native-runtime performance of Exception..ctor with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String_System_Exception() { new Exception("hello", null!); }
    // Purpose: Benchmark native-runtime performance of Exception.get_Message with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::get_Message:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_get_Message_System_String() { _ = new Exception().Message; }
    // Purpose: Benchmark native-runtime performance of Exception.get_InnerException with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_get_InnerException_System_Exception() { _ = new Exception().InnerException; }
    // Purpose: Benchmark native-runtime performance of Exception.get_StackTrace with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::get_StackTrace:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_get_StackTrace_System_String() { }
    // Purpose: Benchmark native-runtime performance of Exception.get_HResult with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::get_HResult:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_get_HResult_System_Int32() { _ = new Exception().HResult; }
    // Purpose: Benchmark native-runtime performance of Exception.ToString with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_ToString_System_String() { _ = new Exception().ToString(); }
    // Purpose: Benchmark native-runtime performance of Exception.GetBaseException with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_GetBaseException_System_Exception() { _ = new Exception().GetBaseException(); }
    // Purpose: Benchmark native-runtime performance of ArgumentException..ctor with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ArgumentException_ctor_System_Void_System_String_System_String() { new ArgumentException("hello", "hello"); }
    // Purpose: Benchmark native-runtime performance of ArgumentNullException..ctor with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ArgumentNullException_ctor_System_Void_System_String() { new ArgumentNullException("hello"); }
    // Purpose: Benchmark native-runtime performance of ArgumentOutOfRangeException..ctor with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ArgumentOutOfRangeException_ctor_System_Void_System_String() { new ArgumentOutOfRangeException("hello"); }
    // Purpose: Benchmark native-runtime performance of InvalidOperationException..ctor with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_InvalidOperationException_ctor_System_Void_System_String() { new InvalidOperationException("hello"); }
    // Purpose: Benchmark native-runtime performance of NotSupportedException..ctor with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_NotSupportedException_ctor_System_Void_System_String() { new NotSupportedException("hello"); }
    // Purpose: Benchmark native-runtime performance of NotImplementedException..ctor with typical input
    [BenchmarkSubjectId("System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_NotImplementedException_ctor_System_Void_System_String() { new NotImplementedException("hello"); }}
