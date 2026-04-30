// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/assembly
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Globalization;
using System.IO;
using System.Reflection;

public static class SystemPrivateCoreLib_ReflectionAssemblyNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
            case 4: return Method4();
            case 5: return Method5();
            case 6: return Method6();
            case 7: return Method7();
            case 8: return Method8();
            case 9: return Method9();
            case 10: return Method10();
            case 11: return Method11();
            case 12: return Method12();
            case 13: return Method13();
            case 14: return Method14();
            case 15: return Method15();
            case 16: return Method16();
            case 17: return Method17();
            case 18: return Method18();
            case 19: return Method19();
            case 20: return Method20();
            case 21: return Method21();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:System.Reflection.Assembly()
    static int Method0()
    {
        return typeof(byte).Assembly.GetExecutingAssembly().GetHashCode();
    }

    // [1] System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()
    static int Method1()
    {
        return typeof(byte).Assembly.GetCallingAssembly().GetHashCode();
    }

    // [2] System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()
    static int Method2()
    {
        return typeof(byte).Assembly.GetEntryAssembly().GetHashCode();
    }

    // [3] System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()
    static int Method3()
    {
        return typeof(byte).Assembly.GetName().GetHashCode();
    }

    // [4] System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()
    static int Method4()
    {
        return (typeof(byte).Assembly.FullName?.GetHashCode() ?? 0);
    }

    // [5] System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()
    static int Method5()
    {
        return (typeof(byte).Assembly.Location?.GetHashCode() ?? 0);
    }

    // [6] System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()
    static int Method6()
    {
        return (typeof(byte).Assembly.ImageRuntimeVersion?.GetHashCode() ?? 0);
    }

    // [7] System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()
    static int Method7()
    {
        return typeof(byte).Assembly.GetTypes().GetHashCode();
    }

    // [8] System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()
    static int Method8()
    {
        return typeof(byte).Assembly.GetExportedTypes().GetHashCode();
    }

    // [9] System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()
    static int Method9()
    {
        return typeof(byte).Assembly.GetForwardedTypes().GetHashCode();
    }

    // [10] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)
    static int Method10()
    {
        return typeof(byte).Assembly.GetType("hello").GetHashCode();
    }

    // [11] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)
    static int Method11()
    {
        return typeof(byte).Assembly.GetType("hello", true).GetHashCode();
    }

    // [12] System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)
    static int Method12()
    {
        return typeof(byte).Assembly.GetModule("hello").GetHashCode();
    }

    // [13] System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()
    static int Method13()
    {
        return typeof(byte).Assembly.GetModules().GetHashCode();
    }

    // [14] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)
    static int Method14()
    {
        return typeof(byte).Assembly.GetManifestResourceStream("hello").GetHashCode();
    }

    // [15] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()
    static int Method15()
    {
        return typeof(byte).Assembly.GetManifestResourceNames().GetHashCode();
    }

    // [16] System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()
    static int Method16()
    {
        return typeof(byte).Assembly.EntryPoint.GetHashCode();
    }

    // [17] System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()
    static int Method17()
    {
        return (AssemblyName.Name?.GetHashCode() ?? 0);
    }

    // [18] System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()
    static int Method18()
    {
        return AssemblyName.Version.GetHashCode();
    }

    // [19] System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()
    static int Method19()
    {
        return AssemblyName.CultureInfo.GetHashCode();
    }

    // [20] System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()
    static int Method20()
    {
        return typeof(byte).Assembly.GetHashCode();
    }

    // [21] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
    static int Method21()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String) could not be auto-generated
        return 0;
    }

}