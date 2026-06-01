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
        ["System.String"] = new[] { "\"\"", "\"hello\"", "null!" },
        ["System.IntPtr"] = new[] { "System.IntPtr.Zero", "new System.IntPtr(42)", "new System.IntPtr(-1)" },
        ["System.UIntPtr"] = new[] { "System.UIntPtr.Zero", "new System.UIntPtr(42)" },
        ["System.DateTime"] = new[] { "default(System.DateTime)", "new System.DateTime(2024, 1, 1)" },
        ["System.TimeSpan"] = new[] { "System.TimeSpan.Zero", "System.TimeSpan.FromTicks(42)" },
        ["System.Guid"] = new[] { "default(System.Guid)", "System.Guid.NewGuid()" },
        ["System.Byte[]"] = new[] { "null!", "System.Array.Empty<byte>()", "new byte[]{0, 1, 255}" },
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
        var sets = new List<ValueSet>();
        var usedSignatures = new HashSet<string>();

        // Set 0: all defaults
        AddUnique(sets, usedSignatures, methodIndex,
            paramTypes.Select(t => DefaultValue(t)).ToArray());

        // Sets 1-3: one boundary at a time (up to 3 params)
        for (int i = 0; i < Math.Min(paramTypes.Length, 3); i++)
        {
            AddUnique(sets, usedSignatures, methodIndex,
                paramTypes.Select((t, idx) =>
                    idx == i ? BoundaryValue(t, 0) : DefaultValue(t)).ToArray());
        }

        // Ensure at least 2 sets
        if (sets.Count < 2 && paramTypes.Length > 0)
        {
            AddUnique(sets, usedSignatures, methodIndex,
                paramTypes.Select(t => DefaultValue(t)).ToArray());
        }

        // One AutoFixture-generated random set (only for types that serialize cleanly)
        if (_autoFixture is not null)
        {
            var fixtureArgs = paramTypes.Select(t =>
            {
                var expr = _autoFixture.TryGenerateExpression(t);
                return expr ?? DefaultValue(t);
            }).ToArray();
            AddUnique(sets, usedSignatures, methodIndex, fixtureArgs);
        }

        return sets;
    }

    private string DefaultValue(string typeName)
    {
        // Span<T>: use default
        if (typeName.StartsWith("System.Span<") || typeName.StartsWith("System.ReadOnlySpan<"))
            return $"default({CSharpSerializer.ToCSharpTypeName(typeName)})";

        if (typeName.EndsWith('&'))
        {
            var baseType = typeName[..^1].Trim();
            return $"out {_serializer.DefaultExpression(baseType)}";
        }
        return _serializer.DefaultExpression(typeName);
    }

    private string BoundaryValue(string typeName, int variantIndex)
    {
        // Span<T>/ReadOnlySpan<T>: use default
        if (typeName.StartsWith("System.Span<") || typeName.StartsWith("System.ReadOnlySpan<"))
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
}
