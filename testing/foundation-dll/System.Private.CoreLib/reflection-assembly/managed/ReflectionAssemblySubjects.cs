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
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()
    public static void Subject_0()
    {
        try { _ = ((Assembly.GetExecutingAssembly()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()
    public static void Subject_1()
    {
        try { _ = ((Assembly.GetCallingAssembly()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()
    public static void Subject_2()
    {
        try { _ = ((Assembly.GetEntryAssembly()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()
    public static void Subject_3()
    {
        try { _ = ((typeof(byte).Assembly.GetName()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()
    public static void Subject_4()
    {
        try { _ = ((typeof(byte).Assembly.FullName).Length); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()
    public static void Subject_5()
    {
        try { _ = ((typeof(byte).Assembly.Location).Length); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()
    public static void Subject_6()
    {
        try { _ = ((typeof(byte).Assembly.ImageRuntimeVersion).Length); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()
    public static void Subject_7()
    {
        try { _ = ((typeof(byte).Assembly.GetTypes()).Length); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()
    public static void Subject_8()
    {
        try { _ = ((typeof(byte).Assembly.GetExportedTypes()).Length); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()
    public static void Subject_9()
    {
        try { _ = ((typeof(byte).Assembly.GetForwardedTypes()).Length); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)
    public static void Subject_10()
    {
        try { _ = ((typeof(byte).Assembly.GetType("hello")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)
    public static void Subject_11()
    {
        try { _ = ((typeof(byte).Assembly.GetType("hello", true)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)
    public static void Subject_12()
    {
        try { _ = ((typeof(byte).Assembly.GetModule("hello")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()
    public static void Subject_13()
    {
        try { _ = ((typeof(byte).Assembly.GetModules()).Length); }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)
    public static void Subject_14()
    {
        try { _ = ((typeof(byte).Assembly.GetManifestResourceStream("hello")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [15] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()
    public static void Subject_15()
    {
        try { _ = ((typeof(byte).Assembly.GetManifestResourceNames()).Length); }
        catch { _exitCode = 1; }
    }

    // [16] System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()
    public static void Subject_16()
    {
        try { _ = ((typeof(byte).Assembly.EntryPoint).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [17] System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()
    public static void Subject_17()
    {
        try { _ = ((default(AssemblyName)!.Name).Length); }
        catch { _exitCode = 1; }
    }

    // [18] System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()
    public static void Subject_18()
    {
        try { _ = ((default(AssemblyName)!.Version).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [19] System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()
    public static void Subject_19()
    {
        try { _ = ((default(AssemblyName)!.CultureInfo).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [20] System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()
    public static void Subject_20()
    {
        try { _ = ((typeof(byte).Assembly).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [21] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
}