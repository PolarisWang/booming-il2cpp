// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/assembly
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Globalization;
using System.IO;
using System.Reflection;

public static partial class ReflectionAssemblySubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()
    public static void Subject_0()
    {
        if (((Assembly.GetExecutingAssembly()).GetHashCode()) != 7880838) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()
    public static void Subject_1()
    {
        if (((Assembly.GetCallingAssembly()).GetHashCode()) != 7880838) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()
    public static void Subject_2()
    {
        if (((Assembly.GetEntryAssembly()).GetHashCode()) != 7880838) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()
    public static void Subject_3()
    {
        if (((typeof(byte).Assembly.GetName()).GetHashCode()) != 56793269) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()
    public static void Subject_4()
    {
        if (((typeof(byte).Assembly.FullName).Length) != 90) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()
    public static void Subject_5()
    {
        if (((typeof(byte).Assembly.Location).Length) != 86) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()
    public static void Subject_6()
    {
        if (((typeof(byte).Assembly.ImageRuntimeVersion).Length) != 10) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()
    public static void Subject_7()
    {
        if (((typeof(byte).Assembly.GetTypes()).Length) != 2897) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()
    public static void Subject_8()
    {
        if (((typeof(byte).Assembly.GetExportedTypes()).Length) != 1378) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()
    public static void Subject_9()
    {
        if (((typeof(byte).Assembly.GetForwardedTypes()).Length) != 0) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)
    public static void Subject_10()
    {
        try { typeof(byte).Assembly.GetType("hello"); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [11] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)
    public static void Subject_11()
    {
        try { typeof(byte).Assembly.GetType("hello", true); _exitCode = 1; }
        catch (TypeLoadException) { }
    }

    // [12] System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)
    public static void Subject_12()
    {
        try { typeof(byte).Assembly.GetModule("hello"); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [13] System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()
    public static void Subject_13()
    {
        if (((typeof(byte).Assembly.GetModules()).Length) != 1) _exitCode = 1;
    }

    // [14] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)
    public static void Subject_14()
    {
        try { typeof(byte).Assembly.GetManifestResourceStream("hello"); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [15] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()
    public static void Subject_15()
    {
        if (((typeof(byte).Assembly.GetManifestResourceNames()).Length) != 4) _exitCode = 1;
    }

    // [16] System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()
    public static void Subject_16()
    {
        try { _ = typeof(byte).Assembly.EntryPoint; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [17] System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()
    public static void Subject_17()
    {
        try { _ = default(AssemblyName)!.Name; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [18] System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()
    public static void Subject_18()
    {
        try { _ = default(AssemblyName)!.Version; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [19] System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()
    public static void Subject_19()
    {
        try { _ = default(AssemblyName)!.CultureInfo; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [20] System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()
    public static void Subject_20()
    {
        if (((typeof(byte).Assembly).GetHashCode()) != 115000) _exitCode = 1;
    }

    // [21] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
    public static void Subject_21()
    {
        if (((Type.GetType("System.Int32")).GetHashCode()) != 1495009) _exitCode = 1;
    }

}