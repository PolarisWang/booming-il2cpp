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

// Auto-generated patch-side skeletons for FamilySystem.text.jsonReaderPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.text.jsonReaderPatch
{
    // Purpose: Verify Utf8JsonReader.Utf8JsonReader: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::Utf8JsonReader::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Utf8JsonReader_Utf8JsonReader_Void_System_Object() { _ = Utf8JsonReader.Utf8JsonReader:(42); }
    // Purpose: Verify Utf8JsonReader.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Utf8JsonReader_get_Property_System_Void() { Utf8JsonReader.Property; }
    // Purpose: Verify Utf8JsonReader.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Utf8JsonReader_Method1_System_Void_P0() { Utf8JsonReader.Method1(null!); }
    // Purpose: Verify Utf8JsonReader.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Utf8JsonReader_Method2_System_Void_P0_P1() { Utf8JsonReader.Method2(null!, null!); }
    // Purpose: Verify Utf8JsonReader.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Utf8JsonReader_Validate_System_Void_P0() { Utf8JsonReader.Validate(null!); }
    // Purpose: Verify Utf8JsonReader.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Utf8JsonReader_get_Value_System_Void() { Utf8JsonReader.Value; }
    // Purpose: Verify Utf8JsonReader.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Utf8JsonReader_set_Value_System_Void_P0() { Utf8JsonReader.Value = null!; }
    // Purpose: Verify Utf8JsonReader.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Utf8JsonReader_get_Count_System_Void() { Utf8JsonReader.Count; }
    // Purpose: Verify Utf8JsonReader.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Utf8JsonReader_set_Count_System_Void_P0() { Utf8JsonReader.Count = null!; }
    // Purpose: Verify Utf8JsonReader.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Utf8JsonReader_get_IsEmpty_System_Void() { Utf8JsonReader.IsEmpty; }
    // Purpose: Verify Utf8JsonReader.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Utf8JsonReader_set_IsEmpty_System_Void_P0() { Utf8JsonReader.IsEmpty = null!; }
    // Purpose: Verify Utf8JsonReader.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Utf8JsonReader_get_Key_System_Void() { Utf8JsonReader.Key; }
    // Purpose: Verify Utf8JsonReader.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Utf8JsonReader::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Utf8JsonReader_set_Key_System_Void_P0() { Utf8JsonReader.Key = null!; }}
