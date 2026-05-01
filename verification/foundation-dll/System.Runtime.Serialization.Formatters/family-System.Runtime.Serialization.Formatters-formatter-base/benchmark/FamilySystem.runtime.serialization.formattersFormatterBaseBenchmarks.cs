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

// Auto-generated benchmark skeletons for FamilySystem.runtime.serialization.formattersFormatterBaseBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.serialization.formattersFormatterBaseBenchmarks
{
    // Purpose: Benchmark native-runtime performance of Formatter.Formatter: with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Formatter::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_Formatter_Void() { _ = Formatter.Formatter:(); }
    // Purpose: Benchmark native-runtime performance of Formatter.Deserialize with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Deserialize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_Deserialize_System_Void_P0() { Formatter.Deserialize(null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.GetNext with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::GetNext:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_GetNext_System_Void_P0() { Formatter.GetNext(null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.Schedule with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Schedule:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_Schedule_System_Void_P0() { Formatter.Schedule(null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.Serialize with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Serialize:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_Serialize_System_Void_P0_P1() { Formatter.Serialize(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteArray with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteArray:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteArray_System_Void_P0_P1_P2() { Formatter.WriteArray(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteBoolean with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteBoolean:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteBoolean_System_Void_P0_P1() { Formatter.WriteBoolean(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteByte with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteByte:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteByte_System_Void_P0_P1() { Formatter.WriteByte(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteChar with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteChar:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteChar_System_Void_P0_P1() { Formatter.WriteChar(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteDateTime with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteDateTime:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteDateTime_System_Void_P0_P1() { Formatter.WriteDateTime(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteDecimal with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteDecimal:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteDecimal_System_Void_P0_P1() { Formatter.WriteDecimal(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteDouble with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteDouble:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteDouble_System_Void_P0_P1() { Formatter.WriteDouble(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteInt16 with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteInt16:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteInt16_System_Void_P0_P1() { Formatter.WriteInt16(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteInt32 with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteInt32:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteInt32_System_Void_P0_P1() { Formatter.WriteInt32(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteInt64 with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteInt64:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteInt64_System_Void_P0_P1() { Formatter.WriteInt64(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteMember with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteMember:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteMember_System_Void_P0_P1() { Formatter.WriteMember(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteObjectRef with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteObjectRef:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteObjectRef_System_Void_P0_P1_P2() { Formatter.WriteObjectRef(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteSByte with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteSByte:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteSByte_System_Void_P0_P1() { Formatter.WriteSByte(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteSingle with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteSingle:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteSingle_System_Void_P0_P1() { Formatter.WriteSingle(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteTimeSpan with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteTimeSpan:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteTimeSpan_System_Void_P0_P1() { Formatter.WriteTimeSpan(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteUInt16 with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteUInt16:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteUInt16_System_Void_P0_P1() { Formatter.WriteUInt16(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteUInt32 with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteUInt32:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteUInt32_System_Void_P0_P1() { Formatter.WriteUInt32(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteUInt64 with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteUInt64:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteUInt64_System_Void_P0_P1() { Formatter.WriteUInt64(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.WriteValueType with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteValueType:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_WriteValueType_System_Void_P0_P1_P2() { Formatter.WriteValueType(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of Formatter.get_Binder with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Binder:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_get_Binder_System_Void() { Formatter.Binder; }
    // Purpose: Benchmark native-runtime performance of Formatter.get_Context with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Context:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_get_Context_System_Void() { Formatter.Context; }
    // Purpose: Benchmark native-runtime performance of Formatter.get_SurrogateSelector with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_SurrogateSelector:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_get_SurrogateSelector_System_Void() { Formatter.SurrogateSelector; }
    // Purpose: Benchmark native-runtime performance of Formatter.set_Binder with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Binder:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_set_Binder_System_Void_P0() { Formatter.Binder = null!; }
    // Purpose: Benchmark native-runtime performance of Formatter.set_Context with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Context:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_set_Context_System_Void_P0() { Formatter.Context = null!; }
    // Purpose: Benchmark native-runtime performance of Formatter.set_SurrogateSelector with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_SurrogateSelector:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatter_set_SurrogateSelector_System_Void_P0() { Formatter.SurrogateSelector = null!; }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.FormatterConverter: with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::FormatterConverter::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_FormatterConverter_Void() { _ = FormatterConverter.FormatterConverter:(); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.Convert with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::Convert:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_Convert_System_Void_P0_P1() { FormatterConverter.Convert(null!, null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToBoolean with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToBoolean:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToBoolean_System_Void_P0() { FormatterConverter.ToBoolean(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToByte with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToByte:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToByte_System_Void_P0() { FormatterConverter.ToByte(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToChar with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToChar:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToChar_System_Void_P0() { FormatterConverter.ToChar(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToDateTime with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDateTime:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToDateTime_System_Void_P0() { FormatterConverter.ToDateTime(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToDecimal with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDecimal:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToDecimal_System_Void_P0() { FormatterConverter.ToDecimal(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToDouble with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDouble:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToDouble_System_Void_P0() { FormatterConverter.ToDouble(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToInt16 with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt16:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToInt16_System_Void_P0() { FormatterConverter.ToInt16(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToInt32 with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt32:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToInt32_System_Void_P0() { FormatterConverter.ToInt32(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToInt64 with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt64:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToInt64_System_Void_P0() { FormatterConverter.ToInt64(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToSByte with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToSByte:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToSByte_System_Void_P0() { FormatterConverter.ToSByte(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToSingle with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToSingle:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToSingle_System_Void_P0() { FormatterConverter.ToSingle(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToString with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToString:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToString_System_Void_P0() { FormatterConverter.ToString(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToUInt16 with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt16:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToUInt16_System_Void_P0() { FormatterConverter.ToUInt16(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToUInt32 with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt32:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToUInt32_System_Void_P0() { FormatterConverter.ToUInt32(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterConverter.ToUInt64 with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt64:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToUInt64_System_Void_P0() { FormatterConverter.ToUInt64(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterServices.CheckTypeSecurity with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::CheckTypeSecurity:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_CheckTypeSecurity_System_Void_P0_() { FormatterServices.CheckTypeSecurity(null!, null!); }
    // Purpose: Benchmark native-runtime performance of FormatterServices.GetObjectData with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetObjectData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetObjectData_System_Void_P0_P1() { FormatterServices.GetObjectData(null!, null!); }
    // Purpose: Benchmark native-runtime performance of FormatterServices.GetSafeUninitializedObject with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSafeUninitializedObject:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSafeUninitializedObject_System() { FormatterServices.GetSafeUninitializedObject(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterServices.GetSerializableMembers with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSerializableMembers:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSerializableMembers_System_Voi() { FormatterServices.GetSerializableMembers(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterServices.GetSerializableMembers with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSerializableMembers:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSerializableMembers_System_Voi() { FormatterServices.GetSerializableMembers(null!, null!); }
    // Purpose: Benchmark native-runtime performance of FormatterServices.GetSurrogateForCyclicalReference with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSurrogateForCyclicalReference:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSurrogateForCyclicalReference_() { FormatterServices.GetSurrogateForCyclicalReference(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterServices.GetTypeFromAssembly with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetTypeFromAssembly:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetTypeFromAssembly_System_Void_P() { FormatterServices.GetTypeFromAssembly(null!, null!); }
    // Purpose: Benchmark native-runtime performance of FormatterServices.GetUninitializedObject with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetUninitializedObject:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetUninitializedObject_System_Voi() { FormatterServices.GetUninitializedObject(null!); }
    // Purpose: Benchmark native-runtime performance of FormatterServices.PopulateObjectMembers with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::PopulateObjectMembers:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_PopulateObjectMembers_System_Void() { FormatterServices.PopulateObjectMembers(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.BinaryFormatter: with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::BinaryFormatter::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_BinaryFormatter_V() { _ = BinaryFormatter.BinaryFormatter:(); }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.BinaryFormatter: with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::BinaryFormatter::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_BinaryFormatter_V() { _ = BinaryFormatter.BinaryFormatter:(42, 42); }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.Deserialize with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Deserialize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_Deserialize_Syste() { BinaryFormatter.Deserialize(null!); }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.Serialize with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Serialize:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_Serialize_System_() { BinaryFormatter.Serialize(null!, null!); }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.get_AssemblyFormat with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_AssemblyFormat:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_AssemblyForma() { BinaryFormatter.AssemblyFormat; }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.get_Binder with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Binder:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_Binder_System() { BinaryFormatter.Binder; }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.get_Context with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Context:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_Context_Syste() { BinaryFormatter.Context; }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.get_FilterLevel with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_FilterLevel:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_FilterLevel_S() { BinaryFormatter.FilterLevel; }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.get_SurrogateSelector with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_SurrogateSelector:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_SurrogateSele() { BinaryFormatter.SurrogateSelector; }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.get_TypeFormat with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_TypeFormat:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_get_TypeFormat_Sy() { BinaryFormatter.TypeFormat; }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.set_AssemblyFormat with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_AssemblyFormat:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_AssemblyForma() { BinaryFormatter.AssemblyFormat = null!; }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.set_Binder with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Binder:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_Binder_System() { BinaryFormatter.Binder = null!; }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.set_Context with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Context:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_Context_Syste() { BinaryFormatter.Context = null!; }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.set_FilterLevel with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_FilterLevel:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_FilterLevel_S() { BinaryFormatter.FilterLevel = null!; }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.set_SurrogateSelector with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_SurrogateSelector:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_SurrogateSele() { BinaryFormatter.SurrogateSelector = null!; }
    // Purpose: Benchmark native-runtime performance of BinaryFormatter.set_TypeFormat with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_TypeFormat:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter_set_TypeFormat_Sy() { BinaryFormatter.TypeFormat = null!; }
    // Purpose: Benchmark native-runtime performance of IFieldInfo.get_FieldNames with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldNames:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_get_FieldNames_System_Void() { IFieldInfo.FieldNames; }
    // Purpose: Benchmark native-runtime performance of IFieldInfo.get_FieldTypes with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldTypes:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_get_FieldTypes_System_Void() { IFieldInfo.FieldTypes; }
    // Purpose: Benchmark native-runtime performance of IFieldInfo.set_FieldNames with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldNames:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_set_FieldNames_System_Void_P0() { IFieldInfo.FieldNames = null!; }
    // Purpose: Benchmark native-runtime performance of IFieldInfo.set_FieldTypes with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldTypes:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_set_FieldTypes_System_Void_P0() { IFieldInfo.FieldTypes = null!; }
    // Purpose: Benchmark native-runtime performance of IFormatter.Deserialize with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Deserialize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_Deserialize_System_Void_P0() { IFormatter.Deserialize(null!); }
    // Purpose: Benchmark native-runtime performance of IFormatter.Serialize with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Serialize:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_Serialize_System_Void_P0_P1() { IFormatter.Serialize(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IFormatter.get_Binder with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Binder:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_get_Binder_System_Void() { IFormatter.Binder; }
    // Purpose: Benchmark native-runtime performance of IFormatter.get_Context with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Context:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_get_Context_System_Void() { IFormatter.Context; }
    // Purpose: Benchmark native-runtime performance of IFormatter.get_SurrogateSelector with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_SurrogateSelector:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_get_SurrogateSelector_System_Void() { IFormatter.SurrogateSelector; }
    // Purpose: Benchmark native-runtime performance of IFormatter.set_Binder with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Binder:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_set_Binder_System_Void_P0() { IFormatter.Binder = null!; }
    // Purpose: Benchmark native-runtime performance of IFormatter.set_Context with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Context:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_set_Context_System_Void_P0() { IFormatter.Context = null!; }
    // Purpose: Benchmark native-runtime performance of IFormatter.set_SurrogateSelector with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_SurrogateSelector:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_IFormatter_set_SurrogateSelector_System_Void_P0() { IFormatter.SurrogateSelector = null!; }}
