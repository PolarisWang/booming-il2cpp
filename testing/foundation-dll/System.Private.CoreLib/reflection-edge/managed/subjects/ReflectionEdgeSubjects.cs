// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/edge
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionEdgeSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetElementType:System.Type()
    public static void Subject_0()
    {
        // non-callable: System.Private.CoreLib/System.Type::GetElementType:System.Type()
    }

    // [1] System.Private.CoreLib/System.Type::MakeArrayType:System.Type()
    public static void Subject_1()
    {
        // non-callable: System.Private.CoreLib/System.Type::MakeArrayType:System.Type()
    }

    // [2] System.Private.CoreLib/System.Type::MakeArrayType:System.Type(System.Int32)
    public static void Subject_2()
    {
        // non-callable: System.Private.CoreLib/System.Type::MakeArrayType:System.Type(System.Int32)
    }

    // [3] System.Private.CoreLib/System.Type::MakeByRefType:System.Type()
    public static void Subject_3()
    {
        // non-callable: System.Private.CoreLib/System.Type::MakeByRefType:System.Type()
    }

    // [4] System.Private.CoreLib/System.Type::MakePointerType:System.Type()
    public static void Subject_4()
    {
        // non-callable: System.Private.CoreLib/System.Type::MakePointerType:System.Type()
    }

    // [5] System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()
    public static void Subject_5()
    {
        // non-callable: System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()
    }

    // [6] System.Private.CoreLib/System.Type::GetEnumUnderlyingType:System.Type()
    public static void Subject_6()
    {
        // non-callable: System.Private.CoreLib/System.Type::GetEnumUnderlyingType:System.Type()
    }

    // [7] System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String,System.Reflection.BindingFlags)
    public static void Subject_7()
    {
        // non-callable: System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String,System.Reflection.BindingFlags)
    }

    // [8] System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)
    public static void Subject_8()
    {
        try { _ = ((typeof(System.IConvertible).IsAssignableFrom(typeof(int))) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)
    public static void Subject_9()
    {
        try { _ = ((typeof(System.MemberAccessException).IsSubclassOf(typeof(System.Exception))) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()
    public static void Subject_10()
    {
        // non-callable: System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()
    }

}