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

// Auto-generated patch-side skeletons for PrimitiveNumericConversionsPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static partial class PrimitiveNumericConversionsPatch
{
    // Purpose: Verify Convert.ToBoolean executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToBoolean:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Convert_ToBoolean_System_Boolean_System_String() { }
    // Purpose: Verify Convert.ToByte executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToByte:System.Byte(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Convert_ToByte_System_Byte_System_String() { }
    // Purpose: Verify Convert.ToInt16 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToInt16:System.Int16(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Convert_ToInt16_System_Int16_System_String() { }
    // Purpose: Verify Convert.ToInt32 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Convert_ToInt32_System_Int32_System_String() { }
    // Purpose: Verify Convert.ToInt64 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Convert_ToInt64_System_Int64_System_String() { }
    // Purpose: Verify Convert.ToSingle executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Convert_ToSingle_System_Single_System_String() { }
    // Purpose: Verify Convert.ToDouble executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Convert_ToDouble_System_Double_System_String() { }
    // Purpose: Verify Convert.ToDecimal executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Convert_ToDecimal_System_Decimal_System_String() { }
    // Purpose: Verify Convert.ToString executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Convert_ToString_System_String_System_Int32() { _ = Convert.ToString(42); }
    // Purpose: Verify Convert.ToString executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToString:System.String(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Convert_ToString_System_String_System_Double() { _ = Convert.ToString(42.0); }
    // Purpose: Verify Convert.ToDecimal executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Convert_ToDecimal_System_Decimal_System_Double() { }
    // Purpose: Verify Convert.ToInt32 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Convert_ToInt32_System_Int32_System_Double() { }
    // Purpose: Verify Int32.Parse executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Int32_Parse_System_Int32_System_String() { }
    // Purpose: Verify Int64.Parse executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Int64_Parse_System_Int64_System_String() { }
    // Purpose: Verify Double.Parse executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Private.CoreLib/System.Double::Parse:System.Double(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Private_CoreLib_System_Double_Parse_System_Double_System_String() { }}
