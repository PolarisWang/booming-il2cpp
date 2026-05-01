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

// Auto-generated benchmark skeletons for FamilySystem.runtime.serialization.formattersFormatterServicesConverterBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.serialization.formattersFormatterServicesConverterBenchmarks
{
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
    // Purpose: Benchmark native-runtime performance of SerializationObjectManager.SerializationObjectManager: with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationObjectManager::SerializationObjectManager::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationObjectManager_SerializationObjectManag() { _ = SerializationObjectManager.SerializationObjectManager:(42); }
    // Purpose: Benchmark native-runtime performance of SerializationObjectManager.RaiseOnSerializedEvent with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationObjectManager::RaiseOnSerializedEvent:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationObjectManager_RaiseOnSerializedEvent_S() { SerializationObjectManager.RaiseOnSerializedEvent(); }
    // Purpose: Benchmark native-runtime performance of SerializationObjectManager.RegisterObject with typical input
    [BenchmarkSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationObjectManager::RegisterObject:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationObjectManager_RegisterObject_System_Vo() { SerializationObjectManager.RegisterObject(null!); }}
