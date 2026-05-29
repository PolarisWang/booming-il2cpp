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
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name:System.String()
    public static void Subject_0()
    {
        try { _ = ((default(ParameterInfo)!.Name).Length); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Reflection.ParameterInfo::get_ParameterType:System.Type()
    public static void Subject_1()
    {
        try { _ = ((default(ParameterInfo)!.ParameterType).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Reflection.ParameterInfo::get_DefaultValue:System.Object()
    public static void Subject_2()
    {
        try { _ = ((default(ParameterInfo)!.DefaultValue).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Reflection.ParameterInfo::get_RawDefaultValue:System.Object()
    public static void Subject_3()
    {
        try { _ = ((default(ParameterInfo)!.RawDefaultValue).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Reflection.ParameterInfo::get_HasDefaultValue:System.Boolean()
    public static void Subject_4()
    {
        try { _ = ((default(ParameterInfo)!.HasDefaultValue) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Attributes:System.Reflection.ParameterAttributes()
    public static void Subject_5()
    {
        try { _ = ((default(ParameterInfo)!.Attributes).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Reflection.ParameterInfo::get_Position:System.Int32()
    public static void Subject_6()
    {
        try { _ = default(ParameterInfo)!.Position; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Reflection.ParameterInfo::GetRequiredCustomModifiers:System.Type[]()
    // [8] System.Private.CoreLib/System.Reflection.ParameterInfo::GetOptionalCustomModifiers:System.Type[]()
    // [9] System.Private.CoreLib/System.Reflection.MethodBase::GetParameters:System.Reflection.ParameterInfo[]()
}