// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/hotupdate
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

public static partial class ReflectionHotupdateSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()
    public static void Subject_0()
    {
        try { if (((typeof(byte).Assembly.GetModules()).Length) != ((typeof(byte).Assembly.GetModules()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()
    public static void Subject_1()
    {
        try { if (((typeof(byte).Assembly.GetTypes()).Length) != ((typeof(byte).Assembly.GetTypes()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
    public static void Subject_2()
    {
        try { if (((Type.GetTypeFromHandle(default(System.RuntimeTypeHandle))).GetHashCode()) != ((Type.GetTypeFromHandle(default(System.RuntimeTypeHandle))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)
    public static void Subject_3()
    {
        try { if (((typeof(byte).Assembly.GetType("hello")).GetHashCode()) != ((typeof(byte).Assembly.GetType("hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}