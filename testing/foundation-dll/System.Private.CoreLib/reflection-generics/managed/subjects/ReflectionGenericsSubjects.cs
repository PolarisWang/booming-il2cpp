// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/generics
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class ReflectionGenericsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()
    public static void Subject_0()
    {
        try { _ = ((typeof(byte).GetGenericTypeDefinition()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])
    public static void Subject_1()
    {
        try { _ = ((typeof(byte).MakeGenericType(Array.Empty<System.Type>())).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()
    public static void Subject_2()
    {
        try { _ = ((typeof(byte).GetGenericArguments()).Length); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Type::GetGenericParameterConstraints:System.Type[]()
    public static void Subject_3()
    {
        try { _ = ((typeof(byte).GetGenericParameterConstraints()).Length); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Type::get_GenericParameterPosition:System.Int32()
    public static void Subject_4()
    {
        try { _ = typeof(byte).GenericParameterPosition; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean()
    public static void Subject_5()
    {
        try { _ = ((typeof(byte).ContainsGenericParameters) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()
    public static void Subject_6()
    {
        try { _ = ((typeof(byte).IsGenericType) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition:System.Boolean()
    public static void Subject_7()
    {
        try { _ = ((typeof(byte).IsGenericTypeDefinition) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()
    public static void Subject_8()
    {
        try { _ = ((typeof(byte).IsConstructedGenericType) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Type::MakeArrayType:System.Type()
    public static void Subject_9()
    {
        try { _ = ((typeof(byte).MakeArrayType()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Type::MakeByRefType:System.Type()
    public static void Subject_10()
    {
        try { _ = ((typeof(byte).MakeByRefType()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Type::MakePointerType:System.Type()
    public static void Subject_11()
    {
        try { _ = ((typeof(byte).MakePointerType()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

}