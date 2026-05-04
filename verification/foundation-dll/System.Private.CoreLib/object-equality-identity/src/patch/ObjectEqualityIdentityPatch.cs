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

public static partial class ObjectEqualityIdentityPatch
{
    // ============================================================
    // [HotUpdateSubjectId] 热更新 Patch→Host — 对象相等与标识
    // ============================================================

    /// <summary>
    /// Patch 侧验证 Object.Equals 实例方法调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Object_Equals_Instance()
    {
        _ = new object().Equals(42);
    }

    /// <summary>
    /// Patch 侧验证 Object.ReferenceEquals 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Object_ReferenceEquals()
    {
        var a = new object();
        _ = Object.ReferenceEquals(a, a);
    }

    /// <summary>
    /// Patch 侧验证 Object.GetType 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Object::GetType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ObjectEqualityIdentity)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Object_GetType()
    {
        _ = new object().GetType();
    }
}
