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

// Auto-generated patch-side skeletons for FamilySystem.text.jsonSchemaPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.text.jsonSchemaPatch
{
    // Purpose: Verify JsonSchemaExporter.JsonSchemaExporter: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::JsonSchemaExporter::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_JsonSchemaExporter_Void_System_Object() { _ = JsonSchemaExporter.JsonSchemaExporter:(42); }
    // Purpose: Verify JsonSchemaExporter.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_get_Property_System_Void() { JsonSchemaExporter.Property; }
    // Purpose: Verify JsonSchemaExporter.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_Method1_System_Void_P0() { JsonSchemaExporter.Method1(null!); }
    // Purpose: Verify JsonSchemaExporter.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_Method2_System_Void_P0_P1() { JsonSchemaExporter.Method2(null!, null!); }
    // Purpose: Verify JsonSchemaExporter.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_Validate_System_Void_P0() { JsonSchemaExporter.Validate(null!); }
    // Purpose: Verify JsonSchemaExporter.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_get_Value_System_Void() { JsonSchemaExporter.Value; }
    // Purpose: Verify JsonSchemaExporter.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_set_Value_System_Void_P0() { JsonSchemaExporter.Value = null!; }
    // Purpose: Verify JsonSchemaExporter.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_get_Count_System_Void() { JsonSchemaExporter.Count; }
    // Purpose: Verify JsonSchemaExporter.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_set_Count_System_Void_P0() { JsonSchemaExporter.Count = null!; }
    // Purpose: Verify JsonSchemaExporter.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_get_IsEmpty_System_Void() { JsonSchemaExporter.IsEmpty; }
    // Purpose: Verify JsonSchemaExporter.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_set_IsEmpty_System_Void_P0() { JsonSchemaExporter.IsEmpty = null!; }
    // Purpose: Verify JsonSchemaExporter.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_get_Key_System_Void() { JsonSchemaExporter.Key; }
    // Purpose: Verify JsonSchemaExporter.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporter_set_Key_System_Void_P0() { JsonSchemaExporter.Key = null!; }}
