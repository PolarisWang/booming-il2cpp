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

public static partial class DelegateCoreInvocationPatch
{
    // ============================================================
    // [HotUpdateSubjectId] 热更新 Patch→Host — 委托操作
    // ============================================================

    /// <summary>
    /// Patch 侧验证 Delegate.Combine 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Delegate_Combine()
    {
        Action a = () => { };
        Action b = () => { };
        _ = Delegate.Combine(a, b);
    }

    /// <summary>
    /// Patch 侧验证 Delegate.DynamicInvoke 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Delegate_DynamicInvoke()
    {
        Func<int, int> func = x => x + 1;
        _ = func.DynamicInvoke(41);
    }

    /// <summary>
    /// Patch 侧验证 Delegate.get_Target 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Delegate::get_Target:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_DelegateCoreInvocation)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Delegate_GetTarget()
    {
        Action a = () => { };
        _ = a.Target;
    }
}
