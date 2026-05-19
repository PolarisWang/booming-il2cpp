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

// Hot-update stubs for family/System.Private.CoreLib/exception/throw-diagnostics
public static class ExceptionThrowDiagnosticsPatch
{
    // Verify Exception..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String() { new Exception("hello"); }
    // Verify Exception..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String_System_Exception() { new Exception("hello", null!); }
    // Verify Exception.get_Message after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::get_Message:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_get_Message_System_String() { _ = new Exception().Message; }
    // Verify Exception.get_InnerException after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_get_InnerException_System_Exception() { _ = new Exception().InnerException; }
    // Verify Exception.get_StackTrace after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::get_StackTrace:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_get_StackTrace_System_String() { _ = ((new Exception().StackTrace) ?? ""); }
    // Verify Exception.get_HResult after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::get_HResult:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_get_HResult_System_Int32() { _ = new Exception().HResult; }
    // Verify Exception.ToString after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_ToString_System_String() { _ = new Exception().ToString(); }
    // Verify Exception.GetBaseException after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_GetBaseException_System_Exception() { _ = new Exception().GetBaseException(); }
    // Verify ArgumentException..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ArgumentException_ctor_System_Void_System_String_System_String() { new ArgumentException("hello", "hello"); }
    // Verify ArgumentNullException..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ArgumentNullException_ctor_System_Void_System_String() { new ArgumentNullException("hello"); }
    // Verify ArgumentOutOfRangeException..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ArgumentOutOfRangeException_ctor_System_Void_System_String() { new ArgumentOutOfRangeException("hello"); }
    // Verify InvalidOperationException..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_InvalidOperationException_ctor_System_Void_System_String() { new InvalidOperationException("hello"); }
    // Verify NotSupportedException..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_NotSupportedException_ctor_System_Void_System_String() { new NotSupportedException("hello"); }
    // Verify NotImplementedException..ctor after hot-update (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_NotImplementedException_ctor_System_Void_System_String() { new NotImplementedException("hello"); }}
