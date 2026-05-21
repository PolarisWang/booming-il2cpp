// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/array/indexing-copy
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

public static partial class ArrayIndexingCopySubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)
    public static void Subject_0()
    {
        try { Array.Copy(new byte[4], new byte[4], 4); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)
    public static void Subject_1()
    {
        try { Array.Copy(new byte[4], 0, new byte[4], 0, 4); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)
    public static void Subject_2()
    {
        try { Array.Clear(new byte[4], 0, 4); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Array::Resize:System.Void(T[]&,System.Int32)
    public static void Subject_3()
    {
        try { ((System.Func<int>)(() => { byte[] __arr = new byte[4]; Array.Resize(ref __arr, 8); return __arr.Length; }))(); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Array::Sort:System.Void(System.Array)
    public static void Subject_4()
    {
        try { Array.Sort(Array.Empty<byte>()); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Collections.IComparer)
    public static void Subject_5()
    {
        try { Array.Sort(new byte[1], System.Collections.Generic.Comparer<byte>.Default); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)
    public static void Subject_6()
    {
        try { if (Array.BinarySearch(new byte[4], (byte)42) != Array.BinarySearch(new byte[4], (byte)42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Int32,System.Int32,System.Object)
    public static void Subject_7()
    {
        try { if (Array.BinarySearch(new byte[4], 0, 4, (byte)42) != Array.BinarySearch(new byte[4], 0, 4, (byte)42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)
    public static void Subject_8()
    {
        try { if (Array.IndexOf(new byte[4], (byte)42) != Array.IndexOf(new byte[4], (byte)42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Array::LastIndexOf:System.Int32(System.Array,System.Object)
    public static void Subject_9()
    {
        try { if (Array.LastIndexOf(new byte[4], (byte)42) != Array.LastIndexOf(new byte[4], (byte)42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Array::Reverse:System.Void(System.Array)
    public static void Subject_10()
    {
        try { Array.Reverse(Array.Empty<byte>()); }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Array::GetLength:System.Int32(System.Int32)
    public static void Subject_11()
    {
        try { if (new int[4].GetLength(0) != new int[4].GetLength(0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)
    public static void Subject_12()
    {
        try { if (((new int[1].GetValue(0)).GetHashCode()) != ((new int[1].GetValue(0)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}