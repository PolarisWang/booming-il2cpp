// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/generics
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class ReflectionGenericsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()
    public static void Subject_0()
    {
        try { typeof(byte).GetGenericTypeDefinition(); _exitCode = 1; }
        catch (InvalidOperationException) { }
    }

    // [1] System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])
    public static void Subject_1()
    {
        try { typeof(byte).MakeGenericType(Array.Empty<System.Type>()); _exitCode = 1; }
        catch (InvalidOperationException) { }
    }

    // [2] System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()
    public static void Subject_2()
    {
        if (((typeof(byte).GetGenericArguments()).Length) != 0) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Type::GetGenericParameterConstraints:System.Type[]()
    public static void Subject_3()
    {
        try { typeof(byte).GetGenericParameterConstraints(); _exitCode = 1; }
        catch (InvalidOperationException) { }
    }

    // [4] System.Private.CoreLib/System.Type::get_GenericParameterPosition:System.Int32()
    public static void Subject_4()
    {
        try { _ = typeof(byte).GenericParameterPosition; _exitCode = 1; }
        catch (InvalidOperationException) { }
    }

    // [5] System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean()
    public static void Subject_5()
    {
        if (((typeof(byte).ContainsGenericParameters) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()
    public static void Subject_6()
    {
        if (((typeof(byte).IsGenericType) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition:System.Boolean()
    public static void Subject_7()
    {
        if (((typeof(byte).IsGenericTypeDefinition) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()
    public static void Subject_8()
    {
        if (((typeof(byte).IsConstructedGenericType) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Type::MakeArrayType:System.Type()
    public static void Subject_9()
    {
        if (((typeof(byte).MakeArrayType()).GetHashCode()) != 35342034) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Type::MakeByRefType:System.Type()
    public static void Subject_10()
    {
        if (((typeof(byte).MakeByRefType()).GetHashCode()) != 56793269) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.Type::MakePointerType:System.Type()
    public static void Subject_11()
    {
        if (((typeof(byte).MakePointerType()).GetHashCode()) != 115000) _exitCode = 1;
    }

}