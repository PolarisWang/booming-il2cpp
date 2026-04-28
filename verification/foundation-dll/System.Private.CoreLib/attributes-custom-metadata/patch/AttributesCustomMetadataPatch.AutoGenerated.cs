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

// Auto-generated patch-side skeletons for AttributesCustomMetadataPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class AttributesCustomMetadataPatch
{
    // Purpose: Verify Attribute.GetCustomAttribute executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_MemberInfo_System_Type() { _ = typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute)); }
    // Purpose: Verify Attribute.GetCustomAttribute executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Attribute_GetCustomAttribute_System_Attribute_System_Reflection_Assembly_System_Type() { _ = typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute)); }
    // Purpose: Verify Attribute.GetCustomAttributes executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo_System_Type() { _ = typeof(byte).Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute)); }
    // Purpose: Verify Attribute.GetCustomAttributes executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly_System_Type() { _ = typeof(byte).Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute)); }
    // Purpose: Verify Attribute.GetCustomAttributes executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_MemberInfo() { _ = typeof(byte).Assembly.GetCustomAttributes(); }
    // Purpose: Verify Attribute.GetCustomAttributes executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Attribute_GetCustomAttributes_System_Attribute_System_Reflection_Assembly() { _ = typeof(byte).Assembly.GetCustomAttributes(); }
    // Purpose: Verify Attribute.IsDefined executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.MemberInfo,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_MemberInfo_System_Type() { _ = typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute)); }
    // Purpose: Verify Attribute.IsDefined executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.Assembly,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Attribute_IsDefined_System_Boolean_System_Reflection_Assembly_System_Type() { _ = typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute)); }
    // Purpose: Verify Attribute.get_TypeId executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Attribute::get_TypeId:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_AttributesCustomMetadata)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Attribute_get_TypeId_System_Object() { }}
