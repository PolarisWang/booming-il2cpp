// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/module
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Reflection;

public static partial class ReflectionModuleSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.Module::get_FullyQualifiedName:System.String()
    public static void Subject_0()
    {
        try { if (((default(Module)!.FullyQualifiedName).Length) != ((default(Module)!.FullyQualifiedName).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()
    public static void Subject_1()
    {
        try { if (((default(Module)!.Name).Length) != ((default(Module)!.Name).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()
    public static void Subject_2()
    {
        try { if (((default(Module)!.Assembly).GetHashCode()) != ((default(Module)!.Assembly).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Reflection.Module::GetType:System.Type(System.String)
    public static void Subject_3()
    {
        try { if (((default(Module)!.GetType("hello")).GetHashCode()) != ((default(Module)!.GetType("hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()
    public static void Subject_4()
    {
        try { if (((default(Module)!.GetTypes()).Length) != ((default(Module)!.GetTypes()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type)
    public static void Subject_5()
    {
        try { if (((new List<Attribute>(typeof(byte).Module.GetCustomAttributes(false)).Count).Length) != ((new List<Attribute>(typeof(byte).Module.GetCustomAttributes(false)).Count).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}