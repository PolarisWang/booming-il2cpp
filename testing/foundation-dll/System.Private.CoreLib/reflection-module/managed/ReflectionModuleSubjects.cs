// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/module
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionModuleSubjects
{
    // [0] System.Private.CoreLib/System.Reflection.Module::get_FullyQualifiedName:System.String()
    [Fact]
    public static void Subject_0()
    {
        try { var __val = ((typeof(byte).Module.FullyQualifiedName).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()
    [Fact]
    public static void Subject_1()
    {
        try { var __val = ((typeof(byte).Module.Name).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()
    [Fact]
    public static void Subject_2()
    {
        try { var __val = ((typeof(byte).Module.Assembly).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Reflection.Module::GetType:System.Type(System.String)
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((typeof(byte).Module.GetType("hello")).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()
    [Fact]
    public static void Subject_4()
    {
        try { var __val = ((typeof(byte).Module.GetTypes()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type)
}