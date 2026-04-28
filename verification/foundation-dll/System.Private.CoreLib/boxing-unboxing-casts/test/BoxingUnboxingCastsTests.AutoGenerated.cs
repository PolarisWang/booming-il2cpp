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

// Auto-generated skeleton for Boxing Unboxing Casts (family/System.Private.CoreLib/boxing/unboxing-casts).
public partial class BoxingUnboxingCastsTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type()
    {
    var result = Convert.ChangeType(42, typeof(byte));
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Convert_ChangeType_System_Object_System_Object_System_Type_System_IFormatProvider()
    {
    _ = Convert.ChangeType(42, typeof(byte), null!);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_IsAssignableFrom_System_Boolean_System_Type()
    {
    var result = typeof(byte).IsAssignableFrom(typeof(byte));
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_IsInstanceOfType_System_Boolean_System_Object()
    {
    var result = typeof(byte).IsInstanceOfType(42);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_IsSubclassOf_System_Boolean_System_Type()
    {
    var result = typeof(byte).IsSubclassOf(typeof(byte));
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Type_IsAssignableTo_System_Boolean_System_Type()
    {
    var result = typeof(byte).IsAssignableTo(typeof(byte));
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — MemberwiseClone requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object()
    {
    // TODO: needs-manual — MemberwiseClone requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object()
    {
    var result = RuntimeHelpers.GetObjectValue(42);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32()
    {
    var result = Array.CreateInstance(typeof(byte), 42);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32()
    {
    var result = Array.CreateInstance(typeof(byte), 42, 42);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BoxingUnboxingCasts)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_ValueType_Equals_System_Boolean_System_Object()
    {
    var result = ((ValueType)42).Equals(42);
    Xunit.Assert.NotNull((object)result);
    }}
