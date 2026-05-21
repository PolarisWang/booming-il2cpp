// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/generics
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;

public static partial class ReflectionGenericsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()
    public static void Subject_0()
    {
        try { if (((typeof(byte).GetGenericTypeDefinition()).GetHashCode()) != ((typeof(byte).GetGenericTypeDefinition()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])
    public static void Subject_1()
    {
        try { if (((typeof(byte).MakeGenericType(Array.Empty<System.Type>())).GetHashCode()) != ((typeof(byte).MakeGenericType(Array.Empty<System.Type>())).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()
    public static void Subject_2()
    {
        try { if (((typeof(byte).GetGenericArguments()).Length) != ((typeof(byte).GetGenericArguments()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Type::GetGenericParameterConstraints:System.Type[]()
    public static void Subject_3()
    {
        try { if (((typeof(byte).GetGenericParameterConstraints()).Length) != ((typeof(byte).GetGenericParameterConstraints()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Type::get_GenericParameterPosition:System.Int32()
    public static void Subject_4()
    {
        try { if (typeof(byte).GenericParameterPosition != typeof(byte).GenericParameterPosition) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean()
    public static void Subject_5()
    {
        try { if (((typeof(byte).ContainsGenericParameters) ? 1 : 0) != ((typeof(byte).ContainsGenericParameters) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()
    public static void Subject_6()
    {
        try { if (((typeof(byte).IsGenericType) ? 1 : 0) != ((typeof(byte).IsGenericType) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition:System.Boolean()
    public static void Subject_7()
    {
        try { if (((typeof(byte).IsGenericTypeDefinition) ? 1 : 0) != ((typeof(byte).IsGenericTypeDefinition) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()
    public static void Subject_8()
    {
        try { if (((typeof(byte).IsConstructedGenericType) ? 1 : 0) != ((typeof(byte).IsConstructedGenericType) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Type::MakeArrayType:System.Type()
    public static void Subject_9()
    {
        try { if (((typeof(byte).MakeArrayType()).GetHashCode()) != ((typeof(byte).MakeArrayType()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Type::MakeByRefType:System.Type()
    public static void Subject_10()
    {
        try { if (((typeof(byte).MakeByRefType()).GetHashCode()) != ((typeof(byte).MakeByRefType()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Type::MakePointerType:System.Type()
    public static void Subject_11()
    {
        try { if (((typeof(byte).MakePointerType()).GetHashCode()) != ((typeof(byte).MakePointerType()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}