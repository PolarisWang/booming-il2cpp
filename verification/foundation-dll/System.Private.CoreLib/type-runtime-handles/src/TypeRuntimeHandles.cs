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

public partial class TypeRuntimeHandles
{
    // ============================================================
    // [BenchmarkSubjectId] 基准测试 — 运行时类型句柄
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Type_GetTypeFromHandle()
    {
        _ = Type.GetTypeFromHandle(typeof(int).TypeHandle);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Type_IsValueType()
    {
        _ = typeof(int).IsValueType;
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Type::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Type_FullName()
    {
        _ = typeof(int).FullName;
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新 Host→Patch — 类型句柄操作
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Type_GetTypeFromHandle()
    {
        _ = Type.GetTypeFromHandle(typeof(int).TypeHandle);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Type_IsArray()
    {
        _ = typeof(int[]).IsArray;
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Type_GetMethods()
    {
        _ = typeof(string).GetMethods();
    }
}
