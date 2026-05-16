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
        if (((Assembly.GetExecutingAssembly()).GetHashCode()) != ((Assembly.GetExecutingAssembly()).GetHashCode())) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()
    public static void Subject_1()
    {
        if (((Assembly.GetCallingAssembly()).GetHashCode()) != ((Assembly.GetCallingAssembly()).GetHashCode())) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()
    public static void Subject_2()
    {
        if (((Assembly.GetEntryAssembly()).GetHashCode()) != ((Assembly.GetEntryAssembly()).GetHashCode())) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()
    public static void Subject_3()
    {
        if (((typeof(byte).Assembly.GetName()).GetHashCode()) != ((typeof(byte).Assembly.GetName()).GetHashCode())) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()
    public static void Subject_4()
    {
        if (((typeof(byte).Assembly.FullName).Length) != ((typeof(byte).Assembly.FullName).Length)) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()
    public static void Subject_5()
    {
        if (((typeof(byte).Assembly.Location).Length) != ((typeof(byte).Assembly.Location).Length)) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()
    public static void Subject_6()
    {
        if (((typeof(byte).Assembly.ImageRuntimeVersion).Length) != ((typeof(byte).Assembly.ImageRuntimeVersion).Length)) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()
    public static void Subject_7()
    {
        if (((typeof(byte).Assembly.GetTypes()).Length) != ((typeof(byte).Assembly.GetTypes()).Length)) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()
    public static void Subject_8()
    {
        if (((typeof(byte).Assembly.GetExportedTypes()).Length) != ((typeof(byte).Assembly.GetExportedTypes()).Length)) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()
    public static void Subject_9()
    {
        if (((typeof(byte).Assembly.GetForwardedTypes()).Length) != ((typeof(byte).Assembly.GetForwardedTypes()).Length)) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)
    public static void Subject_10()
    {
        if (((typeof(byte).Assembly.GetType("hello")).GetHashCode()) != ((typeof(byte).Assembly.GetType("hello")).GetHashCode())) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)
    public static void Subject_11()
    {
        if (((typeof(byte).Assembly.GetType("hello", true)).GetHashCode()) != ((typeof(byte).Assembly.GetType("hello", true)).GetHashCode())) _exitCode = 1;
    }

    // [12] System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)
    public static void Subject_12()
    {
        if (((typeof(byte).Assembly.GetModule("hello")).GetHashCode()) != ((typeof(byte).Assembly.GetModule("hello")).GetHashCode())) _exitCode = 1;
    }

    // [13] System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()
    public static void Subject_13()
    {
        if (((typeof(byte).Assembly.GetModules()).Length) != ((typeof(byte).Assembly.GetModules()).Length)) _exitCode = 1;
    }

    // [14] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)
    public static void Subject_14()
    {
        if (((typeof(byte).Assembly.GetManifestResourceStream("hello")).GetHashCode()) != ((typeof(byte).Assembly.GetManifestResourceStream("hello")).GetHashCode())) _exitCode = 1;
    }

    // [15] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()
    public static void Subject_15()
    {
        if (((typeof(byte).Assembly.GetManifestResourceNames()).Length) != ((typeof(byte).Assembly.GetManifestResourceNames()).Length)) _exitCode = 1;
    }

    // [16] System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()
    public static void Subject_16()
    {
        if (((typeof(byte).Assembly.EntryPoint).GetHashCode()) != ((typeof(byte).Assembly.EntryPoint).GetHashCode())) _exitCode = 1;
    }

    // [17] System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()
    public static void Subject_17()
    {
        if (((default(AssemblyName)!.Name).Length) != ((default(AssemblyName)!.Name).Length)) _exitCode = 1;
    }

    // [18] System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()
    public static void Subject_18()
    {
        if (((default(AssemblyName)!.Version).GetHashCode()) != ((default(AssemblyName)!.Version).GetHashCode())) _exitCode = 1;
    }

    // [19] System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()
    public static void Subject_19()
    {
        if (((default(AssemblyName)!.CultureInfo).GetHashCode()) != ((default(AssemblyName)!.CultureInfo).GetHashCode())) _exitCode = 1;
    }

    // [20] System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()
    public static void Subject_20()
    {
        if (((typeof(byte).Assembly).GetHashCode()) != ((typeof(byte).Assembly).GetHashCode())) _exitCode = 1;
    }

    // [21] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
    public static void Subject_21()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String) could not be auto-generated
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(); break;
            case 4: Subject_4(); break;
            case 5: Subject_5(); break;
            case 6: Subject_6(); break;
            case 7: Subject_7(); break;
            case 8: Subject_8(); break;
            case 9: Subject_9(); break;
            case 10: Subject_10(); break;
            case 11: Subject_11(); break;
            case 12: Subject_12(); break;
            case 13: Subject_13(); break;
            case 14: Subject_14(); break;
            case 15: Subject_15(); break;
            case 16: Subject_16(); break;
            case 17: Subject_17(); break;
            case 18: Subject_18(); break;
            case 19: Subject_19(); break;
            case 20: Subject_20(); break;
            case 21: Subject_21(); break;
        }
    }

}