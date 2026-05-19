// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/module
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Reflection;

public static partial class ReflectionModuleSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.Module::get_FullyQualifiedName:System.String()
    public static void Subject_0()
    {
        try { _ = default(Module)!.FullyQualifiedName; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()
    public static void Subject_1()
    {
        try { _ = default(Module)!.Name; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()
    public static void Subject_2()
    {
        try { _ = default(Module)!.Assembly; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Private.CoreLib/System.Reflection.Module::GetType:System.Type(System.String)
    public static void Subject_3()
    {
        try { default(Module)!.GetType("hello"); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()
    public static void Subject_4()
    {
        try { default(Module)!.GetTypes(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type)

}