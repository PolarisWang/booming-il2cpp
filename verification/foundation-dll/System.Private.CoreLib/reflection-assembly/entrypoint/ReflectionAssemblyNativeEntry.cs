// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/assembly
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Globalization;
using System.IO;
using System.Reflection;

public static class ReflectionAssemblyNativeEntry
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
        return (int)(typeof(byte).Assembly.GetExecutingAssembly());
    }

    // [1] System.Private.CoreLib/System.Reflection.Assembly::GetCallingAssembly:System.Reflection.Assembly()
    static int Method1()
    {
        return (int)(typeof(byte).Assembly.GetCallingAssembly());
    }

    // [2] System.Private.CoreLib/System.Reflection.Assembly::GetEntryAssembly:System.Reflection.Assembly()
    static int Method2()
    {
        return (int)(typeof(byte).Assembly.GetEntryAssembly());
    }

    // [3] System.Private.CoreLib/System.Reflection.Assembly::GetName:System.Reflection.AssemblyName()
    static int Method3()
    {
        return (int)(typeof(byte).Assembly.GetName());
    }

    // [4] System.Private.CoreLib/System.Reflection.Assembly::get_FullName:System.String()
    static int Method4()
    {
        return (int)(typeof(byte).Assembly.FullName);
    }

    // [5] System.Private.CoreLib/System.Reflection.Assembly::get_Location:System.String()
    static int Method5()
    {
        return (int)(typeof(byte).Assembly.Location);
    }

    // [6] System.Private.CoreLib/System.Reflection.Assembly::get_ImageRuntimeVersion:System.String()
    static int Method6()
    {
        return (int)(typeof(byte).Assembly.ImageRuntimeVersion);
    }

    // [7] System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()
    static int Method7()
    {
        return (int)(typeof(byte).Assembly.GetTypes());
    }

    // [8] System.Private.CoreLib/System.Reflection.Assembly::GetExportedTypes:System.Type[]()
    static int Method8()
    {
        return (int)(typeof(byte).Assembly.GetExportedTypes());
    }

    // [9] System.Private.CoreLib/System.Reflection.Assembly::GetForwardedTypes:System.Type[]()
    static int Method9()
    {
        return (int)(typeof(byte).Assembly.GetForwardedTypes());
    }

    // [10] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)
    static int Method10()
    {
        return (int)(typeof(byte).Assembly.GetType("hello"));
    }

    // [11] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String,System.Boolean)
    static int Method11()
    {
        return (int)(typeof(byte).Assembly.GetType("hello", true));
    }

    // [12] System.Private.CoreLib/System.Reflection.Assembly::GetModule:System.Reflection.Module(System.String)
    static int Method12()
    {
        return (int)(typeof(byte).Assembly.GetModule("hello"));
    }

    // [13] System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()
    static int Method13()
    {
        return (int)(typeof(byte).Assembly.GetModules());
    }

    // [14] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceStream:System.IO.Stream(System.String)
    static int Method14()
    {
        return (int)(typeof(byte).Assembly.GetManifestResourceStream("hello"));
    }

    // [15] System.Private.CoreLib/System.Reflection.Assembly::GetManifestResourceNames:System.String[]()
    static int Method15()
    {
        return (int)(typeof(byte).Assembly.GetManifestResourceNames());
    }

    // [16] System.Private.CoreLib/System.Reflection.Assembly::get_EntryPoint:System.Reflection.MethodInfo()
    static int Method16()
    {
        return (int)(typeof(byte).Assembly.EntryPoint);
    }

    // [17] System.Private.CoreLib/System.Reflection.AssemblyName::get_Name:System.String()
    static int Method17()
    {
        return (int)(AssemblyName.Name);
    }

    // [18] System.Private.CoreLib/System.Reflection.AssemblyName::get_Version:System.Version()
    static int Method18()
    {
        return (int)(AssemblyName.Version);
    }

    // [19] System.Private.CoreLib/System.Reflection.AssemblyName::get_CultureInfo:System.Globalization.CultureInfo()
    static int Method19()
    {
        return (int)(AssemblyName.CultureInfo);
    }

    // [20] System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()
    static int Method20()
    {
        return (int)(typeof(byte).Assembly);
    }

    // [21] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
    static int Method21()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String) could not be auto-generated
        return 0;
    }

}