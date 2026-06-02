using System.Collections.Concurrent;
using System.Reflection;

namespace Chaos.IL2CPP.Tools.AutoTestGenerator;

public sealed class ValueGenerator
{
    private readonly CSharpSerializer _serializer;
    private readonly AutoFixtureAllower? _autoFixture;

    // Cache for enum type detection (Type.GetType is slow)
    private static readonly ConcurrentDictionary<string, bool> EnumTypeCache = new(StringComparer.Ordinal);

    private static readonly Dictionary<string, string[]> BoundaryValues = new(StringComparer.Ordinal)
    {
        ["System.Boolean"] = new[] { "false", "true" },
        ["System.Byte"] = new[] { "0", "1", "255" },
        ["System.SByte"] = new[] { "0", "1", "-1", "-128", "127" },
        ["System.Int16"] = new[] { "0", "1", "-1", "-32768", "32767" },
        ["System.UInt16"] = new[] { "0", "1", "65535" },
        ["System.Int32"] = new[] { "0", "1", "-1", "42", "2147483647" },
        ["System.UInt32"] = new[] { "0", "1", "4294967295" },
        ["System.Int64"] = new[] { "0", "1", "-1", "9223372036854775807" },
        ["System.UInt64"] = new[] { "0", "1", "18446744073709551615" },
        ["System.Single"] = new[] { "0f", "1f", "-1f", "3.14f" },
        ["System.Double"] = new[] { "0d", "1d", "-1d", "3.14d" },
        ["System.Decimal"] = new[] { "0m", "1m", "-1m", "3.14m" },
        ["System.Char"] = new[] { "'\\0'", "'A'", "'z'" },
        ["System.String"] = new[] { "\"\"", "\"hello\"", "null!", "\"\\0\"", "\"ABC123\"" },
        ["System.IntPtr"] = new[] { "System.IntPtr.Zero", "new System.IntPtr(42)", "new System.IntPtr(-1)" },
        ["System.UIntPtr"] = new[] { "System.UIntPtr.Zero", "new System.UIntPtr(42)" },
        ["System.DateTime"] = new[] { "default(System.DateTime)", "new System.DateTime(2024, 1, 1)" },
        ["System.TimeSpan"] = new[] { "System.TimeSpan.Zero", "System.TimeSpan.FromTicks(42)" },
        ["System.Guid"] = new[] { "default(System.Guid)", "System.Guid.NewGuid()" },
        ["System.Byte[]"] = new[] { "default(System.Byte[])!", "System.Array.Empty<byte>()", "new byte[]{0, 1, 255}" },
        ["System.Int32[]"] = new[] { "default(System.Int32[])!", "System.Array.Empty<int>()", "new int[]{0, -1, 42}" },
        ["System.Int64[]"] = new[] { "default(System.Int64[])!", "System.Array.Empty<long>()", "new long[]{0, -1, 42}" },
        ["System.DateTimeOffset"] = new[] { "default(System.DateTimeOffset)", "new System.DateTimeOffset(2024, 1, 1, 0, 0, 0, System.TimeSpan.Zero)", "System.DateTimeOffset.MaxValue" },
        ["System.Version"] = new[] { "default(System.Version)!", "new System.Version(1, 0)", "new System.Version(1, 2, 3, 4)" },
        ["System.Uri"] = new[] { "default(System.Uri)!", "new System.Uri(\"https://example.com\")", "new System.Uri(\"/relative\", System.UriKind.Relative)" },
        ["System.Half"] = new[] { "default(System.Half)", "(System.Half)0f", "(System.Half)1f" },
        ["System.Numerics.BigInteger"] = new[] { "default(System.Numerics.BigInteger)", "System.Numerics.BigInteger.Zero", "System.Numerics.BigInteger.One" },
        ["System.Numerics.Complex"] = new[] { "default(System.Numerics.Complex)", "new System.Numerics.Complex(0, 0)", "new System.Numerics.Complex(1, 1)" },
        ["System.Int128"] = new[] { "default(System.Int128)", "(System.Int128)0", "(System.Int128)42" },
        ["System.UInt128"] = new[] { "default(System.UInt128)", "(System.UInt128)0", "(System.UInt128)42" },
        ["System.DateOnly"] = new[] { "default(System.DateOnly)", "new System.DateOnly(2024, 1, 1)", "new System.DateOnly(2024, 12, 31)" },
        ["System.TimeOnly"] = new[] { "default(System.TimeOnly)", "new System.TimeOnly(12, 0, 0)", "new System.TimeOnly(23, 59, 59)" },
        ["System.Range"] = new[] { "System.Range.All", "new System.Range(0, 5)", "new System.Range(1, ^1)" },
        ["System.Index"] = new[] { "System.Index.Start", "new System.Index(5)", "System.Index.End" },
    };

    // Known BCL delegate type short names (extracted by DllScanner.GetTypeName)
    private static readonly HashSet<string> DelegateTypeNames = new(StringComparer.Ordinal)
    {
        "Predicate", "Action", "Converter", "Comparison", "Func", "EventHandler",
    };

    // Interface types with known non-null constructible expressions
    private static readonly HashSet<string> SmartConstructibleInterfaces = new(StringComparer.Ordinal)
    {
        "IEnumerable", "IComparer", "IEqualityComparer",
    };

    // Common interface/abstract types that need non-null expressions to avoid
    // ArgumentNullException. Maps base type name → factory(generic type args) → C# expression.
    // These are checked after delegate/array/interface handlers and before default(T).
    private static readonly Dictionary<string, Func<string[], string>> NullGuardSafeDefaults = new(StringComparer.Ordinal)
    {
        ["IList"] = typeArgs => $"System.Array.Empty<{typeArgs[0]}>()",
        ["ICollection"] = typeArgs => $"System.Array.Empty<{typeArgs[0]}>()",
        ["IReadOnlyList"] = typeArgs => $"System.Array.Empty<{typeArgs[0]}>()",
        ["IReadOnlyCollection"] = typeArgs => $"System.Array.Empty<{typeArgs[0]}>()",
        ["IDictionary"] = typeArgs => $"new System.Collections.Generic.Dictionary<{typeArgs[0]}, {typeArgs[1]}>()",
        ["IReadOnlyDictionary"] = typeArgs => $"new System.Collections.Generic.Dictionary<{typeArgs[0]}, {typeArgs[1]}>()",
        ["IEnumerator"] = typeArgs => $"System.Linq.Enumerable.Empty<{typeArgs[0]}>().GetEnumerator()",
        ["IComparer"] = typeArgs => typeArgs.Length > 0
            ? $"System.Collections.Generic.Comparer<{typeArgs[0]}>.Default"
            : "System.Collections.Comparer.Default",
        ["IEqualityComparer"] = typeArgs => typeArgs.Length > 0
            ? $"System.Collections.Generic.EqualityComparer<{typeArgs[0]}>.Default"
            : "System.Collections.EqualityComparer.Default",
        ["IFormatProvider"] = _ => "System.Globalization.CultureInfo.InvariantCulture",
        ["ISet"] = typeArgs => $"new System.Collections.Generic.HashSet<{typeArgs[0]}>()",
        ["IComparable"] = typeArgs => typeArgs.Length > 0
            ? $"default({typeArgs[0]})"
            : "default(System.Int32)",
        ["Comparer"] = typeArgs => typeArgs.Length > 0
            ? $"System.Collections.Generic.Comparer<{typeArgs[0]}>.Default"
            : $"System.Collections.Generic.Comparer<System.Int32>.Default",
        ["EqualityComparer"] = typeArgs => typeArgs.Length > 0
            ? $"System.Collections.Generic.EqualityComparer<{typeArgs[0]}>.Default"
            : $"System.Collections.Generic.EqualityComparer<System.Int32>.Default",
        ["IConvertible"] = _ => "42",
        ["IEnumerable"] = typeArgs => typeArgs.Length > 0
            ? $"System.Linq.Enumerable.Empty<{typeArgs[0]}>()"
            : "System.Linq.Enumerable.Empty<object>()",
        ["IOrderedEnumerable"] = typeArgs => $"System.Linq.Enumerable.Empty<{typeArgs[0]}>().OrderBy(x => x)",
        ["Stream"] = _ => "System.IO.Stream.Null",
        ["TextReader"] = _ => "System.IO.TextReader.Null",
        ["TextWriter"] = _ => "System.IO.TextWriter.Null",
        ["IProgress"] = typeArgs => $"new System.Progress<{typeArgs[0]}>(_ => {{ }})",
        ["IAsyncEnumerable"] = typeArgs => $"System.Linq.AsyncEnumerable.Empty<{typeArgs[0]}>()",
        ["IAsyncEnumerator"] = typeArgs => $"System.Linq.AsyncEnumerable.Empty<{typeArgs[0]}>().GetAsyncEnumerator()",
        ["IProducerConsumerCollection"] = typeArgs => $"new System.Collections.Concurrent.ConcurrentBag<{typeArgs[0]}>()",
        ["IBufferWriter"] = typeArgs => $"new System.Buffers.ArrayBufferWriter<{typeArgs[0]}>()",
        ["IReadOnlySet"] = typeArgs => $"new System.Collections.Generic.HashSet<{typeArgs[0]}>()",
    };

    public ValueGenerator(CSharpSerializer serializer, AutoFixtureAllower? autoFixture = null)
    {
        _serializer = serializer;
        _autoFixture = autoFixture;
    }

    /// <summary>
    /// Generate 3-4 value sets per method using only pre-defined boundary values + defaults.
    /// No runtime AutoFixture calls — all expressions are statically determined C# strings.
    /// Handles enum types, byte[], and Span&lt;T&gt;.
    /// </summary>
    public IReadOnlyList<ValueSet> Generate(MethodSignature method, int methodIndex)
    {
        if (method.Parameters.Count == 0)
            return new[] { new ValueSet(methodIndex, Array.Empty<string>()) };

        var paramTypes = method.Parameters.Select(p => p.TypeName).ToArray();
        var isRefStructParam = method.Parameters.Select(p => p.IsRefStruct).ToArray();
        var sets = new List<ValueSet>();
        var usedSignatures = new HashSet<string>();

        // Set 0: all defaults
        AddUnique(sets, usedSignatures, methodIndex,
            paramTypes.Select((t, i) => DefaultValue(t, isRefStructParam[i])).ToArray());

        // Sets 1-3: one boundary at a time (up to 3 params)
        // Skip for TrimExcess — boundary value 0 on seeded collection throws ArgumentOutOfRangeException
        if (method.Name != "TrimExcess")
        {
            for (int i = 0; i < Math.Min(paramTypes.Length, 3); i++)
            {
                AddUnique(sets, usedSignatures, methodIndex,
                    paramTypes.Select((t, idx) =>
                        idx == i ? BoundaryValue(t, 0, isRefStructParam[idx]) : DefaultValue(t, isRefStructParam[idx])).ToArray());
            }
        }

        // Ensure at least 2 sets
        if (sets.Count < 2 && paramTypes.Length > 0)
        {
            AddUnique(sets, usedSignatures, methodIndex,
                paramTypes.Select((t, i) => DefaultValue(t, isRefStructParam[i])).ToArray());
        }

        // Smart set: non-null values for delegate/interface/array parameters
        var smartArgs = new string[paramTypes.Length];
        for (int i = 0; i < paramTypes.Length; i++)
        {
            var t = paramTypes[i];
            if (isRefStructParam[i])
                smartArgs[i] = DefaultValue(t, true);
            else if (TryGetDelegateExpression(t, out var delegateExpr))
                smartArgs[i] = delegateExpr;
            else if (TryGetArrayExpression(t, out var arrayExpr))
            {
                // CopyTo/TryCopyTo methods need arrays with enough capacity for seed data.
                // Array.Empty<T>() is always too small and causes ArgumentException.
                if (t.EndsWith("[]") && method.Name is "CopyTo" or "TryCopyTo")
                {
                    var elemType = t[..^2];
                    var csElemType = CSharpSerializer.ToCSharpTypeName(elemType);
                    smartArgs[i] = $"new {csElemType}[8]";
                }
                else
                {
                    smartArgs[i] = arrayExpr;
                }
            }
            else if (TryGetInterfaceExpression(t, out var ifaceExpr))
                smartArgs[i] = ifaceExpr;
            else if (TryGetNullGuardSafeExpression(t, out var safeExpr))
                smartArgs[i] = safeExpr;
            else
                smartArgs[i] = DefaultValue(t, false);
        }
        AddUnique(sets, usedSignatures, methodIndex, smartArgs);

        // Collection state variant: populate the first collection-like parameter with
        // non-empty data (e.g. List<T> with 2 elements, Dictionary<K,V> with 1 entry).
        // This exercises methods that behave differently on empty vs. populated collections
        // (e.g. Contains, Count, indexers, CopyTo buffer sizing).
        var collArgs = (string[])smartArgs.Clone();
        bool hasCollection = false;
        for (int i = 0; i < paramTypes.Length && !hasCollection; i++)
        {
            var t = paramTypes[i];
            if (isRefStructParam[i]) continue;

            if (t.EndsWith("[]"))
            {
                var elemType = t[..^2];
                var csElemType = CSharpSerializer.ToCSharpTypeName(elemType);
                collArgs[i] = $"new {csElemType}[2] {{ default({csElemType})!, default({csElemType})! }}";
                hasCollection = true;
            }
            else if (TryGetCollectionInterfaceExpression(t, out var populatedExpr))
            {
                collArgs[i] = populatedExpr;
                hasCollection = true;
            }
        }
        if (hasCollection)
            AddUnique(sets, usedSignatures, methodIndex, collArgs);

        // One AutoFixture-generated random set (only for types that serialize cleanly)
        if (_autoFixture is not null)
        {
            var fixtureArgs = paramTypes.Select((t, i) =>
            {
                // Ref struct params can't be used with AutoFixture (generic arg restriction)
                if (isRefStructParam[i])
                    return DefaultValue(t, true);
                var expr = _autoFixture.TryGenerateExpression(t);
                return expr ?? DefaultValue(t, false);
            }).ToArray();
            AddUnique(sets, usedSignatures, methodIndex, fixtureArgs);
        }

        return sets;
    }

    private string DefaultValue(string typeName, bool isRefStruct = false)
    {
        // Ref structs: use default (can't be boxed or used as generic arg)
        if (isRefStruct)
            return $"default({CSharpSerializer.ToCSharpTypeName(typeName)})";

        if (typeName.EndsWith('&'))
        {
            var baseType = typeName[..^1].Trim();
            return $"out {_serializer.DefaultExpression(baseType)}";
        }
        return _serializer.DefaultExpression(typeName);
    }

    private string BoundaryValue(string typeName, int variantIndex, bool isRefStruct = false)
    {
        // Ref structs: use default
        if (isRefStruct)
            return $"default({CSharpSerializer.ToCSharpTypeName(typeName)})";

        if (BoundaryValues.TryGetValue(typeName, out var values) && variantIndex < values.Length)
            return values[variantIndex];

        if (typeName.EndsWith('&'))
        {
            var baseType = typeName[..^1].Trim();
            return $"out {_serializer.DefaultExpression(baseType)}";
        }

        // Try enum detection for types not in BoundaryValues
        if (IsEnumType(typeName))
        {
            var csType = CSharpSerializer.MapToCSharpType(typeName);
            return $"({csType})0";
        }

        return _serializer.DefaultExpression(typeName);
    }

    /// <summary>
    /// Best-effort check if a type name refers to an enum. Uses runtime Type.GetType()
    /// which works for BCL types but not MLC-only types.
    /// </summary>
    private static bool IsEnumType(string typeName)
    {
        return EnumTypeCache.GetOrAdd(typeName, static name =>
        {
            try
            {
                var type = Type.GetType(name, throwOnError: false);
                return type?.IsEnum == true;
            }
            catch
            {
                return false;
            }
        });
    }

    private static void AddUnique(List<ValueSet> sets, HashSet<string> used, int methodIndex, string[] args)
    {
        var sig = string.Join("|", args);
        if (used.Add(sig))
            sets.Add(new ValueSet(sets.Count, args));
    }

    /// <summary>
    /// Try to generate a non-null C# lambda expression for a known BCL delegate type.
    /// Handles Predicate&lt;T&gt;, Action&lt;T...&gt;, Converter&lt;T,TResult&gt;,
    /// Comparison&lt;T&gt;, Func&lt;T...,TResult&gt;, EventHandler.
    /// </summary>
    /// <summary>
    /// Extract the short (unqualified) base name from a possibly-qualified type name.
    /// "System.Collections.Generic.IList" → "IList", "IList" → "IList"
    /// </summary>
    private static string GetShortBaseName(string typeName)
    {
        var gaStart = typeName.IndexOf('<');
        var baseName = gaStart >= 0 ? typeName[..gaStart] : typeName;
        var lastDot = baseName.LastIndexOf('.');
        return lastDot >= 0 ? baseName[(lastDot + 1)..] : baseName;
    }

    private static bool TryGetDelegateExpression(string typeName, out string expr)
    {
        expr = null!;

        var baseName = GetShortBaseName(typeName);

        if (!DelegateTypeNames.Contains(baseName))
            return false;

        var gaStart = typeName.IndexOf('<');
        string[] csTypeArgs;
        if (gaStart >= 0)
        {
            var argsPart = typeName[(gaStart + 1)..^1];
            csTypeArgs = CSharpSerializer.SplitGenericArgs(argsPart)
                .Select(CSharpSerializer.ToCSharpTypeName)
                .ToArray();
        }
        else
        {
            csTypeArgs = Array.Empty<string>();
        }

        expr = baseName switch
        {
            "Predicate" when csTypeArgs.Length >= 1
                => $"({csTypeArgs[0]} x) => true",

            "Action" when csTypeArgs.Length >= 1
                => $"({string.Join(", ", csTypeArgs.Select((t, i) => $"{t} arg{i}"))}) => {{ }}",

            "Converter" when csTypeArgs.Length >= 2
                => $"({csTypeArgs[0]} x) => default({csTypeArgs[1]})!",

            "Comparison" when csTypeArgs.Length >= 1
                => $"({csTypeArgs[0]} x, {csTypeArgs[0]} y) => 0",

            "Func" when csTypeArgs.Length >= 1
                => $"({string.Join(", ", csTypeArgs[..^1].Select((t, i) => $"{t} arg{i}"))}) => default({csTypeArgs[^1]})!",

            "EventHandler" when csTypeArgs.Length == 0
                => "(object? sender, System.EventArgs e) => { }",

            "EventHandler" when csTypeArgs.Length >= 1
                => $"(object? sender, {csTypeArgs[0]} e) => {{ }}",

            _ => null
        };

        return expr is not null;
    }

    /// <summary>
    /// Try to generate a non-null C# expression for an array type parameter.
    /// Produces System.Array.Empty&lt;T&gt;() for types ending with "[]".
    /// </summary>
    private static bool TryGetArrayExpression(string typeName, out string expr)
    {
        expr = null!;

        if (!typeName.EndsWith("[]"))
            return false;

        var elementType = typeName[..^2];
        var csElementType = CSharpSerializer.ToCSharpTypeName(elementType);
        expr = $"System.Array.Empty<{csElementType}>()";
        return true;
    }

    /// <summary>
    /// Try to generate a non-null C# expression for a constructible interface type.
    /// IEnumerable&lt;T&gt; → Enumerable.Empty&lt;T&gt;(),
    /// IComparer&lt;T&gt; → Comparer&lt;T&gt;.Default,
    /// IEqualityComparer&lt;T&gt; → EqualityComparer&lt;T&gt;.Default.
    /// </summary>
    private static bool TryGetInterfaceExpression(string typeName, out string expr)
    {
        expr = null!;

        var baseName = GetShortBaseName(typeName);

        if (!SmartConstructibleInterfaces.Contains(baseName))
            return false;

        var gaStart = typeName.IndexOf('<');
        string[] csTypeArgs;
        if (gaStart >= 0)
        {
            var argsPart = typeName[(gaStart + 1)..^1];
            csTypeArgs = CSharpSerializer.SplitGenericArgs(argsPart)
                .Select(CSharpSerializer.ToCSharpTypeName)
                .ToArray();
        }
        else
        {
            csTypeArgs = Array.Empty<string>();
        }

        expr = baseName switch
        {
            "IEnumerable" when csTypeArgs.Length >= 1
                => $"System.Linq.Enumerable.Empty<{csTypeArgs[0]}>()",

            "IComparer" when csTypeArgs.Length >= 1
                => $"System.Collections.Generic.Comparer<{csTypeArgs[0]}>.Default",

            "IEqualityComparer" when csTypeArgs.Length >= 1
                => $"System.Collections.Generic.EqualityComparer<{csTypeArgs[0]}>.Default",

            _ => null
        };

        return expr is not null;
    }

    /// <summary>
    /// Try to generate a non-null C# expression for common interface/abstract types
    /// that would otherwise cause ArgumentNullException when default(T)! is used.
    /// Handles IList&lt;T&gt;, IDictionary&lt;K,V&gt;, IEnumerator&lt;T&gt;,
    /// IComparer (non-generic), IFormatProvider, Stream, TextReader, etc.
    /// Falls back to the NullGuardSafeDefaults dictionary.
    /// </summary>
    private static bool TryGetNullGuardSafeExpression(string typeName, out string expr)
    {
        expr = null!;

        var baseName = GetShortBaseName(typeName);

        if (!NullGuardSafeDefaults.TryGetValue(baseName, out var factory))
            return false;

        var gaStart = typeName.IndexOf('<');
        string[] csTypeArgs;
        if (gaStart >= 0)
        {
            var argsPart = typeName[(gaStart + 1)..^1];
            csTypeArgs = CSharpSerializer.SplitGenericArgs(argsPart)
                .Select(CSharpSerializer.ToCSharpTypeName)
                .ToArray();
        }
        else
        {
            csTypeArgs = Array.Empty<string>();
        }

        expr = factory(csTypeArgs);
        return true;
    }

    /// <summary>
    /// Generate a populated collection expression for collection-like interfaces.
    /// Returns expressions like "new List&lt;int&gt; { default(int)!, default(int)! }"
    /// so that methods see non-empty collections (vs. Array.Empty from smart set).
    /// Supports IList&lt;T&gt;, ICollection&lt;T&gt;, IReadOnlyList&lt;T&gt;,
    /// IReadOnlyCollection&lt;T&gt;, IDictionary&lt;K,V&gt;, IReadOnlyDictionary&lt;K,V&gt;.
    /// </summary>
    private static bool TryGetCollectionInterfaceExpression(string typeName, out string expr)
    {
        expr = null!;

        var gaStart = typeName.IndexOf('<');
        if (gaStart < 0) return false;

        var baseName = typeName[..gaStart];
        var argsPart = typeName[(gaStart + 1)..^1];
        var csTypeArgs = CSharpSerializer.SplitGenericArgs(argsPart)
            .Select(CSharpSerializer.ToCSharpTypeName)
            .ToArray();

        switch (baseName)
        {
            case "IList" or "ICollection" or "IReadOnlyList" or "IReadOnlyCollection"
                when csTypeArgs.Length >= 1:
                expr = $"new System.Collections.Generic.List<{csTypeArgs[0]}> {{ default({csTypeArgs[0]})!, default({csTypeArgs[0]})! }}";
                return true;

            case "IDictionary" or "IReadOnlyDictionary"
                when csTypeArgs.Length >= 2:
                expr = $"new System.Collections.Generic.Dictionary<{csTypeArgs[0]}, {csTypeArgs[1]}> {{ {{ default({csTypeArgs[0]})!, default({csTypeArgs[1]})! }} }}";
                return true;

            default:
                return false;
        }
    }
}
