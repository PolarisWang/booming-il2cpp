// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/partitioning
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class PartitioningPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Linq/System.Linq.Enumerable::Chunk{TSource}:System.Collections.Generic.IEnumerable{TSource{}}(System.Collections.Generic.IEnumerable{TSource},System.Int32)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Linq/System.Linq.Enumerable::Skip{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Int32)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Linq/System.Linq.Enumerable::SkipLast{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Int32)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Linq/System.Linq.Enumerable::SkipWhile{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Linq/System.Linq.Enumerable::SkipWhile{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32,System.Boolean})
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Linq/System.Linq.Enumerable::Take{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Int32)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Linq/System.Linq.Enumerable::Take{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Range)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Linq/System.Linq.Enumerable::TakeLast{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Int32)
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Linq/System.Linq.Enumerable::TakeWhile{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.Linq/System.Linq.Enumerable::TakeWhile{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32,System.Boolean})
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    public static void Run(int entryIndex)
    {
        try
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}