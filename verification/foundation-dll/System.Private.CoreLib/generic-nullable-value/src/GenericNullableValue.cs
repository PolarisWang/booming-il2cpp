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

public partial class GenericNullableValue
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试 — Nullable 操作
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Nullable_HasValue()
    {
        int? val = 42;
        _ = val.HasValue;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Nullable_GetValueOrDefault()
    {
        int? val = 42;
        _ = val.GetValueOrDefault();
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Nullable`1::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Nullable_ToString()
    {
        int? val = 42;
        _ = val.ToString();
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新 Host→Patch — Nullable 操作
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Nullable_HasValue()
    {
        int? val = 42;
        _ = val.HasValue;
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Nullable_Equals()
    {
        int? val = 42;
        _ = val.Equals(42);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GenericNullableValue)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Nullable_GetHashCode()
    {
        int? val = 42;
        _ = val.GetHashCode();
    }
}
