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

public static partial class ConvertCharPatch
{
    // ========== 热更新方法 (Patch -> Host) ==========

    /// <summary>
    /// 测试目的：补丁侧验证 Convert.ToChar(Boolean) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ToChar_Boolean()
    {
        _ = Convert.ToChar(true);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Convert.ToChar(Byte) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ToChar_Byte()
    {
        _ = Convert.ToChar((byte)65);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Convert.ToChar(Int32) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ToChar_Int32()
    {
        _ = Convert.ToChar(65);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Convert.ToChar(String) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ToChar_String()
    {
        _ = Convert.ToChar("A");
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Convert.ToChar(Double) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ToChar_Double()
    {
        _ = Convert.ToChar(65.0);
    }
}
