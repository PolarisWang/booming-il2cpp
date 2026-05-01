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

// Auto-generated patch-side skeletons for FamilySystem.text.jsonConvertersPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.text.jsonConvertersPatch
{
    // Purpose: Verify JsonConverter.JsonConverter: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::JsonConverter::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonConverter_JsonConverter_Void_System_Object() { _ = JsonConverter.JsonConverter:(42); }
    // Purpose: Verify JsonConverter.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonConverter_get_Property_System_Void() { JsonConverter.Property; }
    // Purpose: Verify JsonConverter.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonConverter_Method1_System_Void_P0() { JsonConverter.Method1(null!); }
    // Purpose: Verify JsonConverter.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonConverter_Method2_System_Void_P0_P1() { JsonConverter.Method2(null!, null!); }
    // Purpose: Verify JsonConverter.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonConverter_Validate_System_Void_P0() { JsonConverter.Validate(null!); }
    // Purpose: Verify JsonConverter.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonConverter_get_Value_System_Void() { JsonConverter.Value; }
    // Purpose: Verify JsonConverter.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonConverter_set_Value_System_Void_P0() { JsonConverter.Value = null!; }
    // Purpose: Verify JsonConverter.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonConverter_get_Count_System_Void() { JsonConverter.Count; }
    // Purpose: Verify JsonConverter.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonConverter_set_Count_System_Void_P0() { JsonConverter.Count = null!; }
    // Purpose: Verify JsonConverter.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonConverter_get_IsEmpty_System_Void() { JsonConverter.IsEmpty; }
    // Purpose: Verify JsonConverter.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonConverter_set_IsEmpty_System_Void_P0() { JsonConverter.IsEmpty = null!; }
    // Purpose: Verify JsonConverter.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonConverter_get_Key_System_Void() { JsonConverter.Key; }
    // Purpose: Verify JsonConverter.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonConverter::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonConverter_set_Key_System_Void_P0() { JsonConverter.Key = null!; }}
