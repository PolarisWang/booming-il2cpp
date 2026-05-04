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

public static partial class TypeRuntimeHandlesPatch
{
    // ============================================================
    // [HotUpdateSubjectId] 热更新 Patch→Host — 类型句柄操作
    // ============================================================

    /// <summary>
    /// Patch 侧验证 Type.GetTypeFromHandle 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Type_GetTypeFromHandle()
    {
        _ = Type.GetTypeFromHandle(typeof(int).TypeHandle);
    }

    /// <summary>
    /// Patch 侧验证 Type.get_IsValueType 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Type_IsValueType()
    {
        _ = typeof(int).IsValueType;
    }

    /// <summary>
    /// Patch 侧验证 Type.get_FullName 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_TypeRuntimeHandles)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Type_FullName()
    {
        _ = typeof(int).FullName;
    }
}
