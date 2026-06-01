// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/assembly
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionAssemblySubjects
{
    // [0] System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()
    [Fact]
    public static void Subject_0()
    {
        try { var __val = ((Assembly.GetExecutingAssembly()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()
    [Fact]
    public static void Subject_1()
    {
        try { var __val = ((Assembly.GetCallingAssembly()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()
    [Fact]
    public static void Subject_2()
    {
        try { var __val = ((Assembly.GetEntryAssembly()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((typeof(byte).Assembly.GetName()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()
    [Fact]
    public static void Subject_4()
    {
        try { var __val = ((typeof(byte).Assembly.FullName).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()
    [Fact]
    public static void Subject_5()
    {
        try { var __val = ((typeof(byte).Assembly.Location).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()
    [Fact]
    public static void Subject_6()
    {
        try { var __val = ((typeof(byte).Assembly.ImageRuntimeVersion).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()
    [Fact]
    public static void Subject_7()
    {
        try { var __val = ((typeof(byte).Assembly.GetTypes()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()
    [Fact]
    public static void Subject_8()
    {
        try { var __val = ((typeof(byte).Assembly.GetExportedTypes()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()
    [Fact]
    public static void Subject_9()
    {
        try { var __val = ((typeof(byte).Assembly.GetForwardedTypes()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)
    [Fact]
    public static void Subject_10()
    {
        try { var __val = ((typeof(byte).Assembly.GetType("hello")).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [11] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)
    [Fact]
    public static void Subject_11()
    {
        try { var __val = ((typeof(byte).Assembly.GetType("hello", true)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [12] System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)
    [Fact]
    public static void Subject_12()
    {
        try { var __val = ((typeof(byte).Assembly.GetModule("hello")).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [13] System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()
    [Fact]
    public static void Subject_13()
    {
        try { var __val = ((typeof(byte).Assembly.GetModules()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [14] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)
    [Fact]
    public static void Subject_14()
    {
        try { var __val = ((typeof(byte).Assembly.GetManifestResourceStream("hello")).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [15] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()
    [Fact]
    public static void Subject_15()
    {
        try { var __val = ((typeof(byte).Assembly.GetManifestResourceNames()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [16] System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()
    [Fact]
    public static void Subject_16()
    {
        try { var __val = ((typeof(byte).Assembly.EntryPoint).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [17] System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()
    [Fact]
    public static void Subject_17()
    {
        try { var __val = ((new AssemblyName("Test").Name).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [18] System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()
    [Fact]
    public static void Subject_18()
    {
        try { var __val = ((new AssemblyName("Test").Version).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [19] System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()
    [Fact]
    public static void Subject_19()
    {
        try { var __val = ((new AssemblyName("Test").CultureInfo).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [20] System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()
    [Fact]
    public static void Subject_20()
    {
        try { var __val = ((typeof(byte).Assembly).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [21] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
}