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

public static partial class BoxingUnboxingCastsPatch
{
    // ========== 热更新方法 (Patch -> Host) ==========

    /// <summary>
    /// 测试目的：补丁侧验证 Convert.ChangeType 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ChangeType()
    {
        _ = Convert.ChangeType(42, typeof(byte));
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Convert.ChangeType 带 IFormatProvider 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ChangeType_WithProvider()
    {
        _ = Convert.ChangeType(42, typeof(byte), null!);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Type.IsAssignableFrom 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_IsAssignableFrom()
    {
        _ = typeof(object).IsAssignableFrom(typeof(string));
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Type.IsInstanceOfType 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_IsInstanceOfType()
    {
        _ = typeof(int).IsInstanceOfType(42);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Array.CreateInstance(Type, int) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_CreateInstance_OneDim()
    {
        _ = Array.CreateInstance(typeof(int), 10);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 Array.CreateInstance(Type, int, int) 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_CreateInstance_TwoDim()
    {
        _ = Array.CreateInstance(typeof(int), 3, 4);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 RuntimeHelpers.GetObjectValue 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_GetObjectValue()
    {
        _ = RuntimeHelpers.GetObjectValue(42);
    }

    /// <summary>
    /// 测试目的：补丁侧验证 ValueType.Equals 从补丁调用宿主的正确性
    /// </summary>
    [HotUpdateSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void HotUpdate_ValueType_Equals()
    {
        _ = ((ValueType)42).Equals(42);
    }
}
