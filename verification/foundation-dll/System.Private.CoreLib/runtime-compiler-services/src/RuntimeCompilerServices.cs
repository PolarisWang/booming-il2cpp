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

public partial class RuntimeCompilerServices
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试 — RuntimeHelpers
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_RuntimeHelpers_GetHashCode()
    {
        _ = RuntimeHelpers.GetHashCode(new object());
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_RuntimeHelpers_Equals()
    {
        var a = new object();
        _ = RuntimeHelpers.Equals(a, a);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_RuntimeHelpers_GetObjectValue()
    {
        _ = RuntimeHelpers.GetObjectValue(42);
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新 Host→Patch — RuntimeHelpers
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_RuntimeHelpers_GetHashCode()
    {
        _ = RuntimeHelpers.GetHashCode(new object());
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_RuntimeHelpers_Equals()
    {
        var a = new object();
        _ = RuntimeHelpers.Equals(a, a);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.FormattableStringFactory::Create:System.Runtime.CompilerServices.FormattableString(System.String,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_RuntimeCompilerServices)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_FormattableStringFactory_Create()
    {
        _ = FormattableStringFactory.Create("Hello {0}", 42);
    }
}
