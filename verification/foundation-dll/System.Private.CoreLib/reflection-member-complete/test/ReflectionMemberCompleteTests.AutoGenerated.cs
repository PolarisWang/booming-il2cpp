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

// Auto-generated skeleton for MemberInfo Complete (family/System.Private.CoreLib/reflection/member-complete).
public partial class ReflectionMemberCompleteTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_MemberInfo_get_Name_System_String()
    {
    var result = typeof(string).GetMembers()[0].Name;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_MemberInfo_get_MemberType_System_Reflection_MemberTypes()
    {
    _ = typeof(string).GetMembers()[0].MemberType;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_MemberInfo_get_DeclaringType_System_Type()
    {
    var result = typeof(string).GetMembers()[0].DeclaringType;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_MemberInfo_get_ReflectedType_System_Type()
    {
    var result = typeof(string).GetMembers()[0].ReflectedType;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_MemberInfo_get_MetadataToken_System_Int32()
    {
    var result = typeof(string).GetMembers()[0].MetadataToken;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_MethodInfo_get_ReturnType_System_Type()
    {
    var result = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.ReturnType;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_MethodInfo_GetBaseDefinition_System_Reflection_MethodInfo()
    {
    _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.GetBaseDefinition();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_MethodBase_get_MethodHandle_System_RuntimeMethodHandle()
    {
    _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.MethodHandle;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_MethodBase_get_CallingConvention_System_Reflection_CallingConventions()
    {
    _ = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.CallingConvention;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_MethodBase_get_IsPublic_System_Boolean()
    {
    var result = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.IsPublic;
    Xunit.Assert.True(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_MethodBase_get_IsStatic_System_Boolean()
    {
    var result = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.IsStatic;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_MethodBase_get_IsVirtual_System_Boolean()
    {
    var result = typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.IsVirtual;
    Xunit.Assert.True(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldType_System_Type()
    {
    var result = typeof(string).GetField("Empty")!.FieldType;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsStatic_System_Boolean()
    {
    var result = typeof(string).GetField("Empty")!.IsStatic;
    Xunit.Assert.True(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsInitOnly_System_Boolean()
    {
    var result = typeof(string).GetField("Empty")!.IsInitOnly;
    Xunit.Assert.True(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_FieldInfo_get_IsLiteral_System_Boolean()
    {
    var result = typeof(string).GetField("Empty")!.IsLiteral;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_PropertyInfo_get_PropertyType_System_Type()
    {
    var result = typeof(string).GetProperty("Length")!.PropertyType;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_PropertyInfo_get_CanRead_System_Boolean()
    {
    var result = typeof(string).GetProperty("Length")!.CanRead;
    Xunit.Assert.True(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_PropertyInfo_get_CanWrite_System_Boolean()
    {
    var result = typeof(string).GetProperty("Length")!.CanWrite;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_PropertyInfo_GetIndexParameters_System_Reflection_ParameterInfo()
    {
    _ = typeof(string).GetProperty("Length")!.GetIndexParameters();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_FieldInfo_get_FieldHandle_System_RuntimeFieldHandle()
    {
    _ = typeof(string).GetField("Empty")!.FieldHandle;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention:System.Reflection.CallingConventions()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionMemberComplete)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_ConstructorInfo_get_CallingConvention_System_Reflection_CallingConventions()
    {
    _ = typeof(object).GetConstructor(System.Type.EmptyTypes)!.CallingConvention;
    }}
