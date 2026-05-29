// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/module
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionModuleSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.Module::get_FullyQualifiedName:System.String()
    public static void Subject_0()
    {
        try { _ = ((default(Module)!.FullyQualifiedName).Length); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Reflection.Module::get_Name:System.String()
    public static void Subject_1()
    {
        try { _ = ((default(Module)!.Name).Length); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Reflection.Module::get_Assembly:System.Reflection.Assembly()
    public static void Subject_2()
    {
        try { _ = ((default(Module)!.Assembly).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Reflection.Module::GetType:System.Type(System.String)
    public static void Subject_3()
    {
        try { _ = ((default(Module)!.GetType("hello")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Reflection.Module::GetTypes:System.Type[]()
    public static void Subject_4()
    {
        try { _ = ((default(Module)!.GetTypes()).Length); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type)
}