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
        try { var _ = ((typeof(byte).GetElementType()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Type::MakeArrayType:System.Type()
    public static void Subject_1()
    {
        try { var _ = ((typeof(byte).MakeArrayType()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Type::MakeArrayType:System.Type(System.Int32)
    public static void Subject_2()
    {
        try { var _ = ((typeof(byte).MakeArrayType(42)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Type::MakeByRefType:System.Type()
    public static void Subject_3()
    {
        try { var _ = ((typeof(byte).MakeByRefType()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Type::MakePointerType:System.Type()
    public static void Subject_4()
    {
        try { var _ = ((typeof(byte).MakePointerType()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()
    public static void Subject_5()
    {
        try { var _ = ((typeof(byte).GetGenericTypeDefinition()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Type::GetEnumUnderlyingType:System.Type()
    public static void Subject_6()
    {
        try { var _ = ((typeof(byte).GetEnumUnderlyingType()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String,System.Reflection.BindingFlags)
    public static void Subject_7()
    {
        try { var _ = ((typeof(byte).GetNestedType("hello", System.Reflection.BindingFlags.Default)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)
    public static void Subject_8()
    {
        try { var _ = ((typeof(byte).IsAssignableFrom(typeof(byte))) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)
    public static void Subject_9()
    {
        try { var _ = ((typeof(byte).IsSubclassOf(typeof(byte))) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()
    public static void Subject_10()
    {
        try { var _ = ((typeof(byte).GetInterfaces()).Length); }
        catch { _exitCode = 1; }
    }

}