// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/array/indexing/copy
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections;

public static partial class ArrayIndexingCopySubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)
    public static void Subject_0()
    {
        Array.Copy(new byte[4], new byte[4], 4);
    }

    // [1] System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)
    public static void Subject_1()
    {
        Array.Copy(new byte[4], 0, new byte[4], 0, 4);
    }

    // [2] System.Private.CoreLib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)
    public static void Subject_2()
    {
        Array.Clear(new byte[4], 0, 4);
    }

    // [3] System.Private.CoreLib/System.Array::Resize:System.Void(T[]&,System.Int32)
    public static void Subject_3()
    {
        // TODO: System.Private.CoreLib/System.Array::Resize:System.Void(T[]&,System.Int32) could not be auto-generated
    }

    // [4] System.Private.CoreLib/System.Array::Sort:System.Void(System.Array)
    public static void Subject_4()
    {
        Array.Sort(Array.Empty<byte>());
    }

    // [5] System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Collections.IComparer)
    public static void Subject_5()
    {
        Array.Sort(new byte[1], System.Collections.Generic.Comparer<byte>.Default);
    }

    // [6] System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)
    public static void Subject_6()
    {
        if (Array.BinarySearch(new byte[4], (byte)42) != Array.BinarySearch(new byte[4], (byte)42)) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Int32,System.Int32,System.Object)
    public static void Subject_7()
    {
        if (Array.BinarySearch(new byte[4], 0, 4, (byte)42) != Array.BinarySearch(new byte[4], 0, 4, (byte)42)) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)
    public static void Subject_8()
    {
        if (Array.IndexOf(new byte[4], (byte)42) != Array.IndexOf(new byte[4], (byte)42)) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Array::LastIndexOf:System.Int32(System.Array,System.Object)
    public static void Subject_9()
    {
        if (Array.LastIndexOf(new byte[4], (byte)42) != Array.LastIndexOf(new byte[4], (byte)42)) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Array::Reverse:System.Void(System.Array)
    public static void Subject_10()
    {
        Array.Reverse(Array.Empty<byte>());
    }

    // [11] System.Private.CoreLib/System.Array::GetLength:System.Int32(System.Int32)
    public static void Subject_11()
    {
        if (new int[4].GetLength(0) != new int[4].GetLength(0)) _exitCode = 1;
    }

    // [12] System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)
    public static void Subject_12()
    {
        if (((new int[1].GetValue(0)).GetHashCode()) != ((new int[1].GetValue(0)).GetHashCode())) _exitCode = 1;
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(); break;
            case 4: Subject_4(); break;
            case 5: Subject_5(); break;
            case 6: Subject_6(); break;
            case 7: Subject_7(); break;
            case 8: Subject_8(); break;
            case 9: Subject_9(); break;
            case 10: Subject_10(); break;
            case 11: Subject_11(); break;
            case 12: Subject_12(); break;
        }
    }

}