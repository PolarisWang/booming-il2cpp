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

// Auto-generated skeleton for Activation And Instantiation (family/System.Private.CoreLib/reflection/activation).
public partial class ReflectionActivationTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type()
    {
    var result = Activator.CreateInstance(typeof(byte));
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type_System_Object()
    {
    var result = Activator.CreateInstance(typeof(byte), Array.Empty<System.Object>());
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:T()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Activator_CreateInstance_T()
    {
    _ = Activator.CreateInstance<object>();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Activator_CreateInstance_System_Object_System_Type_System_Reflection_BindingFlags_System_R()
    {
    _ = Activator.CreateInstance(typeof(byte), null!, null!, Array.Empty<System.Object>(), null!);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32()
    {
    var result = Array.CreateInstance(typeof(byte), 42);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32_System_Int32()
    {
    var result = Array.CreateInstance(typeof(byte), 42, 42);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Array_CreateInstance_System_Array_System_Type_System_Int32()
    {
    var result = Array.CreateInstance(typeof(byte), Array.Empty<System.Int32>());
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionActivation)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_ConstructorInfo_Invoke_System_Object_System_Object()
    {
    var result = typeof(object).GetConstructor(System.Type.EmptyTypes)!.Invoke(null);
    Xunit.Assert.NotNull((object)result);
    }}
