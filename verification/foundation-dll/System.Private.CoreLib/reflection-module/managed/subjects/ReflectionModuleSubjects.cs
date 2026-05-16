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
        if (((default(Module)!.FullyQualifiedName).Length) != ((default(Module)!.FullyQualifiedName).Length)) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()
    public static void Subject_1()
    {
        if (((default(Module)!.Name).Length) != ((default(Module)!.Name).Length)) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()
    public static void Subject_2()
    {
        if (((default(Module)!.Assembly).GetHashCode()) != ((default(Module)!.Assembly).GetHashCode())) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Reflection.Module::GetType:System.Type(System.String)
    public static void Subject_3()
    {
        if (((default(Module)!.GetType("hello")).GetHashCode()) != ((default(Module)!.GetType("hello")).GetHashCode())) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()
    public static void Subject_4()
    {
        if (((default(Module)!.GetTypes()).Length) != ((default(Module)!.GetTypes()).Length)) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type)
    public static void Subject_5()
    {
        // TODO: System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type) could not be auto-generated
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
        }
    }

}