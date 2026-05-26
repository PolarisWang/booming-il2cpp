// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/edge
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

public static partial class ReflectionEdgeSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetElementType:System.Type()
    public static void Subject_0()
    {
        try { if (((typeof(byte).GetElementType()).GetHashCode()) != ((typeof(byte).GetElementType()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Type::MakeArrayType:System.Type()
    public static void Subject_1()
    {
        try { if (((typeof(byte).MakeArrayType()).GetHashCode()) != ((typeof(byte).MakeArrayType()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Type::MakeArrayType:System.Type(System.Int32)
    public static void Subject_2()
    {
        try { if (((typeof(byte).MakeArrayType(42)).GetHashCode()) != ((typeof(byte).MakeArrayType(42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Type::MakeByRefType:System.Type()
    public static void Subject_3()
    {
        try { if (((typeof(byte).MakeByRefType()).GetHashCode()) != ((typeof(byte).MakeByRefType()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Type::MakePointerType:System.Type()
    public static void Subject_4()
    {
        try { if (((typeof(byte).MakePointerType()).GetHashCode()) != ((typeof(byte).MakePointerType()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()
    public static void Subject_5()
    {
        try { if (((typeof(byte).GetGenericTypeDefinition()).GetHashCode()) != ((typeof(byte).GetGenericTypeDefinition()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Type::GetEnumUnderlyingType:System.Type()
    public static void Subject_6()
    {
        try { if (((typeof(byte).GetEnumUnderlyingType()).GetHashCode()) != ((typeof(byte).GetEnumUnderlyingType()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String,System.Reflection.BindingFlags)
    public static void Subject_7()
    {
        try { if (((typeof(byte).GetNestedType("hello", System.Reflection.BindingFlags.Default)).GetHashCode()) != ((typeof(byte).GetNestedType("hello", System.Reflection.BindingFlags.Default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)
    public static void Subject_8()
    {
        try { if (((typeof(byte).IsAssignableFrom(typeof(byte))) ? 1 : 0) != ((typeof(byte).IsAssignableFrom(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)
    public static void Subject_9()
    {
        try { if (((typeof(byte).IsSubclassOf(typeof(byte))) ? 1 : 0) != ((typeof(byte).IsSubclassOf(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()
    public static void Subject_10()
    {
        try { if (((typeof(byte).GetInterfaces()).Length) != ((typeof(byte).GetInterfaces()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}