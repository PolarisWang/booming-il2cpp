// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/module
// Assembly: System.Private.CoreLib
// Variant: semantic-patch

using System;
using System.Reflection;

public static class ReflectionModuleSemanticPatchEntry
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
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Reflection.Module::get_FullyQualifiedName:System.String()
    static int Method0()
    {
        return (typeof(int).Assembly.GetModules()[0].FullyQualifiedName?.GetHashCode() ?? 0);
    }

    // [1] System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()
    static int Method1()
    {
        return (typeof(int).Assembly.GetModules()[0].Name?.GetHashCode() ?? 0);
    }

    // [2] System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()
    static int Method2()
    {
        return typeof(int).Assembly.GetModules()[0].Assembly.GetHashCode();
    }

    // [3] System.Private.CoreLib/System.Reflection.Module::GetType:System.Type(System.String)
    static int Method3()
    {
        return typeof(int).Assembly.GetModules()[0].GetType("99").GetHashCode();
    }

    // [4] System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()
    static int Method4()
    {
        return typeof(int).Assembly.GetModules()[0].GetTypes().GetHashCode();
    }

    // [5] System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type)
    static int Method5()
    {
        return typeof(int).Assembly.GetModules()[0].GetCustomAttributes(typeof(int)).GetHashCode();
    }

}