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

public partial class DelegateCoreInvocation
{
    // 辅助方法：供委托绑定的实例方法
    private int AddOne(int x) => x + 1;
    private static int StaticAddOne(int x) => x + 1;

    // ============================================================
    // [BenchmarkSubjectId] 基准测试 — 委托创建与调用
    // ============================================================

    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Delegate_Combine()
    {
        Action a = () => { };
        Action b = () => { };
        _ = Delegate.Combine(a, b);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_Delegate_DynamicInvoke()
    {
        Func<int, int> func = x => x + 1;
        _ = func.DynamicInvoke(41);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_MulticastDelegate_GetInvocationList()
    {
        Action a = () => { };
        Action b = () => { };
        var combined = (Action)Delegate.Combine(a, b);
        _ = combined.GetInvocationList();
    }

    // ============================================================
    // [HotUpdateSubjectId] 热更新 Host→Patch — 委托操作
    // ============================================================

    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Delegate_Combine()
    {
        Action a = () => { };
        Action b = () => { };
        _ = Delegate.Combine(a, b);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Delegate_DynamicInvoke()
    {
        Func<int, int> func = x => x + 1;
        _ = func.DynamicInvoke(41);
    }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_Delegate_GetMethod()
    {
        Func<int, int> func = x => x + 1;
        _ = func.Method;
    }
}
