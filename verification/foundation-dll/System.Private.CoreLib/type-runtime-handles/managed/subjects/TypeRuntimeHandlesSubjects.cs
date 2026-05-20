// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/type/runtime/handles
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Reflection;

public static partial class TypeRuntimeHandlesSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
    public static void Subject_0()
    {
        if (((Type.GetType("System.Int32")).GetHashCode()) != 7880838) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)
    public static void Subject_1()
    {
        if (((Type.GetType("System.Int32", false)).GetHashCode()) != 7880838) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)
    public static void Subject_2()
    {
        if (((Type.GetType("System.Int32", false, false)).GetHashCode()) != 7880838) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
    public static void Subject_3()
    {
        try { Type.GetTypeFromHandle(default(System.RuntimeTypeHandle)); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()
    public static void Subject_4()
    {
        if (((typeof(byte).IsValueType) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()
    public static void Subject_5()
    {
        if (((typeof(byte).IsEnum) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()
    public static void Subject_6()
    {
        if (((typeof(byte).IsArray) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()
    public static void Subject_7()
    {
        if (((typeof(byte).TypeHandle).GetHashCode()) != 56793269) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Type::get_FullName:System.String()
    public static void Subject_8()
    {
        if (((typeof(byte).FullName).Length) != 11) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Type::get_Name:System.String()
    public static void Subject_9()
    {
        if (((typeof(byte).Name).Length) != 4) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Type::get_BaseType:System.Type()
    public static void Subject_10()
    {
        if (((typeof(byte).BaseType).GetHashCode()) != 115000) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()
    public static void Subject_11()
    {
        if (((typeof(byte).GetMethods()).Length) != 47) _exitCode = 1;
    }

    // [12] System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()
    public static void Subject_12()
    {
        if (((typeof(byte).GetFields()).Length) != 2) _exitCode = 1;
    }

}