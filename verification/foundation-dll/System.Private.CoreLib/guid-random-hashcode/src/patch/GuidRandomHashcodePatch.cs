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

public static partial class GuidRandomHashcodePatch
{
    // ============================================================
    // [HotUpdateDirection(PatchToHost)] 热更新补丁侧方法
    // 验证热更新后从 Patch 侧调用宿主侧 Guid/Random/HashCode 方法的正确性
    // ============================================================

    /// <summary>
    /// 验证 Patch 侧调用 Guid.NewGuid 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::NewGuid:System.Guid()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_GuidNewGuid_PatchToHost()
    {
        _ = Guid.NewGuid();
    }

    /// <summary>
    /// 验证 Patch 侧调用 Guid.ToString 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Guid::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_GuidToString_PatchToHost()
    {
        _ = Guid.NewGuid().ToString();
    }

    /// <summary>
    /// 验证 Patch 侧调用 Random.NextDouble 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Random::NextDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_RandomNextDouble_PatchToHost()
    {
        _ = new Random().NextDouble();
    }

    /// <summary>
    /// 验证 Patch 侧调用 HashCode.Add 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.HashCode::Add:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_HashCodeAdd_PatchToHost()
    {
        HashCode hc = default;
        hc.Add("patch-to-host-value");
    }

    /// <summary>
    /// 验证 Patch 侧调用 HashCode.Combine 正确路由到宿主
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_HashCodeCombine_PatchToHost()
    {
        _ = HashCode.Combine(1, 2);
    }
}
