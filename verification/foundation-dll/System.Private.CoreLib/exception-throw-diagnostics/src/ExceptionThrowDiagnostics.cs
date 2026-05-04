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

public partial class ExceptionThrowDiagnostics
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试 — 异常构造与属性读取
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Exception_Ctor_String()
    {
        _ = new Exception("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Exception::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Exception_ToString()
    {
        _ = new Exception("hello").ToString();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ArgumentNullException_Ctor()
    {
        _ = new ArgumentNullException("param");
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新 Host→Patch — 异常构造与属性
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Exception_Ctor_WithInner()
    {
        _ = new Exception("outer", new Exception("inner"));
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Exception::get_Message:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Exception_GetMessage()
    {
        _ = new Exception("hello").Message;
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ExceptionThrowDiagnostics)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_InvalidOperationException_Ctor()
    {
        _ = new InvalidOperationException("invalid");
    }
}
