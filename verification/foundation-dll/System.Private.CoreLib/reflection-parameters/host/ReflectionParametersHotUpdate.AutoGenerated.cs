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

// Auto-generated hot-update skeletons for ReflectionParametersHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class ReflectionParametersHotUpdate
{
    // Purpose: Verify ParameterInfo.get_Name executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_ParameterInfo_get_Name_System_String() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].Name; }
    // Purpose: Verify ParameterInfo.get_ParameterType executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_ParameterInfo_get_ParameterType_System_Type() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].ParameterType; }
    // Purpose: Verify ParameterInfo.get_DefaultValue executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_DefaultValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_ParameterInfo_get_DefaultValue_System_Object() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].DefaultValue; }
    // Purpose: Verify ParameterInfo.get_RawDefaultValue executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_RawDefaultValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_ParameterInfo_get_RawDefaultValue_System_Object() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].RawDefaultValue; }
    // Purpose: Verify ParameterInfo.get_HasDefaultValue executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_ParameterInfo_get_HasDefaultValue_System_Boolean() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].HasDefaultValue; }
    // Purpose: Verify ParameterInfo.get_Attributes executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Attributes:System.Reflection.ParameterAttributes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_ParameterInfo_get_Attributes_System_Reflection_ParameterAttributes() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].Attributes; }
    // Purpose: Verify ParameterInfo.get_Position executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Position:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_ParameterInfo_get_Position_System_Int32() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].Position; }
    // Purpose: Verify ParameterInfo.GetRequiredCustomModifiers executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::GetRequiredCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_ParameterInfo_GetRequiredCustomModifiers_System_Type() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].GetRequiredCustomModifiers(); }
    // Purpose: Verify ParameterInfo.GetOptionalCustomModifiers executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::GetOptionalCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_ParameterInfo_GetOptionalCustomModifiers_System_Type() { _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].GetOptionalCustomModifiers(); }
    // Purpose: Verify MethodBase.GetParameters executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Reflection_MethodBase_GetParameters_System_Reflection_ParameterInfo() { _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.GetParameters(); }}
