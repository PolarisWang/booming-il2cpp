// Handwritten custom entry subjects
// Family: family/System.Linq/partitioning
// Assembly: System.Linq
// Variant: custom

public static partial class PartitioningSubjects
{
    // [0] Enumerable.Chunk(IEnumerable<int>, int)
    public static void CustomEntrySubject_0()
    {
        System.Linq.Enumerable.Chunk(new int[] { 1, 2, 3, 4, 5, 6, 7 }, 3).GetEnumerator().MoveNext();
    }

    // [1] Enumerable.Skip(IEnumerable<int>, int)
    public static void CustomEntrySubject_1()
    {
        System.Linq.Enumerable.Skip(new int[] { 10, 20, 30, 40, 50 }, 2).GetEnumerator().MoveNext();
    }

    // [2] Enumerable.SkipLast(IEnumerable<int>, int)
    public static void CustomEntrySubject_2()
    {
        System.Linq.Enumerable.SkipLast(new int[] { 1, 2, 3, 4, 5 }, 2).GetEnumerator().MoveNext();
    }

    // [3] Enumerable.SkipWhile(IEnumerable<int>, Func<int,bool>)
    public static void CustomEntrySubject_3()
    {
        System.Linq.Enumerable.SkipWhile(new int[] { 1, 2, 3, 4, 5 }, (int x) => x < 3).GetEnumerator().MoveNext();
    }

    // [4] Enumerable.SkipWhile(IEnumerable<int>, Func<int,int,bool>)
    public static void CustomEntrySubject_4()
    {
        System.Linq.Enumerable.SkipWhile(new int[] { 1, 2, 3, 4, 5 }, (int x, int i) => i < 2).GetEnumerator().MoveNext();
    }

    // [5] Enumerable.Take(IEnumerable<int>, int)
    public static void CustomEntrySubject_5()
    {
        System.Linq.Enumerable.Take(new int[] { 1, 2, 3, 4, 5 }, 3).GetEnumerator().MoveNext();
    }

    // [6] Enumerable.Take(IEnumerable<int>, Range)
    public static void CustomEntrySubject_6()
    {
        System.Linq.Enumerable.Take(new int[] { 10, 20, 30, 40, 50 }, 1..4).GetEnumerator().MoveNext();
    }

    // [7] Enumerable.TakeLast(IEnumerable<int>, int)
    public static void CustomEntrySubject_7()
    {
        System.Linq.Enumerable.TakeLast(new int[] { 1, 2, 3, 4, 5 }, 2).GetEnumerator().MoveNext();
    }

    // [8] Enumerable.TakeWhile(IEnumerable<int>, Func<int,bool>)
    public static void CustomEntrySubject_8()
    {
        System.Linq.Enumerable.TakeWhile(new int[] { 1, 2, 3, 4, 5 }, (int x) => x < 4).GetEnumerator().MoveNext();
    }

    // [9] Enumerable.TakeWhile(IEnumerable<int>, Func<int,int,bool>)
    public static void CustomEntrySubject_9()
    {
        System.Linq.Enumerable.TakeWhile(new int[] { 1, 2, 3, 4, 5 }, (int x, int i) => i < 3).GetEnumerator().MoveNext();
    }
}
