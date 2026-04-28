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

// Auto-generated hot-update skeletons for ConvertCharHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class ConvertCharHotUpdate
{
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Boolean() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Byte() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Char() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_DateTime() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Decimal() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Double() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int16() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int32() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int64() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object_System_IFormatProvider() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_SByte() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Single() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String_System_IFormatProvider() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt16() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt32() { }
    // Purpose: Verify Convert.ToChar executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt64() { }}
