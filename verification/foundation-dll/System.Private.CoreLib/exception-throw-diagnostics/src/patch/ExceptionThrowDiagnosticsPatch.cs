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

public static partial class ExceptionThrowDiagnosticsPatch
{
    // ============================================================
    // [HotUpdateSubjectId] 热更新 Patch→Host — 异常调用
    // ============================================================

    /// <summary>
    /// Patch 侧验证 Exception..ctor(string) 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Exception_Ctor_String()
    {
        _ = new Exception("hello");
    }

    /// <summary>
    /// Patch 侧验证 Exception.ToString 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Exception_ToString()
    {
        _ = new Exception("hello").ToString();
    }

    /// <summary>
    /// Patch 侧验证 ArgumentNullException..ctor 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_ArgumentNullException_Ctor()
    {
        _ = new ArgumentNullException("param");
    }
}
