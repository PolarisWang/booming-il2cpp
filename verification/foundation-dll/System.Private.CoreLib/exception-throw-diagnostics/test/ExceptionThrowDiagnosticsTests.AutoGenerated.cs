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
using Xunit;

// Auto-generated skeleton for Exception Throw Diagnostics (family/System.Private.CoreLib/exception/throw-diagnostics).
public partial class ExceptionThrowDiagnosticsTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String()
    {
    new Exception("hello");
    }

    [MethodSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String_System_Exception()
    {
    new Exception("hello", null!);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Exception::get_Message:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Exception_get_Message_System_String()
    {
    var result = new Exception().Message;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Exception_get_InnerException_System_Exception()
    {
    _ = new Exception().InnerException;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Exception::get_StackTrace:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — get_StackTrace with 0 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Exception_get_StackTrace_System_String()
    {
    // TODO: needs-manual — get_StackTrace with 0 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Exception::get_HResult:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Exception_get_HResult_System_Int32()
    {
    var result = new Exception().HResult;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Exception::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Exception_ToString_System_String()
    {
    var result = new Exception().ToString();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Exception_GetBaseException_System_Exception()
    {
    _ = new Exception().GetBaseException();
    }

    [MethodSubjectId("System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_ArgumentException_ctor_System_Void_System_String_System_String()
    {
    new ArgumentException("hello", "hello");
    }

    [MethodSubjectId("System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_ArgumentNullException_ctor_System_Void_System_String()
    {
    new ArgumentNullException("hello");
    }

    [MethodSubjectId("System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_ArgumentOutOfRangeException_ctor_System_Void_System_String()
    {
    new ArgumentOutOfRangeException("hello");
    }

    [MethodSubjectId("System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_InvalidOperationException_ctor_System_Void_System_String()
    {
    new InvalidOperationException("hello");
    }

    [MethodSubjectId("System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_NotSupportedException_ctor_System_Void_System_String()
    {
    new NotSupportedException("hello");
    }

    [MethodSubjectId("System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_NotImplementedException_ctor_System_Void_System_String()
    {
    new NotImplementedException("hello");
    }}
