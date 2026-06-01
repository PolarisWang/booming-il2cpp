// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/array/indexing/copy
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class ArrayIndexingCopySubjects
{
    // [0] System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)
    [Fact]
    public static void Subject_0()
    {
        try { Array.Copy(new byte[4], new byte[4], 4); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)
    [Fact]
    public static void Subject_1()
    {
        try { Array.Copy(new byte[4], 0, new byte[4], 0, 4); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)
    [Fact]
    public static void Subject_2()
    {
        try { Array.Clear(new byte[4], 0, 4); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Array::Resize:System.Void(T[]&,System.Int32)
    [Fact]
    public static void Subject_3()
    {
        try { ((System.Func<int>)(() => { byte[] __arr = new byte[4]; Array.Resize(ref __arr, 8); return __arr.Length; }))(); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Array::Sort:System.Void(System.Array)
    [Fact]
    public static void Subject_4()
    {
        try { Array.Sort(Array.Empty<byte>()); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Collections.IComparer)
    [Fact]
    public static void Subject_5()
    {
        try { Array.Sort(new byte[1], System.Collections.Generic.Comparer<byte>.Default); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)
    [Fact]
    public static void Subject_6()
    {
        try { var __val = Array.BinarySearch(new byte[4], (byte)42); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Int32,System.Int32,System.Object)
    [Fact]
    public static void Subject_7()
    {
        try { var __val = Array.BinarySearch(new byte[4], 0, 4, (byte)42); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)
    [Fact]
    public static void Subject_8()
    {
        try { var __val = Array.IndexOf(new byte[4], (byte)42); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.Array::LastIndexOf:System.Int32(System.Array,System.Object)
    [Fact]
    public static void Subject_9()
    {
        try { var __val = Array.LastIndexOf(new byte[4], (byte)42); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.Array::Reverse:System.Void(System.Array)
    [Fact]
    public static void Subject_10()
    {
        try { Array.Reverse(Array.Empty<byte>()); }
        catch { }
    }

    // [11] System.Private.CoreLib/System.Array::GetLength:System.Int32(System.Int32)
    [Fact]
    public static void Subject_11()
    {
        try { var __val = new int[4].GetLength(0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [12] System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)
    [Fact]
    public static void Subject_12()
    {
        try { var __val = ((new int[1].GetValue(0)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}