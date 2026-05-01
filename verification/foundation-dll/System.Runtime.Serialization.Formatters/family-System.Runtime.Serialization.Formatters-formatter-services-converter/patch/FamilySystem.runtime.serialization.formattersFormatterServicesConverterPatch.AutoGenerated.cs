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

// Auto-generated patch-side skeletons for FamilySystem.runtime.serialization.formattersFormatterServicesConverterPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.runtime.serialization.formattersFormatterServicesConverterPatch
{
    // Purpose: Verify FormatterConverter.FormatterConverter: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::FormatterConverter::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_FormatterConverter_Void() { _ = FormatterConverter.FormatterConverter:(); }
    // Purpose: Verify FormatterConverter.Convert executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::Convert:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_Convert_System_Void_P0_P1() { FormatterConverter.Convert(null!, null!); }
    // Purpose: Verify FormatterConverter.ToBoolean executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToBoolean:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToBoolean_System_Void_P0() { FormatterConverter.ToBoolean(null!); }
    // Purpose: Verify FormatterConverter.ToByte executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToByte:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToByte_System_Void_P0() { FormatterConverter.ToByte(null!); }
    // Purpose: Verify FormatterConverter.ToChar executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToChar:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToChar_System_Void_P0() { FormatterConverter.ToChar(null!); }
    // Purpose: Verify FormatterConverter.ToDateTime executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDateTime:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToDateTime_System_Void_P0() { FormatterConverter.ToDateTime(null!); }
    // Purpose: Verify FormatterConverter.ToDecimal executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDecimal:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToDecimal_System_Void_P0() { FormatterConverter.ToDecimal(null!); }
    // Purpose: Verify FormatterConverter.ToDouble executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDouble:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToDouble_System_Void_P0() { FormatterConverter.ToDouble(null!); }
    // Purpose: Verify FormatterConverter.ToInt16 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt16:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToInt16_System_Void_P0() { FormatterConverter.ToInt16(null!); }
    // Purpose: Verify FormatterConverter.ToInt32 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt32:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToInt32_System_Void_P0() { FormatterConverter.ToInt32(null!); }
    // Purpose: Verify FormatterConverter.ToInt64 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt64:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToInt64_System_Void_P0() { FormatterConverter.ToInt64(null!); }
    // Purpose: Verify FormatterConverter.ToSByte executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToSByte:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToSByte_System_Void_P0() { FormatterConverter.ToSByte(null!); }
    // Purpose: Verify FormatterConverter.ToSingle executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToSingle:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToSingle_System_Void_P0() { FormatterConverter.ToSingle(null!); }
    // Purpose: Verify FormatterConverter.ToString executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToString:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToString_System_Void_P0() { FormatterConverter.ToString(null!); }
    // Purpose: Verify FormatterConverter.ToUInt16 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt16:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToUInt16_System_Void_P0() { FormatterConverter.ToUInt16(null!); }
    // Purpose: Verify FormatterConverter.ToUInt32 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt32:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToUInt32_System_Void_P0() { FormatterConverter.ToUInt32(null!); }
    // Purpose: Verify FormatterConverter.ToUInt64 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt64:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterConverter_ToUInt64_System_Void_P0() { FormatterConverter.ToUInt64(null!); }
    // Purpose: Verify FormatterServices.CheckTypeSecurity executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::CheckTypeSecurity:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_CheckTypeSecurity_System_Void_P0_() { FormatterServices.CheckTypeSecurity(null!, null!); }
    // Purpose: Verify FormatterServices.GetObjectData executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetObjectData:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetObjectData_System_Void_P0_P1() { FormatterServices.GetObjectData(null!, null!); }
    // Purpose: Verify FormatterServices.GetSafeUninitializedObject executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSafeUninitializedObject:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSafeUninitializedObject_System() { FormatterServices.GetSafeUninitializedObject(null!); }
    // Purpose: Verify FormatterServices.GetSerializableMembers executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSerializableMembers:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSerializableMembers_System_Voi() { FormatterServices.GetSerializableMembers(null!); }
    // Purpose: Verify FormatterServices.GetSerializableMembers executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSerializableMembers:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSerializableMembers_System_Voi() { FormatterServices.GetSerializableMembers(null!, null!); }
    // Purpose: Verify FormatterServices.GetSurrogateForCyclicalReference executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSurrogateForCyclicalReference:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetSurrogateForCyclicalReference_() { FormatterServices.GetSurrogateForCyclicalReference(null!); }
    // Purpose: Verify FormatterServices.GetTypeFromAssembly executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetTypeFromAssembly:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetTypeFromAssembly_System_Void_P() { FormatterServices.GetTypeFromAssembly(null!, null!); }
    // Purpose: Verify FormatterServices.GetUninitializedObject executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetUninitializedObject:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_GetUninitializedObject_System_Voi() { FormatterServices.GetUninitializedObject(null!); }
    // Purpose: Verify FormatterServices.PopulateObjectMembers executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::PopulateObjectMembers:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_FormatterServices_PopulateObjectMembers_System_Void() { FormatterServices.PopulateObjectMembers(null!, null!, null!); }
    // Purpose: Verify IFieldInfo.get_FieldNames executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldNames:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_get_FieldNames_System_Void() { IFieldInfo.FieldNames; }
    // Purpose: Verify IFieldInfo.get_FieldTypes executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldTypes:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_get_FieldTypes_System_Void() { IFieldInfo.FieldTypes; }
    // Purpose: Verify IFieldInfo.set_FieldNames executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldNames:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_set_FieldNames_System_Void_P0() { IFieldInfo.FieldNames = null!; }
    // Purpose: Verify IFieldInfo.set_FieldTypes executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldTypes:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_Formatters_IFieldInfo_set_FieldTypes_System_Void_P0() { IFieldInfo.FieldTypes = null!; }
    // Purpose: Verify SerializationObjectManager.SerializationObjectManager: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationObjectManager::SerializationObjectManager::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationObjectManager_SerializationObjectManag() { _ = SerializationObjectManager.SerializationObjectManager:(42); }
    // Purpose: Verify SerializationObjectManager.RaiseOnSerializedEvent executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationObjectManager::RaiseOnSerializedEvent:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationObjectManager_RaiseOnSerializedEvent_S() { SerializationObjectManager.RaiseOnSerializedEvent(); }
    // Purpose: Verify SerializationObjectManager.RegisterObject executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationObjectManager::RegisterObject:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationObjectManager_RegisterObject_System_Vo() { SerializationObjectManager.RegisterObject(null!); }}
