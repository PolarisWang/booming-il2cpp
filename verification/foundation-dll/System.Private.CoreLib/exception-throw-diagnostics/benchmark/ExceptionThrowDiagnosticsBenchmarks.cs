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

// Benchmark stubs for family/System.Private.CoreLib/exception/throw-diagnostics
public static class ExceptionThrowDiagnosticsBenchmarks
{
    // Benchmark Exception..ctor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String() { new Exception("hello"); }
    // Benchmark Exception..ctor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String_System_Exception() { new Exception("hello", default); }
    // Benchmark Exception.get_Message native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::get_Message:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_get_Message_System_String() { _ = new Exception().Message; }
    // Benchmark Exception.get_InnerException native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_get_InnerException_System_Exception() { _ = new Exception().InnerException; }
    // Benchmark Exception.get_StackTrace native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::get_StackTrace:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_get_StackTrace_System_String() { _ = ((new Exception().StackTrace) ?? ""); }
    // Benchmark Exception.get_HResult native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::get_HResult:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_get_HResult_System_Int32() { _ = new Exception().HResult; }
    // Benchmark Exception.ToString native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_ToString_System_String() { _ = new Exception().ToString(); }
    // Benchmark Exception.GetBaseException native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_GetBaseException_System_Exception() { _ = new Exception().GetBaseException(); }
    // Benchmark ArgumentException..ctor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ArgumentException_ctor_System_Void_System_String_System_String() { new ArgumentException("hello", "hello"); }
    // Benchmark ArgumentNullException..ctor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ArgumentNullException_ctor_System_Void_System_String() { new ArgumentNullException("hello"); }
    // Benchmark ArgumentOutOfRangeException..ctor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ArgumentOutOfRangeException_ctor_System_Void_System_String() { new ArgumentOutOfRangeException("hello"); }
    // Benchmark InvalidOperationException..ctor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_InvalidOperationException_ctor_System_Void_System_String() { new InvalidOperationException("hello"); }
    // Benchmark NotSupportedException..ctor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_NotSupportedException_ctor_System_Void_System_String() { new NotSupportedException("hello"); }
    // Benchmark NotImplementedException..ctor native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_NotImplementedException_ctor_System_Void_System_String() { new NotImplementedException("hello"); }
    // Benchmark OSR.HotLoop native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/OSR::HotLoop:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_OSR_HotLoop_System_Void() { OSR.HotLoop(); }
    // Benchmark Memory.CopyBlock native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/Memory::CopyBlock:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_Memory_CopyBlock_System_Void() { default(Memory<byte>).CopyBlock(); }
    // Benchmark Memory.InitBlock native-runtime throughput
    [BenchmarkSubjectId("System.Private.CoreLib/Memory::InitBlock:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_Memory_InitBlock_System_Void() { default(Memory<byte>).InitBlock(); }}
