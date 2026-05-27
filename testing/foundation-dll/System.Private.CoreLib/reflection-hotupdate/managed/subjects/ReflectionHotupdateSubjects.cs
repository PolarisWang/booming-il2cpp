// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/hotupdate
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
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
        try { var _ = ((typeof(byte).Assembly.GetModules()).Length); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()
    public static void Subject_1()
    {
        try { var _ = ((typeof(byte).Assembly.GetTypes()).Length); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
    public static void Subject_2()
    {
        try { var _ = ((Type.GetTypeFromHandle(default(System.RuntimeTypeHandle))).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)
    public static void Subject_3()
    {
        try { var _ = ((typeof(byte).Assembly.GetType("hello")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

}