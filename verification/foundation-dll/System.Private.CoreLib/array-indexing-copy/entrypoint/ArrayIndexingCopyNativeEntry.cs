// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/array/indexing/copy
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Collections;

public static class ArrayIndexingCopyNativeEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
            case 4: return Method4();
            case 5: return Method5();
            case 6: return Method6();
            case 7: return Method7();
            case 8: return Method8();
            case 9: return Method9();
            case 10: return Method10();
            case 11: return Method11();
            case 12: return Method12();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)
    static int Method0()
    {
        Array.Copy(new byte[4], new byte[4], 4);
        return (int)0;
    }

    // [1] System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)
    static int Method1()
    {
        Array.Copy(new byte[4], 0, new byte[4], 0, 4);
        return (int)0;
    }

    // [2] System.Private.CoreLib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)
    static int Method2()
    {
        Array.Clear(new byte[4], 0, 4);
        return (int)0;
    }

    // [3] System.Private.CoreLib/System.Array::Resize:System.Void(T[]&,System.Int32)
    static int Method3()
    {
    var refLocal_0 = Array.Empty<T>();
        Array.Resize(ref refLocal_0, 42);
        return (int)refLocal_0;
    }

    // [4] System.Private.CoreLib/System.Array::Sort:System.Void(System.Array)
    static int Method4()
    {
        Array.Sort(Array.Empty<byte>());
        return (int)0;
    }

    // [5] System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Collections.IComparer)
    static int Method5()
    {
        Array.Sort<byte>(new byte[1], (IComparer<byte>)null!);
        return (int)0;
    }

    // [6] System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)
    static int Method6()
    {
        return Array.BinarySearch(new byte[4], (byte)42);
    }

    // [7] System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Int32,System.Int32,System.Object)
    static int Method7()
    {
        return Array.BinarySearch(new byte[4], 0, 4, (byte)42);
    }

    // [8] System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)
    static int Method8()
    {
        return Array.IndexOf(new byte[4], (byte)42);
    }

    // [9] System.Private.CoreLib/System.Array::LastIndexOf:System.Int32(System.Array,System.Object)
    static int Method9()
    {
        return Array.LastIndexOf(new byte[4], (byte)42);
    }

    // [10] System.Private.CoreLib/System.Array::Reverse:System.Void(System.Array)
    static int Method10()
    {
        Array.Reverse(Array.Empty<byte>());
        return (int)0;
    }

    // [11] System.Private.CoreLib/System.Array::GetLength:System.Int32(System.Int32)
    static int Method11()
    {
        return new int[4, 4].GetLength(0);
    }

    // [12] System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)
    static int Method12()
    {
        return (int)(new int[1].GetValue(0));
    }

}