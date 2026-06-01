// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/span/memory-buffers
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;

public static partial class SpanMemoryBuffersSubjects
{
    // [0] System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32)
    [Fact]
    public static void Subject_0()
    {
        try { var __val = ((new Span<byte>(new byte[4])[0]).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32)
    [Fact]
    public static void Subject_1()
    {
        try { var __val = ((new Span<byte>(new byte[4]).Slice(1).Length).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32)
    [Fact]
    public static void Subject_2()
    {
        try { var __val = ((new Span<byte>(new byte[4]).Slice(1, 2).Length).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32)
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((new byte[1].AsSpan().ToArray()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span`1)
    [Fact]
    public static void Subject_4()
    {
        try { default(Span<byte>).CopyTo(default(Span<byte>)); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.Span`1::get_Empty:System.Span`1()
    [Fact]
    public static void Subject_5()
    {
        try { var __val = ((Span<byte>.Empty).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32)
    [Fact]
    public static void Subject_6()
    {
        try { var __val = ((new ReadOnlySpan<byte>(new byte[4])[0]).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32)
    [Fact]
    public static void Subject_7()
    {
        try { var __val = ((new ReadOnlySpan<byte>(new byte[4]).Slice(1).Length).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:T[]()
    [Fact]
    public static void Subject_8()
    {
        try { var __val = ((default(ReadOnlySpan<byte>).ToArray()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)
    [Fact]
    public static void Subject_9()
    {
        try { var __val = ((new Memory<byte>(new byte[4]).Slice(1).Length).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()
    [Fact]
    public static void Subject_10()
    {
        try { var __val = ((default(Memory<byte>).Span).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [11] System.Private.CoreLib/System.Memory`1::ToArray:T[]()
    [Fact]
    public static void Subject_11()
    {
        try { var __val = ((default(Memory<byte>).ToArray()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [12] System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)
    [Fact]
    public static void Subject_12()
    {
        try { var __val = ((MemoryMarshal.GetReference(default(ReadOnlySpan<byte>))).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [13] System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)
    [Fact]
    public static void Subject_13()
    {
        try { var __val = ((MemoryMarshal.GetReference(default(Span<byte>))).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}