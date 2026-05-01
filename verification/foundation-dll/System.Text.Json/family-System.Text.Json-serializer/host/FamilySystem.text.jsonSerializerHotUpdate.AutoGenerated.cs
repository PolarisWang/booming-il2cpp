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

// Auto-generated hot-update skeletons for FamilySystem.text.jsonSerializerHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.text.jsonSerializerHotUpdate
{
    // Purpose: Verify JsonSerializer.JsonSerializer: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::JsonSerializer::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_JsonSerializer_JsonSerializer_Void_System_Object() { _ = JsonSerializer.JsonSerializer:(42); }
    // Purpose: Verify JsonSerializer.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_JsonSerializer_get_Property_System_Void() { JsonSerializer.Property; }
    // Purpose: Verify JsonSerializer.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_JsonSerializer_Method1_System_Void_P0() { JsonSerializer.Method1(null!); }
    // Purpose: Verify JsonSerializer.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_JsonSerializer_Method2_System_Void_P0_P1() { JsonSerializer.Method2(null!, null!); }
    // Purpose: Verify JsonSerializer.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_JsonSerializer_Validate_System_Void_P0() { JsonSerializer.Validate(null!); }
    // Purpose: Verify JsonSerializer.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_JsonSerializer_get_Value_System_Void() { JsonSerializer.Value; }
    // Purpose: Verify JsonSerializer.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_JsonSerializer_set_Value_System_Void_P0() { JsonSerializer.Value = null!; }
    // Purpose: Verify JsonSerializer.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_JsonSerializer_get_Count_System_Void() { JsonSerializer.Count; }
    // Purpose: Verify JsonSerializer.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_JsonSerializer_set_Count_System_Void_P0() { JsonSerializer.Count = null!; }
    // Purpose: Verify JsonSerializer.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_JsonSerializer_get_IsEmpty_System_Void() { JsonSerializer.IsEmpty; }
    // Purpose: Verify JsonSerializer.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_JsonSerializer_set_IsEmpty_System_Void_P0() { JsonSerializer.IsEmpty = null!; }
    // Purpose: Verify JsonSerializer.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_JsonSerializer_get_Key_System_Void() { JsonSerializer.Key; }
    // Purpose: Verify JsonSerializer.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.JsonSerializer::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Text_Json_System_Text_Json_JsonSerializer_set_Key_System_Void_P0() { JsonSerializer.Key = null!; }}
