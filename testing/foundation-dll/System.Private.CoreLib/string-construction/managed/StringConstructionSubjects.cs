// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/string/construction
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class StringConstructionSubjects
{
    // [0] System.Private.CoreLib/System.String::.ctor:System.Void(System.Char[])
    [Fact]
    public static void Subject_0()
    {
        try { new String(Array.Empty<System.Char>()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.String::.ctor:System.Void(System.Char,System.Int32)
    [Fact]
    public static void Subject_1()
    {
        try { new String('A', 42); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.String::.ctor:System.Void(System.Char[],System.Int32,System.Int32)
    [Fact]
    public static void Subject_2()
    {
        try { new String(Array.Empty<System.Char>(), 42, 42); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.String::IsNullOrEmpty:System.Boolean(System.String)
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((String.IsNullOrEmpty("hello")) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.String::IsNullOrWhiteSpace:System.Boolean(System.String)
    [Fact]
    public static void Subject_4()
    {
        try { var __val = ((String.IsNullOrWhiteSpace("hello")) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.String::Copy:System.String(System.String)
    [Fact]
    public static void Subject_5()
    {
        try { var __val = ((String.Copy("hello")).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.String::Intern:System.String(System.String)
    [Fact]
    public static void Subject_6()
    {
        try { var __val = ((String.Intern("hello")).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.String::IsInterned:System.String(System.String)
    [Fact]
    public static void Subject_7()
    {
        try { var __val = ((String.IsInterned("hello")).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}