// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/parameters
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionParametersSubjects
{
    // [0] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()
    // [1] System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type()
    // [2] System.Private.CoreLib/System.Reflection.ParameterInfo::get_DefaultValue:System.Object()
    // [3] System.Private.CoreLib/System.Reflection.ParameterInfo::get_RawDefaultValue:System.Object()
    // [4] System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean()
    // [5] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Attributes:System.Reflection.ParameterAttributes()
    // [6] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Position:System.Int32()
    // [7] System.Private.CoreLib/System.Reflection.ParameterInfo::GetRequiredCustomModifiers:System.Type[]()
    // [8] System.Private.CoreLib/System.Reflection.ParameterInfo::GetOptionalCustomModifiers:System.Type[]()
    // [9] System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()
    [Fact]
    public static void Subject_9()
    {
        try { var __val = ((default(MethodInfo)!.GetParameters()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}