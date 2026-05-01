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

// Auto-generated patch-side skeletons for FamilySystem.text.jsonAttributesPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.text.jsonAttributesPatch
{
    // Purpose: Verify JsonAttribute.JsonAttribute: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::JsonAttribute::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_JsonAttribute_Void_System_Object() { _ = JsonAttribute.JsonAttribute:(42); }
    // Purpose: Verify JsonAttribute.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_get_Property_System_Void() { JsonAttribute.Property; }
    // Purpose: Verify JsonAttribute.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_Method1_System_Void_P0() { JsonAttribute.Method1(null!); }
    // Purpose: Verify JsonAttribute.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_Method2_System_Void_P0_P1() { JsonAttribute.Method2(null!, null!); }
    // Purpose: Verify JsonAttribute.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_Validate_System_Void_P0() { JsonAttribute.Validate(null!); }
    // Purpose: Verify JsonAttribute.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_get_Value_System_Void() { JsonAttribute.Value; }
    // Purpose: Verify JsonAttribute.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_set_Value_System_Void_P0() { JsonAttribute.Value = null!; }
    // Purpose: Verify JsonAttribute.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_get_Count_System_Void() { JsonAttribute.Count; }
    // Purpose: Verify JsonAttribute.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_set_Count_System_Void_P0() { JsonAttribute.Count = null!; }
    // Purpose: Verify JsonAttribute.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_get_IsEmpty_System_Void() { JsonAttribute.IsEmpty; }
    // Purpose: Verify JsonAttribute.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_set_IsEmpty_System_Void_P0() { JsonAttribute.IsEmpty = null!; }
    // Purpose: Verify JsonAttribute.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_get_Key_System_Void() { JsonAttribute.Key; }
    // Purpose: Verify JsonAttribute.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonAttribute::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonAttribute_set_Key_System_Void_P0() { JsonAttribute.Key = null!; }}
