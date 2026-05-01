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

// Auto-generated hot-update skeletons for FamilySystem.runtime.serialization.formattersFormatterBaseHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.runtime.serialization.formattersFormatterBaseHotUpdate
{
    // Purpose: Verify Formatter.Formatter: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Formatter::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_Formatter_Void() { _ = Formatter.Formatter:(); }
    // Purpose: Verify Formatter.Deserialize executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Deserialize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_Deserialize_System_Void_P0() { Formatter.Deserialize(null!); }
    // Purpose: Verify Formatter.GetNext executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::GetNext:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_GetNext_System_Void_P0() { Formatter.GetNext(null!); }
    // Purpose: Verify Formatter.Schedule executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Schedule:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_Schedule_System_Void_P0() { Formatter.Schedule(null!); }
    // Purpose: Verify Formatter.Serialize executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Serialize:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_Serialize_System_Void_P0_P1() { Formatter.Serialize(null!, null!); }
    // Purpose: Verify Formatter.WriteArray executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteArray:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteArray_System_Void_P0_P1_P2() { Formatter.WriteArray(null!, null!, null!); }
    // Purpose: Verify Formatter.WriteBoolean executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteBoolean:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteBoolean_System_Void_P0_P1() { Formatter.WriteBoolean(null!, null!); }
    // Purpose: Verify Formatter.WriteByte executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteByte:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteByte_System_Void_P0_P1() { Formatter.WriteByte(null!, null!); }
    // Purpose: Verify Formatter.WriteChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteChar:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteChar_System_Void_P0_P1() { Formatter.WriteChar(null!, null!); }
    // Purpose: Verify Formatter.WriteDateTime executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteDateTime:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteDateTime_System_Void_P0_P1() { Formatter.WriteDateTime(null!, null!); }
    // Purpose: Verify Formatter.WriteDecimal executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteDecimal:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteDecimal_System_Void_P0_P1() { Formatter.WriteDecimal(null!, null!); }
    // Purpose: Verify Formatter.WriteDouble executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteDouble:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteDouble_System_Void_P0_P1() { Formatter.WriteDouble(null!, null!); }
    // Purpose: Verify Formatter.WriteInt16 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteInt16:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteInt16_System_Void_P0_P1() { Formatter.WriteInt16(null!, null!); }
    // Purpose: Verify Formatter.WriteInt32 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteInt32:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteInt32_System_Void_P0_P1() { Formatter.WriteInt32(null!, null!); }
    // Purpose: Verify Formatter.WriteInt64 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteInt64:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteInt64_System_Void_P0_P1() { Formatter.WriteInt64(null!, null!); }
    // Purpose: Verify Formatter.WriteMember executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteMember:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteMember_System_Void_P0_P1() { Formatter.WriteMember(null!, null!); }
    // Purpose: Verify Formatter.WriteObjectRef executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteObjectRef:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteObjectRef_System_Void_P0_P1_P2() { Formatter.WriteObjectRef(null!, null!, null!); }
    // Purpose: Verify Formatter.WriteSByte executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteSByte:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteSByte_System_Void_P0_P1() { Formatter.WriteSByte(null!, null!); }
    // Purpose: Verify Formatter.WriteSingle executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteSingle:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteSingle_System_Void_P0_P1() { Formatter.WriteSingle(null!, null!); }
    // Purpose: Verify Formatter.WriteTimeSpan executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteTimeSpan:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteTimeSpan_System_Void_P0_P1() { Formatter.WriteTimeSpan(null!, null!); }
    // Purpose: Verify Formatter.WriteUInt16 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteUInt16:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteUInt16_System_Void_P0_P1() { Formatter.WriteUInt16(null!, null!); }
    // Purpose: Verify Formatter.WriteUInt32 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteUInt32:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteUInt32_System_Void_P0_P1() { Formatter.WriteUInt32(null!, null!); }
    // Purpose: Verify Formatter.WriteUInt64 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteUInt64:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteUInt64_System_Void_P0_P1() { Formatter.WriteUInt64(null!, null!); }
    // Purpose: Verify Formatter.WriteValueType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteValueType:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteValueType_System_Void_P0_P1_P2() { Formatter.WriteValueType(null!, null!, null!); }
    // Purpose: Verify Formatter.get_Binder executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Binder:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_get_Binder_System_Void() { Formatter.Binder; }
    // Purpose: Verify Formatter.get_Context executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Context:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_get_Context_System_Void() { Formatter.Context; }
    // Purpose: Verify Formatter.get_SurrogateSelector executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_SurrogateSelector:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_get_SurrogateSelector_System_Void() { Formatter.SurrogateSelector; }
    // Purpose: Verify Formatter.set_Binder executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Binder:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_set_Binder_System_Void_P0() { Formatter.Binder = null!; }
    // Purpose: Verify Formatter.set_Context executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Context:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_set_Context_System_Void_P0() { Formatter.Context = null!; }
    // Purpose: Verify Formatter.set_SurrogateSelector executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_SurrogateSelector:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_set_SurrogateSelector_System_Void_P0() { Formatter.SurrogateSelector = null!; }
    // Purpose: Verify FormatterConverter.FormatterConverter: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::FormatterConverter::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_FormatterConverter_Void() { _ = FormatterConverter.FormatterConverter:(); }
    // Purpose: Verify FormatterConverter.Convert executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::Convert:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_Convert_System_Void_P0_P1() { FormatterConverter.Convert(null!, null!); }
    // Purpose: Verify FormatterConverter.ToBoolean executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToBoolean:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToBoolean_System_Void_P0() { FormatterConverter.ToBoolean(null!); }
    // Purpose: Verify FormatterConverter.ToByte executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToByte:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToByte_System_Void_P0() { FormatterConverter.ToByte(null!); }
    // Purpose: Verify FormatterConverter.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToChar:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToChar_System_Void_P0() { FormatterConverter.ToChar(null!); }
    // Purpose: Verify FormatterConverter.ToDateTime executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDateTime:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToDateTime_System_Void_P0() { FormatterConverter.ToDateTime(null!); }
    // Purpose: Verify FormatterConverter.ToDecimal executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDecimal:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToDecimal_System_Void_P0() { FormatterConverter.ToDecimal(null!); }
    // Purpose: Verify FormatterConverter.ToDouble executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDouble:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToDouble_System_Void_P0() { FormatterConverter.ToDouble(null!); }
    // Purpose: Verify FormatterConverter.ToInt16 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt16:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToInt16_System_Void_P0() { FormatterConverter.ToInt16(null!); }
    // Purpose: Verify FormatterConverter.ToInt32 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt32:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToInt32_System_Void_P0() { FormatterConverter.ToInt32(null!); }
    // Purpose: Verify FormatterConverter.ToInt64 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt64:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToInt64_System_Void_P0() { FormatterConverter.ToInt64(null!); }
    // Purpose: Verify FormatterConverter.ToSByte executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToSByte:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToSByte_System_Void_P0() { FormatterConverter.ToSByte(null!); }
    // Purpose: Verify FormatterConverter.ToSingle executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToSingle:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToSingle_System_Void_P0() { FormatterConverter.ToSingle(null!); }
    // Purpose: Verify FormatterConverter.ToString executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToString:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToString_System_Void_P0() { FormatterConverter.ToString(null!); }
    // Purpose: Verify FormatterConverter.ToUInt16 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt16:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToUInt16_System_Void_P0() { FormatterConverter.ToUInt16(null!); }
    // Purpose: Verify FormatterConverter.ToUInt32 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt32:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToUInt32_System_Void_P0() { FormatterConverter.ToUInt32(null!); }
    // Purpose: Verify FormatterConverter.ToUInt64 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt64:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToUInt64_System_Void_P0() { FormatterConverter.ToUInt64(null!); }
    // Purpose: Verify FormatterServices.CheckTypeSecurity executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::CheckTypeSecurity:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_CheckTypeSecurity_System_Void_P0_() { FormatterServices.CheckTypeSecurity(null!, null!); }
    // Purpose: Verify FormatterServices.GetObjectData executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetObjectData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetObjectData_System_Void_P0_P1() { FormatterServices.GetObjectData(null!, null!); }
    // Purpose: Verify FormatterServices.GetSafeUninitializedObject executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSafeUninitializedObject:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSafeUninitializedObject_System() { FormatterServices.GetSafeUninitializedObject(null!); }
    // Purpose: Verify FormatterServices.GetSerializableMembers executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSerializableMembers:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSerializableMembers_System_Voi() { FormatterServices.GetSerializableMembers(null!); }
    // Purpose: Verify FormatterServices.GetSerializableMembers executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSerializableMembers:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSerializableMembers_System_Voi() { FormatterServices.GetSerializableMembers(null!, null!); }
    // Purpose: Verify FormatterServices.GetSurrogateForCyclicalReference executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSurrogateForCyclicalReference:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSurrogateForCyclicalReference_() { FormatterServices.GetSurrogateForCyclicalReference(null!); }
    // Purpose: Verify FormatterServices.GetTypeFromAssembly executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetTypeFromAssembly:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetTypeFromAssembly_System_Void_P() { FormatterServices.GetTypeFromAssembly(null!, null!); }
    // Purpose: Verify FormatterServices.GetUninitializedObject executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetUninitializedObject:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetUninitializedObject_System_Voi() { FormatterServices.GetUninitializedObject(null!); }
    // Purpose: Verify FormatterServices.PopulateObjectMembers executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::PopulateObjectMembers:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_PopulateObjectMembers_System_Void() { FormatterServices.PopulateObjectMembers(null!, null!, null!); }
    // Purpose: Verify BinaryFormatter.BinaryFormatter: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::BinaryFormatter::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_BinaryFormatter_V() { _ = BinaryFormatter.BinaryFormatter:(); }
    // Purpose: Verify BinaryFormatter.BinaryFormatter: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::BinaryFormatter::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_BinaryFormatter_V() { _ = BinaryFormatter.BinaryFormatter:(42, 42); }
    // Purpose: Verify BinaryFormatter.Deserialize executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Deserialize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_Deserialize_Syste() { BinaryFormatter.Deserialize(null!); }
    // Purpose: Verify BinaryFormatter.Serialize executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Serialize:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_Serialize_System_() { BinaryFormatter.Serialize(null!, null!); }
    // Purpose: Verify BinaryFormatter.get_AssemblyFormat executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_AssemblyFormat:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_AssemblyForma() { BinaryFormatter.AssemblyFormat; }
    // Purpose: Verify BinaryFormatter.get_Binder executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Binder:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_Binder_System() { BinaryFormatter.Binder; }
    // Purpose: Verify BinaryFormatter.get_Context executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Context:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_Context_Syste() { BinaryFormatter.Context; }
    // Purpose: Verify BinaryFormatter.get_FilterLevel executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_FilterLevel:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_FilterLevel_S() { BinaryFormatter.FilterLevel; }
    // Purpose: Verify BinaryFormatter.get_SurrogateSelector executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_SurrogateSelector:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_SurrogateSele() { BinaryFormatter.SurrogateSelector; }
    // Purpose: Verify BinaryFormatter.get_TypeFormat executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_TypeFormat:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_TypeFormat_Sy() { BinaryFormatter.TypeFormat; }
    // Purpose: Verify BinaryFormatter.set_AssemblyFormat executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_AssemblyFormat:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_AssemblyForma() { BinaryFormatter.AssemblyFormat = null!; }
    // Purpose: Verify BinaryFormatter.set_Binder executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Binder:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_Binder_System() { BinaryFormatter.Binder = null!; }
    // Purpose: Verify BinaryFormatter.set_Context executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Context:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_Context_Syste() { BinaryFormatter.Context = null!; }
    // Purpose: Verify BinaryFormatter.set_FilterLevel executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_FilterLevel:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_FilterLevel_S() { BinaryFormatter.FilterLevel = null!; }
    // Purpose: Verify BinaryFormatter.set_SurrogateSelector executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_SurrogateSelector:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_SurrogateSele() { BinaryFormatter.SurrogateSelector = null!; }
    // Purpose: Verify BinaryFormatter.set_TypeFormat executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_TypeFormat:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_TypeFormat_Sy() { BinaryFormatter.TypeFormat = null!; }
    // Purpose: Verify IFieldInfo.get_FieldNames executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldNames:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_get_FieldNames_System_Void() { IFieldInfo.FieldNames; }
    // Purpose: Verify IFieldInfo.get_FieldTypes executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldTypes:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_get_FieldTypes_System_Void() { IFieldInfo.FieldTypes; }
    // Purpose: Verify IFieldInfo.set_FieldNames executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldNames:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_set_FieldNames_System_Void_P0() { IFieldInfo.FieldNames = null!; }
    // Purpose: Verify IFieldInfo.set_FieldTypes executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldTypes:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_set_FieldTypes_System_Void_P0() { IFieldInfo.FieldTypes = null!; }
    // Purpose: Verify IFormatter.Deserialize executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Deserialize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_Deserialize_System_Void_P0() { IFormatter.Deserialize(null!); }
    // Purpose: Verify IFormatter.Serialize executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Serialize:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_Serialize_System_Void_P0_P1() { IFormatter.Serialize(null!, null!); }
    // Purpose: Verify IFormatter.get_Binder executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Binder:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_get_Binder_System_Void() { IFormatter.Binder; }
    // Purpose: Verify IFormatter.get_Context executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Context:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_get_Context_System_Void() { IFormatter.Context; }
    // Purpose: Verify IFormatter.get_SurrogateSelector executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_SurrogateSelector:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_get_SurrogateSelector_System_Void() { IFormatter.SurrogateSelector; }
    // Purpose: Verify IFormatter.set_Binder executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Binder:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_set_Binder_System_Void_P0() { IFormatter.Binder = null!; }
    // Purpose: Verify IFormatter.set_Context executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Context:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_set_Context_System_Void_P0() { IFormatter.Context = null!; }
    // Purpose: Verify IFormatter.set_SurrogateSelector executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_SurrogateSelector:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_set_SurrogateSelector_System_Void_P0() { IFormatter.SurrogateSelector = null!; }}
