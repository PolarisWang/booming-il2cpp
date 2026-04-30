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
using Xunit;

// Auto-generated skeleton for Parameter Info (family/System.Private.CoreLib/reflection/parameters).
public partial class ReflectionParametersTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_ParameterInfo_get_Name_System_String()
    {
    var result = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].Name;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_ParameterInfo_get_ParameterType_System_Type()
    {
    var result = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].ParameterType;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_DefaultValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_ParameterInfo_get_DefaultValue_System_Object()
    {
    var result = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].DefaultValue;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_RawDefaultValue:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_ParameterInfo_get_RawDefaultValue_System_Object()
    {
    var result = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].RawDefaultValue;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_ParameterInfo_get_HasDefaultValue_System_Boolean()
    {
    var result = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].HasDefaultValue;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Attributes:System.Reflection.ParameterAttributes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_ParameterInfo_get_Attributes_System_Reflection_ParameterAttributes()
    {
    _ = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].Attributes;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::get_Position:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_ParameterInfo_get_Position_System_Int32()
    {
    var result = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].Position;
    Xunit.Assert.Equal(0, result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::GetRequiredCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_ParameterInfo_GetRequiredCustomModifiers_System_Type()
    {
    var result = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].GetRequiredCustomModifiers();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.ParameterInfo::GetOptionalCustomModifiers:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_ParameterInfo_GetOptionalCustomModifiers_System_Type()
    {
    var result = typeof(string).GetMethod("IndexOf", new[] { typeof(char) })!.GetParameters()[0].GetOptionalCustomModifiers();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionParameters)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_MethodBase_GetParameters_System_Reflection_ParameterInfo()
    {
    _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.GetParameters();
    }}
