// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/convert/char
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class ConvertCharSubjects
{
    // [0] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)
    [Fact]
    public static void Subject_0()
    {
        try { var __val = (int)(Convert.ToChar(true)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)
    [Fact]
    public static void Subject_1()
    {
        try { var __val = (int)(Convert.ToChar((byte)42)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)
    [Fact]
    public static void Subject_2()
    {
        try { var __val = (int)(Convert.ToChar('A')); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)
    [Fact]
    public static void Subject_3()
    {
        try { var __val = (int)(Convert.ToChar(DateTime.UtcNow)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)
    [Fact]
    public static void Subject_4()
    {
        try { var __val = (int)(Convert.ToChar(42m)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)
    [Fact]
    public static void Subject_5()
    {
        try { var __val = (int)(Convert.ToChar(42.0)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)
    [Fact]
    public static void Subject_6()
    {
        try { var __val = (int)(Convert.ToChar((short)42)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)
    [Fact]
    public static void Subject_7()
    {
        try { var __val = (int)(Convert.ToChar(42)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)
    [Fact]
    public static void Subject_8()
    {
        try { var __val = (int)(Convert.ToChar(42L)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)
    [Fact]
    public static void Subject_9()
    {
        try { var __val = (int)(Convert.ToChar(null!)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)
    [Fact]
    public static void Subject_10()
    {
        try { var __val = (int)(Convert.ToChar(null!, null)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [11] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)
    [Fact]
    public static void Subject_11()
    {
        try { var __val = (int)(Convert.ToChar((sbyte)42)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [12] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)
    [Fact]
    public static void Subject_12()
    {
        try { var __val = (int)(Convert.ToChar(42.0f)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [13] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)
    [Fact]
    public static void Subject_13()
    {
        try { var __val = (int)(Convert.ToChar("hello")); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [14] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)
    [Fact]
    public static void Subject_14()
    {
        try { var __val = (int)(Convert.ToChar("hello", null)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [15] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)
    [Fact]
    public static void Subject_15()
    {
        try { var __val = (int)(Convert.ToChar((ushort)42)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [16] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)
    [Fact]
    public static void Subject_16()
    {
        try { var __val = (int)(Convert.ToChar(42u)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [17] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)
    [Fact]
    public static void Subject_17()
    {
        try { var __val = (int)(Convert.ToChar(42uL)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}