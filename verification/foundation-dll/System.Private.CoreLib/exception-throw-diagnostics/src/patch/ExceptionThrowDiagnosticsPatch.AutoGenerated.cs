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

// Auto-generated patch-side skeletons for ExceptionThrowDiagnosticsPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class ExceptionThrowDiagnosticsPatch
{
    // Purpose: Verify Exception..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String() { new Exception("hello"); }
    // Purpose: Verify Exception..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_ctor_System_Void_System_String_System_Exception() { new Exception("hello", null!); }
    // Purpose: Verify Exception.get_Message executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::get_Message:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_get_Message_System_String() { _ = new Exception().Message; }
    // Purpose: Verify Exception.get_InnerException executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_get_InnerException_System_Exception() { _ = new Exception().InnerException; }
    // Purpose: Verify Exception.get_StackTrace executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::get_StackTrace:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_get_StackTrace_System_String() { }
    // Purpose: Verify Exception.get_HResult executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::get_HResult:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_get_HResult_System_Int32() { _ = new Exception().HResult; }
    // Purpose: Verify Exception.ToString executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_ToString_System_String() { _ = new Exception().ToString(); }
    // Purpose: Verify Exception.GetBaseException executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Exception_GetBaseException_System_Exception() { _ = new Exception().GetBaseException(); }
    // Purpose: Verify ArgumentException..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ArgumentException_ctor_System_Void_System_String_System_String() { new ArgumentException("hello", "hello"); }
    // Purpose: Verify ArgumentNullException..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ArgumentNullException_ctor_System_Void_System_String() { new ArgumentNullException("hello"); }
    // Purpose: Verify ArgumentOutOfRangeException..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_ArgumentOutOfRangeException_ctor_System_Void_System_String() { new ArgumentOutOfRangeException("hello"); }
    // Purpose: Verify InvalidOperationException..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_InvalidOperationException_ctor_System_Void_System_String() { new InvalidOperationException("hello"); }
    // Purpose: Verify NotSupportedException..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_NotSupportedException_ctor_System_Void_System_String() { new NotSupportedException("hello"); }
    // Purpose: Verify NotImplementedException..ctor executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_NotImplementedException_ctor_System_Void_System_String() { new NotImplementedException("hello"); }}
