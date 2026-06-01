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
    // [0] System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()
    [Fact]
    public static void Subject_0()
    {
        try { var __val = ((typeof(byte).GetGenericTypeDefinition()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])
    [Fact]
    public static void Subject_1()
    {
        try { var __val = ((typeof(byte).MakeGenericType(Array.Empty<System.Type>())).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()
    [Fact]
    public static void Subject_2()
    {
        try { var __val = ((typeof(byte).GetGenericArguments()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Type::GetGenericParameterConstraints:System.Type[]()
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((typeof(byte).GetGenericParameterConstraints()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Type::get_GenericParameterPosition:System.Int32()
    [Fact]
    public static void Subject_4()
    {
        try { var __val = typeof(byte).GenericParameterPosition; Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean()
    [Fact]
    public static void Subject_5()
    {
        try { var __val = ((typeof(byte).ContainsGenericParameters) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()
    [Fact]
    public static void Subject_6()
    {
        try { var __val = ((typeof(byte).IsGenericType) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition:System.Boolean()
    [Fact]
    public static void Subject_7()
    {
        try { var __val = ((typeof(byte).IsGenericTypeDefinition) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()
    [Fact]
    public static void Subject_8()
    {
        try { var __val = ((typeof(byte).IsConstructedGenericType) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.Type::MakeArrayType:System.Type()
    [Fact]
    public static void Subject_9()
    {
        try { var __val = ((typeof(byte).MakeArrayType()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.Type::MakeByRefType:System.Type()
    [Fact]
    public static void Subject_10()
    {
        try { var __val = ((typeof(byte).MakeByRefType()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [11] System.Private.CoreLib/System.Type::MakePointerType:System.Type()
    [Fact]
    public static void Subject_11()
    {
        try { var __val = ((typeof(byte).MakePointerType()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}