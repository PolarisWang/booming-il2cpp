// Auto-generated native-AOT entry point
// Family: family/System.Linq/aggregation
// Assembly: System.Linq
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;

public static partial class AggregationSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Linq/System.Linq.Enumerable::Aggregate{TSource}:TSource(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TSource,TSource})
    // [1] System.Linq/System.Linq.Enumerable::Aggregate{TSource,TAccumulate}:TAccumulate(System.Collections.Generic.IEnumerable{TSource},TAccumulate,System.Func{TAccumulate,TSource,TAccumulate})
    // [2] System.Linq/System.Linq.Enumerable::Aggregate{TSource,TAccumulate,TResult}:TResult(System.Collections.Generic.IEnumerable{TSource},TAccumulate,System.Func{TAccumulate,TSource,TAccumulate},System.Func{TAccumulate,TResult})
    // [3] System.Linq/System.Linq.Enumerable::AggregateBy{TSource,TKey,TAccumulate}:System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{TKey,TAccumulate}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},TAccumulate,System.Func{TAccumulate,TSource,TAccumulate},System.Collections.Generic.IEqualityComparer{TKey})
    // [4] System.Linq/System.Linq.Enumerable::AggregateBy{TSource,TKey,TAccumulate}:System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{TKey,TAccumulate}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Func{TKey,TAccumulate},System.Func{TAccumulate,TSource,TAccumulate},System.Collections.Generic.IEqualityComparer{TKey})
    // [5] System.Linq/System.Linq.Enumerable::Count{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource})
    // [6] System.Linq/System.Linq.Enumerable::Count{TSource}:System.Int32(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})
    // [7] System.Linq/System.Linq.Enumerable::CountBy{TSource,TKey}:System.Collections.Generic.IEnumerable{System.Collections.Generic.KeyValuePair{TKey,System.Int32}}(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,TKey},System.Collections.Generic.IEqualityComparer{TKey})
    // [8] System.Linq/System.Linq.Enumerable::LongCount{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource})
    // [9] System.Linq/System.Linq.Enumerable::LongCount{TSource}:System.Int64(System.Collections.Generic.IEnumerable{TSource},System.Func{TSource,System.Boolean})
    // [10] System.Linq/System.Linq.Enumerable::TryGetNonEnumeratedCount{TSource}:System.Boolean(System.Collections.Generic.IEnumerable{TSource},System.Int32&)
}