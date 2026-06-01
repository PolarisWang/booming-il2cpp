// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/enum/parsing
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class EnumParsingSubjects
{
    // [0] System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)
    [Fact]
    public static void Subject_0()
    {
        try { var __val = ((Enum.Format(typeof(byte), null!, "hello")).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)
    [Fact]
    public static void Subject_1()
    {
        try { var __val = ((Enum.GetName(typeof(DayOfWeek), (object)1)).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)
    [Fact]
    public static void Subject_2()
    {
        try { var __val = ((Enum.GetName(typeof(DayOfWeek), (object)1)).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((Enum.GetNames(typeof(byte))).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)
    [Fact]
    public static void Subject_4()
    {
        try { var __val = (int)(((Array)(Enum.GetValues(typeof(byte)))).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)
    [Fact]
    public static void Subject_5()
    {
        try { var __val = ((Enum.IsDefined(typeof(byte), null!)) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)
    [Fact]
    public static void Subject_6()
    {
        try { var __val = ((Enum.Parse(typeof(DayOfWeek), "Monday")).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)
    [Fact]
    public static void Subject_7()
    {
        try { var __val = ((Enum.Parse(typeof(DayOfWeek), "Monday", true)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.Enum::ToString:System.String()
    [Fact]
    public static void Subject_8()
    {
        try { var __val = ((Enum.Format(typeof(byte), (byte)42, "G")).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.Enum::ToString:System.String(System.String)
    [Fact]
    public static void Subject_9()
    {
        try { var __val = ((Enum.Format(typeof(byte), (byte)42, "X")).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)
    [Fact]
    public static void Subject_10()
    {
        try { var __val = ((Enum.TryParse(typeof(DayOfWeek), "Monday", true, out object _)) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [11] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)
    [Fact]
    public static void Subject_11()
    {
        try { var __val = ((Enum.TryParse(typeof(DayOfWeek), "Monday", out object _)) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [12] System.Private.CoreLib/System.Enum::BoxToString:System.String()
}