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

// Auto-generated hot-update skeletons for FamilySystem.text.jsonMetadataTypeinfoHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.text.jsonMetadataTypeinfoHotUpdate
{
    // Purpose: Verify JsonTypeInfo.JsonTypeInfo: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::JsonTypeInfo::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_JsonTypeInfo_Void_System_Object() { _ = JsonTypeInfo.JsonTypeInfo:(42); }
    // Purpose: Verify JsonTypeInfo.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_get_Property_System_Void() { JsonTypeInfo.Property; }
    // Purpose: Verify JsonTypeInfo.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_Method1_System_Void_P0() { JsonTypeInfo.Method1(null!); }
    // Purpose: Verify JsonTypeInfo.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_Method2_System_Void_P0_P1() { JsonTypeInfo.Method2(null!, null!); }
    // Purpose: Verify JsonTypeInfo.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_Validate_System_Void_P0() { JsonTypeInfo.Validate(null!); }
    // Purpose: Verify JsonTypeInfo.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_get_Value_System_Void() { JsonTypeInfo.Value; }
    // Purpose: Verify JsonTypeInfo.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_set_Value_System_Void_P0() { JsonTypeInfo.Value = null!; }
    // Purpose: Verify JsonTypeInfo.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_get_Count_System_Void() { JsonTypeInfo.Count; }
    // Purpose: Verify JsonTypeInfo.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_set_Count_System_Void_P0() { JsonTypeInfo.Count = null!; }
    // Purpose: Verify JsonTypeInfo.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_get_IsEmpty_System_Void() { JsonTypeInfo.IsEmpty; }
    // Purpose: Verify JsonTypeInfo.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_set_IsEmpty_System_Void_P0() { JsonTypeInfo.IsEmpty = null!; }
    // Purpose: Verify JsonTypeInfo.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_get_Key_System_Void() { JsonTypeInfo.Key; }
    // Purpose: Verify JsonTypeInfo.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.Metadata.JsonTypeInfo::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_Serialization_Metadata_JsonTypeInfo_set_Key_System_Void_P0() { JsonTypeInfo.Key = null!; }}
