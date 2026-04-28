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

// Hot-update stubs for family/System.Private.CoreLib/convert/char
public static class ConvertCharHotUpdate
{
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Boolean() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Byte() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Char() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_DateTime() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Decimal() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Double() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int16() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int32() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int64() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object_System_IFormatProvider() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_SByte() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Single() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String_System_IFormatProvider() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt16() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt32() { }
    // Verify Convert.ToChar after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt64() { }}
