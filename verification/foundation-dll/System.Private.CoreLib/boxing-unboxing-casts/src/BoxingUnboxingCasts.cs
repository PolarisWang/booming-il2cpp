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

public partial class BoxingUnboxingCasts
{
    // ========== Benchmark 方法 ==========

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_ChangeType()
    {
        _ = Convert.ChangeType(42, typeof(byte));
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_IsAssignableFrom()
    {
        _ = typeof(object).IsAssignableFrom(typeof(string));
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_IsInstanceOfType()
    {
        _ = typeof(int).IsInstanceOfType(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_CreateInstance_OneDim()
    {
        _ = Array.CreateInstance(typeof(int), 10);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_GetObjectValue()
    {
        _ = RuntimeHelpers.GetObjectValue(42);
    }

    // ========== 热更新方法 (Host -> Patch) ==========

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ChangeType()
    {
        _ = Convert.ChangeType(42, typeof(byte));
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_IsAssignableFrom()
    {
        _ = typeof(object).IsAssignableFrom(typeof(string));
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_IsInstanceOfType()
    {
        _ = typeof(int).IsInstanceOfType(42);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_CreateInstance_OneDim()
    {
        _ = Array.CreateInstance(typeof(int), 10);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_GetObjectValue()
    {
        _ = RuntimeHelpers.GetObjectValue(42);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_ValueType_Equals()
    {
        _ = ((ValueType)42).Equals(42);
    }
}
