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

// Auto-generated skeleton for Generics Reflection (family/System.Private.CoreLib/reflection/generics).
public partial class ReflectionGenericsTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetGenericTypeDefinition_System_Type()
    {
    var result = typeof(System.Collections.Generic.List<int>).GetGenericTypeDefinition();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_MakeGenericType_System_Type_System_Type()
    {
    var result = typeof(System.Collections.Generic.List<>).MakeGenericType(Array.Empty<System.Type>());
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetGenericArguments_System_Type()
    {
    var result = typeof(System.Collections.Generic.List<int>).GetGenericArguments();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::GetGenericParameterConstraints:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_GetGenericParameterConstraints_System_Type()
    {
    var result = typeof(System.Collections.Generic.List<int>).GetGenericParameterConstraints();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_GenericParameterPosition:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_GenericParameterPosition_System_Int32()
    {
    var result = typeof(System.Collections.Generic.List<>).GetGenericArguments()[0].GenericParameterPosition;
    Xunit.Assert.Equal(0, result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_ContainsGenericParameters_System_Boolean()
    {
    var result = typeof(System.Collections.Generic.List<>).ContainsGenericParameters();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsGenericType_System_Boolean()
    {
    var result = typeof(System.Collections.Generic.List<int>).IsGenericType;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsGenericTypeDefinition_System_Boolean()
    {
    var result = typeof(System.Collections.Generic.List<int>).IsGenericTypeDefinition;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_get_IsConstructedGenericType_System_Boolean()
    {
    var result = typeof(System.Collections.Generic.List<int>).IsConstructedGenericType;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::MakeArrayType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_MakeArrayType_System_Type()
    {
    var result = typeof(byte).MakeArrayType();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::MakeByRefType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_MakeByRefType_System_Type()
    {
    var result = typeof(byte).MakeByRefType();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::MakePointerType:System.Type()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionGenerics)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_MakePointerType_System_Type()
    {
    var result = typeof(byte).MakePointerType();
    Xunit.Assert.NotNull((object)result);
    }}
