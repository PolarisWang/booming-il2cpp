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
    // [0] System.Private.CoreLib/System.Type::GetElementType:System.Type()
    [Fact]
    public static void Subject_0()
    {
        try { var __val = ((typeof(byte).GetElementType()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Type::MakeArrayType:System.Type()
    [Fact]
    public static void Subject_1()
    {
        try { var __val = ((typeof(byte).MakeArrayType()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Type::MakeArrayType:System.Type(System.Int32)
    [Fact]
    public static void Subject_2()
    {
        try { var __val = ((typeof(byte).MakeArrayType(42)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Type::MakeByRefType:System.Type()
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((typeof(byte).MakeByRefType()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Type::MakePointerType:System.Type()
    [Fact]
    public static void Subject_4()
    {
        try { var __val = ((typeof(byte).MakePointerType()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()
    [Fact]
    public static void Subject_5()
    {
        try { var __val = ((typeof(byte).GetGenericTypeDefinition()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.Type::GetEnumUnderlyingType:System.Type()
    [Fact]
    public static void Subject_6()
    {
        try { var __val = ((typeof(byte).GetEnumUnderlyingType()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String,System.Reflection.BindingFlags)
    [Fact]
    public static void Subject_7()
    {
        try { var __val = ((typeof(byte).GetNestedType("hello", System.Reflection.BindingFlags.Default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)
    [Fact]
    public static void Subject_8()
    {
        try { var __val = ((typeof(System.IConvertible).IsAssignableFrom(typeof(int))) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)
    [Fact]
    public static void Subject_9()
    {
        try { var __val = ((typeof(System.MemberAccessException).IsSubclassOf(typeof(System.Exception))) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()
    [Fact]
    public static void Subject_10()
    {
        try { var __val = ((typeof(byte).GetInterfaces()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}