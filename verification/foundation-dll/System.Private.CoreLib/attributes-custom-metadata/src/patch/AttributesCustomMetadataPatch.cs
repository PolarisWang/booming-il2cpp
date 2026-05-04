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

public static partial class AttributesCustomMetadataPatch
{
    // ============================================================
    // [HotUpdateSubjectId] 热更新 Patch→Host — 自定义属性
    // ============================================================

    /// <summary>
    /// Patch 侧验证 Attribute.GetCustomAttribute 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Attribute_GetCustomAttribute()
    {
        _ = typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute));
    }

    /// <summary>
    /// Patch 侧验证 Attribute.IsDefined 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Attribute_IsDefined()
    {
        _ = typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute));
    }

    /// <summary>
    /// Patch 侧验证 Attribute.GetCustomAttributes 调用 Host 侧
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_Attribute_GetCustomAttributes_Assembly()
    {
        _ = typeof(byte).Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute), false);
    }
}
