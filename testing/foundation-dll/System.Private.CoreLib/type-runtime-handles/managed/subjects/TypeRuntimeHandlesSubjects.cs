// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/type/runtime/handles
// Assembly: System.Private.CoreLib
// Variant: subjects

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
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String) could not be auto-generated
    }

    // [1] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)
    public static void Subject_1()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean) could not be auto-generated
    }

    // [2] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)
    public static void Subject_2()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean) could not be auto-generated
    }

    // [3] System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
    public static void Subject_3()
    {
        try { if (((Type.GetTypeFromHandle(default(System.RuntimeTypeHandle))).GetHashCode()) != ((Type.GetTypeFromHandle(default(System.RuntimeTypeHandle))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()
    public static void Subject_4()
    {
        try { if (((typeof(byte).IsValueType) ? 1 : 0) != ((typeof(byte).IsValueType) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()
    public static void Subject_5()
    {
        try { if (((typeof(byte).IsEnum) ? 1 : 0) != ((typeof(byte).IsEnum) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()
    public static void Subject_6()
    {
        try { if (((typeof(byte).IsArray) ? 1 : 0) != ((typeof(byte).IsArray) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()
    public static void Subject_7()
    {
        try { if (((typeof(byte).TypeHandle).GetHashCode()) != ((typeof(byte).TypeHandle).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Type::get_FullName:System.String()
    public static void Subject_8()
    {
        try { if (((typeof(byte).FullName).Length) != ((typeof(byte).FullName).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Type::get_Name:System.String()
    public static void Subject_9()
    {
        try { if (((typeof(byte).Name).Length) != ((typeof(byte).Name).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Type::get_BaseType:System.Type()
    public static void Subject_10()
    {
        try { if (((typeof(byte).BaseType).GetHashCode()) != ((typeof(byte).BaseType).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()
    public static void Subject_11()
    {
        try { if (((typeof(byte).GetMethods()).Length) != ((typeof(byte).GetMethods()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()
    public static void Subject_12()
    {
        try { if (((typeof(byte).GetFields()).Length) != ((typeof(byte).GetFields()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}