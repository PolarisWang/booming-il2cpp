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

// Auto-generated patch-side skeletons for FamilySystem.text.jsonSourceGenerationContextPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.text.jsonSourceGenerationContextPatch
{
    // Purpose: Verify JsonSerializerContext.JsonSerializerContext: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::JsonSerializerContext::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_JsonSerializerContext_Void_System_Object() { _ = JsonSerializerContext.JsonSerializerContext:(42); }
    // Purpose: Verify JsonSerializerContext.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_get_Property_System_Void() { JsonSerializerContext.Property; }
    // Purpose: Verify JsonSerializerContext.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_Method1_System_Void_P0() { JsonSerializerContext.Method1(null!); }
    // Purpose: Verify JsonSerializerContext.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_Method2_System_Void_P0_P1() { JsonSerializerContext.Method2(null!, null!); }
    // Purpose: Verify JsonSerializerContext.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_Validate_System_Void_P0() { JsonSerializerContext.Validate(null!); }
    // Purpose: Verify JsonSerializerContext.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_get_Value_System_Void() { JsonSerializerContext.Value; }
    // Purpose: Verify JsonSerializerContext.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_set_Value_System_Void_P0() { JsonSerializerContext.Value = null!; }
    // Purpose: Verify JsonSerializerContext.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_get_Count_System_Void() { JsonSerializerContext.Count; }
    // Purpose: Verify JsonSerializerContext.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_set_Count_System_Void_P0() { JsonSerializerContext.Count = null!; }
    // Purpose: Verify JsonSerializerContext.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_get_IsEmpty_System_Void() { JsonSerializerContext.IsEmpty; }
    // Purpose: Verify JsonSerializerContext.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_set_IsEmpty_System_Void_P0() { JsonSerializerContext.IsEmpty = null!; }
    // Purpose: Verify JsonSerializerContext.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_get_Key_System_Void() { JsonSerializerContext.Key; }
    // Purpose: Verify JsonSerializerContext.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_JsonSerializerContext_set_Key_System_Void_P0() { JsonSerializerContext.Key = null!; }}
