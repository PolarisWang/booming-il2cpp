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

// Auto-generated patch-side skeletons for FamilySystem.text.jsonPolymorphismReferencePatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.text.jsonPolymorphismReferencePatch
{
    // Purpose: Verify ReferenceHandler.ReferenceHandler: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::ReferenceHandler::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_ReferenceHandler_Void_System_Object() { _ = ReferenceHandler.ReferenceHandler:(42); }
    // Purpose: Verify ReferenceHandler.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_Property_System_Void() { ReferenceHandler.Property; }
    // Purpose: Verify ReferenceHandler.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_Method1_System_Void_P0() { ReferenceHandler.Method1(null!); }
    // Purpose: Verify ReferenceHandler.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_Method2_System_Void_P0_P1() { ReferenceHandler.Method2(null!, null!); }
    // Purpose: Verify ReferenceHandler.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_Validate_System_Void_P0() { ReferenceHandler.Validate(null!); }
    // Purpose: Verify ReferenceResolver.ReferenceResolver: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ReferenceResolver::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_ReferenceResolver_Void_System_Object() { _ = ReferenceResolver.ReferenceResolver:(42); }
    // Purpose: Verify ReferenceResolver.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_Property_System_Void() { ReferenceResolver.Property; }
    // Purpose: Verify ReferenceResolver.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_Method1_System_Void_P0() { ReferenceResolver.Method1(null!); }
    // Purpose: Verify ReferenceResolver.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_Method2_System_Void_P0_P1() { ReferenceResolver.Method2(null!, null!); }
    // Purpose: Verify ReferenceResolver.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_Validate_System_Void_P0() { ReferenceResolver.Validate(null!); }
    // Purpose: Verify ReferenceHandler.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_Value_System_Void() { ReferenceHandler.Value; }
    // Purpose: Verify ReferenceHandler.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_set_Value_System_Void_P0() { ReferenceHandler.Value = null!; }
    // Purpose: Verify ReferenceHandler.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_Count_System_Void() { ReferenceHandler.Count; }
    // Purpose: Verify ReferenceHandler.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_set_Count_System_Void_P0() { ReferenceHandler.Count = null!; }
    // Purpose: Verify ReferenceHandler.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_IsEmpty_System_Void() { ReferenceHandler.IsEmpty; }
    // Purpose: Verify ReferenceHandler.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_set_IsEmpty_System_Void_P0() { ReferenceHandler.IsEmpty = null!; }
    // Purpose: Verify ReferenceHandler.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_get_Key_System_Void() { ReferenceHandler.Key; }
    // Purpose: Verify ReferenceHandler.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceHandler_set_Key_System_Void_P0() { ReferenceHandler.Key = null!; }
    // Purpose: Verify ReferenceResolver.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_Value_System_Void() { ReferenceResolver.Value; }
    // Purpose: Verify ReferenceResolver.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_set_Value_System_Void_P0() { ReferenceResolver.Value = null!; }
    // Purpose: Verify ReferenceResolver.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_Count_System_Void() { ReferenceResolver.Count; }
    // Purpose: Verify ReferenceResolver.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_set_Count_System_Void_P0() { ReferenceResolver.Count = null!; }
    // Purpose: Verify ReferenceResolver.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_IsEmpty_System_Void() { ReferenceResolver.IsEmpty; }
    // Purpose: Verify ReferenceResolver.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_set_IsEmpty_System_Void_P0() { ReferenceResolver.IsEmpty = null!; }
    // Purpose: Verify ReferenceResolver.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_get_Key_System_Void() { ReferenceResolver.Key; }
    // Purpose: Verify ReferenceResolver.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Serialization_ReferenceResolver_set_Key_System_Void_P0() { ReferenceResolver.Key = null!; }}
