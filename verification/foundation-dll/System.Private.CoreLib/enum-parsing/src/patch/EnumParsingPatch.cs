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

public static partial class EnumParsingPatch
{
    // ========== 热更新方法 (Patch -> Host) ==========

    /// <summary>
    /// 测试目的：补丁侧验证 Enum.Parse(Type, string) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_Parse()
    {
        _ = Enum.Parse(typeof(DayOfWeek), "Monday");
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Enum.Parse(Type, string, bool) 忽略大小写从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_Parse_IgnoreCase()
    {
        _ = Enum.Parse(typeof(DayOfWeek), "monday", ignoreCase: true);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Enum.ToString() 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ToString()
    {
        _ = DayOfWeek.Monday.ToString();
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Enum.IsDefined 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_IsDefined()
    {
        _ = Enum.IsDefined(typeof(DayOfWeek), "Monday");
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Enum.GetValues 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_GetValues()
    {
        _ = Enum.GetValues(typeof(DayOfWeek));
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Enum.GetNames 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_GetNames()
    {
        _ = Enum.GetNames(typeof(DayOfWeek));
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Enum.TryParse(Type, string, out object) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_TryParse()
    {
        object result;
        Enum.TryParse(typeof(DayOfWeek), "Tuesday", out result);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Enum.GetName(Type, object) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_GetName()
    {
        _ = Enum.GetName(typeof(DayOfWeek), DayOfWeek.Wednesday);
    }
}
