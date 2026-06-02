// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/boxing/unboxing/casts
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading;

public static partial class BoxingUnboxingCastsSubjects
{
    // [0] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)
    [Fact]
    public static void Subject_0()
    {
        try { var __val = ((Convert.ChangeType(null!, typeof(byte))).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)
    [Fact]
    public static void Subject_1()
    {
        try { var __val = ((Convert.ChangeType(null!, typeof(byte), null)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)
    [Fact]
    public static void Subject_2()
    {
        try { var __val = ((typeof(System.IConvertible).IsAssignableFrom(typeof(int))) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((typeof(string).IsInstanceOfType("hello")) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)
    [Fact]
    public static void Subject_4()
    {
        try { var __val = ((typeof(System.MemberAccessException).IsSubclassOf(typeof(System.Exception))) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)
    [Fact]
    public static void Subject_5()
    {
        try { var __val = ((typeof(System.IConvertible).IsAssignableTo(typeof(int))) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    // [7] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
    [Fact]
    public static void Subject_7()
    {
        try { var __val = ((RuntimeHelpers.GetObjectValue(null!)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    [Fact]
    public static void Subject_8()
    {
        try { var __val = (int)(((Array)(Array.CreateInstance(typeof(byte), 42))).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    [Fact]
    public static void Subject_9()
    {
        try { var __val = (int)(((Array)(Array.CreateInstance(typeof(byte), 42, 42))).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    [Fact]
    public static void Subject_10()
    {
        try { var __val = ((((ValueType)42).Equals(null!)) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}