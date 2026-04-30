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

// Auto-generated skeleton for Module Reflection (family/System.Private.CoreLib/reflection/module).
public partial class ReflectionModuleTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Reflection.Module::get_FullyQualifiedName:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_Module_get_FullyQualifiedName_System_String()
    {
    var result = typeof(string).Assembly.GetModules()[0].FullyQualifiedName;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_Module_get_Name_System_String()
    {
    var result = typeof(string).Assembly.GetModules()[0].Name;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_Module_get_Assembly_System_Reflection_Assembly()
    {
    _ = typeof(string).Assembly.GetModules()[0].Assembly;
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.Module::GetType:System.Type(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_Module_GetType_System_Type_System_String()
    {
    var result = typeof(string).Assembly.GetModules()[0].GetType("System.String");
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_Module_GetTypes_System_Type()
    {
    var result = typeof(string).Assembly.GetModules()[0].GetTypes();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ReflectionModule)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Reflection_Module_GetCustomAttributes_System_Object_System_Type()
    {
    var result = typeof(string).Assembly.GetModules()[0].GetCustomAttributes(typeof(System.Reflection.AssemblyDescriptionAttribute));
    Xunit.Assert.NotNull((object)result);
    }}
