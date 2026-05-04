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

public static partial class PrimitiveNumericConversionsPatch
{
    // ========== 热更新方法 (Patch -> Host) ==========

    /// <summary>
    /// 测试目的：补丁侧验证 Convert.ToInt32(String) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ToInt32_String()
    {
        _ = Convert.ToInt32("12345");
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Convert.ToInt64(String) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ToInt64_String()
    {
        _ = Convert.ToInt64("9876543210");
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Convert.ToDouble(String) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ToDouble_String()
    {
        _ = Convert.ToDouble("3.14159");
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Convert.ToSingle(String) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ToSingle_String()
    {
        _ = Convert.ToSingle("2.71828");
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Convert.ToString(Int32) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ToString_Int32()
    {
        _ = Convert.ToString(42);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Int32.Parse 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_Int32_Parse()
    {
        _ = int.Parse("1024");
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Int64.Parse 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_Int64_Parse()
    {
        _ = long.Parse("9876543210");
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Double.Parse 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Double::Parse:System.Double(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_Double_Parse()
    {
        _ = double.Parse("3.14159");
    }
}
