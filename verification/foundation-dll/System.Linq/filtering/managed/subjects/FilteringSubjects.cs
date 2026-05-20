// Auto-generated native-AOT entry point
// Family: family/System.Linq/filtering
// Assembly: System.Linq
// Variant: subjects

using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

public static partial class FilteringSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Linq/System.Linq.Enumerable::All{TSource}:System.Boolean(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})
    public static void Subject_0()
    {
        try { if (((Enumerable.All{TSource}(null!, null!, null!)) ? 1 : 0) != ((Enumerable.All{TSource}(null!, null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Linq/System.Linq.Enumerable::Any{TSource}:System.Boolean(System.Collections.Generic.IEnumerable{TSource})
    public static void Subject_1()
    {
        try { if (((Enumerable.Any{TSource}(null!)) ? 1 : 0) != ((Enumerable.Any{TSource}(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Linq/System.Linq.Enumerable::Any{TSource}:System.Boolean(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})
    public static void Subject_2()
    {
        try { if (((Enumerable.Any{TSource}(null!, null!, null!)) ? 1 : 0) != ((Enumerable.Any{TSource}(null!, null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Linq/System.Linq.Enumerable::Cast{TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.IEnumerable)
    public static void Subject_3()
    {
        try { if ((int)(Enumerable.Cast{TResult}(null!)) != (int)(Enumerable.Cast{TResult}(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Linq/System.Linq.Enumerable::DistinctBy{TSource,TKey}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey})
    public static void Subject_4()
    {
        // TODO: System.Linq/System.Linq.Enumerable::DistinctBy{TSource,TKey}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey}) could not be auto-generated
    }

    // [5] System.Linq/System.Linq.Enumerable::DistinctBy{TSource,TKey}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IEqualityComparer{TKey})
    public static void Subject_5()
    {
        // TODO: System.Linq/System.Linq.Enumerable::DistinctBy{TSource,TKey}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IEqualityComparer{TKey}) could not be auto-generated
    }

    // [6] System.Linq/System.Linq.Enumerable::OfType{TResult}:System.Collections.Generic.IEnumerable{TResult}(System.Collections.IEnumerable)
    public static void Subject_6()
    {
        try { if ((int)(Enumerable.OfType{TResult}(null!)) != (int)(Enumerable.OfType{TResult}(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Linq/System.Linq.Enumerable::Where{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})
    public static void Subject_7()
    {
        try { if ((int)(Enumerable.Where{TSource}(null!, null!, null!)) != (int)(Enumerable.Where{TSource}(null!, null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Linq/System.Linq.Enumerable::Where{TSource}:System.Collections.Generic.IEnumerable{TSource}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Int32,System.Boolean})
    public static void Subject_8()
    {
        try { if ((int)(Enumerable.Where{TSource}(null!, null!, 42, null!)) != (int)(Enumerable.Where{TSource}(null!, null!, 42, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}