using Chaos.TestFramework;
// Handwritten custom entry subjects
// Family: family/System.Linq/partitioning
// Assembly: System.Linq
// Variant: custom

public static partial class PartitioningSubjects
{
    // [0] Enumerable.Chunk(IEnumerable<int>, int)
    [Fact]
    public static void CustomEntrySubject_0()
    {
        try
        {
        System.Linq.Enumerable.Chunk(new int[] { 1, 2, 3, 4, 5, 6, 7 }, 3).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [1] Enumerable.Skip(IEnumerable<int>, int)
    [Fact]
    public static void CustomEntrySubject_1()
    {
        try
        {
        System.Linq.Enumerable.Skip(new int[] { 10, 20, 30, 40, 50 }, 2).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [2] Enumerable.SkipLast(IEnumerable<int>, int)
    [Fact]
    public static void CustomEntrySubject_2()
    {
        try
        {
        System.Linq.Enumerable.SkipLast(new int[] { 1, 2, 3, 4, 5 }, 2).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [3] Enumerable.SkipWhile(IEnumerable<int>, Func<int,bool>)
    [Fact]
    public static void CustomEntrySubject_3()
    {
        try
        {
        System.Linq.Enumerable.SkipWhile(new int[] { 1, 2, 3, 4, 5 }, (int x) => x < 3).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [4] Enumerable.SkipWhile(IEnumerable<int>, Func<int,int,bool>)
    [Fact]
    public static void CustomEntrySubject_4()
    {
        try
        {
        System.Linq.Enumerable.SkipWhile(new int[] { 1, 2, 3, 4, 5 }, (int x, int i) => i < 2).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [5] Enumerable.Take(IEnumerable<int>, int)
    [Fact]
    public static void CustomEntrySubject_5()
    {
        try
        {
        System.Linq.Enumerable.Take(new int[] { 1, 2, 3, 4, 5 }, 3).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [6] Enumerable.Take(IEnumerable<int>, Range)
    [Fact]
    public static void CustomEntrySubject_6()
    {
        try
        {
        System.Linq.Enumerable.Take(new int[] { 10, 20, 30, 40, 50 }, 1..4).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [7] Enumerable.TakeLast(IEnumerable<int>, int)
    [Fact]
    public static void CustomEntrySubject_7()
    {
        try
        {
        System.Linq.Enumerable.TakeLast(new int[] { 1, 2, 3, 4, 5 }, 2).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [8] Enumerable.TakeWhile(IEnumerable<int>, Func<int,bool>)
    [Fact]
    public static void CustomEntrySubject_8()
    {
        try
        {
        System.Linq.Enumerable.TakeWhile(new int[] { 1, 2, 3, 4, 5 }, (int x) => x < 4).GetEnumerator().MoveNext();
        }
        catch { }

    }

    // [9] Enumerable.TakeWhile(IEnumerable<int>, Func<int,int,bool>)
    [Fact]
    public static void CustomEntrySubject_9()
    {
        try
        {
        System.Linq.Enumerable.TakeWhile(new int[] { 1, 2, 3, 4, 5 }, (int x, int i) => i < 3).GetEnumerator().MoveNext();
        }
        catch { }

    }
}
