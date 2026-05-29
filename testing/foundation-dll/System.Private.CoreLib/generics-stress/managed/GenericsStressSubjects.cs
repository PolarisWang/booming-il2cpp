// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/generics/stress
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

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
        try { _ = Array.BinarySearch(new byte[4], (byte)42); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)
    public static void Subject_2()
    {
        try { _ = Array.IndexOf(new byte[4], (byte)42); }
        catch { _exitCode = 1; }
    }

}