// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/type/runtime-handles
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

public static partial class TypeRuntimeHandlesSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
    public static void Subject_0()
    {
        // needs-manual — GetType with 1 params requires manual implementation: System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
    }

    // [1] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)
    public static void Subject_1()
    {
        // needs-manual — GetType with 2 params requires manual implementation: System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)
    }

    // [2] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)
    public static void Subject_2()
    {
        // needs-manual — GetType with 3 params requires manual implementation: System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)
    }

    // [3] System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
    public static void Subject_3()
    {
        try { var _ = ((Type.GetTypeFromHandle(default(System.RuntimeTypeHandle))).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()
    public static void Subject_4()
    {
        try { var _ = ((typeof(byte).IsValueType) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()
    public static void Subject_5()
    {
        try { var _ = ((typeof(byte).IsEnum) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()
    public static void Subject_6()
    {
        try { var _ = ((typeof(byte).IsArray) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()
    public static void Subject_7()
    {
        try { var _ = ((typeof(byte).TypeHandle).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Type::get_FullName:System.String()
    public static void Subject_8()
    {
        try { var _ = ((typeof(byte).FullName).Length); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Type::get_Name:System.String()
    public static void Subject_9()
    {
        try { var _ = ((typeof(byte).Name).Length); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Type::get_BaseType:System.Type()
    public static void Subject_10()
    {
        try { var _ = ((typeof(byte).BaseType).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()
    public static void Subject_11()
    {
        try { var _ = ((typeof(byte).GetMethods()).Length); }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()
    public static void Subject_12()
    {
        try { var _ = ((typeof(byte).GetFields()).Length); }
        catch { _exitCode = 1; }
    }

}