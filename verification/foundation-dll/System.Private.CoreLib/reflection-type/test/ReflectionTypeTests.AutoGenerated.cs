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

// Auto-generated skeleton for Type Reflection Queries (family/System.Private.CoreLib/reflection/type).
public partial class ReflectionTypeTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetType_System_Type_System_String()
    {
    var result = System.Type.GetType("System.String");
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean()
    {
    var result = System.Type.GetType("System.String", false);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetType_System_Type_System_String_System_Boolean_System_Boolean()
    {
    var result = System.Type.GetType("System.String", false, false);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetTypeFromHandle_System_Type_System_RuntimeTypeHandle()
    {
    var result = System.Type.GetTypeFromHandle(typeof(string).TypeHandle);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_Name_System_String()
    {
    var result = typeof(byte).Name;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_FullName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_FullName_System_String()
    {
    var result = typeof(byte).FullName;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_Namespace:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_Namespace_System_String()
    {
    var result = typeof(byte).Namespace;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_AssemblyQualifiedName_System_String()
    {
    var result = typeof(byte).AssemblyQualifiedName;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_TypeHandle_System_RuntimeTypeHandle()
    {
    _ = typeof(byte).TypeHandle;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsValueType_System_Boolean()
    {
    var result = typeof(byte).IsValueType;
    Xunit.Assert.True(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsEnum_System_Boolean()
    {
    var result = typeof(byte).IsEnum;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsAbstract_System_Boolean()
    {
    var result = typeof(byte).IsAbstract;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsSealed_System_Boolean()
    {
    var result = typeof(byte).IsSealed;
    Xunit.Assert.True(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsInterface_System_Boolean()
    {
    var result = typeof(byte).IsInterface;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsArray_System_Boolean()
    {
    var result = typeof(byte).IsArray;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsByRef_System_Boolean()
    {
    var result = typeof(byte).IsByRef;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsPointer_System_Boolean()
    {
    var result = typeof(byte).IsPointer;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsGenericType_System_Boolean()
    {
    var result = typeof(byte).IsGenericType;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsConstructedGenericType_System_Boolean()
    {
    var result = typeof(byte).IsConstructedGenericType;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsNested_System_Boolean()
    {
    var result = typeof(byte).IsNested;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsPublic_System_Boolean()
    {
    var result = typeof(byte).IsPublic;
    Xunit.Assert.True(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsNotPublic_System_Boolean()
    {
    var result = typeof(byte).IsNotPublic;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsNestedPublic_System_Boolean()
    {
    var result = typeof(byte).IsNestedPublic;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsNestedPrivate_System_Boolean()
    {
    var result = typeof(byte).IsNestedPrivate;
    Xunit.Assert.False(result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_BaseType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_BaseType_System_Type()
    {
    var result = typeof(byte).BaseType;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_DeclaringType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_DeclaringType_System_Type()
    {
    var result = typeof(byte).DeclaringType;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_ReflectedType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_ReflectedType_System_Type()
    {
    var result = typeof(byte).ReflectedType;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_UnderlyingSystemType_System_Type()
    {
    var result = typeof(byte).UnderlyingSystemType;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo()
    {
    _ = typeof(byte).GetMethods();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetMethods_System_Reflection_MethodInfo_System_Reflection_BindingFlags()
    {
    _ = typeof(byte).GetMethods(null!);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo()
    {
    _ = typeof(byte).GetFields();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetFields_System_Reflection_FieldInfo_System_Reflection_BindingFlags()
    {
    _ = typeof(byte).GetFields(null!);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetProperties_System_Reflection_PropertyInfo()
    {
    _ = typeof(byte).GetProperties();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetProperties_System_Reflection_PropertyInfo_System_Reflection_BindingFlags()
    {
    _ = typeof(byte).GetProperties(null!);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetEvents_System_Reflection_EventInfo()
    {
    _ = typeof(byte).GetEvents();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetConstructors_System_Reflection_ConstructorInfo()
    {
    _ = typeof(byte).GetConstructors();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetConstructors_System_Reflection_ConstructorInfo_System_Reflection_BindingFlags()
    {
    _ = typeof(byte).GetConstructors(null!);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetMembers_System_Reflection_MemberInfo()
    {
    _ = typeof(byte).GetMembers();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetInterfaces_System_Type()
    {
    var result = typeof(byte).GetInterfaces();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetElementType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetElementType_System_Type()
    {
    var result = typeof(byte).GetElementType();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetNestedTypes_System_Type()
    {
    var result = typeof(byte).GetNestedTypes();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetNestedType_System_Type_System_String()
    {
    var result = typeof(byte).GetNestedType("hello");
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionType)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_Assembly_System_Reflection_Assembly()
    {
    _ = typeof(byte).Assembly;
    }}
