// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/generics/stress
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;

public static partial class GenericsStressSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Int32,System.Int32,System.Collections.Generic.IComparer<System.Int32>)
    public static void Subject_0()
    {
        try { Array.Sort(Array.Empty<byte>(), 42, 42, default); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)
    public static void Subject_1()
    {
        try { if (Array.BinarySearch(new byte[4], (byte)42) != Array.BinarySearch(new byte[4], (byte)42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)
    public static void Subject_2()
    {
        try { if (Array.IndexOf(new byte[4], (byte)42) != Array.IndexOf(new byte[4], (byte)42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}