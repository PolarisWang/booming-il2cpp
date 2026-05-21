// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/assembly
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;

public static partial class ReflectionAssemblySubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()
    public static void Subject_0()
    {
        try { if (((Assembly.GetExecutingAssembly()).GetHashCode()) != ((Assembly.GetExecutingAssembly()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()
    public static void Subject_1()
    {
        try { if (((Assembly.GetCallingAssembly()).GetHashCode()) != ((Assembly.GetCallingAssembly()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()
    public static void Subject_2()
    {
        try { if (((Assembly.GetEntryAssembly()).GetHashCode()) != ((Assembly.GetEntryAssembly()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()
    public static void Subject_3()
    {
        try { if (((typeof(byte).Assembly.GetName()).GetHashCode()) != ((typeof(byte).Assembly.GetName()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()
    public static void Subject_4()
    {
        try { if (((typeof(byte).Assembly.FullName).Length) != ((typeof(byte).Assembly.FullName).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()
    public static void Subject_5()
    {
        try { if (((typeof(byte).Assembly.Location).Length) != ((typeof(byte).Assembly.Location).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()
    public static void Subject_6()
    {
        try { if (((typeof(byte).Assembly.ImageRuntimeVersion).Length) != ((typeof(byte).Assembly.ImageRuntimeVersion).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()
    public static void Subject_7()
    {
        try { if (((typeof(byte).Assembly.GetTypes()).Length) != ((typeof(byte).Assembly.GetTypes()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()
    public static void Subject_8()
    {
        try { if (((typeof(byte).Assembly.GetExportedTypes()).Length) != ((typeof(byte).Assembly.GetExportedTypes()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()
    public static void Subject_9()
    {
        try { if (((typeof(byte).Assembly.GetForwardedTypes()).Length) != ((typeof(byte).Assembly.GetForwardedTypes()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)
    public static void Subject_10()
    {
        try { if (((typeof(byte).Assembly.GetType("hello")).GetHashCode()) != ((typeof(byte).Assembly.GetType("hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)
    public static void Subject_11()
    {
        try { if (((typeof(byte).Assembly.GetType("hello", true)).GetHashCode()) != ((typeof(byte).Assembly.GetType("hello", true)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)
    public static void Subject_12()
    {
        try { if (((typeof(byte).Assembly.GetModule("hello")).GetHashCode()) != ((typeof(byte).Assembly.GetModule("hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()
    public static void Subject_13()
    {
        try { if (((typeof(byte).Assembly.GetModules()).Length) != ((typeof(byte).Assembly.GetModules()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)
    public static void Subject_14()
    {
        try { if (((typeof(byte).Assembly.GetManifestResourceStream("hello")).GetHashCode()) != ((typeof(byte).Assembly.GetManifestResourceStream("hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()
    public static void Subject_15()
    {
        try { if (((typeof(byte).Assembly.GetManifestResourceNames()).Length) != ((typeof(byte).Assembly.GetManifestResourceNames()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()
    public static void Subject_16()
    {
        try { if (((typeof(byte).Assembly.EntryPoint).GetHashCode()) != ((typeof(byte).Assembly.EntryPoint).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()
    public static void Subject_17()
    {
        try { if (((default(AssemblyName)!.Name).Length) != ((default(AssemblyName)!.Name).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()
    public static void Subject_18()
    {
        try { if (((default(AssemblyName)!.Version).GetHashCode()) != ((default(AssemblyName)!.Version).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()
    public static void Subject_19()
    {
        try { if (((default(AssemblyName)!.CultureInfo).GetHashCode()) != ((default(AssemblyName)!.CultureInfo).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()
    public static void Subject_20()
    {
        try { if (((typeof(byte).Assembly).GetHashCode()) != ((typeof(byte).Assembly).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)

}