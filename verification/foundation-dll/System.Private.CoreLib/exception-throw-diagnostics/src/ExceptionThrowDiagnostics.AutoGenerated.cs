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

// Auto-generated skeleton for Exception Throw Diagnostics (family/System.Private.CoreLib/exception/throw-diagnostics).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class ExceptionThrowDiagnostics
{
    // Exception..ctor
    [MethodSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String()
    {
    new Exception("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String() { new Exception("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String() { new Exception("hello"); }

    // Exception..ctor
    [MethodSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String_System_Exception()
    {
    new Exception("hello", null!);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String_System_Exception() { new Exception("hello", null!); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String_System_Exception() { new Exception("hello", null!); }

    // Exception.get_Message
    [MethodSubjectId("System.Private.CoreLib/System.Exception::get_Message:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Exception_get_Message_System_String()
    {
    var result = new Exception().Message;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::get_Message:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_get_Message_System_String() { _ = new Exception().Message; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::get_Message:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Exception_get_Message_System_String() { _ = new Exception().Message; }

    // Exception.get_InnerException
    [MethodSubjectId("System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Exception_get_InnerException_System_Exception()
    {
    _ = new Exception().InnerException;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_get_InnerException_System_Exception() { _ = new Exception().InnerException; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Exception_get_InnerException_System_Exception() { _ = new Exception().InnerException; }

    // Exception.get_StackTrace
    [MethodSubjectId("System.Private.CoreLib/System.Exception::get_StackTrace:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Exception_get_StackTrace_System_String()
    {
    // TODO: needs-manual — get_StackTrace with 0 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::get_StackTrace:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_get_StackTrace_System_String() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::get_StackTrace:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Exception_get_StackTrace_System_String() { }

    // Exception.get_HResult
    [MethodSubjectId("System.Private.CoreLib/System.Exception::get_HResult:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Exception_get_HResult_System_Int32()
    {
    var result = new Exception().HResult;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::get_HResult:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_get_HResult_System_Int32() { _ = new Exception().HResult; }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::get_HResult:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Exception_get_HResult_System_Int32() { _ = new Exception().HResult; }

    // Exception.ToString
    [MethodSubjectId("System.Private.CoreLib/System.Exception::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Exception_ToString_System_String()
    {
    var result = new Exception().ToString();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_ToString_System_String() { _ = new Exception().ToString(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Exception_ToString_System_String() { _ = new Exception().ToString(); }

    // Exception.GetBaseException
    [MethodSubjectId("System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Exception_GetBaseException_System_Exception()
    {
    _ = new Exception().GetBaseException();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Exception_GetBaseException_System_Exception() { _ = new Exception().GetBaseException(); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Exception_GetBaseException_System_Exception() { _ = new Exception().GetBaseException(); }

    // ArgumentException..ctor
    [MethodSubjectId("System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_ArgumentException_ctor_System_Void_System_String_System_String()
    {
    new ArgumentException("hello", "hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ArgumentException_ctor_System_Void_System_String_System_String() { new ArgumentException("hello", "hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_ArgumentException_ctor_System_Void_System_String_System_String() { new ArgumentException("hello", "hello"); }

    // ArgumentNullException..ctor
    [MethodSubjectId("System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_ArgumentNullException_ctor_System_Void_System_String()
    {
    new ArgumentNullException("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ArgumentNullException_ctor_System_Void_System_String() { new ArgumentNullException("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_ArgumentNullException_ctor_System_Void_System_String() { new ArgumentNullException("hello"); }

    // ArgumentOutOfRangeException..ctor
    [MethodSubjectId("System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_ArgumentOutOfRangeException_ctor_System_Void_System_String()
    {
    new ArgumentOutOfRangeException("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_ArgumentOutOfRangeException_ctor_System_Void_System_String() { new ArgumentOutOfRangeException("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_ArgumentOutOfRangeException_ctor_System_Void_System_String() { new ArgumentOutOfRangeException("hello"); }

    // InvalidOperationException..ctor
    [MethodSubjectId("System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_InvalidOperationException_ctor_System_Void_System_String()
    {
    new InvalidOperationException("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_InvalidOperationException_ctor_System_Void_System_String() { new InvalidOperationException("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_InvalidOperationException_ctor_System_Void_System_String() { new InvalidOperationException("hello"); }

    // NotSupportedException..ctor
    [MethodSubjectId("System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_NotSupportedException_ctor_System_Void_System_String()
    {
    new NotSupportedException("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_NotSupportedException_ctor_System_Void_System_String() { new NotSupportedException("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_NotSupportedException_ctor_System_Void_System_String() { new NotSupportedException("hello"); }

    // NotImplementedException..ctor
    [MethodSubjectId("System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_NotImplementedException_ctor_System_Void_System_String()
    {
    new NotImplementedException("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_NotImplementedException_ctor_System_Void_System_String() { new NotImplementedException("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_NotImplementedException_ctor_System_Void_System_String() { new NotImplementedException("hello"); }
}
